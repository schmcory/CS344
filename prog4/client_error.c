//Author: Cory Schmidt
//Description: This program runs error checking on user input 
//Date: 12/06/2019

//header files
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

#include "otp.h"

/*
    Name: clientError
    -----------------
    Description:
        Checks if output is not equal to 4 and prints appropriate error 
    Parameters: 
        Argument count and argument list from stdin (ex: otp_dec or otp_enc [plaintext_file] [key_file] [listening_port])
*/
void clientError(int argc, char *argv[]) {
    // Check usage & args
    if (argc != 4) {
        if(strcmp(argv[0], "otp_enc") == 0)
            fprintf(stderr,"USAGE: %s plaintext key port \n", argv[0]); 
        else if(strcmp(argv[0], "otp_dec") == 0)
            fprintf(stderr,"USAGE: %s ciphertext key port \n", argv[0]); 
        
        exit(0); 
    } 
    
    /*/for testing purposes 
    int i; 
    for(i = 0; i < argc; ++i) 
        printf("%s", argv[i]);*/
}