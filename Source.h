// Giacomo Sguotti 10667547 - SPTF Scheduling
// Source.h

#ifndef __SPTFSCHEDULING_SOURCE_H_
#define __SPTFSCHEDULING_SOURCE_H_

#include <omnetpp.h>
#include <ModifiedMessage_m.h>

using namespace omnetpp;

class Source : public cSimpleModule
{
    public:
        Source();
        virtual ~Source();
    private:
        cMessage *sendMessageEvent;
        int nbGenMessages; // msg counter
        double avgInterArrivalTime; // lambda

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

#endif
