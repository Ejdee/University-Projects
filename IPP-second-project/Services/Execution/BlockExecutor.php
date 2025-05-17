<?php

namespace IPP\Student\Services\Execution;

use IPP\Student\Helpers\ContextStack;
use IPP\Student\Helpers\ElementSorter;
use IPP\Student\Tables\SymbolTable;
use IPP\Student\Visitors\XMLElementVisitor;

class BlockExecutor
{
    private ContextStack $contextStack;
    private SymbolTable $symbolTable;
    private XMLElementVisitor $XMLElementVisitor;
    private const int UNDEFINEDINSTANCEID = 0;

    public function __construct(ContextStack $contextStack, SymbolTable $symbolTable)
    {
        $this->contextStack = $contextStack;
        $this->symbolTable = $symbolTable;
    }

    public function setXMLElementVisitor(XMLElementVisitor $XMLElementVisitor): void
    {
        $this->XMLElementVisitor = $XMLElementVisitor;
    }

    /** @param array<int> $args */
    public function execute(\DOMElement $blockElement, array $args, int $selfId): int
    {
        // add the block to the context stack
        $this->contextStack->addContext($selfId);

        $result = self::UNDEFINEDINSTANCEID;

        // initialize the parameters of the block
        $paramNames = ElementSorter::getParametersSorted($blockElement);

        $this->symbolTable->addScope();
        for ($i = 0; $i < count($args); $i++) {
            $this->symbolTable->addSymbol($paramNames[$i], $args[$i]);
        }

        // interpret the assignments
        $assigns = ElementSorter::getAssignsSorted($blockElement);
        foreach ($assigns as $assign) {
            $result = $this->XMLElementVisitor->visit($assign);
        }

        // remove the scope after executing the block
        $this->symbolTable->removeScope();
        // remove the context after executing the block
        $this->contextStack->removeContext();

        return $result;
    }
}
