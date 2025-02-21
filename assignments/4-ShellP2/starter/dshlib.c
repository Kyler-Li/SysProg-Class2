#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

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

/* DO NOT FORGET TO IMPLEMENT THESE FUNCTIONS!!!
int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);

and these too:
//built in command stuff
typedef enum {
    BI_CMD_EXIT,
    BI_CMD_DRAGON,
    BI_CMD_CD,
    BI_NOT_BI,
    BI_EXECUTED,
    BI_RC,
} Built_In_Cmds;
Built_In_Cmds match_command(const char *input); 
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd);

//main execution context
int exec_local_cmd_loop();
int exec_cmd(cmd_buff_t *cmd);

//output constants
#define CMD_OK_HEADER       "PARSED COMMAND LINE - TOTAL COMMANDS %d\n"
#define CMD_WARN_NO_CMD     "warning: no commands provided\n"
#define CMD_ERR_PIPE_LIMIT  "error: piping limited to %d commands\n"

*/

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) 
{
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd->argv[0], "exit") == 0) {
        return BI_CMD_EXIT;
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc < 2) {
            return BI_CMD_CD; // Do nothing if no argument
        }
        if (chdir(cmd->argv[1]) != 0) {
            perror("cd");
            return BI_RC;
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

int exec_local_cmd_loop()
{
    char cmd_line[SH_CMD_MAX];
    int rc = 0;
    cmd_buff_t cmd;

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

        rc = build_cmd_buff(cmd_line, &cmd);
        if (rc == WARN_NO_CMDS) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }
        else if (rc != OK) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }

        // (DONE) TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
        // the cd command should chdir to the provided directory; if no directory is provided, do nothing

        Built_In_Cmds bi_result = exec_built_in_cmd(&cmd);
        if (bi_result == BI_CMD_EXIT) {
            break;
        } 

        // (DONE) TODO IMPLEMENT if not built-in command, fork/exec as an external command
        // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
        if (bi_result != BI_NOT_BI) {
            continue;
        }

        // IMPLEMENT THE REST OF THE REQUIREMENTS
        // execute external command 
        rc = exec_cmd(&cmd);
        if (rc != OK) {
            printf("%d\n", ERR_EXEC_CMD);
        }

    }

    return OK;
}