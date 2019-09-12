#include<ncurses.h>


WINDOW * mainwin, * listing, * input;


/*
Initialises the display for the emulator


*/
void initui(){

	if((mainwin = initscr()) == NULL )
		goto err;

	noecho();


	return;

	err:
		fprintf(stderr, "Error setting up terminal interface");
		exit(0);

}



/*
ends the terminal interface freeing the resources associated with it


*/
void endui(){



	endwin();


}