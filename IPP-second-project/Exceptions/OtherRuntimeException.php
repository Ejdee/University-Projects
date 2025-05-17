<?php

namespace IPP\Student\Exceptions;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class OtherRuntimeException extends IPPException
{
    public function __construct(string $message = "Other runtime error.", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::INTERPRET_TYPE_ERROR, $previous, false);
    }
}
