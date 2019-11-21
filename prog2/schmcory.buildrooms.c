//Author: Cory Schmidt
//Description: Program creates a series of files that hold descriptions of the in-game rooms and how the rooms are connected.
//To run: To run: gcc -o schmcory.buildrooms schmcory.buildrooms.c
//Date: 11/1/2019

/***************************************************** DECLARATIONS *****************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/******************************** HEADER FILES ********************************/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

/****************************** GLOBAL VARIABLES ******************************/
#define TOTAL_ROOMS_COUNT 10        //global constant, rooms count = 10
#define MAX_ROOMS_COUNT 7           //global constant, rooms count = 7
#define MIN_CONNECTIONS_COUNT 3     //global constant, room connections = 3
#define MAX_CONNECTIONS_COUNT 6     //global constant, room connections = 6 

/*
*   Enum: boolean (for C89)
*   ---------------
*   Holds true/false values
*   Source: https://cboard.cprogramming.com/c-programming/35204-how-define-boolean-c.html
*/
typedef enum {
	FALSE,
	TRUE
} boolean; 

/*
*   Char Array: roomNames
*   ---------------
*   Global array holds olds 10 harcoded room names 
*/
char* roomNames [TOTAL_ROOMS_COUNT] = {
        "Great",
        "Library",
        "Common",
        "Trophy",
        "Owlery",
        "Tower",
        "Chamber",
        "Dungeon",
        "Kitchen",
        "Study",
};

/*
*   Struct: Room
*   ----------------------------------------------------
*   Room struct holds details for 7 rooms
*       name pointer (ex: Great, Library, etc.)
*       type pointer (START_ROOM, END_ROOM, MID_ROOM)
*       number of connections (3 > x < 6)
*       connections array holds id corresponding to connections array
*       id variable holds the id of each room 
*/
struct ROOM  
{
    int id; 
    char* name;
    int numConnections;
    int connections[MAX_CONNECTIONS_COUNT];
    char* type;
};

struct ROOM *roomList[MAX_ROOMS_COUNT]; //instance of struct object holds shuffled roomList array

/****************************** FUNCTION PROTOTYPES ******************************/
int GetRandomRoom();
boolean IsGraphFull(); 
boolean CanAddConnectionFrom(struct ROOM *x);
boolean ConnectionAlreadyExists(struct ROOM *x, struct ROOM *y);
void ConnectRoom(struct ROOM *x, struct ROOM *y);
void AddRandomConnection(); 
boolean IsSameRoom(struct ROOM *x, struct ROOM *y); 

/*************************************************** CREATE DIRECTORY ***************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/*
*   Function: createDirectory
*   -------------------------
*   creates a directory with 0755 permissions
*   0755 - read, execute access for everyone; write access by owner
*/
char* createDirectory() {
    int buffer = 21;            //variable holds buffer count (prefix + 5 characters for pid)
    int pid = getpid();         //variable calls builtin C function to get process id
    
    char* directory = malloc(buffer * sizeof(char));  //dynamically creates array of chars
    char* prefix = "schmcory.rooms." ;    //prefix is set to my user id and rooms
    
    //counts characters and stores in directory variable
    snprintf(directory, buffer, "%s%d", prefix, pid);
    //printf("%s\n", directory); //for testing purposes
    
    struct stat st = {0};   //initialize system struct
    
    //if statement checks if directory exists
    if(stat(directory, &st) == -1) {
        mkdir(directory, 0755);  //creates directory 
    }

    return directory;   //return directory name
}

/***************************************************** CREATE ROOMS *****************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/*
*   Function: getRandomInt
*   -------------------------
*   returns a random integer between 0 and 10
*   parameter(s): none 
*/
int getRandomInt() {
    int random = rand() % TOTAL_ROOMS_COUNT; 
    return random; 
}

/*
*   Function: shuffleArrays
*   -------------------------
*   returns shuffled array
*/
void shuffleArray() {
    int i;  //iterator variable
    
    for(i = 0; i < 10; i++) {
        int randomRoom = getRandomInt();  //initializes variable to return value of getRandomInt() function
        
        char* temp = roomNames[randomRoom];     //temp variable holds a random integer 
        roomNames[randomRoom] = roomNames[i];   //swap random integer with the current iteration of roomNames array
        roomNames[i] = temp;                    //swap current iteration of roomNames array with temp 
    }
}

/*
*   Function: initalizeRooms
*   -------------------------
*   initializes room ids and room names
*   fills roomList->name array with shuffled names from roomNames array 
*   SOURCE: https://bit.ly/32KWsHf
*/
void initializeRoomList() {
    int i;  //array iterators
    
    //initializes memory for 7 rooms in the roomList array 
    for(i = 0; i < MAX_ROOMS_COUNT; i++) {
        roomList[i] = (struct ROOM *)malloc(sizeof(struct ROOM)); 
    } 
    
    //for loop initializes roomList array using values from the roomNames array
    for(i = 0; i < MAX_ROOMS_COUNT; i++) {
        
        roomList[i]->id = i;                //initialize roomList->id array with iterator variable
        roomList[i]->numConnections = 0;
        roomList[i]->name = roomNames[i];   //intialize roomList->name array with shuffled roomNames array
        
        if(i > 0 && i < 6)
            roomList[i]->type = "MID_ROOM";
        
        //switch statement sets roomList type to START_ROOM, MID_ROOM OR END_ROOM
        /*switch(i) {
            case 0:
                roomList[i]->type = "START_ROOM";   //roomList[0] == START_ROOM
                break;
            case 6:
                roomList[i]->type = "END_ROOM";    //roomList[6] === START_ROOM
                break;
            default: 
                roomList[i]->type = "MID_ROOM";   //roomList[1-5] == MID_ROOM
        }*/
    }
    roomList[0]->type = "START_ROOM";
    roomList[6]->type = "END_ROOM"; 
}

/*
*   Function: createFiles
*   -------------------------
*   creates 7 files from the roomNames array in the specified directory 
*   writes name, 3 to 6 connections, and a type per file
*/
//void createFiles(struct ROOM *array[]) {
void createFiles() {
    //-------------------------------------------------Create files--------------------------------------------------//
    
    char* dir = createDirectory(); //createDirectory string holds results of createDirectory 

    char* filePath = malloc(32 * sizeof(char)); //initialize filePath string
    
    int i, j, k;                      //iterator variable
    ssize_t nread, nwritten;    //read and write variables
    char readBuffer[32];        //readBuffer array 
    char connectionName[8];     //variable holds the connection room name
    
    for(i = 0; i < 7; i++) {
        sprintf(filePath, "%s/%s_room", dir, roomList[i]->name); 
        //printf("%s\n", filePath); //for testing purposes

        //open file and assign permissions
        int file_descriptor = open(filePath, O_RDWR | O_APPEND | O_CREAT, 0600);

        //check if file is open
        if (file_descriptor == -1) {
            printf("open() failed on \"%s\"\n", filePath); 
            exit(1); 
        }
        
        //Write room name to file
        char* roomName = malloc(32 * sizeof(char));
        sprintf(roomName, "ROOM NAME: %s\n", roomList[i]->name);
        nwritten=write(file_descriptor, roomName, strlen(roomName) * sizeof(char));
        
         
        //Write room connections to file
        for(j = 0; j < roomList[i]->numConnections; j++) {
            for(k = 0; k < MAX_ROOMS_COUNT; k++) {
                if(roomList[i]->connections[j] == roomList[k]->id) {
                    strcpy(connectionName, roomList[k]->name);
                    break;
                }
            }
            
            char* roomName = malloc(32 * sizeof(char));
            sprintf(roomName, "CONNECTION %d: %s\n", j + 1, connectionName);
            nwritten=write(file_descriptor, roomName, strlen(roomName) * sizeof(char)); 
        }  
        
        //Write room type to file
        char* roomType = malloc(32 * sizeof(char));
        sprintf(roomType, "ROOM TYPE: %s\n", roomList[i]->type);
        nwritten=write(file_descriptor, roomType, strlen(roomType) * sizeof(char));
        
        //Read back files 
        memset(readBuffer, '\0', sizeof(readBuffer)); 
        lseek(file_descriptor, 0, SEEK_SET); 
        nread=read(file_descriptor, readBuffer, sizeof(readBuffer)); 
        
        close(file_descriptor);     //close file_descriptor
    } 
    
    free(dir);  //free directory 
}

/*
*   Function: debugRooms
*   -------------------------
*   prints values for debugging
/
void debugRooms() {
    
    int i;  //iterator variable
    
    printf("*---------DEBUGGER---------*");
    
    //prints roomNames array
    for(i = 0; i < TOTAL_ROOMS_COUNT; i++) 
        printf("Rooms Array: %s,", roomNames[i]);
    
    printf("\n");   //newline
    
    //prints roomNames array after they've been shuffled
    for(i = 0; i < TOTAL_ROOMS_COUNT; i++) 
        printf("Rooms Array: %s,", roomNames[i]);
    
    char* dir = createDirectory(); //createDirectory string holds results of createDirectory 
    
    printf("Directory: %s\n\n", dir); //prints directory name
    
    //prints roomList ids and names
    printf("*---------Rooms---------*");
    for(i = 0; i < MAX_ROOMS_COUNT; i++)
        printf("Room %d: %s\n", roomList[i]->id, roomList[i]->name); 
    
    printf("\n");
    
    //prints roomList names and types
    printf("*-------Room Types------*");
    for(i = 0; i < MAX_ROOMS_COUNT; i++) 
        printf("%s_room: %s\n", roomList[i]->name, roomList[i]->type); 
        
    int i, j; 
    for(i = 0; i < MAX_ROOMS_COUNT; i++) {
        printf("ROOM NAME: %s\n", roomList[i]->name);
    
        for(j = 0; j < roomList[i]->numConnections; j++) {
            printf("CONNECTION %d: %s\n", j+1, roomList[i]->connections[j]);
        
        
        printf("ROOM TYPE: %s\n", roomList[i]->type);
    }
}

/************************************************** CREATE CONNECTIONS **************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/*
*   Function: getRandomRoom
*   -------------------------
*   returns a random integer between 0 and 7
*   parameter(s): none 
*/
int GetRandomRoom() {
    int random = rand() % MAX_ROOMS_COUNT; 
    return random; 
}

/*
*   Function: isGraphFull
*   -------------------------
*   loops through the roomList to test if each of the rooms connections is less than 3
*   if each rooms connections is less than 3, return false
*   parameter(s): none 
*/
boolean IsGraphFull() {
  int i;
  for(i=0; i < MAX_ROOMS_COUNT; i++)
  {
    if(roomList[i]->numConnections < MIN_CONNECTIONS_COUNT)
      return FALSE;
  }
  return TRUE;
}

/*
*   Function: addRandomConnection
*   -------------------------
*   returns enum data type (TRUE OR FALSE)
*   returns true if all rooms have between 3 and 6 outbound connections
*   parameter(s): none 
*/
void AddRandomConnection() {
  int A;  
  int B;
  
  while(TRUE)
  {
    A = GetRandomRoom();

    if (CanAddConnectionFrom(roomList[A]) == TRUE) {
      break;
    }
  }

  do
  {
  
  	B = GetRandomRoom(); 
    
  }while(CanAddConnectionFrom(roomList[B]) == FALSE || IsSameRoom(roomList[A], roomList[B]) == TRUE || ConnectionAlreadyExists(roomList[A], roomList[B]) == TRUE);
  
  ConnectRoom(roomList[A], roomList[B]); 
  ConnectRoom(roomList[B], roomList[A]); 
  
}

/*
*   Function: canAddConnectionFrom
*   -------------------------
*   returns enum data type (TRUE OR FALSE)
*   returns true if a connection can be added (< 6 outbound connections), otherwise false
*   parameter(s): pointer to instance of room struct (x) 
*/
boolean CanAddConnectionFrom(struct ROOM *x) {

  if(x->numConnections < MAX_CONNECTIONS_COUNT)
    return TRUE;
  
  else 
    return FALSE;
}

/*
*   Function: connectionAlreadyExists 
*   -------------------------
*   returns enum data type (TRUE OR FALSE)
*   return true is a connection room room x to room y already exists, otherwise false
*   parameter(s): two instances of room struct (x, y) 
*/
boolean ConnectionAlreadyExists(struct ROOM *x, struct ROOM *y) {
  int i;
  boolean isConnected;      //boolean variable 
  for(i = 0; i < x->numConnections; i++)
  {
    if(x->connections[i] == y->id)
    {
      isConnected = TRUE;
      break;
    }
  }
  if(isConnected == TRUE)
    return TRUE;
  else
    return FALSE;
}

/*
*   Function: connectRoom
*   -------------------------
*   connects room x and room y together 
*   returns nothing
*   parameter(s): two instances of room struct (x, y) 
*/
void ConnectRoom(struct ROOM *x, struct ROOM *y) {
  x->connections[x->numConnections] = y->id;
  x->numConnections++;
}

/*
*   Function: isSameRoom
*   -------------------------
*   returns enum data type (TRUE OR FALSE)
*   returns true is room x and room y are the same room, otherwise false
*   parameter(s): two instances of room struct (x, y) 
*/
boolean IsSameRoom(struct ROOM *x, struct ROOM *y) {
  if(x->id == y->id)
    return TRUE;
  else
    return FALSE;
}

/******************************** MAIN FUNCTION ********************************/
int main() {
  srand(time(NULL));    //seed time

  shuffleArray(); 					//call shuffleArray function to shuffle rooms
  initializeRoomList(); 	//call initializeRoomList function 

  //while each room has less than 3 connections
  while (IsGraphFull() == FALSE) {
    AddRandomConnection();  
  }
  
  createFiles();        //create all the files
    
  //debugRooms(); 
        
  return 0;
}
