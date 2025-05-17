<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Services\Execution\BuiltInMethodExecutor;
use IPP\Student\Tables\InstanceTable;

class IfTrueIfFalseCommand implements IBuiltInMethodCommand
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
        if (count($args) !== 2) {
            throw new OtherRuntimeException("Invalid arguments count for ifTrue:ifFalse: method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if ($receiverInstance->getClassRefName() == "True") {
            return $this->builtInMethodExecutor->callArgumentWithMethod($args[0], "value", []);
        } else {
            return $this->builtInMethodExecutor->callArgumentWithMethod($args[1], "value", []);
        }
    }
}
