//Author: Cory Schmidt
//Description: 
//  This program is the driver function for the server
//      Takes input from the user
//      Calls serverError function to test input
//      Calls server function and passes arguments
//To run: otp_enc_d [listening_port]
//Date: 12/06/2019

#include "otp.h"    //header file

int main(int argc, char *argv[])
{
    serverError(argc, argv);    //error checking
    
    server(argv);               //call server function
    
	return 0; 
}