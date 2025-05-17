<?php

namespace IPP\Student\Models;

class Attribute
{
    private string $name;
    private int $refInstanceId;
    private int $instanceId;

    public function __construct(string $name, int $refInstanceId, int $instanceId)
    {
        $this->name = $name;
        $this->refInstanceId = $refInstanceId;
        $this->instanceId = $instanceId;
    }

    public function getInstanceId(): int
    {
        return $this->instanceId;
    }

    public function setInstanceId(int $instanceId): void
    {
        $this->instanceId = $instanceId;
    }

    public function getName(): string
    {
        return $this->name;
    }

    public function setName(string $name): void
    {
        $this->name = $name;
    }

    public function getRefInstanceId(): int
    {
        return $this->refInstanceId;
    }

    public function setRefInstanceId(int $refInstanceId): void
    {
        $this->refInstanceId = $refInstanceId;
    }
}
