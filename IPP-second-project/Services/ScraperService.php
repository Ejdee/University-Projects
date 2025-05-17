<?php

namespace IPP\Student\Services;

use IPP\Student\Tables\ClassTable;
use IPP\Student\Models\Method;

class ScraperService
{
    private ClassTable $classTable;
    private \DOMDocument $root;

    public function __construct(ClassTable $table, \DOMDocument $root)
    {
        $this->classTable = $table;
        $this->root = $root;
    }

    public function scrape(): void
    {
        // add other methods to the method table and class table
        $classNodes = $this->root->getElementsByTagName("class");
        foreach ($classNodes as $classNode) {
            $className = $classNode->getAttribute("name");
            $parentName = $classNode->getAttribute("parent");

            $methodNodes = $classNode->getElementsByTagName("method");

            $methods = [];
            foreach ($methodNodes as $methodNode) {
                // Get the name and the body of the method
                $methodName = $methodNode->getAttribute("selector");
                $methodBody = $methodNode->firstElementChild;

                $methods[] = new Method($methodName, $methodBody);
            }

            $this->classTable->addUserDefinedClass($className, $methods, $parentName);
        }
    }
}
