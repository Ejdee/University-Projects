<?php

namespace IPP\Student\Models;

class Instance
{
    /** @var array<Attribute> */
    private array $attributes = [];

    /** @var array<string> */
    private array $methods;
    private string $ClassRefName;
    private mixed $value = null;

    /** @param array<string> $methods */
    public function __construct(string $ClassRefName, array $methods, mixed $value = null)
    {
        $this->ClassRefName = $ClassRefName;
        $this->value = $value;
        $this->attributes = [];
        $this->methods = $methods;
    }

    public function getClassRefName(): string
    {
        return $this->ClassRefName;
    }

    /** @return array<string> */
    public function getMethods(): array
    {
        return $this->methods;
    }

    /** @return array<Attribute> */
    public function getAttributes(): array
    {
        return $this->attributes;
    }

    public function getValue(): mixed
    {
        return $this->value;
    }

    public function addAttribute(Attribute $attribute): void
    {
        $this->attributes[$attribute->getName()] = $attribute;
    }

    public function setValue(mixed $value): void
    {
        $this->value = $value;
    }
}
