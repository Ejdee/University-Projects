<?php

namespace IPP\Student\Exceptions;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class WrongArgumentException extends IPPException
{
    public function __construct(string $message = "Invalid argument.", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::INTERPRET_VALUE_ERROR, $previous, false);
    }
}
