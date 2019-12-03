//Author: Cory Schmidt
//Description: 
//  This program prints a key of user specified length
//      27 possible key values: 26 capital letters and space character
//To run: keygen [keylength], ex: keygen 10
//Date: 12/06/2019

#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 

#include "otp.h"

int main(int argc, char* argv[]) { 
    
    if (argc != 2) { fprintf(stderr,"USAGE: %s keylength\n", argv[0]); exit(1); } // Check usage & args
    
    //declare variables
    int keylength = atoi(argv[1]);         //change argv[1] from char to integer 
    int i;                                 //iterator variable
    int randNum;                           //holds the a random number 
    
    /*/for testing purposes 
    for(i = 0; i < argc; ++i) 
        printf("%s", argv[i]);*/
    
    srand(time(0));                        //seed time
    
    //printf("%d", keylength);             //for testing purposes  
    
    //for loop prints random letters
    for(i = 0; i < keylength; i++) {
        randNum = rand() % 27; 
        printf("%c", letters[randNum]); 
    }
    
    printf("\n");                         //print newline
    
    return 0;
}