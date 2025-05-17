<?php

namespace IPP\Student\Tables;

use IPP\Student\Models\ClassModel;
use IPP\Student\Models\Method;

/** @implements TableInterface<string, ClassModel> */
class ClassTable implements TableInterface
{
    /** @var array<string, ClassModel> */
    private array $table = [];

    public function __construct()
    {
        $this->initializeBuiltInClasses();
    }

    private function initializeBuiltInClasses(): void
    {
        $this->addBuiltInClass("Object", null, [
            'identicalTo:', 'equalTo:', 'asString', 'isNumber', 'isString', 'isBlock', 'isNil', 'new', 'from:'
        ]);
        $this->addBuiltInClass("Nil", "Object", [
            'asString'
        ]);
        $this->addBuiltInClass("Integer", "Object", [
            'equalTo:', 'greaterThan:', 'plus:', 'minus:', 'multiplyBy:', 'divBy:', 'asString', 'asInteger',
            'timesRepeat:'
        ]);
        $this->addBuiltInClass("String", "Object", [
            'read', 'print', 'equalTo:', 'asString', 'asInteger', 'concatenateWith:', 'startsWith:', 'endsBefore:',
            'startsWith:endsBefore:'
        ]);
        $this->addBuiltInClass("Block", "Object", [
            'whileTrue:',
        ]);
        $this->addBuiltInClass("True", "Object", [
            'not', 'and:', 'or:', 'ifTrue:', 'ifFalse:', 'ifTrue:ifFalse:'
        ]);
        $this->addBuiltInClass("False", "Object", [
            'not', 'and:', 'or:', 'ifTrue:', 'ifFalse:', 'ifTrue:ifFalse:'
        ]);
    }

    /**
     * @param string $name
     * @param ClassModel $item
     */
    public function addItem(mixed $name, mixed $item): void
    {
        if (isset($this->table[$name])) {
            throw new \RuntimeException("Class '$name' already exists.");
        }

        $this->table[$name] = $item;
    }

    public function classExists(string $name): bool
    {
        return isset($this->table[$name]);
    }

    /** @param array<string> $methods */
    public function addBuiltInClass(string $name, ?string $parent = null, array $methods = []): void
    {
        $methodsToAdd = [];
        foreach ($methods as $method) {
            $methodsToAdd[] = new Method($method, null, true);
        }

        $classInfo = new ClassModel($name, $parent, $methodsToAdd);
        $this->addItem($name, $classInfo);
    }

    /** @param array<Method> $methods */
    public function addUserDefinedClass(string $name, array $methods, ?string $parent = null): void
    {
        if (isset($this->table[$name])) {
            throw new \RuntimeException("Class '$name' already exists.");
        }

        $classInfo = new ClassModel($name, $parent, $methods);
        $this->addItem($name, $classInfo);
    }

    /** @return array<string> */
    public function getMethodNamesFromClass(string $typeOfInstance): array
    {
        $methods = [];

        if (!isset($this->table[$typeOfInstance])) {
            return $methods;
        }

        $tmpClass = $typeOfInstance;

        // look for the method in the current class and its parent classes
        while ($tmpClass) {
            $methodsFromTheClass = $this->table[$tmpClass]->getMethods();
            foreach ($methodsFromTheClass as $method) {
                $methods[] = $method->getName();
            }
            $tmpClass = $this->table[$tmpClass]->getParentName();
        }

        return $methods;
    }

    public function lookUpMethodByName(string $className, string $methodName, bool $superLookUpMode): ?Method
    {
        $tmpClass = $className;
        if ($superLookUpMode) {
            // look for the method in the parent classes
            $tmpClass = $this->table[$className]->getParentName();
        }
        // look for the method in the current class and its parent classes
        while ($tmpClass) {
            $methods = $this->table[$tmpClass]->getMethods();
            foreach ($methods as $method) {
                if ($method->getName() === $methodName) {
                    return $method;
                }
            }
            $tmpClass = $this->table[$tmpClass]->getParentName();
        }
        return null;
    }

    public function isSubclassOf(string $classRefName, string $className): bool
    {
        if (!isset($this->table[$classRefName])) {
            return false;
        }

        $currentClass = $classRefName;

        // check if the class name matches the instance's class reference name
        if ($currentClass === $className) {
            return true;
        }

        // check if the class name is a superclass of the instance's class reference name
        while ($currentClass) {
            if ($currentClass === $className) {
                return true;
            }
            $currentClass = $this->table[$currentClass]->getParentName();
        }

        return false;
    }

    public function printTable(): void
    {
        foreach ($this->table as $name => $item) {
            echo "Name: $name, ";
            echo "Type: Class, ";
            echo "Parent: " . ($item->getParentName() ?? 'null') . ", ";
            echo "Methods: " . implode(", ", array_map(fn($method) => $method->getName(), $item->getMethods())) . ", ";
            echo "\n------------------\n";
        }
    }
}
