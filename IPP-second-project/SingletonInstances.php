<?php

namespace IPP\Student;

enum SingletonInstances: int
{
    case NonDefined = 0;
    case True = 1;
    case False = 2;
    case Nil = 3;
}
