#include "rodos.h"
#include "topics.h"
#include "math.h"

class SpeedCalc :  public Subscriber {
public:
    SpeedCalc() : Subscriber(position, "SpeedCalc") { }
    Pos p0,p1;
    long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
        p0=p1;
        p1=*(Pos*)data;
        double v = sqrt((p0.x-p1.x)*(p0.x-p1.x)+(p0.y-p1.y)*(p0.y-p1.y)+(p0.z-p1.z)*(p0.z-p1.z));
        speed.publish(v);
        return 1;
    }
} speedCalc;

