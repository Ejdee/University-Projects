<?php

namespace IPP\Student\Models;

use IPP\Student\Exceptions\OtherRuntimeException;

class Method
{
    private string $name;
    private int $selectorArity;
    private ?\DOMElement $body;
    private bool $isBuiltIn;
    private bool $isAttributeSetter = false;
    private bool $isAttributeGetter = false;

    public function __construct(
        string $name,
        ?\DOMElement $body,
        bool $isBuiltIn = false,
        bool $isAttributeSetter = false,
        bool $isAttributeGetter = false
    ) {
        $this->name = $name;
        $this->body = $body;
        $this->selectorArity = substr_count($name, ":");
        $this->isBuiltIn = $isBuiltIn;
        $this->isAttributeSetter = $isAttributeSetter;
        $this->isAttributeGetter = $isAttributeGetter;
    }

    // Getters for the properties
    public function getName(): string
    {
        return $this->name;
    }

    public function getBody(): \DOMElement
    {
        return $this->body ?? throw new OtherRuntimeException("Method body is not set.");
    }

    public function getSelectorArity(): int
    {
        return $this->selectorArity;
    }

    public function isBuiltIn(): bool
    {
        return $this->isBuiltIn;
    }

    public function isAttributeSetter(): bool
    {
        return $this->isAttributeSetter;
    }

    public function isAttributeGetter(): bool
    {
        return $this->isAttributeGetter;
    }

    // Setters for the properties
    public function setBody(?\DOMElement $body): void
    {
        $this->body = $body;
    }
}
