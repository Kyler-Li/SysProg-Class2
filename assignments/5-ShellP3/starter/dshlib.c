#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

char *token;
char *exe;
char *args;

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

 // Local prototype functions
void trim(char *str);
int parse_pipeline(char *cmd_line, command_list_t *clist);
// Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd);
// int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);
// int exec_cmd(cmd_buff_t *cmd);
// int exec_local_cmd_loop();
// int execute_pipeline(command_list_t *clist);

// ASSIGNMENT 3
void trim(char *str) {
    char *start = str;
    while ((unsigned char)*start == SPACE_CHAR) {
        start++;
    }

    char *end = start + strlen(start) - 1;
    while (end > start && *end == SPACE_CHAR) {
        end--;
    }

    *(end + 1) = '\0'; // Null-terminate after the last non-space character

    memmove(str, start, end - start + 2); // Shift the string to the beginning
}


 // ASSIGNMENT 4
 // if a command isn't built in, use exec
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) 
{ 
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd->argv[0], "exit") == 0) {
        printf("exiting...\n");
        return BI_CMD_EXIT;
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc < 2) {
            return BI_CMD_CD; // Do nothing if no argument
        }
        printf("Chaging directory to: %s\n", cmd->argv[1]);
        if (chdir(cmd->argv[1]) != 0) {
            perror("cd");
            return BI_NOT_BI;
        }
        return BI_CMD_CD;
    }

    return BI_NOT_BI;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    char *cmd_ptr = cmd_line;
    
    // Trim leading spaces
    while (isspace((unsigned char)*cmd_ptr)) cmd_ptr++;

    char *arg_start = NULL;
    int in_quotes = 0;

    cmd_buff->_cmd_buffer = strdup(cmd_ptr);  // Allocate buffer for parsed input
    char *p = cmd_buff->_cmd_buffer;

    while (*p) {
        if (*p == '"') {
            in_quotes = !in_quotes; // Toggle quote state
            memmove(p, p + 1, strlen(p)); // Remove quote
            continue;
        } 
        if (isspace((unsigned char)*p) && !in_quotes) {
            *p = '\0';  // End current argument
            if (arg_start) {
                cmd_buff->argv[cmd_buff->argc++] = arg_start;
                arg_start = NULL;
            }
        } else if (!arg_start) {
            arg_start = p;  // Start of new argument
        }
        p++;
    }

    // Capture last argument if it exists
    if (arg_start) {
        cmd_buff->argv[cmd_buff->argc++] = arg_start;
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;  // Null-terminate argv

    if (cmd_buff->argc > 0) {
        return OK;
    }
    
    return WARN_NO_CMDS;
}

int exec_cmd(cmd_buff_t *cmd)
{
    pid_t pid = fork();
    int status;

    if (pid < 0) {
        perror("fork failed");
        return ERR_MEMORY;
    } else if (pid == 0) { // Child
        execvp(cmd->argv[0], cmd->argv);
        perror("exec failed");
        exit(ERR_EXEC_CMD);
    } else { // Parent
        waitpid(pid, &status, 0);
    }

    return OK;
}

int parse_pipeline(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    char *saveptr;
    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);

    while (token != NULL && clist->num < CMD_MAX) {
        // Trim leading and trailing spaces from the token
        trim(token);

        // Parse the command into a cmd_buff_t structure
        if (build_cmd_buff(token, &clist->commands[clist->num]) != OK) {
            return WARN_NO_CMDS;
        }

        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }

    if (clist->num > CMD_MAX) {
        return ERR_TOO_MANY_COMMANDS;
    }

    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_commands = clist->num;
    int pipefds[2 * (num_commands - 1)];
    pid_t pids[num_commands];

    // create pipes
    for (int i = 0; i < num_commands; i++) {
        if (pipe(pipefds + 2 * i) < 0) {
            perror("pipe failed");
            return ERR_MEMORY;
        }
    }

    // fork and execute command
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("Fork failed");
            return ERR_MEMORY;
        } else if (pids[i] == 0) { // child
            // redirect input
            if (i > 0) {
                dup2(pipefds[2 * (i - 1)], STDIN_FILENO);
            }

            // redirect output
            if (i < num_commands - 1) {
                dup2(pipefds[2 * i + 1], STDOUT_FILENO);
            }

            // close all pipe file descriptors
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]);
            }

            // execute command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("exec failed");
            exit(ERR_EXEC_CMD);
        }
    }

    // close all pipe files descriptors in parent
    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipefds[i]);
        // close(pipefds[i * 2 + 1]);
    }

    // wait for all children
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}

int exec_local_cmd_loop()
{
    char cmd_line[SH_CMD_MAX];
    int rc = 0;
    command_list_t clist;

    // (DONE) TODO IMPLEMENT MAIN LOOP 
    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break; // Handle EOF
        }

        cmd_line[strcspn(cmd_line, "\n")] = '\0';
        // printf("The cmd_line: '%s'\n", cmd_line); // debugging statement

        if (strlen(cmd_line) == 0) {
            continue;
        }

        if (strcmp(cmd_line, EXIT_CMD) == 0) {
            printf("exiting...\n");
            break;
        }

        rc = parse_pipeline(cmd_line, &clist);

        if (rc == WARN_NO_CMDS) {
            printf("%s\n", CMD_WARN_NO_CMD);
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        } else if (rc == OK) {
            bool has_built_in = false;
            for (int i = 0; i < clist.num; i++) {
                Built_In_Cmds bi_result = exec_built_in_cmd(&clist.commands[i]);
                if (bi_result == BI_CMD_EXIT) {
                    return OK;
                } else if (bi_result != BI_NOT_BI) {
                    has_built_in = true;
                }
            }
            if (!has_built_in) {
                execute_pipeline(&clist);
            }
        }

    }

    return OK;
}
