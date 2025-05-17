<?php

namespace IPP\Student\Interfaces;

interface IBuiltInMethodCommand
{
    /** @param array<int> $args */
    public function execute(int $receiver, array $args): int;
}
