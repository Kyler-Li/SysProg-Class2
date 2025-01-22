#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    // buff: pointer to buffer
    // user_str: pointer to the raw, string input
    // len: 50 spaces of chars
    // Input: Takes in the internal buffer, supplied string, and len of buff
    // Output: Returns an integer?
    //TODO: #4:  Implement the setup buff as per the directions
    int str_len = 0;
    char *src_ptr = user_str;
    char char_space = ' ';
    char *dest_ptr = buff;
    // int prev_char = 0;
    int last_white = 0; // if last char of string is a whitespace

    while (*src_ptr == char_space || *src_ptr == '\t') {
        src_ptr++; // Move past leading spaces or tabs
    }

    while (*src_ptr != '\0') {
        if (str_len >= len) {
            return -1; // user-supplied string is too large
        }
        // handle non-whitespace chars
        if (*src_ptr != char_space && *src_ptr != '\t') {
            *dest_ptr = *src_ptr; // copying the chars per direction
            dest_ptr++;
            str_len++;
            last_white = 0;
        } else { // handle whitespace chars
            if (last_white == 0) {
                *dest_ptr = char_space; // Include only a single space to prevent repeated ws
                dest_ptr++;
                str_len++;
                last_white = 1;
            }
            
        }
        src_ptr++;
        
    }

    if (str_len > 0 && *(dest_ptr - 1) == char_space) {
        dest_ptr--; 
        str_len--;
    }

    int final_slen = str_len;

    while (str_len < len) { // fill the remaining buffer with periods
        *dest_ptr = '.';
        dest_ptr++;
        str_len++;
    }

    return final_slen;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(buff[i]);
    }
    printf("]");
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){ // Purpose: count words except spaces
    // input: a line of text modified from set_up buff
    // output: an integer indicating the number of words counted
    // len: buffer length, str_len: string length, buff: the buffer input
    //YOU MUST IMPLEMENT
    int wc = 0;                    // counts words
    int word_start = 0;               // if you're currently in a word
    char space_char = ' ';         // whitespace

    if (str_len > len) {
        printf("Error: String length exceeds buff length.");
        exit(1);
    }
    
    for (int i = 0; i < str_len; i++) { 
        char curr_char = buff[i]; 

        // for every word (start of one)
        if (!word_start && curr_char != space_char) {
            wc++;
            word_start = 1;
        } else {
            if (curr_char == space_char) {
                word_start = 0;
            }      
        }
    }
    return wc;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void reverse_string(char *buff, int len){ // for -r command
    // 1
    int start_idx = 0;
    int end_idx = len - 1; 

    while (start_idx < end_idx) { // 2
        char tmp_char = buff[start_idx];
        buff[start_idx] = buff[end_idx]; // 2a
        buff[end_idx] = tmp_char;
        start_idx++; // 2b
        end_idx--; // 2c
    }
}

void word_print(char *buff) {
    //suggested local variables
    int len = strlen(buff);
    int last_char_idx = len-1; // index of last char - len-1;
    int wc = 0;                         // counts words
    int wlen = 0;                       // length of current word
    int word_start = 0;            // am I at the start of a new word
    char space_char = ' ';

    // input: a line of text w/ or w/o spaces
    // output: an integer indicating the number of words counted
    
    for (int i = 0; i < len; i++) { 
        char curr_char = buff[i]; 

        // for every word (start of one)
        if (!word_start && curr_char != space_char) {
            wc++;
            printf("%d. ", wc);
            word_start = 1;
            wlen = 0;
        }

        // if we're in a word
        if (word_start && (curr_char != space_char)) {
            printf("%c", curr_char);
            wlen++;
        }

        if ((word_start && curr_char == space_char) || 
        (word_start && i == last_char_idx)) { // 2
            printf("(%d)\n", wlen);
            word_start = 0;
            wlen = 0;
        }
    }
    printf("\nNumber of words returned: %d\n", wc);
}

void word_replace(char *buff, char old_string, char new_string, int str_len); // -x
    // int word_start = 0;         // initialized to false
    // char space_char = ' ';
    // char old_s = old_string;
    // char new_s = new_s;

    // for (int i = 0; i < BUFFER_SZ; i++) {
    //     char curr_char = buff[i];

    //     if (word_start && (curr_char != space_char)) { // In a new word
    //         printf("%c", curr_char);
    //     }

    // }
// }

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    // char *old_string;       // former for the -x function
    // char *new_string;       // replacer for the -x function

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      This is safe because it first checks whether the arguments
    //      in the cli is less than 2 which implies that this conditional
    //      requires at least 2 arguments with a hyphen in between
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //(DONE) TODO:  #2 Document the purpose of the if statement below
    //      If the number of cli arguments is less than 3, it
    //      calls the usage function to display the correct
    //      usage and exits.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //(DONE) TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ);
    if (buff == NULL) {
        printf("Memory allocation failed.");
        exit(1);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    // printf("Test: %d\n", user_str_len);
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //(DONE) TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff, user_str_len);
            // printf("%s\n", buff);
            break;
        case 'w':
            printf("Word Print\n----------\n");
            word_print(input_string);
            break;
        // case 'x':
        //     if (argc < 5) {
        //         printf("Error: -x option requires old and new words as arguments.\n");
        //         usage(argv[0]);
        //         exit(1);
        //     }
        //     old_string = argv[3];  // Old character to replace
        //     new_string = argv[4];  // New character to replace with

        //     word_replace(buff, old_string, new_string, user_str_len);
        //     print_buff(buff, BUFFER_SZ);
        //     break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //(DONE) TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//(DONE) TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          Providing both the pointer to the buffer and the length
//          as parameters is a good practice because it enhances the 
//          function’s flexibility and safety.