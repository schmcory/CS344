//Author: Cory Schmidt
//Description: 
//  Header file for OTP program
//      Declares function prototypes 
//Date: 12/06/2019

#ifndef OTP_H
#define OTP_H

static const char letters[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";     //static const array of available chars 

void clientError(int, char* []); 
void serverError(int, char* []);

int client(char* []);
int server(char* []);

void encrypt(char [], char [], char []);
void decrypt(char [], char [], char []);
    
#endif