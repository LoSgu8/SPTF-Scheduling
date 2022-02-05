// Giacomo Sguotti 10667547 - SPTF Scheduling
// Source.cc
#include "Source.h"

Define_Module(Source);

Source::Source()
{
    sendMessageEvent = nullptr;
}

Source::~Source()
{
    cancelAndDelete(sendMessageEvent);
}

void Source::initialize()
{
    sendMessageEvent = new cMessage("sendMessageEvent");
    nbGenMessages = 0;

    avgInterArrivalTime = par("avgInterArrivalTime").doubleValue();

    //scheduleAt(simTime(), sendMessageEvent);
    // send the first msg according the avgInterArrivalTime parameter
    scheduleAt(simTime()+exponential(avgInterArrivalTime), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{

    int randomNumber;
    // Generate packet name
    char msgname[20];
    sprintf(msgname, "message-%d", ++nbGenMessages);

    // Generate the packet to be sent to M/G/1 queue module
    ModifiedMessage *message = new ModifiedMessage(msgname);

    // Send it out to the M/G/1 System
    send(message, "out");

    //schedule next packet
    scheduleAt(simTime()+exponential(avgInterArrivalTime), sendMessageEvent); // in this course we'll use only sendMessageEvent as message event
}
