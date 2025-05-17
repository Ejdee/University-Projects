<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class NotCommand implements IBuiltInMethodCommand
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
            throw new OtherRuntimeException("Invalid arguments count for not method.");
        }

        if ($this->instanceTable->isInstanceOf("True", $receiver)) {
            return SingletonInstances::False->value;
        } else {
            return SingletonInstances::True->value;
        }
    }
}
