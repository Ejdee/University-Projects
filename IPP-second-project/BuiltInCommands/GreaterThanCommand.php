<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class GreaterThanCommand implements IBuiltInMethodCommand
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
            throw new OtherRuntimeException("Invalid arguments count for greaterThan: method.");
        }

        $arg = $this->instanceTable->getInstanceFromTheTable($args[0]);
        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if (!$this->instanceTable->isInstanceOf("Integer", $args[0])) {
            throw new WrongArgumentException("greaterThan: method requires Integer argument.");
        }

        if ($receiverInstance->getValue() > $arg->getValue()) {
            return SingletonInstances::True->value;
        } else {
            return SingletonInstances::False->value;
        }
    }
}
