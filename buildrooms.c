/***************************************************************
FILENAME: irwinbre.buildrooms.c
AUTHOR: Brent Irwin
PURPOSE: Program 2 - CS 344
DATE: 12 May 2017
DESCRIPTION: Creates a series of files that hold descriptions
	of the in-game rooms and how the rooms are connected.
***************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void randomFill(int[], int, int, int);
void makeRoom(int*[], const char*[], int, int);

int main(){

	// Seed srand(). Should only be done once
	srand(time(NULL));

	// List 10 names in an array
	const char *names[10];
	names[0] = "cellar";
	names[1] = "kitchen";
	names[2] = "ballroom";
	names[3] = "conservatory";
	names[4] = "billiard room";
	names[5] = "library";
	names[6] = "study";
	names[7] = "hall";
	names[8] = "lounge";
	names[9] = "dining room";

	// Create matrix of connections
	int *rooms[7];
	int room0[7];
	int room1[7];
	int room2[7];
	int room3[7];
	int room4[7];
	int room5[7];
	int room6[7];
	rooms[0] = room0;
	rooms[1] = room1;
	rooms[2] = room2;
	rooms[3] = room3;
	rooms[4] = room4;
	rooms[5] = room5;
	rooms[6] = room6;

	// Fill with -1
	int i, j, k;
	for (i=0; i<7; i++) {
		for (j=0; j<7; j++) {
			rooms[i][j] = -1;
		}
	}

	// Create "temp" array of length 7 and fill it up
	int temp[7];
	randomFill(temp, 0, 6, 6);

	// Assign temp[x] to roomx[0] and save values to index
	for (i=0; i<7; i++) {
		rooms[i][0] = temp[i];
	}
	
	// Fill up the rest of the arrays
	for (i=0; i<7; i++) {
		// Flush temp
		for (j=0; j<7; j++) {
			temp[j] = -1;
		}

		// Assign the current room number to temp[0]
		temp[0] = rooms[i][0];

		// Check for previous rooms that have linked to this room
		// If they have, link back to them
		// And increment the position variable
		int pos = 1;
		for (j=0; j<i; j++) {
			for (k=0; k<7; k++) {
				if (rooms[j][k] == i) {
					temp[pos] = rooms[j][0];
					pos++;
				}
			}
		}

		// Choose a random number 3-6 rooms to link to
		// If there are already more than 3 rooms linked,
		// That minimum value should change accordingly
		int links = 0;
		int min = 3;
		if (pos > 3)
			min = pos;
		// Doing a modulus of 7-min caused an error when pos==7
		// So if pos==7, all the links are alreayd full, and you
		// can just skip the random fill part
		if (pos != 7) {
			links = min + rand() % (7 - min);

			// Call randomFill()
			randomFill(temp, pos, links, 6);
		}
		// Copy temp[] to room[]
		for (j=0; j<7; j++) {
			rooms[i][j] = temp[j];
		}
	}

	// Currently all of our arrays are 0-6
	// Create an index so 0-6 maps to 0-9 and change all the numbers
	int index[7];
	randomFill(index, 0, 6, 9);
	for (i=0; i<7; i++) {
		for (j=0; j<7 && rooms[i][j] != -1; j++) {
			 rooms[i][j] = index[rooms[i][j]];
		}
	}
/*
	// TEST
	for (i=0; i<7; i++) {
		for (j=0; j<7; j++) {
			printf("%d ", rooms[i][j]);
		}
		printf("\n");
	}
*/
	// Create files based on matrix
	// Create directory called irwinbre.rooms.$$
	int pid = getpid();
	char dir[24];
	sprintf(dir, "irwinbre.rooms.%d", pid);
	mkdir(dir, 0700);

	// For each of the 7 rooms, make the file
	for (i=0; i<7; i++) {
		makeRoom(rooms, names, i, pid);
	}
	
return 0;
}

/***************************************************************
 			void randomFill(int[], int, int)

 randomFill() fills an array with distinct, random integers 0-n.

INPUT:
	int arr[]	The array to fill (passed by reference obviously)
	int a		The position in the array to begin
	int z		The position in the array to end
	int n		The highest possible number to generate

OUTPUT:
	void - n/a
***************************************************************/
void randomFill(int arr[], int a, int z, int n)
{
	int i, j, cur;
	int exists = 0;

	// For each item in the array that needs to be filled
	for (i=a; i<=z; i++) {
		// Generate a new number 0-6 and make sure it isn't already used
		// Try again if it has already been used
		do {
			exists = 0;
			cur = rand() % (n+1);
			for (j=0; j<i; j++) {				
				if (cur == arr[j]) {
					exists = 1;
					break;
				}
			}
		} while (exists);
		// Once a unique one is found, assign it to the next spot
		// And move on
		arr[i] = cur;
	}
}

/***************************************************************
			void makeRooms(int*[], const char*[])

makeRooms() creates files based on the values in the arrays

INPUT:
	int* rooms[]		The matrix of room connections
	const char* names[]	The names of the rooms by number
	int room			The #th room being created
	int pid				The process ID

OUTPUT:
	void - n/a
***************************************************************/

void makeRoom(int* rooms[], const char* names[], int room, int pid) {

	// Set the proper ROOM TYPE
	char* roomType;
	if (room == 0)
		roomType = "START_ROOM";
	else if (room == 6)
		roomType = "END_ROOM";
	else
		roomType = "MID_ROOM";

	// Create a new file
	char file[32];
	sprintf(file, "irwinbre.rooms.%d/%d.room", pid, room);
	FILE * newRoom = fopen(file, "w+");

	// Write ROOM NAME
	fprintf(newRoom, "ROOM NAME: %s\n", names[rooms[room][0]]);

	// Write CONNECTIONS
	int i = 1;
	while (rooms[room][i] != -1 && i<7) {
		fprintf(newRoom, "CONNECTION %d: %s\n", i, names[rooms[room][i]]);
		i++;
	}

	// Write ROOM TYPE
	fprintf(newRoom, "ROOM TYPE: %s\n", roomType);

	// Close the file
	fclose(newRoom);

}
