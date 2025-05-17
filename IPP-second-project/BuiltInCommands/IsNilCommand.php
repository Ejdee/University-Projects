<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class IsNilCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;

    public function __construct(InstanceTable $instanceTable)
    {
        $this->instanceTable = $instanceTable;
    }

    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 0) {
            throw new OtherRuntimeException("Invalid arguments count for isNil method.");
        }

        if ($this->instanceTable->isInstanceOf("Nil", $receiver)) {
            return SingletonInstances::True->value;
        } else {
            return SingletonInstances::False->value;
        }
    }
}
