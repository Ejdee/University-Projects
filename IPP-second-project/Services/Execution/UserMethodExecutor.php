<?php

namespace IPP\Student\Services\Execution;

use IPP\Student\Exceptions\DoNotUnderstandException;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Exceptions\WrongArgumentException;
use IPP\Student\Facades\ExecutionFacade;
use IPP\Student\Helpers\ContextStack;
use IPP\Student\Models\Attribute;
use IPP\Student\Models\Block;
use IPP\Student\Models\Method;
use IPP\Student\Tables\InstanceTable;

class UserMethodExecutor
{
    private ContextStack $contextStack;
    private InstanceTable $instanceTable;
    private ExecutionFacade $executionFacade;

    public function __construct(ContextStack $contextStack, InstanceTable $instanceTable)
    {
        $this->contextStack = $contextStack;
        $this->instanceTable = $instanceTable;
    }

    public function setExecutionFacade(ExecutionFacade $executionFacade): void
    {
        $this->executionFacade = $executionFacade;
    }

    /** @param array<int> $args */
    public function execute(Method $method, array $args, int $receiverId): int
    {
        $result = 0;
        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiverId);

        $this->contextStack->addContext($receiverId);

        // execute the method based on its type
        if ($method->isBuiltIn()) {
            $result = $this->executionFacade->executeBuiltInMethod($method, $args, $receiverId);
        } elseif ($receiverInstance->getClassRefName() === "Block") {
            $blockItem = $receiverInstance->getValue();
            if (!$blockItem instanceof Block) {
                throw new OtherRuntimeException("Block item is not an instance of Block.");
            }
            $result = $this->executionFacade->executeBlock($blockItem->getBody(), $args, $blockItem->getSelfRefId());
        } elseif ($method->isAttributeSetter()) {
            $result = $this->setInstantiateAttribute($receiverId, $args, $method);
        } elseif ($method->isAttributeGetter()) {
            $result = $this->getInstantiateAttribute($receiverId, $method);
        } else {
            $result = $this->executionFacade->executeBlock($method->getBody(), $args, $receiverId);
        }

        $this->contextStack->removeContext();

        return $result;
    }

    /** @param array<int> $args */
    private function setInstantiateAttribute(int $receiverId, array $args, Method $method): int
    {
        if (count($args) > 1) {
            throw new WrongArgumentException("Attribute method can only have one argument.");
        }

        $arg = $this->instanceTable->getInstanceFromTheTable($args[0]);

        // extract the name of the attribute - remove the ending ':'
        $nameOfTheAttribute = substr($method->getName(), 0, -1);
        $this->instanceTable->addAttribute($receiverId, new Attribute(
            $nameOfTheAttribute,
            $receiverId,
            $args[0],
        ));

        return $receiverId;
    }

    private function getInstantiateAttribute(int $receiverId, Method $method): int
    {
        $nameOfTheAttribute = $method->getName();
        $instance = $this->instanceTable->getInstanceFromTheTable($receiverId);
        $attribute = $instance->getAttributes()[$nameOfTheAttribute] ?? null;
        if ($attribute) {
            return $attribute->getInstanceId();
        } else {
            throw new DoNotUnderstandException("Attribute '$nameOfTheAttribute' not found - instance '$receiverId'.");
        }
    }
}
