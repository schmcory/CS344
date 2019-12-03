//Author: Cory Schmidt
//Description: This program runs error checking on user input 
//Date: 12/06/2019

//header files
#include <stdio.h> 
#include <stdlib.h>

#include "otp.h"    

/*
    Name: serverError
    -----------------
    Description:
        Checks if argument count from stdin is not equal to 2
    Parameters: 
        Argument count and argument list from stdin (ex: otp_dec_d or otp_enc_d [listening_port])
*/
void serverError(int argc, char *argv[]) {
    if (argc != 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args
    
    /*/for testing purposes 
    int i; 
    for(i = 0; i < argc; ++i) 
        printf("%s", argv[i]);*/
}
