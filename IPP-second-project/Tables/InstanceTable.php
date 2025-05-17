<?php

namespace IPP\Student\Tables;

use IPP\Student\Exceptions\DoNotUnderstandException;
use IPP\Student\Interpreter;
use IPP\Student\Models\Attribute;
use IPP\Student\Models\Block;
use IPP\Student\Models\Instance;
use IPP\Student\Models\Method;
use IPP\Student\SingletonInstances;

/** @implements TableInterface<int, Instance> */
class InstanceTable implements TableInterface
{
    /** @var array<int, Instance> */
    private array $table = [];
    private int $id = 1;
    private ClassTable $classTable;
    private Interpreter $interpreter;

    public function __construct(ClassTable $classTable, Interpreter $interpreter)
    {
        $this->classTable = $classTable;
        $this->initializeInstanceTable();
        $this->interpreter = $interpreter;
    }

    /**
     * @param int $name
     * @param Instance $item
     */
    public function addItem(mixed $name, mixed $item): void
    {
        if (isset($this->table[$name])) {
            throw new \RuntimeException("Class '$name' already exists.");
        }
        $this->table[$name] = $item;
    }

    public function addInstance(string $typeOfInstance, mixed $value = null): int
    {
        $methods = $this->classTable->getMethodNamesFromClass($typeOfInstance);

        // if the new instance is a block, add the value method with the body to execute
        if ($typeOfInstance === "Block" && $value instanceof Block) {
            $valueMethodName = $this->generateValueMethodFromArity($value->getArity());
            $methods[] = $valueMethodName;
            $methods[] = "whileTrue:";

            // get the methods with its bodies to store to the class table
            $methodsToClassTable[] = new Method($valueMethodName, $value->getBody());
            $methodsToClassTable[] = new Method("whileTrue:", null, true);
            $this->classTable->addUserDefinedClass("Block" . $this->id, $methodsToClassTable, "Object");

            // create the instance
            $instance = new Instance($typeOfInstance, $methods, $value);
            $this->addItem($this->id, $instance);
            return $this->id++;
        }

        $instance = new Instance($typeOfInstance, $methods, $value);
        $this->addItem($this->id, $instance);
        return $this->id++;
    }

    public function removeInstance(int $id): void
    {
        if (!isset($this->table[$id])) {
            throw new \RuntimeException("Instance with ID '$id' does not exist.");
        }
        unset($this->table[$id]);
    }

    public function addAttribute(int $id, Attribute $attribute): void
    {
        if (!isset($this->table[$id])) {
            throw new \RuntimeException("Instance with ID '$id' does not exist.");
        }
        $this->table[$id]->addAttribute($attribute);
    }

    public function getAttribute(int $id, string $name): ?Attribute
    {
        if (!isset($this->table[$id])) {
            throw new \RuntimeException("Instance with ID '$id' does not exist.");
        }
        $instance = $this->getInstanceFromTheTable($id);
        if (isset($instance->getAttributes()[$name])) {
            $attribute = $instance->getAttributes()[$name];
            return $attribute->getInstanceId() === $id ? $attribute : null;
        }
        return null;
    }

    public function getInstanceFromTheTable(int $id): Instance
    {
        if (!isset($this->table[$id])) {
            throw new \RuntimeException("Instance with ID '$id' does not exist.");
        }
        return $this->table[$id];
    }

    private function initializeInstanceTable(): void
    {
        // these instances are singletons
        $this->addInstance("True", true);
        $this->addInstance("False", false);
        $this->addInstance("Nil", "nil");
    }

    public function getSingletonId(string $nameOfInstance): ?int
    {
        return match ($nameOfInstance) {
            "True" => SingletonInstances::True->value,
            "False" => SingletonInstances::False->value,
            "Nil" => SingletonInstances::Nil->value,
            default => null,
        };
    }


    public function getMethodByName(int $mainInstanceId, string $string, bool $superLookUpMode): ?Method
    {
        if (!isset($this->table[$mainInstanceId])) {
            throw new \RuntimeException("Instance with ID '$mainInstanceId' does not exist.");
        }

        $instance = $this->table[$mainInstanceId];

        if ($instance->getClassRefName() === "Block") {
            // the name needs to be corrected for block instances that are references as Block + id of instance
            $className = "Block" . $mainInstanceId;
        } else {
            $className = $instance->getClassRefName();
        }

        $foundMethod = $this->classTable->lookUpMethodByName($className, $string, $superLookUpMode);

        if ($foundMethod === null) {
            // check if there is a possible attribute setter or getter
            if (substr_count($string, ":") == 1) {
                // return the new method with the flag isAttribute
                return new Method($string, null, isAttributeSetter: true);
            } elseif (substr_count($string, ":") == 0) {
                return new Method($string, null, isAttributeGetter: true);
            }

            throw new DoNotUnderstandException("Method '$string' not found in class '$className'.");
        }

        $this->interpreter->setSuperLookUpMode(false);
        return $foundMethod;
    }

    private function generateValueMethodFromArity(int $blockArity): string
    {
        $methodName = "value";
        for ($i = 0; $i < $blockArity; $i++) {
            if ($i === 0) {
                $methodName = $methodName . ":";
            } else {
                $methodName = $methodName . "value:";
            }
        }
        return $methodName;
    }

    public function isInstanceOf(string $className, int $id): bool
    {
        if (!isset($this->table[$id])) {
            throw new \RuntimeException("Instance with ID '$id' does not exist.");
        }

        $instance = $this->getInstanceFromTheTable($id);
        $classRefName = $instance->getClassRefName();

        // check if the class name matches the instance's class reference name
        if ($classRefName === $className) {
            return true;
        }

        // check if the class name is a superclass of the instance's class reference name
        return $this->classTable->isSubclassOf($classRefName, $className);
    }

    public function printTable(): void
    {
        foreach ($this->table as $id => $instance) {
            echo "Instance ID: $id\n";
            echo "Type: " . $instance->getClassRefName() . "\n";
            echo "Methods: " . implode(", ", $instance->getMethods()) . "\n";

            $attributes = $instance->getAttributes();
            if (!empty($attributes)) {
                echo "Attributes:\n";
                foreach ($attributes as $attribute) {
                    $attributeInstance = $this->getInstanceFromTheTable($attribute->getInstanceId());
                    $attributeValue = $attributeInstance->getValue();
                    if (is_string($attributeValue)) {
                        echo "  - Symbol Name: " . $attribute->getName() . "\n";
                        echo "    Instance: " . $attribute->getRefInstanceId() . "\n";
                        echo "    Value: " . $attributeValue . "\n";
                    } else {
                        echo "  - Symbol Name: " . $attribute->getName() . "\n";
                        echo "    Instance: " . $attribute->getRefInstanceId() . "\n";
                        echo "    Value: " . gettype($attributeValue) . "\n";
                    }
                }
            } else {
                echo "Attributes: None\n";
            }

            echo "-------------------------\n";
        }
    }
}
