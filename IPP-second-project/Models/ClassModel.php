<?php

namespace IPP\Student\Models;

class ClassModel
{
    private string $name;
    private ?string $parentName;
    /** @var array<Method> */
    private array $methods;

     /** @param array<Method> $methods */
    public function __construct(string $name, ?string $parentName, array $methods)
    {
        $this->name = $name;
        $this->parentName = $parentName;
        $this->methods = $methods;
    }

    public function getName(): string
    {
        return $this->name;
    }

    public function getParentName(): ?string
    {
        return $this->parentName;
    }

     /** @return array<Method> */
    public function getMethods(): array
    {
        return $this->methods;
    }
}
