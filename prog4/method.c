//Author: Cory Schmidt
//Description: This program includes function to encrypt or decrypt data
//Date: 12/06/2019
//Source: https://stackoverflow.com/questions/30234363/how-can-i-store-an-int-array-into-string/30234430

//header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "otp.h"        

//function prototypes
void stringToInt(char [], int [], int); 
void intToString(char [], int [], int); 

/*
    Name: encrypt
    -----------------
    Description:
        This function handles the plaintext and key arrays from server.c and encrpyts to ciphertext 
        Calls function to change string to integers 
        Adds plaintext and key integers together according to assignment specs
        Calls function change integers back to string
    Parameters: 
        char input[] - array of chars from plaintext file (ex: THE RED GOOSE FLIES AT MIDNIGHT STOP)
        char output[] - array that function parses ciphertext into 
        char key[] - array of chars from key file (ex: QJAKTOBVXT UDDGJBCZEGY) 
*/
void encrypt(char input [], char output [], char key []) {
    //printf("%s\n, %s\n, %s\n, %s\n", letters, input, output, key);
    
    int i, j;                                   //iterator variables
    
    int input_array_length = strlen(input);     //stores length of the input array and numbers array
    int key_array_length = strlen(key);         //stores length of key array
    
    int input_numbers[input_array_length];      //array stores numerical values of chars in input array
    int key_numbers[key_array_length];          //array stores numerical values of chars in key array
    int numbers[input_array_length];            //array stores numberical values after arithmetic ((input + key) %27)
    
    //initialize all arrays to 0
    memset(input_numbers, 0, input_array_length*sizeof(int)); 
    memset(key_numbers, 0, key_array_length*sizeof(int));
    memset(numbers, 0, input_array_length*sizeof(int)); 
    //printf("%d\n%d\n", input_array_length, key_array_length);  //for testing purposes
    
    /*/for testing purposes 
    int i; 
    for(i = 0; i < strlen(input); i++)
        printf("%c", input[i]);
    printf("\n");*/
    
    //call stringToInt functions for both arrays
    stringToInt(input, input_numbers, input_array_length);
    stringToInt(key, key_numbers, key_array_length);
    
    /* -------------- TESTING OUTPUT --------------
    for(i = 0; i < input_array_length; i++) {
        printf("%d, ", input_numbers[i]);
    }
    
    printf("\n");
     
    for(j = 0; j < key_array_length; j++) {
        printf("%d, ", key_numbers[j]);
    }
    
    printf("\n");
    ------------ END TESTING OUTPUT ------------ */
    
    //this adds the array values together and takes remainder of 26 
    for(i = 0; i < input_array_length; i++) {
        numbers[i] = (input_numbers[i] + key_numbers[i]) % 27; 
    }
    
    /*/for testing purposes 
    for(i = 0; i < input_array_length; i++) {
        printf("%d, ", numbers[i]); 
    }
    printf("\n"); //for testing purposes*/

    //call function to change integers back to chars 
    intToString(output, numbers, input_array_length);
    
    /*/for testing purposes
    for(j = 0; j < input_array_length; j++) {
        printf("%c ", output[j]); 
    }*/
    
    output[input_array_length] = '\0'; 
}

/*
    Name: decrypt
    -----------------
    Description:
        This function handles the plaintext and key arrays from server.c and encrpyts to ciphertext 
        Calls function to change string to integers 
        Subtracts key from plaintext integers according to assignment specs
        Calls function change integers back to string
    Parameters: 
        char input[] - array of chars from ciphertext file (ex: IQEJJSEUCGNLHCLUJGQDGQZWRVRMTCVBWLPM)
        char output[] - array that function parses plaintext into 
        char key[] - array of chars from key file (ex: QJAKTOBVXT UDDGJBCZEGY) 
*/
void decrypt(char input [], char output [], char key []) {
    //printf("%s\n, %s\n, %s\n, %s\n", letters, input, output, key);
    
    int i, j;                                   //iterator variables
    
    int input_array_length = strlen(input);     //stores length of the input array and numbers array
    int key_array_length = strlen(key);         //stores length of key array
    
    int input_numbers[input_array_length];      //array stores numerical values of chars in input array
    int key_numbers[key_array_length];          //array stores numerical values of chars in key array
    int numbers[input_array_length];            //array stores numberical values after arithmetic ((input + key) %27)
    
    //initialize all vaules in the arrays to 0
    memset(input_numbers, 0, input_array_length*sizeof(int)); 
    memset(key_numbers, 0, key_array_length*sizeof(int));
    memset(numbers, 0, input_array_length*sizeof(int)); 
    //printf("%d\n%d\n", input_array_length, key_array_length);  //for testing purposes
    
    /*/for testing purposes 
    int i; 
    for(i = 0; i < strlen(input); i++)
        printf("%c", input[i]);
    printf("\n");*/
    
    //call function for both arrays to change char to int 
    stringToInt(input, input_numbers, input_array_length);
    stringToInt(key, key_numbers, key_array_length);
    
    /* -------------- TESTING OUTPUT --------------
    for(i = 0; i < input_array_length; i++) {
        printf("%d, ", input_numbers[i]);
    }
    
    printf("\n");
     
    for(j = 0; j < key_array_length; j++) {
        printf("%d, ", key_numbers[j]);
    }
    
    printf("\n");
    ------------ END TESTING OUTPUT ------------ */
    
    //loop subtracts the array values from one another 
    for(i = 0; i < input_array_length; i++) {
        numbers[i] = (input_numbers[i] - key_numbers[i]);
        
        //if the current value is less than 0, add 26 
        if(numbers[i] < 0)
            numbers[i] += 27; 
    }
    
    /*/for testing purposes 
    for(i = 0; i < input_array_length; i++) {
        printf("%d, ", numbers[i]); 
    }
    printf("\n"); //for testing purposes*/
    
    //call function to change integers back to chars
    intToString(output, numbers, input_array_length);   
    
    /*/for testing purposes
    for(j = 0; j < input_array_length; j++) {
        printf("%c ", output[j]); 
    }*/
    
    output[input_array_length] = '\0'; 
}

/*
    Name: stringToInt
    -----------------
    Description:
        Loops through array and parses numbers into new array
        Tests for equivalency between static letters array and input array
    Parameters: 
        char array[] - char array of either either ciphertext or plaintext input
        char numbers[] - array that the function parases numbers into
        int array_length - length of char array 
*/
void stringToInt(char array [], int numbers[], int array_length) {
    int i, j, k = 0; 
    for(i = 0; i < array_length; i++) {
        for(j = 0; j < 27; j++) {
            if(array[i] == letters[j]) {
                //printf("%c, %d, / %c, %d\n", input[i], i, letters[j], j); for testing purposes 
                numbers[k] = j; 
                k++;
            }
        }
    }
    //printf("%d\n", k);    //for testing purposes 
}

/*
    Name: intToString
    -----------------
    Description:
        Parses letters into new array based on corresponding number in static letters array
    Parameters:
        char array[] - array that the function parases letters into
        char numbers[] - array of numbers corresponding to either ciphertext or plaintext 
        int array_length - length of numbers array 
*/
void intToString(char array [], int numbers [], int array_length) {
    int i; 
    
    for(i = 0; i < array_length; i++)
        array[i] = letters[numbers[i]]; 
}