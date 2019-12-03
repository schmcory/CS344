//Author: Cory Schmidt
//Description: This program simulates a client
//Date: 12/06/2019
//Sources: This code is primarily the file from class lectures, edited for my purposes 
//         https://www.geeksforgeeks.org/isupper-function-c-language/ (isupper, isspace)

//header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "otp.h" 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

/*
    Name: client
    -----------------
    Description:
        This function simulates a client 
        Reads arguments list from stdin 
        Opens files for reading specified in arguments list 
        Transfers input into arrays
        If input passes tests, argv list gets sent to the server
        After server handles input (encryption/decryption), it sends output back
        Opens output to print to stdout 
    Parameters: 
        argv[] list from stdin (ex: otp_dec or otp_enc [plaintext_file] [key_file] [port_number])
*/
int client(char* argv[])
{     
    //printf("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);      //for testing purposes
    
    /* ---------------------------------- Declare variables ---------------------------------- */
    //declare file objects
    FILE* input_file;
    FILE* key_file; 
    FILE* output_file;
    
    //input, key, and output arrays set equal to 70000 possible chars for grading script 
    char input[80000];   
    char key[80000];
    char output[80000];               
    
    int input_length, key_length;  //variables to test input file and key file character length
    
    int badFile = 0;   //variable to handle input validation 
        
    //test to make sure plaintext file has no bad input  
    int i;              //iterator variable 
    char c;             //for testing each char in file 
    int badInput = 0;   //variable to prevent client from writing data to server buffer 
    /* -------------------------------- End declare variables -------------------------------- */
    
    /* -------------------------------- Handle input file contents -------------------------------- */
    //open both files for reading
    input_file = fopen(argv[1], "r");
    key_file = fopen(argv[2], "r");  
    
    //handle stdin validation 
    if(input_file == NULL) {fprintf(stderr, "File '%s' does not exist in this directory\n", argv[1]); badInput=1;}
    if(key_file == NULL) {fprintf(stderr, "File '%s' does not exist in this directory\n", argv[2]); badInput=1;}
    if(badInput == 1) exit(1); 
    
    //transfer file contents into arrays
    fgets(input, 80000, input_file); 
    fgets(key, 80000, key_file); 
    
    //close files
    fclose(input_file);
    fclose(key_file); 
    
    //remove the trailing \n that fgets adds
    input[strcspn(input, "\n")] = '\0';
    key[strcspn(key, "\n")] = '\0';
    
    input_length = strlen(input);       //set input_length to the string length of input array
    key_length = strlen(key);           //set key_length to the string length of key array
    
    //test to make sure keyfile is not shorter than the plaintext
    //terminate if so and send error
    if(key_length < input_length) {
        fprintf(stderr, "Error: key '%s' is too short\n", argv[2]);
        exit(1); 
    }
    
    //loops to check each char in input file 
    for(i = 0; i < input_length; i++) {
        c = input[i]; 
        //printf("%c", c);  //for testing purposes
        
        //if a char other than upper case letter or space is found 
        if(!(isupper(c) || isspace(c))) {
            fprintf(stderr, "%s error: '%s' contains bad characters\n", argv[0], argv[1]);    //print error
            badInput = 1;   //set badInput flag to 1 
            break;          //break to prevent error from printing multiple times
            //exit(1); 
        }
    }
    
    //loops to check each char in input file 
    for(i = 0; i < key_length; i++) {
        c = key[i]; 
        //printf("%c", c);  //for testing purposes
        
        //if a char other than upper case letter or space is found
        if(!(isupper(c) || isspace(c))) {
            fprintf(stderr, "%s error: '%s' contains bad characters\n", argv[0], argv[2]); //print error 
            badInput = 1;            //set badInput flag to 1 
            break;                   //break to prevent error from printing multiple times
            //exit(1); 
        }
    }

    //printf("%d", badInput);   //for testing purposes
    
    /* ----------------------------- End handle input file contents ----------------------------- */
    
    //execute if badInput is not set 
    if(badInput != 1) {
        /* ----------------------------------- Handle sockets ----------------------------------- */
        // CODE FROM THE LECTURE 
        int socketFD, portNumber, charsWritten, charsRead;
        struct sockaddr_in serverAddress;
        struct hostent* serverHostInfo;
        char buffer[300];

        // Set up the server address struct
        memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
        portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
        serverAddress.sin_family = AF_INET; // Create a network-capable socket
        serverAddress.sin_port = htons(portNumber); // Store the port number
        serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address //line changed to "localhost"
        if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
        memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

        // Set up the socket
        socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
        if (socketFD < 0) error("CLIENT: ERROR opening socket");

        // Connect to server
        if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
            error("CLIENT: ERROR connecting");

        //String message together, seperated by \n
        //printf("CLIENT: Enter text to send to the server, and then hit enter: ");
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
        sprintf(buffer, "%s\n%s\n%s\n%s", argv[0], argv[1], argv[2]);  //line changed 

        // Send message to server
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

        // Get return message from server
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
        charsRead = recv(socketFD, buffer, sizeof(buffer)-1, 0); // Read data from the socket, leaving \0 at end
        if (charsRead < 0) error("CLIENT: ERROR reading from socket");
        //printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
        /* ---------------------------------- End handle sockets ---------------------------------- */
        
        //printf("buffer: %s\n", buffer); //for testing purposes 
 
        /* ----------------------------- Handle output file contents ---------------------------- */ 
        
        //printf("buffer: %s\n", buffer);          //for testing purposes 
        
        //if the buffer is empty, this means that server/client were not equivalent 
        if(strcmp(buffer, "") == 0 && strcmp(input, "") != 0)
            exit(2);    //exit status code 2
        
        //open the file recieved 
        output_file = fopen(buffer, "r");          //open the encrypted or decrypted file
        //printf("output_file: %s", output_file);  //for testing purposes 
        
        fgets(output, 80000, output_file);         //transfer to array
        fclose(output_file);                       //close file 

        remove(buffer);                            //delete the file specified in buffer 
        
        output[strcspn(output, "\n")] = '\0';      //remove the trailing \n that fgets adds

        printf("%s\n", output);                    //print it with \n

        /* ----------------------------- End output file contents ----------------------------- */

        close(socketFD); //close the socket
    }
    else {
        exit(1);    //set exit status to 1
    }
    
	return 0;
}
