<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class NewCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;

    public function __construct(InstanceTable $instanceTable)
    {
        $this->instanceTable = $instanceTable;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 0) {
            throw new OtherRuntimeException("Invalid arguments count for new method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);
        $className = $receiverInstance->getClassRefName();

        // if the class is integer or string, set default values
        if ($this->instanceTable->isInstanceOf("Integer", $receiver)) {
            $receiverInstance->setValue(0);
        } elseif ($this->instanceTable->isInstanceOf("String", $receiver)) {
            $receiverInstance->setValue("");
        }

        // the receiver is already an instance from the literal interpreting, just return it
        return $receiver;
    }
}
