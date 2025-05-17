<?php

namespace IPP\Student\Helpers;

use IPP\Student\Exceptions\OtherRuntimeException;

class ElementSorter
{
    /** @return array<\DOMElement> */
    public static function getAssignsSorted(\DOMElement $blockElement): array
    {
        $assigns = [];
        $assignsOrder = [];
        foreach ($blockElement->childNodes as $child) {
            if ($child instanceof \DOMElement && $child->nodeName === "assign") {
                $assigns[] = $child;
                $assignsOrder[] = $child->getAttribute("order");
            }
        }

        // sort the assignments by the order
        array_multisort($assignsOrder, SORT_ASC, $assigns);

        return $assigns;
    }

    /** @return array<string> */
    public static function getParametersSorted(\DOMElement $blockElement): array
    {
        $arity = $blockElement->getAttribute("arity");
        $paramNames = [];
        $paramOrder = [];
        $tmpParam = $blockElement->firstElementChild;
        while ($tmpParam !== null) {
            if ($tmpParam->nodeName === "parameter") {
                $paramNames[] = $tmpParam->getAttribute("name");
                $paramOrder[] = $tmpParam->getAttribute("order");
            }
            $tmpParam = $tmpParam->nextElementSibling;
        }

        // check if the number of parameters is equal to the arity
        if ($arity != count($paramNames)) {
            throw new OtherRuntimeException("Block arity does not match the number of arguments.");
        }

        // sort the parameters by the order
        array_multisort($paramOrder, SORT_ASC, $paramNames);

        return $paramNames;
    }

    /** @return array<\DOMElement> */
    public static function getSendElementsSorted(\DOMElement $sendElement): array
    {
        // @var array<int> $args
        $args = [];
        // @var array<int> $argsOrder
        $argsOrder = [];

        foreach ($sendElement->childNodes as $child) {
            if ($child instanceof \DOMElement && $child->nodeName === "expr") {
                // expression will be always in the first place
                $argsOrder[] = 0;
                $args[] = $child;
            }
            if ($child instanceof \DOMElement && $child->nodeName === "arg") {
                $argsOrder[] = $child->getAttribute("order");
                $args[] = $child;
            }
        }

        // sort the args according to the order attribute
        array_multisort($argsOrder, SORT_ASC, $args);

        return $args;
    }
}
