//Author: Cory Schmidt
//Description: Program provides interface for playing the game using the most recently generated rooms. 
//   Player begins in starting room
//   Player wins upon entering ending room
//   Game exits and displays path
//To run: To run: gcc -o schmcory.adventure schmcory.adventure.c
//Date: 11/1/2019

/***************************************************** DECLARATIONS *****************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/******************************** HEADER FILES ********************************/
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <time.h> 
#include <fcntl.h>

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
    char name[8];
    int numConnections;
    char connections[MAX_CONNECTIONS_COUNT][8];
    char type[10];
};

char* filePath[MAX_ROOMS_COUNT];         //array to hold the file names from the most recently modified directory
struct ROOM* roomList[MAX_ROOMS_COUNT];  //instance of struct object holds shuffled roomList array

pthread_mutex_t lock;       //mutex lock variable
pthread_t tid;              //mutex tid variable

/**************************************************** CREATE TIME FILE **************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/*
*   Function: createTimeFile 
*   --------------------------
*   gets the local time and writes it to a file 
*   SOURCE: https://stackoverflow.com/questions/42166538/create-pthread-that-prints-current-time-to-command-line
*   SOURCE: https://www.thegeekstuff.com/2012/05/c-mutex-examples
*/
void* createTimeFile() {
    
    pthread_mutex_lock(&lock);  //lock mutex
     
    time_t rawtime;
    struct tm * timeinfo;
    char timeOfDay[80];
    //ssize_t nwritten;    //read and write variables

    time (&rawtime);                
    timeinfo = localtime(&rawtime); //change to local time

    //stringify time (00:00pm, Day, Month 00, 0000); 
    strftime(timeOfDay,sizeof(timeOfDay)," %I:%M%P, %A, %B %d, %Y ", timeinfo);

    //printf("%s\n", timeOfDay); //for testing purposes
    
    //open file and assign permissions
    int file_descriptor = open("currentTime.txt", O_RDWR | O_CREAT | O_TRUNC, 0600);

    //check if file is open
    if (file_descriptor == -1) {
        printf("open() failed on \"%s\"\n", "currentTime.txt"); 
        exit(1); 
    }
    
    //write time to file
    write(file_descriptor, timeOfDay, strlen(timeOfDay) * sizeof(char));
    
    close(file_descriptor);         //close file
    
    pthread_mutex_unlock(&lock);  //unlock mutex
}

/***************************************************** GET DIRECTORY ****************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/*
*   Function: getDirectory
*   -------------------------
*   returns the most recently modified directory within the current working directory
*   SOURCE: https://stackoverflow.com/questions/42170824/use-stat-to-get-most-recently-modified-directory
*/
char* getDirectory() {
    
    DIR *directory = opendir(".");   //variable holds the current working directory
    
    struct dirent *subDir;           //holds the sub directory of the current directory
    
    struct stat dStat;               //holds sub directory's stats 
    
    char *prefix = "schmcory.rooms." ;    //prefix is set to my user id and rooms
    
    time_t latest = 0;                            //holds the timestamp of the latest directory 
    
    char* latestDir = malloc(sizeof(char) * 64);; // holds the name of the latest directory
    
    //memset(latestDir, '\0', sizeof(latestDir));    //creates memory for the latest directory
    
    //while loop reads all the files in the current directory
    while ((subDir = readdir(directory)) != NULL) {
        
        //check if the prefix does not exist
        if (strstr(subDir->d_name, prefix) != NULL) { 
            //printf("Error getting info on file\n");
            
            stat(subDir->d_name, &dStat); //get subDirs info 
        
            // If not a directory skip 
            if ((dStat.st_mode & S_IFDIR) != S_IFDIR) {
                continue;
            }
            
            // check with the latest timestamp
            // On finding a more recent file switch that to latest
            if (dStat.st_mtime > latest) {      
                latest = dStat.st_mtime;
                memset(latestDir, '\0', sizeof(latestDir));
                strcpy(latestDir, subDir->d_name);
                
            }
        }
    }
    closedir(directory);
    //printf("Most recently touched directory %s\n", latestDir);
    
    return latestDir; 
}

/*
*   Function: readsDirectory
*   -------------------------
*   reads the files inside the directory passed
*   places the files into an array
*   parameters: directory
*   SOURCE: https://c-for-dummies.com/blog/?p=3246
*   SOURCE: https://stackoverflow.com/questions/41653419/c-store-a-list-of-files-in-a-directory-into-an-array
*/
int readsDirectory(char* directory) {
    DIR *folder;
    struct dirent *entry;
    int i = 0;  
    
    folder = opendir(directory);    //set variable to open directory 
    
    //if directory cannot be opened
    if(folder == NULL)
    {
        perror("Unable to read directory");
        return(1);
    }

    while((entry=readdir(folder)) != NULL)
    { 
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {   //skip first two files in list (directories)
            continue; 
        }
        
                /*
        printf("File %3d: %s\n",
                files,
                entry->d_name
              );
        */
        
        else {
            
            //places file names inside the filePath array
            filePath[i] = malloc(64 * sizeof(char)); 
            strcpy(filePath[i], "./");
            strcat(filePath[i], directory);
            strcat(filePath[i], "/");
            strcat(filePath[i], entry->d_name); 
            i++; 
        }
        
    }
    rewinddir(folder);      //resets the position of directory stream to beginning 
    
    /*
    for(i = 0; i < MAX_ROOMS_COUNT; i++)
        printf("%s\n", filePath[i]);*/
    

    closedir(folder);   
}

/***************************************************** GET ROOM DATA ****************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/*
*   Function: getRoomData
*   -------------------------
*   gets data about the files
*       number of lines (for number of connections)
*       name of room
*       type of room
*   puts the data into the roomList struct 
*   SOURCE: https://www.sanfoundry.com/c-program-number-lines-text-file/
*   SOURCE: https://stackoverflow.com/questions/12767892/ignore-first-word-from-the-line-in-c
*   SOURCE: https://stackoverflow.com/questions/19603267/comparing-user-input-to-a-string-in-c
*/
void getFileData() {
    
    int i;
    FILE* fileptr;
    char line[100];
    char value[10];
    boolean isSame; 
    
     //initializes memory for 7 rooms in the roomList array 
    for(i = 0; i < MAX_ROOMS_COUNT; i++) {
        roomList[i] = (struct ROOM *)malloc(sizeof(struct ROOM)); 
    } 
    
     
    for(i = 0; i < MAX_ROOMS_COUNT; i++) {
        
        fileptr = fopen(filePath[i], "r");
        
        //check if file is open
        if (fileptr == NULL) {
            printf("open() failed on \"%s\"\n", filePath[i]); 
            exit(0); 
        }
        
        int chr = getc(fileptr);    //holds the character count inside the files
        int lineCount = 0;          //holds the line count inside the files
        
        //while all the characters have not been read
        while (chr != EOF) {
            if(chr == '\n') {       //if the character is a newline
                lineCount += 1;     //increment the lineCount variable
            } 
            
            chr = getc(fileptr); 
            
        }
        rewind(fileptr); 
        
        //printf("There are %d lines in %s\n", lineCount, filePath[i]); 
        
        //load the number of connections into the roomList
        //subtract the first line and the last line 
        roomList[i]->numConnections = (lineCount - 2); 
        
        //loads iterator variable as roomList id
        roomList[i]->id = i; 
        
        //printf("There are %d connections in %s\n", roomList[i]->numConnections, filePath[i]);
        
        int line_number = 0;
        while(!feof (fileptr)) {
            if(fgets(line, sizeof(line), fileptr) != NULL) {
                //printf("%s\n", line);
                //sscanf skips the first and second words + spaces
                sscanf(line, "%*s %*s %s", value); 
                //printf("%s\n", value); 
                
                //first line, copies the value (ROOM NAME: %s)
                if(line_number == 0) {
                    strcpy(roomList[i]->name, value);  
                }
                
                //last line, copies the value (ROOM TYPE: %s)
                else if(line_number == lineCount - 1) {
                    strcpy(roomList[i]->type, value); 
                }
                
                //copies connections (CONNECTION #: %s)
                else {
                    strcpy(roomList[i]->connections[line_number - 1], value); 
                }
                
                line_number++; 
            }
        }
        
    }
    
    /*For debugging
    int j; 
    for(i = 0; i < MAX_ROOMS_COUNT; i++) {
        printf("%s\n", filePath[i]);
        printf("ROOM NAME: %s\n", roomList[i]->name);
    
        for(j = 0; j < roomList[i]->numConnections; j++) {
            printf("CONNECTION %d: %s\n", j+1, roomList[i]->connections[j]);
        }
        
        printf("ROOM TYPE: %s\n", roomList[i]->type);
        printf("ROOM ID: %d\n", roomList[i]->id); 
    }*/
    
    
    fclose(fileptr);
        
}
/******************************************************** RUN GAME ******************************************************\
*                                                                                                                        *
*                                                                                                                        *
*                                                                                                                        *
\************************************************************************************************************************/

/*
*   Function: runGame
*   -------------------------
*   function runs game 
*/
int runGame() { 
    struct ROOM* currentLocation;       //instance of room struct
    struct ROOM* endingLocation;        //instance of room struct
    
    int i, j, k;                        //iterator variables
    
    char* checkInput = malloc(10);      //variable holds user input
    int correctInput = 1;               //variable for while loop 
    
    char winPath[50][10];
    
    for(i = 0; i < MAX_ROOMS_COUNT; i++) {
        if(strcmp(roomList[i]->type, "START_ROOM") == 0)
            currentLocation = roomList[i]; 
        
        else if(strcmp(roomList[i]->type, "END_ROOM") == 0)
            endingLocation = roomList[i];  
    }
    
    //strcpy(winPath[0], currentLocation->name); 
    
    int z = 0, pathCount = 0;       //variables used to count the path number
    
    char line[100];
    char value[10];
    FILE* fileptr;
    int err;
    
    //printf("Current Location:%d\n", currentLocation); 
    //printf("Track Location:%s\n", trackLocation[0]); 
    
    do {
        do {
            printf("CURRENT LOCATION: %s\n", currentLocation->name);

            printf("POSSIBLE CONNECTIONS: "); 
            for(i = 0; i < currentLocation->numConnections; i++) {
                printf("%s", currentLocation->connections[i]);
                if(i < currentLocation->numConnections - 1)
                    printf(", ");
                else
                    printf(".");
            }

            printf("\n");

            whereto:
            printf("WHERE TO? >");
            scanf("%s", checkInput); //scan for the users input

            //printf("%s", currentLocation->connections[j]);
            //printf("%s", checkInput); 
            
            if(strcmp(checkInput, "time") == 0) {     //if user types in "time"
                
                pthread_mutex_unlock(&lock);        //unlock mutex
                pthread_join(tid, NULL);            //joins with main thread
                pthread_mutex_lock(&lock);          //locks mutex again
                
                //if thread cannot be created
                err = pthread_create(&(tid), NULL, &createTimeFile, NULL); 
                if(err != 0) {
                    printf("\ncan't create thread: %s\n, strerror(err)"); 
                }

                fileptr = fopen("./currentTime.txt", "r");      //open currentTime text file for read access
        
                //while the file has not been read 
                while(!feof (fileptr)) {
                    if(fgets(line, sizeof(line), fileptr) != NULL) {
                        sscanf("%s", line); 
                    }
                }
                
                printf("%s\n\n", line);                 //print the value from the file
                fclose(fileptr);                        //close file stream
                
                goto whereto;
            }

            //if the user input equals the connection, break out of loop 
            else {
                for(j = 0; j < currentLocation->numConnections; j++) {
                    if(strcmp(checkInput, currentLocation->connections[j]) == 0) {
                        correctInput = 0; 
                        break;
                    }else {
                        correctInput = 1; 
                    }  
                }

                //for error checking
                if(correctInput == 1) {
                    printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
                }
                else {
                    break;  //break out of loop if correctInput is not set to 1
                }
            }

        }while(correctInput != 0);          //while loop stops when user types in correct room
        
        printf("\n");

        //find nextRoom 
        for(k = 0; k < MAX_ROOMS_COUNT; k++) {
            if(strcmp(checkInput, roomList[k]->name) == 0)
                currentLocation = roomList[k];  
        } 
        
        z++;
        pathCount = z;
        strcpy(winPath[z], currentLocation->name);       

        //printf("%s\n", currentLocation->name); 
    }while(currentLocation != endingLocation);             //while the currentLocation is NOT the END_ROOM
    
    
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:", pathCount);
    
    //prevent extra \n 
    if(pathCount > 1) {   
        for(i = 0; i < pathCount; i++) { 
                printf("%s\n", winPath[i]);
        }
        printf("%s\n", endingLocation->name);
    }
    else 
        printf("\n%s\n", endingLocation->name); 
    
    free(checkInput);   //free memory
}


/******************************** MAIN FUNCTION ********************************/
int main() {
    char* directory = getDirectory();   //variable holds the results of the getDirectory function
    
    //printf("%s\n", directory); 
    
    readsDirectory(directory);  //call the readsDirectory function, pass in the most recently modified directory
    
    getFileData();              //call getFileData
    
    int err;
    
    //if pthread cannot be initialized
    if(pthread_mutex_init(&lock, NULL) != 0) {
        printf("mutex failed to initialize\n"); 
        return 1;
    }
    
    pthread_mutex_lock(&lock);  //lock mutex
    
    //if thread cannot be created
    err = pthread_create(&(tid), NULL, &createTimeFile, NULL); 
    if(err != 0) {
        printf("\ncan't create thread: %s\n, strerror(err)"); 
    }
    
    runGame();                      //call runGame function
    
    pthread_cancel(tid);                //cancel tid
    pthread_mutex_destroy(&lock);       //destroy lock 

    return(0);
}
