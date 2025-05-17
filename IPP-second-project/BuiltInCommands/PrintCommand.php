<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Interpreter;

class PrintCommand implements IBuiltInMethodCommand
{
    private Interpreter $interpreter;
    private InstanceTable $instanceTable;

    public function __construct(Interpreter $interpreter, InstanceTable $instanceTable)
    {
        $this->interpreter = $interpreter;
        $this->instanceTable = $instanceTable;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) > 0) {
            throw new OtherRuntimeException("Invalid arguments count for print method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        $receiverValue = $receiverInstance->getValue();
        if (!is_string($receiverValue)) {
            throw new OtherRuntimeException("Invalid receiver value for print method.");
        }

        // handle the escape sequences
        $transformedString = stripcslashes($receiverValue);

        $this->interpreter->getOutput()->writeString($transformedString);
        return $receiver;
    }
}
