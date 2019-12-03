//Author: Cory Schmidt
//Description: 
//  This program is the driver function for the client
//      Takes input from the user
//      Calls clientError function to test input
//      Calls client function and passes arguments
//To run: otp_dec [plaintext] [key] [port]
//Date: 12/06/2019

#include "otp.h"    //header file

int main(int argc, char *argv[])
{
    clientError(argc, argv);    //error checking
    
    client(argv);               //calls client function  
    
	return 0;
}
