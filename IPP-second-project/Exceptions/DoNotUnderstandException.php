<?php

namespace IPP\Student\Exceptions;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class DoNotUnderstandException extends IPPException
{
    public function __construct(string $message = "Do not understand exception.", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::INTERPRET_DNU_ERROR, $previous, false);
    }
}
