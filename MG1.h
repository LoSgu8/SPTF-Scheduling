#ifndef __SPTFSCHEDULING_MG1_H_
#define __SPTFSCHEDULING_MG1_H_

#include <omnetpp.h>
#include <ClassMessage_m.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class MG1 : public cSimpleModule
{
    private:
        ClassMessage *msgInServer; //message from source and now in the server
        cMessage *endOfServiceMsg; // event that tell that the message has been processed

        simsignal_t queueLengthSignal;
        simsignal_t generalQueueingTimeSignal;
        simsignal_t conditionalQueueingTimeSignals[10]; // put a limit to the nb of classes

        cQueue queue; // list of pkts in the queue

    public:
        MG1();
        virtual ~MG1();

    protected:
        static int compareFunc(cObject *a, cObject *b);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void startPacketService(); // no parameters are needed since msgInServer is an attribute of MG1 class
        void putPacketInQueue(ClassMessage *msg);



};

#endif
