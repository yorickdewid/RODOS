#include "rodos.h"
#include "random.h"

static Application module("semaphoretest");

#define MAX_X 30
#define MAX_Y 30
#define RIGHT_LIMIT 20

Semaphore lowerWindow;
Semaphore printProtector;

bool occupied[MAX_X][MAX_Y] = {{0,0,0,0,0,0}, {0,0,0}} ;

void MAIN() {
	  PRINTF(SCREEN_CLEAR);
	  for(int i = 0; i < MAX_X; i++) {
		  PRINTF(SCREEN_MOVE_CURSOR_TO, i, RIGHT_LIMIT+1);
		  PRINTF(":");
	  }
}

static char wormLetter = 'A';

class worm : public Thread {
	int x, y, speed_x, speed_y;
	char letter;
public:
	worm() {
		x = randomTT800Positive() % RIGHT_LIMIT;
		y = randomTT800Positive() % MAX_Y +1 ;
		speed_x = (randomTT800Positive() % 3) -1;
		speed_y = (randomTT800Positive() % 3) -1;
		letter = wormLetter++;
	}

  void run(){

	int last_x, last_y;
    while (1) {
    	occupied[x][y] = false;
    	last_x = x;
    	last_y = y;
	printProtector.enter();
    	PRINTF(SCREEN_MOVE_CURSOR_TO, y, x);
    	if(x == RIGHT_LIMIT+1) PRINTF(":"); else PRINTF(" ");
	printProtector.leave();
    	bool inUpperSide = x <= RIGHT_LIMIT;
    	bool inLowerSide = x >  RIGHT_LIMIT;


	// Change speed randomly
	if(randomTT800Positive() % 10 < 2) {
 	    speed_x = (randomTT800Positive() % 3) -1;
            speed_y = (randomTT800Positive() % 3) -1;
	    if(speed_x == 0 && speed_y == 0) speed_y = 1;
	}


    	if(x >= MAX_X) speed_x = -1;
    	if(y >= MAX_Y) speed_y = -1;
    	if(x <= 1) speed_x = 1;
        if(y <= 1) speed_y = 1;
    	x += speed_x;
    	y += speed_y;

        if(occupied[x][y]) {
        	x = last_x;
        	y = last_y;
        }
        occupied[x][y] = true;

	printProtector.enter();
    	PRINTF(SCREEN_MOVE_CURSOR_TO, y, x);
    	PRINTF("%c", letter);
    	PRINTF(SCREEN_MOVE_CURSOR_TO, 0, 0);
	printProtector.leave();

        if(inUpperSide && x >  RIGHT_LIMIT) lowerWindow.enter();
        if(inLowerSide && x <= RIGHT_LIMIT) lowerWindow.leave();

    	suspendCallerUntil(NOW() + 100*MILLISECONDS);
    }
  }
};


worm wrms[10];





