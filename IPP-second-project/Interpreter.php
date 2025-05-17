<?php

namespace IPP\Student;

use IPP\Core\AbstractInterpreter;
use IPP\Student\Exceptions\DoNotUnderstandException;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Facades\ExecutionFacade;
use IPP\Student\Helpers\ContextStack;
use IPP\Student\Services\Execution\BlockExecutor;
use IPP\Student\Services\Execution\BuiltInMethodExecutor;
use IPP\Student\Services\Execution\UserMethodExecutor;
use IPP\Student\Services\ScraperService;
use IPP\Student\Tables\ClassTable;
use IPP\Student\Tables\InstanceTable;
use IPP\Student\Tables\SymbolTable;
use IPP\Student\Visitors\XMLElementVisitor;

class Interpreter extends AbstractInterpreter
{
    private SymbolTable $symbolTable;
    private ClassTable $classTable;
    private BuiltInMethodExecutor $builtInMethodExecutor;
    private InstanceTable $instanceTable;
    private ContextStack $contextStack;
    private bool $superLookUpMode = false;
    private XMLElementVisitor $XMLElementVisitor;
    private UserMethodExecutor $userMethodExecutor;
    private BlockExecutor $blockExecutorService;

    public function execute(): int
    {

        $dom = $this->source->getDOMDocument();
        $xml = $dom->saveXML();

        if (!$xml) {
            $this->stderr->writeString("Error: Cannot save XML.");
            return 41;
        }

        $this->contextStack = new ContextStack();
        $this->symbolTable = new SymbolTable();
        $this->classTable = new ClassTable();
        $this->instanceTable = new InstanceTable($this->classTable, $this);
        $this->blockExecutorService = new BlockExecutor($this->contextStack, $this->symbolTable);
        $this->builtInMethodExecutor = new BuiltInMethodExecutor($this, $this->instanceTable, $this->contextStack);
        $this->userMethodExecutor = new UserMethodExecutor($this->contextStack, $this->instanceTable);

        $executionFacade = new ExecutionFacade(
            $this->builtInMethodExecutor,
            $this->userMethodExecutor,
            $this->blockExecutorService
        );

        $this->builtInMethodExecutor->setExecutionFacade($executionFacade);
        $this->userMethodExecutor->setExecutionFacade($executionFacade);

        $this->XMLElementVisitor = new XMLElementVisitor(
            $this,
            $this->instanceTable,
            $this->contextStack,
            $this->symbolTable,
            $executionFacade
        );

        $this->blockExecutorService->setXMLElementVisitor($this->XMLElementVisitor);

        // scrape the XML for classes and methods
        $scraper = new ScraperService($this->classTable, $dom);
        $scraper->scrape();

        //echo "Interpreting:\n";
        $this->executeMainRun();
        return 0;
    }

    private function executeMainRun(): int
    {
        if (!$this->classTable->classExists("Main")) {
            throw new OtherRuntimeException("Class 'Main' not found.");
        }

        $mainInstanceId = $this->instanceTable->addInstance("Main");
        $this->contextStack->addContext($mainInstanceId);

        $methodRun = $this->instanceTable->getMethodByName($mainInstanceId, "run", $this->superLookUpMode);
        if (!$methodRun) {
            throw new DoNotUnderstandException("Method 'run' not found in class 'Main'.");
        }
        $this->XMLElementVisitor->visit($methodRun->getBody());
        $this->contextStack->removeContext();

        return 0;
    }

    public function getInput(): \IPP\Core\Interface\InputReader
    {
        return $this->input;
    }

    public function getOutput(): \IPP\Core\Interface\OutputWriter
    {
        return $this->stdout;
    }

    public function getSuperLookUpMode(): bool
    {
        return $this->superLookUpMode;
    }

    public function setSuperLookUpMode(bool $superLookUpMode): void
    {
        $this->superLookUpMode = $superLookUpMode;
    }
}
