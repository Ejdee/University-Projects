<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Interpreter;

class ReadCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;
    private Interpreter $interpreter;

    public function __construct(InstanceTable $instanceTable, Interpreter $interpreter)
    {
        $this->instanceTable = $instanceTable;
        $this->interpreter = $interpreter;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 0) {
            throw new OtherRuntimeException("Invalid arguments count for read method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if (!$this->instanceTable->isInstanceOf("String", $receiver)) {
            throw new WrongArgumentException("read method requires String argument.");
        }

        $loadedString = $this->interpreter->getInput()->readString();

        // set the value to the receiver instance
        $receiverInstance->setValue($loadedString);

        return $receiver;
    }
}
