
/*
	Alex Hatzenbuhler
	Operating Systems Project 2

	Compile command:
		g++ -g -Wall -o project project.cpp -lpthread

	Run command:
		./project


	note:
	on rare occasions, when you compile the program there will be one room
	that is booked 3 times, and another room that is only booked once. This seems
	to be due to the random seed having one guest sleeping for 1 second, and another
	for 3 seconds. The guest that runs for 1 second is the first or second guest, while
	the guest that sleeps for 3 seconds is the last guest to go through. Due to this,
	one room is booked and unbooked twice and booked again before the other room
	is booked and unbooked.

	TO SOLVE THIS ISSUE:
		rm project.cpp

	Then add the file to your directory again and recompile. This should fix the issue.
	Like I mentioned, this is a rare occurance, but something I noticed during testing.

*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <time.h>

using namespace std;

//8 semaphores to control program in threads
sem_t openRooms; 		//number of open rooms
sem_t checkInLine;		//only one person at check in desk at a time - mutex
sem_t checkIn1;			//used for check-in desk to wait for guest to arrive
sem_t checkIn2;			//used for guest to wait for check-in desk to find an open room
sem_t checkOutLine; 	//only one person at check out desk at a time - mutex
sem_t checkOut1;		//used for checkout desk to wait for guest to arrive
sem_t checkOut2;		//used for guest to wait for checkout desk to calculate balance
sem_t checkOut3;		//used for checkout desk to wait for payment from guest


//global variables
int guests = 10;		//number of guests going into the hotel. Used throughout program
bool room[5];			//used to know which rooms are open and which are not. true if open
int roomCount;			//size of array "room", used in for loops
int guestsRoom[10];		//saves the room each guest had
int roomBalance[10];	//saves the amount of the room for each guest

int currentRoomIn;		//used to determine which room is open, and give that room to guest
int currentRoomOut;		//used to determine which room is being used, and to open it
int currentGuestIn; 	//used to tell check-in desk who is currently there
int currentGuestOut;	//used to tell checkout desk who is currently there

//number of guests doing activities. Used for final activity counts.
int totalGuests = 0;
int pool = 0;
int restaurant = 0;
int fitnessCenter = 0;
int businessCenter = 0;

//declare methods for different threads
void* Guest_thread(void* rank);
void* Checkin_thread(void* rank);
void* Checkout_thread(void* rank);

//main
int main(){

	//make thread variables
	pthread_t*	thread_handles;
	long 		thread;
	long 		thread_count;

	//total number of threads
	thread_count = guests + 2;		//guests is a global variable, then add 1 for each desk

	//allocate memory for threads
	thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));

	//initialize semaphores
	sem_init(&openRooms, 0, 5);
	sem_init(&checkInLine, 0, 1);
	sem_init(&checkIn1, 0, 0);
	sem_init(&checkIn2, 0, 0);
	sem_init(&checkOutLine, 0, 1);
	sem_init(&checkOut1, 0, 0);
	sem_init(&checkOut2, 0, 0);
	sem_init(&checkOut3, 0, 0);

	//find the number of rooms. Used in for loops to create and check for open rooms
	roomCount = (sizeof(room)/sizeof(*room));

	srand(time(NULL));		//create random seed for rand()

	//open all of the rooms
	for(int i = 0; i < roomCount; i++)
		room[i] = true;

	//Create threads

	//Guest_thread (threads 1-10 | 0-9 in index)
	for(thread = 0; thread < guests; thread++)
		pthread_create(&thread_handles[thread], NULL, Guest_thread, (void*)thread);

	//Checkin_thread (thread 11 | thread 10 in index)
	pthread_create(&thread_handles[10], NULL, Checkin_thread, (void*)thread);

	//Checkout_thread (thread 12 | thread 11 in index)
	pthread_create(&thread_handles[11], NULL, Checkout_thread, (void*)thread);

	//close threads (join)
	for(thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);

	//destory semaphores
	sem_destroy(&openRooms);
	sem_destroy(&checkInLine);
	sem_destroy(&checkIn1);
	sem_destroy(&checkIn2);
	sem_destroy(&checkOutLine);
	sem_destroy(&checkOut1);
	sem_destroy(&checkOut2);
	sem_destroy(&checkOut3);

	//close the thread handle, free the allocated memory
	free(thread_handles);

	//print final statements
	printf("\n--------------------------------------------------\n");
	printf("FINAL CUSTOMER COUNTS\n");
	printf("Total Guests: %d\n", totalGuests);
	printf("Pool: %d\n", pool);
	printf("Restaurant: %d\n", restaurant);
	printf("Fitness Center: %d\n", fitnessCenter);
	printf("Business Center: %d\n", businessCenter);

	return 0;
}

//thread for guests
void* Guest_thread(void* rank){

	//each guest will have different information, defined by variables
	long my_rank = (long) rank;			//gets rank from thread creation
	int guestNumber = my_rank + 1;		//makes all guests positive, from 1-10
	int roomNumber;						//room number of guest
	int totalBalance;					//balance of the room for guest to pay
	int randomActivity = rand() % 4; 	//random number from 0-3 to choose activity
	int randomSleep = rand() % 3 + 1;	//random number from 1-3 for seconds of sleep

	//------------------------------CHECK IN------------------------------//

	sem_wait(&openRooms); 				//waits for there to be an open room at the hotel
	printf("Guest %d waits for check-in.\n", guestNumber);

	sem_wait(&checkInLine);				//waits for an open spot at the check-in desk
	printf("Guest %d goes to the check-in reservationist.\n", guestNumber);

	currentGuestIn = guestNumber;		//tells check-in desk who is at the desk

	sem_post(&checkIn1);				//tells check-in desk that they are at the desk
	sem_wait(&checkIn2);				//waits for room number from check-in desk

	roomNumber = currentRoomIn;			//sets room number to currentRoomIn from check-in
	guestsRoom[my_rank] = roomNumber;	//sets the guest room number in array
	printf("Guest %d recieves room %d and completes check-in.\n", guestNumber, roomNumber);

	sem_post(&checkInLine);

	//------------------------------ACTIVITY AND SLEEP------------------------------//

	//switch statement to output what activity the guest does
	//print what the guest does, then increment the count for that activity
	switch(randomActivity){
		case 0:
			printf("Guest %d goes to hotel pool.\n", guestNumber);
			pool++;
			break;
		case 1:
			printf("Guest %d goes to hotel restaurant.\n", guestNumber);
			restaurant++;
			break;
		case 2:
			printf("Guest %d goes to hotel fitness center.\n", guestNumber);
			fitnessCenter++;
			break;
		case 3:
			printf("Guest %d goes to hotel business center.\n", guestNumber);
			businessCenter++;
			break;
	}

	//sleep for random amount of time
	sleep(randomSleep);

	//------------------------------CHECK OUT------------------------------//

	sem_wait(&checkOutLine); 			//waits for an open spot at the checkout desk
	printf("Guest %d goes to the checkout reservationist and returns "
						"room %d.\n", guestNumber, roomNumber);

	currentRoomOut = roomNumber;		//tells checkout desk what room the guest was in
	currentGuestOut = guestNumber;		//tells checkout desk who is at the desk

	sem_post(&checkOut1);				//tells checkout desk that they are at the desk
	sem_wait(&checkOut2);				//waits for balance from checkout desk

	totalBalance = roomBalance[my_rank];	//sets totalBalance for current guest

	printf("Guest %d recieves the total balance of $%d.\n", guestNumber, totalBalance);
	printf("Guest %d makes a payment.\n", guestNumber);

	sem_post(&checkOut3);			//tells checkout desk that payment has been made

	//increment the number of guests for final output of number of guests
	totalGuests++;

	return 0;
}

//thread for checkin
void* Checkin_thread(void* rank){

	//for loop so the thread will run 10 times, once for each guest
	for(int i = 0; i < guests; i++){

		sem_wait(&checkIn1);		//waits for guest to arrive at check-in desk
		printf("The check-in reservationist greets Guest %d.\n", currentGuestIn);

		//run through the rooms, find the first open room and set it as currentRoomIn
		for(int j = 0; j < roomCount; j++){
			if(room[j] == true){
				//set the guests room to the current open room | 1-5
				currentRoomIn = j + 1;
				printf("Assign room %d to Guest %d.\n", currentRoomIn, currentGuestIn);

				//set the room to false so another guest can't use it
				room[j] = false;
				break;
			}
		}
		sem_post(&checkIn2);		//tells guest a room has been assigned and can leave
	}

	return NULL;
}

//thread for checkout
void* Checkout_thread(void* rank){

	//for loop so the thread will run 10 times, once for each guest
	for(int i = 0; i < guests; i++){
		sem_wait(&checkOut1);		//waits for guest to arrive at checkout desk
		printf("The checkout reservationist greets Guest %d and recieves the" 
					" key from room %d.\n", currentGuestOut, currentRoomOut);

		//subtract 1 from variables, create index for arrays to use
		int roomIndex = currentRoomOut - 1;
		int guestIndex = currentGuestOut - 1;

		//opens room up so another guest can use it
		room[roomIndex] = true;

		printf("Calculate the balance for Guest %d.\n", currentGuestOut);

		//create an arbitrary balance for the room - room * 100
		roomBalance[guestIndex] = currentRoomOut * 100;

		sem_post(&checkOut2);		//tells guest that they have tshe balance
		sem_wait(&checkOut3);		//waits for guest to make payment

		printf("Recieve $%d from Guest %d and complete the checkout.\n",
							roomBalance[guestIndex], currentGuestOut);

		sem_post(&checkOutLine); 	//tells next guest there is an open spot at checkout
		sem_post(&openRooms);		//tells next guest that there is an open room
	}

	return NULL;
}