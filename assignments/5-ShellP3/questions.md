1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The shell ensures that all child processes complete before continuing by calling waitpid() in a loop for each child process created during pipeline execution. This blocks the parent process (the shell) until all child processes have finished executing. If waitpid() were not called, there would be zombie processes or resource leaks.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Closing unused pipe ends avoids deadlocks and resource leaks.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command is implemented as a built-in because it needs to modify the current working directory of the shell process itself. If cd were implemented as an external command: An external command runs in a separate process and changes to its working directory would not affect the parent shell process; forking a new process just to change the directory would be unnecessarily resource-intensive; and additional mechanisms would be needed to communicate the new directory back to the shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Dynamic memory allocation, modify my parse_pipeline() function to dynamically grow the command list as new commands are parsed from the input, and modify execute_pipeline() to dynamically allocate pipes based on the number of commands. Trade-offs to take into consideration includes either complexity (careful handling of memory allocation functions) and performance.
