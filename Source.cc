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

    avgInterArrivalTime = par("avgInterArrivalTime").intValue();

    L = par("L").doubleValue();

    // trigger handleMessage method
    scheduleAt(simTime(), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{

    int randomNumber;
    // Generate packet name
    char msgname[20];
    sprintf(msgname, "message-%d", ++nbGenMessages);

    // Generate the packet to be sent to M/G/1 queue module
    ClassMessage *message = new ClassMessage(msgname);

    // Assign a class to the message using uniform distribution generator [0, L]
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, L);
    message->setPriority(distribution(generator));

    // Send it out to the M/G/1 System
    send(message, "out");

    //schedule next packet
    scheduleAt(simTime()+exponential(avgInterArrivalTime), sendMessageEvent); // in this course we'll use only sendMessageEvent as message event
}
