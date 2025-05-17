<?php

namespace IPP\Student\Models;

use IPP\Student\Exceptions\OtherRuntimeException;

class Block
{
    private int $blockArity;
    private ?\DOMElement $body;
    /** @var array<string> */
    private array $paramNames = [];
    private int $selfRefId;

    /** @param array<string> $paramNames */
    public function __construct(
        string $blockArity,
        ?\DOMElement $body = null,
        array $paramNames = [],
        int $selfRefId = 0
    ) {
        $this->selfRefId = $selfRefId;
        $this->body = $body;
        $this->blockArity = (int)$blockArity;
        $this->paramNames = $paramNames;
    }

    public function getArity(): int
    {
        return $this->blockArity;
    }
    public function getBody(): \DOMElement
    {
        return $this->body ?? throw new OtherRuntimeException("Block does not have a body.");
    }

    /** @return array<string> */
    public function getParamNames(): array
    {
        return $this->paramNames;
    }

    public function getSelfRefId(): int
    {
        return $this->selfRefId;
    }

    /** @param array<string> $paramNames */
    public function setParamNames(array $paramNames): void
    {
        $this->paramNames = $paramNames;
    }

    public function setSelfRefId(int $selfRefId): void
    {
        $this->selfRefId = $selfRefId;
    }
}
