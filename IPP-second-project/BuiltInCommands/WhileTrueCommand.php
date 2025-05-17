<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Services\Execution\BuiltInMethodExecutor;
use IPP\Student\SingletonInstances;
use IPP\Student\Tables\InstanceTable;

class WhileTrueCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;
    private BuiltInMethodExecutor $builtInMethodExecutor;

    public function __construct(InstanceTable $instanceTable, BuiltInMethodExecutor $builtInMethodExecutor)
    {
        $this->instanceTable = $instanceTable;
        $this->builtInMethodExecutor = $builtInMethodExecutor;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 1) {
            throw new OtherRuntimeException("Invalid arguments count for whileTrue: method.");
        }

        if (!$this->instanceTable->isInstanceOf("Block", $args[0])) {
            throw new WrongArgumentException("whileTrue: method requires Block receiver.");
        }

        // since the receiver is an instance of Block, we need to call it too to get the value

        $blockEvaluation = $this->builtInMethodExecutor->callArgumentWithMethod($receiver, "value", []);
        while ($blockEvaluation == SingletonInstances::True->value) {
            // execute the argument calling the value method
            $this->builtInMethodExecutor->callArgumentWithMethod($args[0], "value", []);
            $blockEvaluation = $this->builtInMethodExecutor->callArgumentWithMethod($receiver, "value", []);
        }

        return $receiver;
    }
}
