<?php

namespace IPP\Student\BuiltInCommands;

use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Services\Execution\BuiltInMethodExecutor;
use IPP\Student\Tables\InstanceTable;

class TimesRepeatCommand implements IBuiltInMethodCommand
{
    private InstanceTable $instanceTable;
    private BuiltInMethodExecutor $builtInMethodExecutor;

    public function __construct(InstanceTable $instanceTable, BuiltInMethodExecutor $builtInMethodExecutor)
    {
        $this->instanceTable = $instanceTable;
        $this->builtInMethodExecutor = $builtInMethodExecutor;
    }

    public function execute(int $receiver, array $args): int
    {
        $result = 0;

        if (count($args) !== 1) {
            throw new OtherRuntimeException("Invalid arguments count for timesRepeat: method.");
        }

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        if (!$this->instanceTable->isInstanceOf("Integer", $receiver)) {
            throw new WrongArgumentException("timesRepeat: requires Integer receiver.");
        }

        $receiverValue = $receiverInstance->getValue();

        if (!is_numeric($receiverValue)) {
            throw new WrongArgumentException("Receiver must be integer for TimesRepeat:.");
        }

        $receiverValue = (int)$receiverValue;

        $loopCount = $receiverValue;

        $tmpInstanceId = $this->instanceTable->addInstance("Integer", 0);
        $tmpInstance = $this->instanceTable->getInstanceFromTheTable($tmpInstanceId);

        for ($i = 1; $i <= $loopCount; $i++) {
            $tmpInstance->setValue($i);
            $result = $this->builtInMethodExecutor->callArgumentWithMethod($args[0], "value:", [$tmpInstanceId]);
        }

        $this->instanceTable->removeInstance($tmpInstanceId);

        return $result;
    }
}
