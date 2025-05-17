<?php

namespace IPP\Student\Facades;

use IPP\Student\Models\Method;
use IPP\Student\Services\Execution\BlockExecutor;
use IPP\Student\Services\Execution\BuiltInMethodExecutor;
use IPP\Student\Services\Execution\UserMethodExecutor;

class ExecutionFacade implements IExecutionFacade
{
    private BuiltInMethodExecutor $builtInMethodExecutor;
    private UserMethodExecutor $userMethodExecutor;
    private BlockExecutor $blockExecutor;

    public function __construct(
        BuiltInMethodExecutor $builtInMethodExecutor,
        UserMethodExecutor $userMethodExecutor,
        BlockExecutor $blockExecutor
    ) {
        $this->builtInMethodExecutor = $builtInMethodExecutor;
        $this->userMethodExecutor = $userMethodExecutor;
        $this->blockExecutor = $blockExecutor;
    }

    /** @param array<int> $args */
    public function executeUserMethod(Method $method, array $args, int $receiver): int
    {
        return $this->userMethodExecutor->execute($method, $args, $receiver);
    }

    /** @param array<int> $args */
    public function executeBuiltInMethod(Method $method, array $args, int $receiver): int
    {
        return $this->builtInMethodExecutor->execute($method, $args, $receiver);
    }

    /** @param array<int> $args */
    public function executeBlock(\DOMElement $blockElement, array $args, int $selfId): int
    {
        return $this->blockExecutor->execute($blockElement, $args, $selfId);
    }
}
