#pragma once



class Activity1PPS : public ListElement {
public:
    static List activityList;

    int64_t suspendedUntil;

    int     frequency; // executions per second
    int64_t offsetFrom1PPs;
    Thread*  associatedThread;


    /** Restrictions:
     *   Each Activity1PPS shall be executed at lease once per second
     *   The offset from 1PPS may not be greater then 1*SECOND/frequency
     */
    Activity1PPS(int freq, int64_t offset, Thread* associatedThreadP = 0, const char* name = "anonymActiviy")
            : ListElement(activityList, name) {
        offsetFrom1PPs = offset;
        frequency = freq;
        this->associatedThread = associatedThreadP;
    }
    ~Activity1PPS() { ERROR("activity deleted"); }

    virtual void init() { }
    virtual void step(int64_t timeNow) { }
};

