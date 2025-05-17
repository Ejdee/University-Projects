<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class ConcatenateWithCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;

    public function __construct(InstanceTable $instanceTable)
    {
        $this->instanceTable = $instanceTable;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 1) {
            throw new OtherRuntimeException("Invalid arguments count for concatenateWith: method.");
        }

        $arg = $this->instanceTable->getInstanceFromTheTable($args[0]);
        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if (!$this->instanceTable->isInstanceOf("String", $args[0])) {
            return SingletonInstances::Nil->value;
        }

        $receiverValue = $receiverInstance->getValue();
        $argValue = $arg->getValue();

        if (is_string($receiverValue) && is_string($argValue)) {
            $concatenatedValue = $receiverValue . $argValue;
            return $this->instanceTable->addInstance("String", $concatenatedValue);
        } else {
            return SingletonInstances::Nil->value;
        }
    }
}
