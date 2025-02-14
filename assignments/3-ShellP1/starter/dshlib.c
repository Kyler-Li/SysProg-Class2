#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

// each command in a stdin w/ pipes
char *token;
char *exe;
char *args;

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
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

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    // input: A "complete" command line input of the user shell AND a pointer to command_list_t
    // output: 0, it matches with 'OK'; this function is essentially a void one with extra features

    // Initialize command list
    memset(clist, 0, sizeof(command_list_t));

    if (*cmd_line == '\0') {
        return WARN_NO_CMDS; 
    }

    // split commands by pipes '|'
    token = strtok(cmd_line, PIPE_STRING);

    while (token != NULL) {
        
        // printf("The cmd_line: %s\n", cmd_line); // cmd_line must ALWAYS be command_one
        // printf("Parsed token: '%s'\n", token);  // debugging line
        // ensure command count does not exceed CMD_MAX
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // trim spaces around command
        trim(token);

        // parse the command
        exe = token;
        args = NULL;

        // Find first space to split exe and args
        char *space_pos = strchr(token, SPACE_CHAR);
        if (space_pos) {
            *space_pos = '\0';
            args = space_pos + 1;
            trim(args);
        }

        // ensure command executable name length is within limit
        if (strlen(exe) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // store executable name
        strcpy(clist->commands[clist->num].exe, exe);
        
        // store arguments as a single string
        if (args && strlen(args) >= ARG_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        if (args) {
            strcpy(clist->commands[clist->num].args, args);
        } else {
            strcpy(clist->commands[clist->num].args, "");
        }

        // Increment command count
        clist->num++;

        // Get next command
        token = strtok(NULL, PIPE_STRING);
    }
    return OK;  // Successfully parsed
}
