<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;

class AsStringCommand implements IBuiltInMethodCommand
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
            throw new OtherRuntimeException("Invalid arguments count for asString method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if ($receiverInstance->getClassRefName() === "String") {
            return $receiver;
        } elseif ($receiverInstance->getClassRefName() === "Nil") {
            return $this->instanceTable->addInstance("String", "nil");
        } elseif ($this->instanceTable->isInstanceOf("Integer", $receiver)) {
            if (is_numeric($receiverInstance->getValue())) {
                return $this->instanceTable->addInstance("String", (string)$receiverInstance->getValue());
            } else {
                throw new OtherRuntimeException("Invalid integer value for asString.");
            }
        } else {
            return $this->instanceTable->addInstance("String", "");
        }
    }
}
