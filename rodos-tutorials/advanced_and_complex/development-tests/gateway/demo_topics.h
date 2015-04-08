
/** Example topics for the tutorials **/

#ifndef __demotopics_h__
#define __demotopics_h__

struct Position {
	float x, y, z;
};


extern Topic<long,   10>   	counter1;
extern Topic<long,   30>    counter2;
extern Topic<Position, 50>  position;


#endif

