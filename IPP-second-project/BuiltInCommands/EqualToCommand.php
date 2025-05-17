<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Models\Method;
use IPP\Student\Services\Execution\BuiltInMethodExecutor;
use IPP\Student\SingletonInstances;
use IPP\Student\Tables\InstanceTable;

class EqualToCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;
    private BuiltInMethodExecutor $executor;

    public function __construct(InstanceTable $instanceTable, BuiltInMethodExecutor $executor)
    {
        $this->instanceTable = $instanceTable;
        $this->executor = $executor;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 1) {
            throw new OtherRuntimeException("Invalid arguments count for equalTo: method.");
        }

        $arg = $this->instanceTable->getInstanceFromTheTable($args[0]);
        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if (
            ($this->instanceTable->isInstanceOf("String", $receiver)
                && $this->instanceTable->isInstanceOf("String", $args[0]))
            || ($this->instanceTable->isInstanceOf("Integer", $receiver)
                && $this->instanceTable->isInstanceOf("Integer", $args[0]))
        ) {
            if ($receiverInstance->getValue() == $arg->getValue()) {
                return SingletonInstances::True->value;
            } else {
                return SingletonInstances::False->value;
            }
        }

        $recAttr = $receiverInstance->getAttributes();
        $argAttr = $arg->getAttributes();
        if (count($recAttr) == 0 && count($argAttr) == 0) {
            // if there are not instantiate attributes, invoke the identicalTo: method
            return $this->executor->execute(new Method("identicalTo:", null, isBuiltIn: true), $args, $receiver);
        } elseif (count($recAttr) != count($argAttr)) {
            // if the number of attributes is different, they are not equal
            return SingletonInstances::False->value;
        } else {
            // if the number of attributes is the same, check if they are equal
            for ($index = 0; $index < count($recAttr); $index++) {
                $recInstance = $this->instanceTable->getInstanceFromTheTable($recAttr[$index]->getInstanceId());
                $argInstance = $this->instanceTable->getInstanceFromTheTable($argAttr[$index]->getInstanceId());
                if ($recInstance->getValue() != $argInstance->getValue()) {
                    return SingletonInstances::False->value;
                }
            }
        }

        return SingletonInstances::True->value;
    }
}
