//Author: Cory Schmidt
//Description: This program simulates a server 
//Date: 12/06/2019
//Source: This code is primarily the file from class lectures, edited for my purposes 

//header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "otp.h" 

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
/*
    Name: server
    -----------------
    Description:
        This function simulates a server
        Reads buffer input from the client 
        Tokenizes string
        Opens files for writing specified in arguments list
        Calls appropriate function for encrypt/decrypt 
        Sends back an output file
    
    Parameters: 
        argv[] list from stdin (ex: otp_dec_d or otp_enc_d [port_number])
*/
int server(char* argv[])
{ 
    //printf("%s %s\n", argv[0], argv[1]);    //for testing purposes
    
    /* ----------------------------------------- Declare variables ----------------------------------------- */
    //VARIABLES FROM CLASS
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[300];
	struct sockaddr_in serverAddress, clientAddress;
    
    pid_t spawnPid = -5;                            //garbage value, holds child process
    
    char method[100];                               //array to hold method name (otp_enc,or otp_dec)
    char input_file_name[100];                      //array to hold input file name
    char key_file_name[100];                        //array to hold key file name
    
    char output_file_name[300] = "output_file";     //array to hold output file name (output_file)   
    
    char input[80000];                              //array to hold recv file contents
    char output[80000];                             //array to hold send file contents
    char key[80000];                                //array to hold key file contents
    
    int port;                                       //int value to hold argv[1]
    
    FILE* input_file;                               //FILE object to open input file (ex: plaintext1 or ciphertext1)
    FILE* key_file;                                 //FILE object to open key file (ex: mykey)
    FILE* output_file;                              //FILE object to open output file
    /* --------------------------------------- End declare variables --------------------------------------- */

    // CODE FROM CLASS 
	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    //add while loop for child processes 
    while(1) {
        
        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0) error("ERROR on accept");
        
        spawnPid = fork();      //fork children
        
        //switch statement for child processes
        switch(spawnPid) {
            case -1:   
                perror("Hull Breach!\n");
                exit(1);
                break;
            case 0:                                 //child process
                // Get the message from the client and display it
                memset(buffer, '\0', sizeof(buffer));
                charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer)-1, 0); // Read the client's message from the socket
                if (charsRead < 0) error("ERROR reading from socket");
                //printf("SERVER: I received this from the client: \"%s\"\n", buffer);
                
                //tokenize input from client 
                char* token = strtok(buffer, "\n"); 
                strcpy(method, token);              //transfer current value to method array  
                token = strtok(NULL, "\n");         //reset token
                strcpy(input_file_name, token);     //transfer current value to input_file_name array
                token = strtok(NULL, "\n");         //reset token
                strcpy(key_file_name, token);       //transfer current value to key_file_name array
                
                //printf("%s %s %s %s\n", method, input_file_name, key_file_name); //for testing purposes
                
                //printf("%d\n", port);    //for testing purposes 
                strcat(method, "_d");      //add _d to end of method 
                
                //if server and client method type (ex: otp_enc and otp_dec_d) are not equal 
                if(strcmp(argv[0], method) != 0) {
                    fprintf(stderr, "Error: could not contact %s on %s\n", argv[0], argv[1]);   //print error
                    exit(2);    //exit the child process with status 2
                }
                  
                //else send the input/output 
                else { 
                    //open input and key files for reading
                    input_file = fopen(input_file_name, "r"); 
                    key_file = fopen(key_file_name, "r"); 
                    
                    //transfer file contents to arrays
                    fgets(input, 80000, input_file); 
                    fgets(key, 80000, key_file); 
                    
                    //close files
                    fclose(input_file); 
                    fclose(key_file);
                    
                    //remove trailing newlines from arrays
                    input[strcspn(input, "\n")] = '\0'; 
                    key[strcspn(key, "\n")] = '\0';
                    
                    //printf("%s\n%s\n", input, key);     //for testing purposes
                    //printf("%s\n", argv[0]); 
                    
                    //printf("%s\n", method);            //for testing purposes 
                    
                    /* --------------------- Handle encrypt v. decrypt --------------------- */ 
                    //if argument 1 in argv array is encrypt 
                    if(strcmp("otp_enc_d", method) == 0)
                        encrypt(input, output, key); 
                    
                    //else if argument 1 in argv array is decrypt 
                    else if(strcmp("otp_dec_d", method) == 0)
                        decrypt(input, output, key);   
                    
                    /* ------------------- End handle encrypt v. decrypt ------------------- */ 
                }
                    
                //printf("output: %s\n", output);               //for testing purposes
                //printf("badInput: %d\n", badInput);           //for testing purposes
                    
                //Open output file and write contents to it 
                output_file = fopen(output_file_name, "w+");
                fprintf(output_file, "%s", output); 
                
                //printf("output_file_name: %s\n", output_file_name);   //for testing purposes
                
                // Send a Success message back to the client
                charsRead = send(establishedConnectionFD, output_file_name, strlen(output_file_name), 0); // Send success back
                //printf("%d\n", charsRead);    //for testing purposes
                if (charsRead < 0) error("ERROR writing to socket");
                
                // Close the existing socket which is connected to the client
                close(establishedConnectionFD); 
                establishedConnectionFD = -1; 
                exit(0); 
                
                break; 
                
            default: 
                //wait for zombie processes and kill them
                close(establishedConnectionFD); 
                establishedConnectionFD = -1;
                wait(NULL);
        }
    }
    
	close(listenSocketFD); // Close the listening socket
    
	return 0; 
}