<?php

namespace IPP\Student\Visitors;

use IPP\Core\Exception\XMLException;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Facades\ExecutionFacade;
use IPP\Student\Helpers\ContextStack;
use IPP\Student\Helpers\ElementSorter;
use IPP\Student\Interpreter;
use IPP\Student\Models\Block;
use IPP\Student\Tables\InstanceTable;
use IPP\Student\Tables\SymbolTable;

class XMLElementVisitor
{
    private Interpreter $interpreter;
    private InstanceTable $instanceTable;
    private ContextStack $contextStack;
    private SymbolTable $symbolTable;
    private int $undefinedInstanceId = 0;
    private ExecutionFacade $executionFacade;

    public function __construct(
        Interpreter $interpreter,
        InstanceTable $instanceTable,
        ContextStack $contextStack,
        SymbolTable $symbolTable,
        ExecutionFacade $executionFacade
    ) {
        $this->instanceTable = $instanceTable;
        $this->interpreter = $interpreter;
        $this->contextStack = $contextStack;
        $this->symbolTable = $symbolTable;
        $this->executionFacade = $executionFacade;
    }

    public function visit(\DOMElement $element): int
    {
        //echo "Visiting element: " . $element->nodeName . "\n";
        return match ($element->nodeName) {
            'block' => $this->visitBlock($element),
            'assign' => $this->visitAssign($element),
            'literal' => $this->visitLiteral($element),
            'var' => $this->visitVar($element),
            'send' => $this->visitSend($element),
            'expr' => $this->visitExpr($element),
            default => $this->visitChildren($element),
        };
    }

    private function visitBlock(\DOMElement $element): int
    {
        $parentElement = $element->parentElement ?? throw new XMLException("Block element has no parent.");

        // if the parent is a method, execute the methods block straight away
        if ($parentElement->nodeName === "method") {
            $this->executionFacade->executeBlock($element, [], $this->contextStack->getCurrentContextId());
        }

        // otherwise, create a new block instance
        $arity = $element->getAttribute("arity");
        $paramNames = ElementSorter::getParametersSorted($element);
        return $this->instanceTable->addInstance("Block", new Block(
            $arity,
            $element,
            $paramNames,
            $this->contextStack->getCurrentContextId(),
        ));
    }

    private function visitAssign(\DOMElement $element): int
    {
        // assignment's first child has to be the var and second child is the expression
        $varElement = $element->firstElementChild;
        $expressionElement = $element->lastElementChild;

        if (!$varElement || !$expressionElement) {
            throw new XMLException("Invalid assignment element.");
        }

        // the expression can be a literal, var, block, or method call
        // it can only have one child element, so we can use firstElementChild
        $varName = $varElement->getAttribute("name");

        $results = $this->visit($expressionElement);

        if ($varName !== "_") {
            $this->symbolTable->addSymbol($varName, $results);
        }

        return $results;
    }

    private function visitLiteral(\DOMElement $element): int
    {
        $className = $element->getAttribute("class");
        $SingletonId = $this->instanceTable->getSingletonId($className);

        // if it is a class literal, create an instance of a class that is stored in value
        if ($className === "class") {
            $className = $element->getAttribute("value");
            $SingletonId = $this->instanceTable->getSingletonId($className);
        }

        if ($SingletonId !== null) {
            // is singleton and return the instance id
            return $SingletonId;
        }

        return $this->instanceTable->addInstance($className, $element->getAttribute("value"));
    }

    private function visitVar(\DOMElement $element): int
    {
        $varName = $element->getAttribute("name");
        $symbol = $this->symbolTable->getItem($varName);

        if ($symbol) {
            return $symbol->getInstanceId();
        } else {
            if ($varName == "self") {
                // self is a special case, it refers to the current context
                return $this->contextStack->getCurrentContextId();
            } elseif ($varName == "super") {
                // super is a special case, it refers to the parent context
                $this->interpreter->setSuperLookUpMode(true);
                return $this->contextStack->getCurrentContextId();
            }
            throw new OtherRuntimeException("Variable '$varName' not found in the symbol table.");
        }
    }

    private function visitSend(\DOMElement $element): int
    {
        $methodName = $element->getAttribute("selector");

        $resultExpr = null;
        $args = [];

        $sortedSubElements = ElementSorter::getSendElementsSorted($element);

        foreach ($sortedSubElements as $subElement) {
            if ($subElement->nodeName === "expr") {
                $resultExpr = $this->visit($subElement);
            } elseif ($subElement->nodeName === "arg") {
                $args[] = $this->visit($subElement);
            }
        }

        if ($resultExpr === null) {
            throw new XMLException("Send element has no expression.");
        }

        $methodToExecute = $this->instanceTable->getMethodByName(
            $resultExpr,
            $methodName,
            $this->interpreter->getSuperLookUpMode()
        );

        if ($methodToExecute !== null) {
            return $this->executionFacade->executeUserMethod($methodToExecute, $args, $resultExpr);
        }

        return $this->undefinedInstanceId;
    }

    private function visitExpr(\DOMElement $element): int
    {
        $expressionStart = $element->firstElementChild ?? throw new XMLException("Expression element is empty.");
        return $this->visit($expressionStart);
    }

    private function visitChildren(\DOMElement $element): int
    {
        $result = 0;
        foreach ($element->childNodes as $child) {
            if ($child instanceof \DOMElement) {
                $result = $this->visit($child);
            }
        }
        return $result;
    }
}
