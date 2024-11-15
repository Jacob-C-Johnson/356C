War Card Game Simulation with PThreads and Sockets 

This program simulates a card game with main acting as the parent to two children threads.
Main singals the cards when to draw and when to quit, the threads will wait for instruction before and after draw.

How To Build: The program uses a make file
- run makefile using make command in terminal

How to run: The program requires a single command-line argument specifying the number of rounds to play. For example:
- run ./war_networked 5

Example output:

Child 1 PID: 140448941668032
Child 2 PID: 140448933275328

Beginning 5 Rounds…

Fight!
---------------------------

Round 1:

Child 1 draws Jack 

Child 2 draws 7 

Child 1 Wins!

---------------------------

Round 2:

Child 1 draws 10 

Child 2 draws 3 

Child 1 Wins!

---------------------------

Round 3:

Child 1 draws 10 

Child 2 draws Queen 

Child 2 Wins!

---------------------------

Round 4:

Child 1 draws Jack 

Child 2 draws Ace 

Child 2 Wins!

---------------------------

Round 5:

Child 1 draws Queen 

Child 2 draws 3 

Child 1 Wins!

---------------------------

---------------------------

Results:
 
Child 1: 3 
 
Child 2: 2 

Child 1 Wins!