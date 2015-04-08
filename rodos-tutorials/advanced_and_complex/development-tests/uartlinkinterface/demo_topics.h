
/** Example topics for the tutorials **/

#ifndef __demotopics_h__
#define __demotopics_h__

struct Position {
	float x, y, z;
};

// TODO: most significant first! -> Invert this
#define CHARS2INT(a,b) (((a)<<8) + (b))

extern Topic<long,   10>   	counter1;
extern Topic<long,   30>    counter2;
extern Topic<Position, 50>  position;


extern Topic<long, CHARS2INT('1', '3')> setCahr;
extern Topic<long, CHARS2INT('1', '2')> echo;


#endif

