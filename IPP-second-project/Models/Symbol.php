<?php

namespace IPP\Student\Models;

class Symbol
{
    private string $name;
    private int $instanceId;

    public function __construct(string $name, int $instanceId)
    {
        $this->name = $name;
        $this->instanceId = $instanceId;
    }

    public function getName(): string
    {
        return $this->name;
    }

    public function getInstanceId(): int
    {
        return $this->instanceId;
    }
}
