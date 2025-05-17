<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;

class ArithmeticOperationCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;
    private string $operation;

    public function __construct(InstanceTable $instanceTable, string $operation)
    {
        $this->instanceTable = $instanceTable;
        $this->operation = $operation;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 1) {
            throw new OtherRuntimeException("Invalid arguments count for arithmetic operation.");
        }

        $arg = $this->instanceTable->getInstanceFromTheTable($args[0]);
        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if (!$this->instanceTable->isInstanceOf("Integer", $args[0])) {
            throw new WrongArgumentException("Arithmetic operation argument requires Integer.");
        }

        $receiverValue = $receiverInstance->getValue();
        $argValue = $arg->getValue();

        if (!is_numeric($receiverValue) || !is_numeric($argValue)) {
            throw new WrongArgumentException("Arithmetic operation requires Integer values.");
        }

        $receiverValue = (int)$receiverValue;
        $argValue = (int)$argValue;

        switch ($this->operation) {
            case '+':
                $result = $receiverValue + $argValue;
                break;
            case '-':
                $result = $receiverValue - $argValue;
                break;
            case '*':
                $result = $receiverValue * $argValue;
                break;
            case '/':
                if ($argValue == 0) {
                    throw new WrongArgumentException("Division by zero.");
                }
                $result = intdiv($receiverValue, $argValue);
                break;
            default:
                throw new OtherRuntimeException("Unknown arithmetic operation.");
        }

        return $this->instanceTable->addInstance("Integer", $result);
    }
}
