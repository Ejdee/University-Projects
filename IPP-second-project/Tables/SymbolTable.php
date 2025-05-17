<?php

namespace IPP\Student\Tables;

use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Models\Symbol;

/** @implements TableInterface<string, Symbol> */
class SymbolTable implements TableInterface
{
    /** @var array<int, array<string, Symbol>> */
    private array $table = [];
    private int $currentScope = 0;

    public function __construct()
    {
        // initialize the table with an empty scope (the first scope)
        $this->table[] = [];
    }

    public function addScope(): void
    {
        $this->table[] = [];
        $this->currentScope++;
    }

    public function removeScope(): void
    {
        if (empty($this->table)) {
            throw new \RuntimeException("No scopes to remove");
        } else {
            array_pop($this->table);
            $this->currentScope--;
        }
    }

    public function addItem(mixed $name, mixed $item): void
    {
        if (empty($this->table)) {
            throw new OtherRuntimeException("Symbol table is empty.");
        }

        $this->table[$this->currentScope][$name] = $item;
    }

    public function addSymbol(string $name, int $instanceId): void
    {
        $symbol = new Symbol($name, $instanceId);
        $this->addItem($name, $symbol);
    }

    public function getItem(string $name): ?Symbol
    {
        return $this->table[$this->currentScope][$name] ?? null;
    }

    public function printTable(): void
    {
        foreach ($this->table as $scope => $symbols) {
            echo "Scope: $scope\n";
            foreach ($symbols as $name => $item) {
                echo "Name: $name, ";
                echo "Instance ID: " . $item->getInstanceId() . ", ";
                echo "\n------------------\n";
            }
        }
    }
}
