<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Tables\InstanceTable;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Models\Instance;

class FromCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;

    public function __construct(InstanceTable $instanceTable)
    {
        $this->instanceTable = $instanceTable;
    }

    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int
    {
        if (count($args) !== 1) {
            throw new OtherRuntimeException("Invalid arguments count for from: method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);
        $arg = $this->instanceTable->getInstanceFromTheTable($args[0]);

        if (!$this->instanceTable->isInstanceOf($arg->getClassRefName(), $receiver)) {
            throw new WrongArgumentException("from: invalid argument.");
        }

        // copy the attributes if there are any
        $this->copyAttributes($arg, $receiverInstance);

        // set the new value
        $receiverInstance->setValue($arg->getValue());

        return $receiver;
    }

    private function copyAttributes(Instance $source, Instance $destination): void
    {
        $sourceAttributes = $source->getAttributes();

        foreach ($sourceAttributes as $attribute) {
            $destination->addAttribute($attribute);
        }
    }
}
