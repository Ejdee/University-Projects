<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class AsIntegerCommand implements IBuiltInMethodCommand
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
            throw new OtherRuntimeException("Invalid arguments count for asInteger: method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);
        if ($receiverInstance->getClassRefName() === "Integer") {
            return $receiver;
        } elseif ($this->instanceTable->isInstanceOf("String", $receiver)) {
            $value = $receiverInstance->getValue();
            if (is_numeric($value)) {
                return $this->instanceTable->addInstance("Integer", (int)$value);
            } else {
                return SingletonInstances::Nil->value;
            }
        } else {
            throw new WrongArgumentException("asInteger invalid receiver.");
        }
    }
}
