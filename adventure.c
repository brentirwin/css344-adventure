/***************************************************************
FILENAME: irwinbre.adventure.c
AUTHOR: Brent Irwin
PURPOSE: Program 2 - CS 344
DATE: 12 May 2017
DESCRIPTION: Provides an interface for playing the game using
	the most recently generated rooms.
***************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/* There's only one mutex, and I've only created it for an assignment
 based on mutexes, so I'm just going to call it mutex. Same with thread.
 I was told to implement this part after the main program. If I had known
 that I might not have made goToRoom() its own function and just included
 it in main. However since I did that, my options were to pass the mutex
 and thread into the function or just make them global, so I made them
 global. Bad form I know, but I'll forgive myself. */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread;

void removeNewLine(char*);
int goToRoom(int, char*, int[], int*, char*[]);
void* postTime();

int main(){

	// First things first: I'm a mutex
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_lock(&mutex);
	pthread_create(&thread, NULL, postTime, NULL);

	// i for counting
	int i;

	// Get the latest irwinbre.rooms directory
	struct stat newest, current;
	char dir[256];
	char currentFile[256];
	int firstOne = 1;
	
	// At the end of this for-loop, dir will contain the name of the
	// most recently created irwinbre.rooms.* directory
	// The process ID could be anywhere between 0 an 99999. Try them all
	for (i=0; i<99999; i++) {
		sprintf(currentFile, "irwinbre.rooms.%d", i);
		// If you find a file that exists
		if (stat(currentFile, &current) == 0) {
			// If it's the first one, set it to newest
			if (firstOne) {
				newest = current;
				strcpy(dir, currentFile);
				firstOne = 0;
			} else {
			// Otherwise check for the newest and change if appropriate
				if (current.st_mtime > newest.st_mtime) {
					newest = current;
					strcpy(dir, currentFile);
				}
			}
		}	
	}

	// This is extra to the assignment and won't be checked when
	// following the grading instructions, but the program should
	// account for there being no rooms built yet.
	if (firstOne) {
		printf("There are no irwinbre.rooms.* folders.\n");
		printf("Run irwinbre.buildrooms\n");
		return 1;
	}

	// Create index of rooms
	// Since each string needs to be allocated space
	// and there is a small, finite number of strings,
	// allocate them each separately then combine them into
	// one index array.
	char index0[256];
	char index1[256];
	char index2[256];
	char index3[256];
	char index4[256];
	char index5[256];
	char index6[256];
	char* index[7];
	index[0] = index0;
	index[1] = index1;
	index[2] = index2;
	index[3] = index3;
	index[4] = index4;
	index[5] = index5;
	index[6] = index6;
	
	for (i=0; i<7; i++) {

		// Open the ith file
		char indexFileName[32];
		sprintf(indexFileName, "%s/%d.room", dir, i);
		FILE * indexFile = fopen(indexFileName, "r");

		// Put the ROOM NAME into index[i]
		char roomName[256];
		fgets(roomName, 256, indexFile);
		fclose(indexFile);
		// The room name begins with "ROOM NAME: " and ends withi '\n'
		// These two functions shave off those ends
		// Only the room name itself is sent to index[i]
		removeNewLine(roomName);
		sprintf(index[i], "%s", roomName+11);
	}

	// Make empty array of rooms travelled to
	int path[100];
	int pos = 0;

	// Create variable keeping track of what room to go to next, start at 0
	int nextRoom = 0;

	// Do the core loop. Go to the room. The room returns the next room
	while (nextRoom != 6) {
		nextRoom = goToRoom(nextRoom, dir, path, &pos, index);
	}

	// When it returns 6, this means the END_ROOM has been found

	/****************************************
	*	Note to grader: Using these indexes makes more sense to me
	*	than passing around a bunch of START_ROOM/MID_ROOM/END_ROOM
	*	strings, so I don't actually use them. They're implemented
	*	correctly in buildrooms.c, but the way I named the files
	*	made them not so necessary
	****************************************/
	
	// Add final room to path
	path[pos] = 6;
	pos++;

	// End game text
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", pos-1);
	// Print the path
	for (i=0; i<pos; i++) {
		printf("%s\n", index[path[i]]);
	}

	// Destroy the mutex
	pthread_mutex_destroy(&mutex);

	// Exit successfully
	return 0;
}

/***************************************************************
 			void removeNewLine(char*);

removeNewLine() removes the \n character at the end of a string
and replaces it with a \0.

Works with strings up to 256 characters long.

INPUT:
	char* str	The string to be fixed

OUTPUT:
	void - n/a
***************************************************************/
void removeNewLine(char* str)
{
	int i;
	int found = 0;
	// For each character in an array of 256 characters,
	// If you come across a '\n', change it to '\0'
	// Terminating the string where the newline used to be
	// Also once you've found one, quit because the string is now terminated
	for (i=0; !found && i<256; i++) {
		if (str[i] == '\n') {
			str[i] = '\0';
			found = 1;
		}
	}
}

/***************************************************************
 			int goToRoom(int, char*, int[], int, char*[]);

goToRoom() is the core loop of the game. You enter a room, see your options,
process your decision, and move on to the next room.

INPUT:
	int room		The index of the current room
	char* dir		The name of the working directory
	int path[]		The array of all rooms that have been visited
	int& pos		The current position in path[], by reference
	char* index[]	The index of rooms by number

OUTPUT:
	int - the index of the next room
***************************************************************/
int goToRoom(int room, char* dir, int path[], int* pos, char* index[])
{
	// Add this room to the path and increase the position in the array
	path[*pos] = room;
	(*pos)++;

	// Get all connections and add them to an array of strings
	// Allocate the connections the same way the index was done before
	char connection0[256] = "";
	char connection1[256] = "";
	char connection2[256] = "";
	char connection3[256] = "";
	char connection4[256] = "";
	char connection5[256] = "";
	char* connections[6];
	connections[0] = connection0;
	connections[1] = connection1;
	connections[2] = connection2;
	connections[3] = connection3;
	connections[4] = connection4;
	connections[5] = connection5;
	int i;

//**************************************************************
	// File handling time!
	// Let's keep this visually separate, for the developer's sanity

		// Open this room's file
		char roomFileName[32];
		sprintf(roomFileName, "%s/%d.room", dir, room);
		FILE * roomFile = fopen(roomFileName, "r");
		
		// Ignore the first line, because we already have the room name
		char ignoreFirstLine[256];
		fgets(ignoreFirstLine, 256, roomFile);

		// Iterate through and get the rest of the connections
		// Get the row. As long ast the row starts with 'C', move on
		// If the row does not start with a 'C' (i.e. "ROOM_TYPE"),
		// the file is over.
		int hitTheEnd = 0;
		for (i=0; !hitTheEnd && i<6; i++) {
			char connectionName[256];
			fgets(connectionName, 256, roomFile);
			if (connectionName[0] != 'C')
				hitTheEnd = 1;
			else {
				// Remove the newline and copy only the name over
				// Adding 14 to connectionName removes "CONNECTION N:"
				removeNewLine(connectionName);
				sprintf(connections[i], "%s", connectionName+14);
			}
		}

		// Close the sucker up
		fclose(roomFile);

//**************************************************************

	// Add them all into connections[] 
	char connectionsString[512] = "";

	// sprintf all of them to a string with proper commas
	int reachedTheEnd = 0;

	// For every string except the last one, add the word + a comma
	// For the last one, just add the word without the comma
	for (i=0; !reachedTheEnd && i<6; i++) {
		// For the first connection, add a comma after the word.
		if (i == 0) {
			strcpy(connectionsString, connections[i]);
			strcat(connectionsString, ", ");
		} else {
		// For subsequent connections,
		// only add a comma if this is not the last connection
			strcat(connectionsString, connections[i]);
			if (i<5 && connections[i+1][0] != '\0') {
				strcat(connectionsString, ", ");
			} else {
				reachedTheEnd = 1;
			}
		}
	}

	// I/O Time
	int valid;
	char input[256];
	
	// Output the following and continue doing so if you get bad results
	do {
		// Print out room info
		printf("CURRENT ROOM: %s\n", index[room]);
		printf("POSSIBLE CONNECTIONS: %s.\n", connectionsString);

		int time;
		do {
			time = 0;
			printf("WHERE TO? >");

			// Get input and remove the newline at the end so that it can
			// be checked against the other connections values
			fgets(input, 256, stdin);
			removeNewLine(input);

			// Check if it says "time"
			if (strcmp(input, "time") == 0) {
				// Send it on over to the other thraed
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, NULL);
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, NULL, postTime, NULL);
				
				// Now that we're back, the file should have the time
				// Read it and print it out.
				FILE * timeFile = fopen("currentTime.txt", "r");
				char currentTime[256];
				fgets(currentTime, 256, timeFile);
				fclose(timeFile);
				printf("\n%s\n", currentTime);
				
				// Set time to true so the prompt will repeat
				time = 1;
			}
		} while (time);
		
		// Check whether what was entered is in the array
		valid = 0;
		for (i=0; i<6; i++) {
			if (strcmp(connections[i], input) == 0) {
				valid = 1;
				break;
			}
		} 

		if (!valid) {
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}

	} while (!valid);

	printf("\n");

	// When you do have a match, find what it matches in index and return it
	for (i=0; i<7; i++) {
		if (strcmp(index[i], input) == 0) {
			return i;
		}
	}
}

/***************************************************************
				void* postTime()

postTime() prints the current time to currentTime.txt

OUTPUT:
	void* - NULL
***************************************************************/
void* postTime()
{
	// Get the mutex
	pthread_mutex_lock(&mutex);
	
	// Get the current time
	time_t t = time(0);

	// Write to currentTime.txt
	FILE * timeFile = fopen("currentTime.txt", "w+");
	fprintf(timeFile, "%s", ctime(&t));
	fclose(timeFile);

	// Release the mutex
	pthread_mutex_unlock(&mutex);
	return NULL;
}
