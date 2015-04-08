
#include "rodos.h"

#include "demo_topics.h"


Topic<long,   10>    counter1("counter1");
Topic<long,   30>    counter2("counter2");
Topic<Position, 50>  position("position");

Topic<long, CHARS2INT('1', '3')> setCahr("SetChar");
Topic<long, CHARS2INT('1', '2')> echo("echo");

