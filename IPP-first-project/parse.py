import sys
import argparse
import re

import lark
from lark import Visitor, Transformer, Token, Tree
from enum import Enum
import xml.etree.ElementTree as ET
from xml.dom import minidom

# Take all the arguments after the name of the program
args = sys.argv[1:]
# If there are some argument, and they are not -h or --help, print error
if len(args) != 0:
    if ('-h' not in args and '--help' not in args) or len(args) > 1:
        print(f"Invalid arguments. Use -h or --help to see usage information.", file=sys.stderr)
        sys.exit(10)

# Parser for command line arguments
parser = argparse.ArgumentParser(add_help=False)
parser.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS, help="Print the help message and exit.")
args = parser.parse_args()

# Store the standard input
input_string = sys.stdin.read()

# Syntax and lexical analysis using Lark

# Rewrite the grammar
grammar = r"""
program: class_def* program_end
program_end: 
        
class_def: "class" CLASSIDENTIF ":" CLASSIDENTIF "{" method* "}"

method: selector block

selector: IDENTIF -> non_par_selector
        | IDENTIF_WITH_PARAM selector_tail -> par_selector

selector_tail: IDENTIF_WITH_PARAM selector_tail -> par_selector
        | -> selector_end

block: "[" block_par "|" block_stat* "]"

block_par: BLOCK_PARAM block_par -> block_par
        | -> block_par_end

block_stat: ASSIGNED_IDENTIF ":=" expr "."
        
expr: expr_base expr_tail expr_end

expr_end: 

expr_tail: IDENTIF -> non_par_sel_method
        | expr_sel *

expr_sel: IDENTIF_WITH_PARAM expr_base

expr_base: INTEGER -> integer_literal
        | STRING -> string_literal
        | IDENTIF -> identif_literal
        | CLASSIDENTIF -> class_identif
        | block 
        | "(" expr ")"

CLASSIDENTIF: /[A-Z][a-zA-Z0-9]*/
IDENTIF: /[a-z_][a-zA-Z0-9_]*/ 
ASSIGNED_IDENTIF: /[a-z_][a-zA-Z0-9_]*/ 
IDENTIF_WITH_PARAM: /[a-z_][a-zA-Z0-9_]*:/
BLOCK_PARAM: /:[a-z_][a-zA-Z0-9_]*/
INTEGER: /[+-]?\d+/
STRING: /'([^\\']|\\[n'\\])*'/

%ignore /\s+/
%ignore /"[^"]*"/
"""

# Parse the input
parser = lark.Lark(grammar, start='program', parser='lalr')

# UnexpectedCharacters = lexical error
# UnexpectedToken = syntax error
try:
    tree = parser.parse(input_string)
except lark.exceptions.UnexpectedCharacters as e:
    print(e, file=sys.stderr)
    sys.exit(21)
except lark.exceptions.UnexpectedToken as e:
    print(e, file=sys.stderr)
    sys.exit(22)

# Semantic analysis

class Types(Enum):
    VARIABLE = 1
    METHOD = 2
    PARAMETER = 3
    ARGUMENT = 4
    CLASS = 5


class SymbolTable:
    def __init__(self):
        self.global_symbols = {}
        self.symbols = [{}]
        self.current_scope = 0

    def add_scope(self):
        self.symbols.append({})
        self.current_scope += 1

    def add_symbol(self, name, symbol_type):
        """
        Add symbol to the current scope of the symbol table with its type.
        :param name: name of the symbol
        :param symbol_type: type of the symbol: variable/method/parameter/argument/class_name
        """
        self.symbols[self.current_scope][name] = symbol_type

    def lookup(self, name):
        """
        Look up a symbol by name. Search the symbol table in current scope and global symbol table.
        :param name: name of the symbol
        """
        if name in self.global_symbols:
            return self.global_symbols[name]
        elif name in self.symbols[self.current_scope]:
            return self.symbols[self.current_scope][name]
        else:
            return None

    def leave_scope(self):
        if self.current_scope == 0:
            print(f"This is the highest scope.")
            return
        self.symbols.pop()
        self.current_scope -= 1

class ClassCollector(Visitor):
    def __init__(self):
        self.classes = {
            'Object': {
                'methods': ['identicalTo:', 'equalTo:', 'asString', 'isNumber', 'isString', 'isBlock', 'isNil', 'new', 'from:'],
                'parent': None
            },
            'Nil': {
                'methods': ['asString'],
                'parent': 'Object'
            },
            'Integer': {
                'methods': ['equalTo:', 'greaterThan:', 'plus:', 'minus:', 'multiplyBy:', 'divBy:', 'asString',
                            'asInteger', 'timesRepeat:'],
                'parent': 'Object'
            },
            'String': {
                'methods': ['read', 'print', 'equalTo:', 'asString', 'asInteger', 'concatenateWith:',
                            'startWith:endsBefore:', 'startWith:', 'endsBefore:'],
                'parent': 'Object'
            },
            'Block': {
                'methods': ['whileTrue'],
                'parent': 'Object'
            },
            'True': {
                'methods': ['not', 'and:', 'or:', 'ifTrue:ifFalse:', 'ifTrue:', 'ifFalse:'],
                'parent': 'Object'
            },
            'False': {
                'methods': ['not', 'and:', 'or:', 'ifTrue:ifFalse:', 'ifFalse:', 'ifTrue:'],
                'parent': 'Object'
            }
        }
        self.current_class_def = None

    def class_def(self, node):
        """
        Add class name and its parent class to the dictionary where we take track of
        all the classes definitions. This way we ensure each class name is unique.
        :param node: node in AST with class definition with this structure:
            - node.children[0].value = name of the class definitions
            - node.children[1].value = name of the parent class
            - node.children[2:] = methods of the class
        """

        class_name = node.children[0].value
        parent_class = node.children[1].value

        # the class name can't exist already
        if class_name in self.classes:
            print(f"Class '{class_name}' already exists.", file=sys.stderr)
            sys.exit(35)
        else:
            self.classes[class_name] = {"parent": parent_class, "methods": []}
            # push the current class name on the stack
            self.current_class_def = class_name

    def method(self, node):
        """
        Add method to the current class being processed.
        :param node: node in AST with method
        """
        # Add method to the current class being scanned
        if self.current_class_def is not None and node.children:
            selector = node.children[0]

            if selector.data == "non_par_selector":
                self.classes[self.current_class_def]["methods"].append(selector.children[0].value)
                return

            # if the selector has parameters, it is split into multiple parts and we have to collect them all
            while selector.data == "par_selector":
                if selector.children:
                    self.classes[self.current_class_def]["methods"].append(selector.children[0].value)
                    if len(selector.children) > 1:
                        selector = selector.children[1]
                    else:
                        break
                else:
                    break

class SemanticAnalyzer(Visitor):
    def __init__(self, classes, keywords_list):
        self.symbol_table = SymbolTable()
        self.current_class_name = None
        self.classes = classes
        self.keywords = keywords_list
        self.selector_pars = 0
        self.block_pars = 0
        self.check_sel_and_block_pars = False
        self.last_receiver = []
        self.first_expr_in_block = False

    def class_def(self, node):
        """
        Add the scope in the symbol table for the class definition and check
        if the parent class does exist. Also store the current name of the class processed
        to the class variable.
        :param node: node in AST with class definition
        """
        self.symbol_table.add_scope()
        self.current_class_name = node.children[0].value

        # if the parent class of the current class definition doesn't exist, exit with error code
        if node.children[1].value not in self.classes:
            print(f"Class '{node.children[1].value}' is not defined.", file=sys.stderr)
            sys.exit(32)

    def method_end(self, node):
        """
        Method end signalize the last method of the class definitions. Therefore, set the
        current_class_name to None again and leave the scope for the class.
        :param node:
        """
        self.current_class_name = None
        self.symbol_table.leave_scope()

    def block(self, node):
        """
        Add new scope for the symbol table. Add to the current scope of the symbol table all
        the methods usable in the current scope. Meaning methods of the current class and all the parent classes.
        :param node: node in AST with block
        """
        self.symbol_table.add_scope()

        # Add all the method we can use in the current block based on the current class we are in
        if self.current_class_name is not None:
            current_class = self.current_class_name
            while current_class is not None:
                for method in self.classes[current_class]["methods"]:
                    self.symbol_table.add_symbol(method, Types.METHOD)

                current_class = self.classes[current_class].get('parent')

    def block_end(self, node):
        """
        Leave the current class scope from the symbol table
        :param node: node in AST with block_end
        """
        self.symbol_table.leave_scope()

    def block_par(self, node):
        """
        If there are some parameters, store them in the symbol table and check if they are
        not reserved keywords or already used.
        :param node: node in AST with block_par
        """
        if len(node.children) != 0:
            # store the parameter name without the leading colon
            param_name = node.children[0].value[1:]
            if param_name in self.keywords:
                print(f"Reserved keyword '{param_name}' used as parameter.", file=sys.stderr)
                sys.exit(22)
            if self.symbol_table.lookup(param_name) is not None:
                print(f"Symbol '{param_name}' already exists.", file=sys.stderr)
                sys.exit(35)
            self.symbol_table.add_symbol(param_name, Types.PARAMETER)
            self.block_pars += 1

    def block_par_end(self, node):
        """
        Check the number of selector arguments and block parameters.
        :param node: node in AST with block_par_end
        """
        if self.check_sel_and_block_pars:
            if self.block_pars != self.selector_pars:
                print(f"Block parameters = {self.block_pars} != {self.selector_pars} = Selector parameters",
                      file=sys.stderr)
                sys.exit(33)

        # reset the counters and flags
        self.block_pars = 0
        self.selector_pars = 0
        self.check_sel_and_block_pars = False

    def block_process(self, node):
        """
        Check if the identifier is not already used as a parameter of the block or as reserved keyword.
        :param node: node in AST with block_process
        """
        if len(node.children) != 0:
            identifier = node.children[0].value
            if identifier in self.keywords:
                print(f"Reserved keyword '{identifier}' used as identifier", file=sys.stderr)
                sys.exit(22)
            # if the identifier is not in the symbol table, add it
            if self.symbol_table.lookup(identifier) is None:
                self.symbol_table.add_symbol(identifier, Types.VARIABLE)
            elif self.symbol_table.lookup(identifier) == Types.PARAMETER:
                # The variable is used
                print(f"Identifier '{identifier}' already exists as a parameter of the block",
                      file=sys.stderr)
                sys.exit(34)

    def block_stat(self, node):
        self.block_process(node)

    def block_stat_next(self, node):
        self.block_process(node)

    def identif_literal(self, node):
        """
        Check the identifier in expression body, it has to exist.
        :param node: node in AST with identif_literal
        """
        name = node.children[0].value
        if self.symbol_table.lookup(name) is None and name not in self.keywords:
            print(f"Identifier '{node.children[0].value}' does not exist.", file=sys.stderr)
            sys.exit(32)

    def par_selector(self, node):
        """
        There is no need to check if the name collides with the reserved keyword, since there is not a single keyword with parameters.
        So just increase the counter of selector parameters and set teh flag to check the number of arguments and parameters of the block
        :param node: node in AST with par_selector
        """

        self.selector_pars += 1
        self.check_sel_and_block_pars = True

    def non_par_selector(self, node):
        """
        Check the selector name and set the flag for check number of selector arguments and block parameters.
        :param node:
        """
        selector_name = node.children[0].value
        if selector_name in self.keywords:
            print(f"Reserved keyword '{selector_name}' used as selector", file=sys.stderr)
            sys.exit(22)
        self.check_sel_and_block_pars = True

    def non_par_sel_method(self, node):
        """
        Check the method call name.
        :param node: node in AST with non_par_sel_method
        """
        sel_method_name = node.children[0].value
        if sel_method_name in self.keywords:
            print(f"Reserved keyword '{sel_method_name}' used as selector method", file=sys.stderr)
            sys.exit(22)

        # if there is a receiver on the stack, check if the method call is valid
        if len(self.last_receiver) != 0:
            self.check_method_receiver(sel_method_name, self.last_receiver[-1])

    def expr_sel(self, node):
        """
        Check the method call name with parameter. The last receiver there was, is pushed on the stack. So call
        the check function with that receiver and the method we are processing.
        :param node: node in AST with non_par_sel_method
        """
        sel_method_name = node.children[0].value
        if sel_method_name in self.keywords:
            print(f"Reserved keyword '{sel_method_name}' used as selector method", file=sys.stderr)
            sys.exit(22)

        if len(self.last_receiver) != 0:
            self.check_method_receiver(sel_method_name, self.last_receiver[-1])

    def expr(self, node):
        """
        If the child of expr is some expression identifier, push it on the stack of the last receiver.
        :param node: node in AST with expr
        """
        if isinstance(node.children[0], Tree) and (node.children[0].data == 'expr_base' or node.children[0].data == 'block'):
            return

        id_type = node.children[0]
        self.last_receiver.append({id_type.data: id_type.children[0].value})

    def expr_end(self, node):
        """
        If there is a receiver on the stack, pop it, since the end of the expression is reached.
        :param node: node in AST with expr_end
        """
        if len(self.last_receiver) == 0:
            return
        self.last_receiver.pop()

    def class_identif(self, node):
        """
        Handle class identifier in expression body. If the name doesn't exist, throw an error.
        :param node: node in AST with class_identif
        """
        name = node.children[0].value

        if name not in self.classes:
            print(f"Class '{name}' does not exist.", file=sys.stderr)
            sys.exit(32)

    def program(self, node):
        pass

    def check_method_receiver(self, sel_method_name, receiver_name):
        """
        Based on the receiver_type, call the check function with correct arguments.
        :param sel_method_name: the name of the method call
        :param receiver_name: key,value of receiver. Key is type and value is the name
        """

        receiver_type, receiver_value = list(receiver_name.items())[0]

        if receiver_type == 'identif_literal':
            return
        elif receiver_type == 'integer_literal':
            # CHECK FOR INTEGER CLASS
            self.check_class_receiver(sel_method_name, "Integer")
        elif receiver_type == 'string_literal':
            # CHECK FOR STRING CLASS
            self.check_class_receiver(sel_method_name, "String")
        elif receiver_type == 'class_identif':
            # check the name of the class
            self.check_class_receiver(sel_method_name, receiver_value)

    def check_class_receiver(self, sel_method_name, current_class):
        """
        Collect all the methods available fot the class and its parents. Then check if the
        method call passed is in the available methods. Throw an error if not.
        :param sel_method_name: the name of the method call
        :param current_class: the name of the receiver class
        """
        methods_available = []
        while current_class is not None:
            for method in self.classes[current_class]["methods"]:
                methods_available.append(method)

            current_class = self.classes[current_class].get('parent')

        if sel_method_name not in methods_available:
            print(f"Method '{sel_method_name}' does not exist.", file=sys.stderr)
            sys.exit(32)



# Collect the classes
class_collector = ClassCollector()
class_collector.visit_topdown(tree)

keywords = {'class', 'self', 'super', 'nil', 'true', 'false'}

# Analyze the semantics
semantic_analyzer = SemanticAnalyzer(class_collector.classes, keywords)
semantic_analyzer.visit_topdown(tree)

# Check if the main with run method is in the program
if 'Main' not in class_collector.classes or 'run' not in class_collector.classes['Main']['methods']:
    print("There is no class named 'Main' with method 'run'.", file=sys.stderr)
    sys.exit(31)

class XMLTransformGenerator(Transformer):
    def __init__(self, first_comm):
        super().__init__()
        self.assign_order = 0
        # comment without the single quotes
        if first_comm:
            self.comment = first_comm[1:-1]
        else:
            self.comment = None

    def program(self, items):
        program_elem = ET.Element('program', {'language': 'SOL25'})

        # set the comment if there is any
        if self.comment is not None:
            program_elem.set('description', self.comment)

        for item in items:
            if item is not None:
                program_elem.append(item)
        return program_elem

    def class_def(self, items):
        class_name = str(items[0])
        parent_name = str(items[1])
        class_elem = ET.Element('class', {'name': class_name, 'parent': parent_name})

        # Add methods
        for item in items[2:]:
            if item is not None:
                class_elem.append(item)
        return class_elem

    def method(self, items):
        method_elem = ET.Element('method', {'selector': str(items[0])})

        # Add block
        for item in items[1:]:
            #print(item)
            if isinstance(item, ET.Element):
                method_elem.append(item)

        return method_elem

    def non_par_selector(self, items):
        return items[0].value

    def par_selector(self, items):
        if items[1] is not None:
            return items[0].value + items[1]
        return items[0].value

    def selector_end(self, items):
        return None

    def block(self, items):

        parameters = []
        if items[0] is not None:
            parameters = list(filter(None, items[0].split(":")))

        block_elem = ET.Element('block', arity=str(len(parameters)))
        for i, param in enumerate(parameters, start=1):
            param_elem = ET.SubElement(block_elem, 'parameter', order=str(i), name=param)

        # process the block_stat
        for i, item in enumerate(items[1:], start=1):
            assign_elem = ET.SubElement(block_elem, 'assign', order=str(i))
            if isinstance(item, tuple):
                var_elem = ET.SubElement(assign_elem, 'var', name=item[0])
                assign_elem.append(item[1])
            if isinstance(item, ET.Element):
                assign_elem.append(item)

        return block_elem

    def block_par_end(self, items):
        return None

    def block_par(self, items):
        if items[1] is not None:
            return items[0].value + items[1]
        return items[0].value

    def integer_literal(self, args):
        expr_elem = ET.Element('expr')
        lit_elem = ET.SubElement(expr_elem, 'literal', {'class': 'Integer', 'value': str(args[0])})
        return expr_elem

    def string_literal(self, items):
        string_value = items[0][1:-1]

        # Now create the XML element
        expr_elem = ET.Element('expr')
        lit_elem = ET.SubElement(expr_elem, 'literal', {'class': 'String', 'value': string_value})

        return expr_elem

    def identif_literal(self, items):
        special_values = {'nil', 'true', 'false'}
        expr_elem = ET.Element('expr')
        if items[0] in special_values:
            if items[0] == 'nil':
                var_elem = ET.SubElement(expr_elem, 'literal', {'class': "Nil", 'value':'nil'})
            if items[0] == 'true':
                var_elem = ET.SubElement(expr_elem, 'literal', {'class': "True", 'value': 'true'})
            if items[0] == 'false':
                var_elem = ET.SubElement(expr_elem, 'literal', {'class': "False", 'value': 'false'})
        else:
            var_elem = ET.SubElement(expr_elem,'var', {'name': items[0]})
        return expr_elem

    def class_identif(self, items):
        expr_elem = ET.Element('expr')
        return expr_elem

    def block_stat(self, items):
        if items[0] is None:
            return None

        return items[0].value, items[1]


    def block_end(self, items):
        return None

    def expr(self, items):
        expr_elem = ET.Element('expr')

        # If there is a tail with parameters (we detect it by the list)
        if items[1] is not None and isinstance(items[1], list):
            sel_elem = ET.SubElement(expr_elem, 'send')
            sel_elem.append(items[0])

            # create the selector argument elements
            sel_name = ""
            i = 1
            for item in items[1]:
                if isinstance(item, tuple):
                    sel_name += item[0]
                    arg_elem = ET.SubElement(sel_elem, 'arg', order=str(i))
                    arg_elem.append(item[1])
                    i += 1

            sel_elem.set('selector', str(sel_name))
            return expr_elem

        # If there is a tail but without parameters (single name of the selector)
        if items[1] is not None:
            sel_elem = ET.SubElement(expr_elem, 'send', selector=items[1])
            sel_elem.append(items[0])
            return expr_elem


        else:
            return items[0]

    def expr_tail(self, items):
        # if the tail is empty, return None
        if len(items) == 0:
            return None
        return items


    def expr_sel(self, items):
        # Only return the children elements
        return items[0].value, items[1]

    def expr_base(self, items):
        if isinstance(items[0], ET.Element):
            if items[0].tag == 'block':
                expr_elem = ET.Element('expr')
                expr_elem.append(items[0])
                return expr_elem
        return items[0]

    def non_par_sel_method(self, items):
        return items[0].value

    def expression_end(self, items):
        return None

    def program_end(self, items):
        return None


# Get the first comment in the input if there is any
first_comment = re.search(r'"[^"]*"', input_string)

# Create the XML generator instance and run the generator
xml_transformer = None
if first_comment is not None:
     xml_transformer = XMLTransformGenerator(first_comment.group())
else:
    xml_transformer = XMLTransformGenerator(None)


xml_tree = xml_transformer.transform(tree)

if isinstance(xml_tree, ET.Element):
    xml_str = ET.tostring(xml_tree, encoding='utf-8', method="xml")
    xml_str = xml_str.decode('utf-8')
    xml_pretty_str = minidom.parseString(xml_str).toprettyxml(indent="  ")

    # Remove the XML declaration added by minidom
    xml_pretty_str = xml_pretty_str.replace('<?xml version="1.0" ?>\n', '', 1)

    # Add the correct declaration manually
    xml_str_with_header = '<?xml version="1.0" encoding="UTF-8"?>\n' + xml_pretty_str

    print(xml_str_with_header)
else:
    print("Transformation did not return a valid XML Element.")
    sys.exit(99)