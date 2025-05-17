<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\SingletonInstances;

class IdenticalToCommand implements IBuiltInMethodCommand
{
    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 1) {
            throw new OtherRuntimeException("Invalid arguments count for identicalTo: method.");
        }

        // if the instances are the same ID, they are identical
        if ($args[0] === $receiver) {
            return SingletonInstances::True->value;
        } else {
            return SingletonInstances::False->value;
        }
    }
}
