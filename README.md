Hotel Reservation Desk In Parallel
Programmer: Alex Hatzenbuhler
NDSU Class: Operating Systems Concepts (CSCI 474

This project was one of the most fun projects I have done in my entire Computer Science education at NDSU. It was a challenging project that tested my skills as a programmer, and really forced me to understand exactly what every line of code was doing. I started this program by setting up the threads and the required thread methods. I also created all the semaphores and global variables that I would need for the entire project. I worked with the 8 semaphores that we discussed in class. Below is a table discussing the semaphores that I use in my program:

Semaphore		Function									Initial Value
_______________|___________________________________________|_____________
openRooms		Room capacity								5
checkInLine		One person at check-in at a time			1
checkIn1		Used for guest to go to check-in desk		0
checkIn2		Used for guest to receive room number		0
CheckOutLine	One person at checkout at a time			1
checkOut1		Used for guest to go to checkout desk		0
checkOut2		Used for guest to receive amount due		0
checkOut3		Used for guest to pay amount				0

Once I had my semaphores and threads in place, it was time to get started on the logic. This was the hardest part of the project. I drew out the guest threads alongside the check-in and checkout thread to see how the back and forth of semaphores would work. I went through a few iterations of my logic before I found the correct sequence. Once I had the starts and stops correct, I needed to figure out the best way to pass information between the threads. I ended up using global variables for most of the information in my program. This allows all of the threads to access information such as what guest has which room, and what the balance of that room is. To store this information I used arrays, where the index of the array correlates to the guest. So guest 0’s room is stores in guestsRoom[0] and their balance is stored in roomBalance[0].  

__________Difficulties & Lessons Learned__________
One issue that I had while creating this program is I wanted the output to be very clean. An example of this is never having a guest 0, and having guest 1-10 instead. To do this I declare variable: 

	int guestNumber = my_rank + 1; 

This allows me to print off a clean guest number. I had an issue with this at one point where I was using the guestNumber to access information in guestsRoom[], and it was returning invalid, since there is no guestsRoom[10]. I also have a similar set up with the room numbers and room[]. A similar issue I had was in the Checkout_thread, where I used the currentRoomOut and currentGuestOut global variables to get information from the guest thread. These variables were on the 1-10 scale, instead of the index. Inside my Checkout_thread method I needed to gather the indexes for both of these variables, so I could correctly access information in the global arrays.

Another small issue I had was with using the rand() function. I found that when I ran my program over and over again, I would get the same number of guests going to each activity, and sleeping for the same amount of time. Upon some research from the Internet, I found that I have to generate a random seed to use for the rand() method. I solved this issue with the following line of code in my main method:

	srand(time(NULL));

A third issue that I ran into was with cout. When using cout, every time you have a “<<” operator, it acts as a new function being sent to the console. Because of how this works, all of my threads were hitting the first print statement at the same time. When executing my program using cout, I would see the output as something like:

	guestguestguest321 waits waitsfor waitsforforcheck-in

I did some more research and found the printf adds all of the elements in the print statement and adds it to a buffer, which is sent to the console. By using printf instead of cout, I was able to solve the issue of pieces being printed inside each other. I toyed with the solution of using semaphores to ensure that only one statement could be printed at a time, but this would have been a much more complicated solution. 

__________Results__________
When running the program, each step of the process is printed in order, the order for each guest X with room Y is:
	
	Guest X waits for check-in.
	Guest X goes to the check-in reservationist.
	Guest X receives room Y and completes check-in. 

	(ONE OF THE FOLLOWING)
	Guest X goes to hotel pool.
	Guest X goes to hotel restaurant.
	Guest X goes to hotel fitness center.
	Guest X goes to hotel business center.

(Guest then sleeps for a random amount of time from 1-3 seconds. This step is not part of the visible output.)
	
Guest X goes to the checkout reservationist and returns room Y.
Guest X receives the total balance of $Z.
Guest X makes a payment.
The program also prints out each step of the check-in reservationists process:

	The check-in reservationist greets Guest X.
	Assign room Y to Guest X.

The program also prints out each step of the checkout reservationists’ process:

The checkout reservationist greets Guest X and receives the key from room Y.
Calculate the balance for Guest X.
Receive $Z from Guest X and complete the checkout.

The output also counts the total number of guests, and the number of guests that went through each activity. Here is an example of this output:


__________Final Notes__________
I have this written inside of my code, but I will mention it here in my summary as well. I ran into this error during testing my project, and it seems to be fairly rare. When compiling the program, there will be one room that is booked 3 times, and another that is only booked once. I believe this is die to the random seed having one guest sleeping for 1 second, and another guest sleeping for 3 seconds. The guest that runs for 1 second is the first or second guest to go through the check-in process, and the guest that sleeps for 3 seconds is the last guest to go through. Due to these rare circumstances one room is booked and unbooked twice and booked again, before the other room is booked and unbooked. A solution to this issue is to remove the project.cpp file, recreate it, and recompile it. As I mentioned, this seems to be a rare circumstance dealing with the random seed and the rand() function. I have gone through the my program many times and it doesn’t seem to be an issue with the logic, but more of a coincidental circumstance.

Overall, this project was a great learning experience, and I had a lot of fun piecing together everything from this semester into one big project. 
