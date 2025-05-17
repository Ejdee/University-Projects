<?php

namespace IPP\Student\Facades;

use IPP\Student\Models\Method;

interface IExecutionFacade
{
    /** @param array<int> $args */
    public function executeUserMethod(Method $method, array $args, int $receiver): int;
    /** @param array<int> $args */
    public function executeBuiltInMethod(Method $method, array $args, int $receiver): int;
    /** @param array<int> $args */
    public function executeBlock(\DOMElement $blockElement, array $args, int $selfId): int;
}
