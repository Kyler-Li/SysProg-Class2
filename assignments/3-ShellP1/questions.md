1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  Because it safely reads user input while preventing buffer overflows by limiting input to a specified length (SH_CMD_MAX).

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() ensures that cmd_buff is allocated on the heap instead of the stack. Since malloc() provides dynamic memory allocation, it gives the potential resizing (realloc()), making the shell more flexible for handling larger inputs.

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces ensures that commands are correctly parsed and executed without unintended errors.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Output Redirection (>) It processes the output of a command to a file instead of a terminal (ls > output.txt). The challenge is Handling file permissions and ensuring that redirection does not overwrite important files. The Input Redirection (<) It processes and takes input from a text file instead of the keyboard. The challenge is detecting and properly handling file read errors, like missing files or lack of permissions. There's also error redirection (2>).

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection modifies where input and output go (e.g., files), whereas piping connects the output of one command to the input of another.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Because keeping STDERR separate ensures that error messages do not mix with regular output. That way, users can process command outputs without being affecting by error messages.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: The shell should detect non-zero exit statuses and notify the user of failures.