#include "Sink.h"

Define_Module(Sink);

void Sink::initialize()
{
}

void Sink::handleMessage(cMessage *msg)
{
    delete msg;
}
