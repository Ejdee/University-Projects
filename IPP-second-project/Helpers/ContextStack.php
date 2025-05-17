<?php

namespace IPP\Student\Helpers;

class ContextStack
{
    /** @var array<int, int> */
    private array $currentContextId = [];

    public function addContext(int $instanceId): void
    {
        $this->currentContextId[] = $instanceId;
    }

    public function removeContext(): void
    {
        array_pop($this->currentContextId);
    }

    public function getCurrentContextId(): int
    {
        if (empty($this->currentContextId)) {
            return 0;
        }
        return end($this->currentContextId);
    }
}
