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

    L = par("L").doubleValue();

    std::uniform_real_distribution<double> distribution(0.0, L);

    // send the first msg according the lambda parameter
    scheduleAt(simTime()+exponential(avgInterArrivalTime), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{

    int randomNumber;
    // Generate packet name
    char msgname[20];
    sprintf(msgname, "message-%d", ++nbGenMessages);

    // Generate the packet to be sent to M/G/1 queue module
    ClassMessage *message = new ClassMessage(msgname);

    // Generate and assign the Service Time to the message using a uniform distribution generator [0, L]
    std::uniform_real_distribution<double> distribution(0.0, L);
    message->setMsgServiceTime(distribution(generator));
    // Assign -1 to msgClass field to the message, it will be changed at by MG1 module at its reception
    message->setMsgClass(getIndex());
    // Assign the actual time to startedQueuingAt field of the message
    message->setStartedQueuingAt(simTime());

    // Send it out to the M/G/1 System
    send(message, "out");

    //schedule next packet
    scheduleAt(simTime()+exponential(avgInterArrivalTime), sendMessageEvent); // in this course we'll use only sendMessageEvent as message event
}
