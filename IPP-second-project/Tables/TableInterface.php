<?php

namespace IPP\Student\Tables;

/**
 * @template K
 * @template T
 */
interface TableInterface
{
    /**
     * @param K $name
     * @param T $item
     */
    public function addItem(mixed $name, mixed $item): void;
    public function printTable(): void;
}
