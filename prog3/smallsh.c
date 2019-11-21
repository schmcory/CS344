//Author: Cory Schmidt
//Description: 
//ssh schmcory@os1.engr.oregonstate.edu
//To run: gcc -std=c99 -o smallsh smallsh.c; smallsh
//Date: 11/20/2019

//header files 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h> 

#define BUFF_LEN 2048       //max buffer length of 2048 characters
#define MAX_ARGS 512        //max arguments count of 512 

/* --------------------------- Function Prototypes --------------------------- */
void getExitMethod(int method);
void execCmnd(char* [], int, char*, char*, int, int, int*, struct sigaction);
void catchSIGINT(int);
void catchSIGTSTP(int); 
void killPids();

/* ------------------------ Global variables ------------------------ */
pid_t ctrlPid;     
int foreground; 
int pids[64]; 
int numPids = 0;
int background = 0;                 //variable for setting the background to t/f

/*------------------------------------------------------------------------------------*\
*                                                                                      *
*                                                                                      *
*                                                                                      *
\*------------------------------------------------------------------------------------*/
int main() {
    
    /* --------------------------------- Variables --------------------------------- */
    char buffer[BUFF_LEN];              //array holds characters from the command line
    char* argv[MAX_ARGS];               //array holds arguments
    int argc = 0;                       //varible holds argument count
    size_t length;                      //declare length variable
    char* token;                        //variable holds tokenized string
    int pid = getpid();                 //variable calls the getpid function
    char* process;                      //variable holds $$ to change to pid
    int exitMethod = 0;                //variable holds the status of a process
    int i, j;                           //iterator variables
    char* input_file = NULL;            //variable holds input_file name, initialized to NULL
    char* output_file = NULL;           //variable holds output_file name, initialized to NULL
    int input = 0;                      //variable for determining if input_file exists
    int output = 0;                     //variable for determining if output_file exists
    
    //initialize argv array to NULL values; 
    for(i = 0; i < MAX_ARGS; i++)
        argv[i] = NULL; 
    
    /* ------------------------------ End variables ------------------------------ */
    
    /* ----------- Signal handling ----------- */
    //Redirect ^C to catchSIGINT function
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = catchSIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &SIGINT_action, NULL);
    
    //Redirect ^Z to catchSIGTSTP function
    struct sigaction SIGTSTP_action = {0};
    SIGTSTP_action.sa_handler = catchSIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL); 
    /* --------- End signal handling --------- */

    
    /* -------------------------------------- Main driver ------------------------------------ */
    while(1) {
        /* ----------------------------- Get input from the user ----------------------------- */
        while(1) {
            printf(": ");                             //prompt for each new command line

            fflush(stdout);                           //flush output buffer

            fgets(buffer, BUFF_LEN, stdin);           //get input from user

            length = strlen(buffer) - 1;              //variable is set to buffer string length

            //remove newline character from user input
            if(buffer[length] == '\n')
                buffer[length] = '\0';
            
            //printf("%s, %d", buffer, length);       //for testing purposes

            //if user enters nothing
            if(length == 0)
                clearerr(stdin);
            else
                break;                               //else break out of loop with current input 
        }
        /* --------------------------- End Get input from the user --------------------------- */
        
        //replace '$$' with the pid and change int to string
        process = strstr(buffer, "$$");
        if(process) {
            sprintf(process, "%d", pid); 
        }
        
        /* ----------------- Tokenzie input and place arguments into array ----------------- */
        token = strtok(buffer, " ");                  //seperates by white space
        j = 0;                                        //reset iterator variable to 0
    
        //while token is not empty
        while(token != NULL) {
            //set background value is user types if &
            if(strcmp(token, "&") == 0) {
                if(foreground == 0)
                    background = 1;
                else
                    background = 0; 
            }
            
            //if the current index of the array is >, then set the output_file to the next index
            //else 
            else if(strcmp(token, ">") == 0) {
                token = strtok(NULL, " ");            //move the token variable
                output_file = token;                  //set the output_file variable to current token 
                output = 1;                           //set output to 1 (for checking later)
            }
            //if the current index of the array is <, then set the input_file to the next index
            else if(strcmp(token, "<") == 0) {
                token = strtok(NULL, " ");            //reset the token variable
                input_file = token;                   //set the input_file variable to current token
                input = 1; 
            }
            else {    
               argv[j] = token;                       //add each word to the argv array 
               j++;                                   //increment iterator variable to get argc    
            }
            
            token = strtok(NULL, " ");                //reset the token variable 
        }
    
        argv[j] = '\0';                                //set last variable to null terminator
        argc = j;                                     //argc variable set to iterator variable
        
        /* -------------------------------- End handle input -------------------------------- */
        
        
        /* -------------------- for testing purposes ----------------
        //printf("%d\n", argc);             
        for(i = 0; i < argc; i++)
            printf("%s ", argv[i]);
        
        printf("\n");
        
        printf("%d %s %d %s", output, output_file, input, input_file);
        -------------------------------------------------------------- */
    
        /* --------------------------- Handle built in commands --------------------------- */
        /*if(strcmp(argv[argc - 1], "&") == 0) {
            if(foreground == 0)
                    background = 1;
            else
                background = 0;
        }*/
        
        //printf("%d", background);             //for testing purposes 
        
        //if the user input was the # character, continue - this is a comment
        if(strncmp(argv[0], "#", 1) == 0)
            continue; 

        //if the user types exit, exit the shell 
        else if(strcmp(argv[0], "exit") == 0)
            exit(0);

        //if the user types cd
        else if(strcmp(argv[0], "cd") == 0) {
            //if the argument count is less than 2 (just cd)
            if(argc < 2) {
                chdir(getenv("HOME"));  //go to HOME directory
            //else go to directory specified by second argument 
            }else {
                if(chdir(argv[1]) == -1) {
                    printf("Directory path not found.\n");
                    fflush(stdout); 
                }
            }    
        }
                
        //if the user types status, call getExitMethod function
        else if(strcmp(argv[0], "status") == 0)
            getExitMethod(exitMethod);
            
       /* -------------------------- End Handle built in commands ------------------------- */

        /* ------------------------------------------ I/O redirection ------------------------------------------ */
        //if the user types in anything else   
        else {

            execCmnd(argv, argc, input_file, output_file, output, input, &exitMethod, SIGINT_action); 
        }
        /* ---------------------------------------- End I/O redirection ---------------------------------------- */
        
        //Reset array to NULL
        for(i = 0; i < MAX_ARGS; i++)
            argv[i] = NULL; 
        
        background = 0, input = 0, output = 0;         //reset background, input, and output to 0
        char* input_file = NULL;                       //reset input_file variable to NULL
        char* output_file = NULL;                      //reset output_file variable to NULL
    }
    
    free(buffer);                                      //free buffer
    free(token);                                       //free token 
    killPids();                                        //kill all pids

    /* -------------------------------------- End main driver ------------------------------------ */
    
    return 0; 
}

/*------------------------------------------------------------------------------------*\
*   Function: getExitStatus                                                            *
*   ------------------------                                                           *
*                                                                                      *
*   SOURCE: https://www.geeksforgeeks.org/exit-status-child-process-linux/             *
\*------------------------------------------------------------------------------------*/
void getExitMethod(int method) {
    //if exit status
    if(WIFEXITED(method)) {
        printf("exit value %d\n", WEXITSTATUS(method)); 
        fflush(stdout);
    }
    //if terminating signal 
    else {
        printf("terminated by signal %d\n", WTERMSIG(method));
        fflush(stdout); 
    }
}

/*------------------------------------------------------------------------------------*\
*   Function: getExitStatus                                                            *
*   ------------------------                                                           *
*                                                                                      *
*   SOURCE: https://www.geeksforgeeks.org/exit-status-child-process-linux/             *
*   SOURCE: Lecture 3.3                                                                *
\*------------------------------------------------------------------------------------*/
void execCmnd(char* argv[], int argc, char* input_file, char* output_file, int output, int input, int* exitStatus, struct sigaction SIGINT_action) {
    
    /*/for testing purposes
    printf("%s %d %s %d %d\n", output_file, output, input_file, input, background, exitStatus);  
    
    int i; 
    for(i = 0; i < argc; i++)
        printf("%s", argv[i]);
    printf("\n"); */
    
    int result;                                                     //variable holds the result assignment
    pid_t spawnPid = -5;                                            //garbage value
    int input_file_descriptor, output_file_descriptor;              //file descriptors  
    spawnPid = fork();                                              //spawnPid calls fork() for child
    
    //from lecture 3.3
    switch(spawnPid) {
        case -1:   
            perror("Hull Breach!\n");
            exit(1);
            break;
        case 0:
            //set SIGINT to SIG_DFL 
            if(foreground == 1) {
                //set the sig handler to the default action  
                SIGINT_action.sa_handler = SIG_DFL; 
                sigaction(SIGINT, &SIGINT_action, NULL); 
            }
            
            //if input was turned on 
            if(input == 1) {
                input_file_descriptor = open(input_file, O_RDONLY);
                
                if(input_file_descriptor == -1) {
                    printf("cannot open %s for input\n", input_file);
                    exit(1); 
                }
                
                result = dup2(input_file_descriptor, 0);
                
                if(result == -1) {
                    perror("source dup2()");
                    exit(2); 
                }
                
                close(input_file_descriptor);
            }
            
            //if output was turned on 
            if(output == 1) {
                output_file_descriptor = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                
                if(output_file_descriptor == -1) {
                    printf("cannot open %s for output\n", output_file);
                    exit(1); 
                }
                
                result = dup2(output_file_descriptor, 1);
                
                if(result == -1) {
                    perror("source dup2()");
                    exit(2); 
                }
                
                close(output_file_descriptor); 
            }
            
            //if file does not exist
            if(execvp(argv[0], (char* const*)argv)) {
                printf("%s: no such file or directory\n", argv[0]);
                fflush(stdout);
                exit(2); 
            }
               
            break;
        default:
            pids[numPids] = spawnPid;
            numPids++; 
            
            //no background, execute normally 
            if(background != 1) {
                ctrlPid = waitpid(spawnPid, exitStatus, 0); 
            }
            else {
                //background was turned on 
                printf("background pid is %d\n", spawnPid);
                fflush(stdout); 
            }
            
            //Check if any process has completed, return immediately with 0 if none have
            spawnPid = waitpid(-1, exitStatus, WNOHANG);

            //while spawnPid > 0, check if background process is finished 
            while(spawnPid > 0) {
                printf("background process %d is done: ", spawnPid);
                getExitMethod(*exitStatus);  
                fflush(stdout); 
                spawnPid = waitpid(-1, exitStatus, WNOHANG);
            } 
        }
}
               
/*------------------------------------------------------------------------------------*\
*   Function: catchSIGINT                                                              *
*   ------------------------                                                           *
*   Catches signal (ctrl^c) and kills the foreground command                           *
*   SOURCE: https://ibm.co/2QlBhZ0                                                     *
*   SOURCE: Lecture 3.3                                                                *
\*------------------------------------------------------------------------------------*/
void catchSIGINT(int signo) {
    //getExitMethod(signo);
    signal(ctrlPid, SIG_IGN); 
}

/*------------------------------------------------------------------------------------*\
*   Function: killPids                                                                 *
*   ------------------------                                                           *
*   Kills all child processes                                                          *   
\*------------------------------------------------------------------------------------*/
void killPids() {
    int i = 0;
    while(i != numPids) {
        kill(pids[i], SIGKILL);
        i++;
    }
    numPids = 0; 
}

/*------------------------------------------------------------------------------------*\
*   Function: catchSIGTSTP                                                             *
*   ------------------------                                                           *
*   Catches signal (ctrl^z) and enters/exits foregound mode                            *  
*   SOURCE: Lecture 3.3                                                                *   
\*------------------------------------------------------------------------------------*/
void catchSIGTSTP(int signo) {
    
    if(foreground == 0) {
        char* message = "\nEntering foreground-only mode (& is now ignored)\n"; 
        write(STDOUT_FILENO, message, 49);
        fflush(stdout);
        foreground = 1; 
    }
    else {
        char* message = "\nExiting foreground-only mode\n"; 
        write(STDOUT_FILENO, message, 29);
        fflush(stdout);
        foreground = 0;      
    }
}
               
