<?php

namespace IPP\Student\Services\Execution;

use IPP\Student\BuiltInCommands\AndCommand;
use IPP\Student\BuiltInCommands\ArithmeticOperationCommand;
use IPP\Student\BuiltInCommands\AsIntegerCommand;
use IPP\Student\BuiltInCommands\AsStringCommand;
use IPP\Student\BuiltInCommands\ConcatenateWithCommand;
use IPP\Student\BuiltInCommands\EqualToCommand;
use IPP\Student\BuiltInCommands\FromCommand;
use IPP\Student\BuiltInCommands\GreaterThanCommand;
use IPP\Student\BuiltInCommands\IdenticalToCommand;
use IPP\Student\BuiltInCommands\IfTrueIfFalseCommand;
use IPP\Student\BuiltInCommands\IsBlockCommand;
use IPP\Student\BuiltInCommands\IsNumberCommand;
use IPP\Student\BuiltInCommands\IsNilCommand;
use IPP\Student\BuiltInCommands\IsStringCommand;
use IPP\Student\BuiltInCommands\NewCommand;
use IPP\Student\BuiltInCommands\NotCommand;
use IPP\Student\BuiltInCommands\OrCommand;
use IPP\Student\BuiltInCommands\PrintCommand;
use IPP\Student\BuiltInCommands\ReadCommand;
use IPP\Student\BuiltInCommands\StartsWithEndsBeforeCommand;
use IPP\Student\BuiltInCommands\TimesRepeatCommand;
use IPP\Student\BuiltInCommands\WhileTrueCommand;
use IPP\Student\Exceptions\DoNotUnderstandException;
use IPP\Student\Exceptions\OtherRuntimeException;
use IPP\Student\Facades\ExecutionFacade;
use IPP\Student\Helpers\ContextStack;
use IPP\Student\Interfaces\IBuiltInMethodCommand;
use IPP\Student\Interpreter;
use IPP\Student\Models\Block;
use IPP\Student\Models\Method;
use IPP\Student\Tables\InstanceTable;

class BuiltInMethodExecutor
{
    private Interpreter $interpreter;
    private InstanceTable $instanceTable;
    private ContextStack $contextStack;
    private ExecutionFacade $executionFacade;

    /** @var array<string, IBuiltInMethodCommand> */
    private array $commands = [];

    public function __construct(Interpreter $interpreter, InstanceTable $instanceTable, ContextStack $contextStack)
    {
        $this->interpreter = $interpreter;
        $this->instanceTable = $instanceTable;
        $this->contextStack = $contextStack;
        $this->registerCommands();
    }

    public function setExecutionFacade(ExecutionFacade $executionFacade): void
    {
        $this->executionFacade = $executionFacade;
    }

    private function registerCommands(): void
    {
        $this->commands['print'] = new PrintCommand($this->interpreter, $this->instanceTable);
        $this->commands['plus:'] = new ArithmeticOperationCommand($this->instanceTable, '+');
        $this->commands['minus:'] = new ArithmeticOperationCommand($this->instanceTable, '-');
        $this->commands['multiplyBy:'] = new ArithmeticOperationCommand($this->instanceTable, '*');
        $this->commands['divBy:'] = new ArithmeticOperationCommand($this->instanceTable, '/');
        $this->commands['asString'] = new AsStringCommand($this->instanceTable);
        $this->commands['asInteger'] = new AsIntegerCommand($this->instanceTable);
        $this->commands['equalTo:'] = new EqualToCommand($this->instanceTable, $this);
        $this->commands['greaterThan:'] = new GreaterThanCommand($this->instanceTable);
        $this->commands['concatenateWith:'] = new ConcatenateWithCommand($this->instanceTable);
        $this->commands['startsWith:endsBefore:'] = new StartsWithEndsBeforeCommand($this->instanceTable);
        $this->commands['not'] = new NotCommand($this->instanceTable);
        $this->commands['and:'] = new AndCommand($this->instanceTable, $this);
        $this->commands['or:'] = new OrCommand($this->instanceTable, $this);
        $this->commands['ifTrue:ifFalse:'] = new IfTrueIfFalseCommand($this->instanceTable, $this);
        $this->commands['timesRepeat:'] = new TimesRepeatCommand($this->instanceTable, $this);
        $this->commands['new'] = new NewCommand($this->instanceTable);
        $this->commands['from:'] = new FromCommand($this->instanceTable);
        $this->commands['read'] = new ReadCommand($this->instanceTable, $this->interpreter);
        $this->commands['whileTrue:'] = new WhileTrueCommand($this->instanceTable, $this);
        $this->commands['identicalTo:'] = new IdenticalToCommand();
        $this->commands['isNil'] = new IsNilCommand($this->instanceTable);
        $this->commands['isBlock'] = new IsBlockCommand($this->instanceTable);
        $this->commands['isNumber'] = new IsNumberCommand($this->instanceTable);
        $this->commands['isString'] = new IsStringCommand($this->instanceTable);
    }

    /** @param array<int> $args */
    public function execute(Method $method, array $args, int $receiver): int
    {
        $methodName = $method->getName();
        if (isset($this->commands[$methodName])) {
            return $this->commands[$methodName]->execute($receiver, $args);
        } else {
            throw new OtherRuntimeException("Built-in method $methodName does not exist.");
        }
    }

    /** @param array<int> $args */
    public function callArgumentWithMethod(int $receiver, string $method, array $args): int
    {
        $method = $this->instanceTable->getMethodByName($receiver, $method, $this->interpreter->getSuperLookUpMode())
            ?? throw new DoNotUnderstandException("Method $method not found for instance ID: $receiver");

        $receiverInstance = $this->instanceTable->getInstanceFromTheTable($receiver);

        // if the receiver is a block, pass it's self reference to the execution
        if ($receiverInstance->getClassRefName() == "Block") {
            $receiverValue = $receiverInstance->getValue();
            if (!$receiverValue instanceof Block) {
                throw new OtherRuntimeException("Invalid receiver value for block method.");
            }
            return $this->executionFacade->executeBlock($method->getBody(), $args, $receiverValue->getSelfRefId());
        } else {
            if ($method->isAttributeGetter()) {
                return $this->executionFacade->executeUserMethod($method, $args, $receiver);
            }
            return $this->executionFacade->executeBlock(
                $method->getBody(),
                $args,
                $this->contextStack->getCurrentContextId()
            );
        }
    }
}
