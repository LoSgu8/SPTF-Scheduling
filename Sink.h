#ifndef __SPTFSCHEDULING_SINK_H_
#define __SPTFSCHEDULING_SINK_H_

#include <omnetpp.h>

using namespace omnetpp;

class Sink : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
