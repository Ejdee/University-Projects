<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class StartsWithEndsBeforeCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;

    public function __construct(InstanceTable $instanceTable)
    {
        $this->instanceTable = $instanceTable;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 2) {
            throw new OtherRuntimeException("Invalid arguments count for startsWith:EndsBefore: method.");
        }

        $arg1 = $this->instanceTable->getInstanceFromTheTable($args[0]);
        $arg2 = $this->instanceTable->getInstanceFromTheTable($args[1]);

        $arg1Value = $arg1->getValue();
        $arg2Value = $arg2->getValue();

        if (!is_numeric($arg1Value) || !is_numeric($arg2Value)) {
            throw new WrongArgumentException("startsWith:EndsBefore: method requires Integer value as arguments.");
        }

        $arg1Value = (int)$arg1Value;
        $arg2Value = (int)$arg2Value;

        // if the arguments are not positive integers, return nil
        if (
            !$this->instanceTable->isInstanceOf("Integer", $args[0]) ||
            !$this->instanceTable->isInstanceOf("Integer", $args[1]) ||
            $arg1->getValue() < 1 ||
            $arg2->getValue() < 1
        ) {
            return SingletonInstances::Nil->value;
        }

        $start = $arg1Value;
        $end = $arg2Value;

        // if the start is greater than the end, return an empty string
        if ($end - $start <= 0) {
            return $this->instanceTable->addInstance("String", "");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);
        $receiverValue = $receiverInstance->getValue();

        // if the receiver is not a string, return nil
        if (!is_string($receiverValue)) {
            throw new WrongArgumentException("startsWith:EndsBefore: method requires String receiver value.");
        }

        // indexing in SOL25 starts from 1
        $substring = substr($receiverValue, $start - 1, $end - $start);

        return $this->instanceTable->addInstance("String", $substring);
    }
}
