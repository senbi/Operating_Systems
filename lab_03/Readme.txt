 Creating a program Just Another Shell (JASH), an interactive shell.
 
This shell will expose the file system to the user and the user can run executable files present in the file system. 
The final version would support interactive interface, running batch files, parallel and sequential executions,
background processes, input/output redirection, piping and cron.

Interactive Operation
After startup, jash should perform the following actions in a loop:
1. Print a prompt consisting of a $ sign followed by a space.
2. Read a line from standard input.
3. Lexically analyse the line to form an array of tokens.
4. Syntactically analyse (i.e. parse) the token to form a command.
5. Execute the command(s) or schedule their execution.


Execution of Commands

A command can be either a file name which needs to be executed or a built-in command. The
function execute_command(…) is intended for this purpose. It takes the tokens from the parser
and based on the type of command proceeds to perform the necessary actions.

File Executions

Any command that is not a built-in command should be assumed to be a file that has to be
executed. You have to search for the file name in the current directory and the directories
mentioned in the PATH variable inherited from the parent shell. Look for execv variants that can do
this. Display an error message if the file is not present in any of the directories. All the tokens
following the file name are to be passed to the executable file as arguments.
Currently you are to not support background execution of processes (i.e. commands ending in
ampersand `&'). All child processes forked by jash should run in the foreground. However, the user
must be able to kill the current child process by sending it a SIGINT (Ctrl+C) signal. SIGINT should
not kill jash itself.

Built-in Commands

1. Change Directory (cd)
Syntax: cd <absolute/relative path>
‘cd’ takes one argument which is the absolute or relative path of the directory you wish to switch to.
Display an error message if the directory does not exist. (Read about chdir for more.)

2. Batch File Execution (run)
Syntax: run <file name>
Here <file name> gives the complete relative or absolute path to the file too.
‘run’ takes one argument which would be a batch file name containing a list of commands
separated by newline. Your shell program should create a child process for the ‘run’ command
instance and the this should then execute the commands one by one.
If one of them executes erroneously you should print the error and continue executing the
subsequent commands. Once all commands are executed, the child process (created for the ‘run’
instance) without exiting the shell (parent process).

3. Parallel Execution (parallel)

Syntax: parallel <command 1> ::: <command 2> ::: … ::: <command n>
‘parallel’ executes commands concurrently, or in parallel. Each command is of the form of one
these mentioned here. Each command in the given list of n commands (n > 0) is to be executed in
parallel in separate child processes.
Example: parallel ls ::: pwd ::: echo abc
In this, three child processes must be created to execute each of the three commands.

4. Sequential Execution (sequential)
Syntax: sequential <command 1> ::: <command 2> ::: … ::: <command n>
‘sequential’ executes commands sequentially, and in a way of the && operator in the bash or other
shells. It is a lazy implementation of the boolean AND operator.
More precisely, it executes the commands sequentially until any command produces an error, in
which case it does not proceed to the next command.
Example 1: sequential ls ::: pwd ::: echo abc
In this, each of the three processes will be executed sequentially.
Example 2: sequential ls ::: invalid ::: echo abc
In this case, since ‘invalid’ is not a valid command, the command(s) after ‘invalid’ (in this case,
‘echo abc’) will not be executed.

5. Exit
‘exit’ command exits the jash program immediately. jash should also exit on receiving the EOF (or
Ctrl+D) character on input. (Just like your bash does.). Other than this, you should ensure that
other signals such as SIGINT are caught appropriately. If any child process is running in the
foreground Ctrl+C or Ctrl+\ should kill it, but should not kill the jash program (parent process). You
will have to design appropriate signal handlers for SIGINT (Ctrl+C), SIGQUIT (Ctrl+\) and any other
signals you are using.

6. File Executions and Error
Any command not of the other forms is a file execution. If the file does not exist the command is to
be treated as invalid. In such a case, an error is to be printed and the whole input line is to be
discarded. Print all error messages to STDERR stream only.

7.  Sequential Execution (sequential_or)
Syntax: sequential_or <command 1> ::: <command 2> ::: … ::: <command n>
‘sequential_or’ executes commands sequentially, and in a way of the || operator in the bash or
other shells. It is a lazy implementation of the boolean OR operator.
More precisely, it executes the commands sequentially until any command does not produce an
error, in which case it does not proceed to the next command. As soon as a command executes
successfully, further processes are not executed.
Example 1: sequential ls ::: echo abc
In this case, if ‘ls’ executes successfully (it most likely will) ‘echo abc’ will not be executed.
Example 2: sequential invalid ::: echo abc
In this case, since ‘invalid’ is not a valid command, ‘echo abc’ will be executed.

8. Combined Commands
Combined commands means two or more built-in commands put together. You will get additional
credit if your program can handle such cases.
Example 1: parallel ls ::: run run_file.txt ::: pwd
Example 2: sequential cd test ::: pwd ::: ls
Example 3: “run” or “parallel” or “sequential” commands within a batch file provided to “run”
