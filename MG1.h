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
        //simsignal_t mm1ServiceTimeSignal;

        cQueue queue; // list of pkts in the queue
        //simsignal_t arrivalTime;

        //double avgServiceTime;

        //bool serverBusy; // 1 busy, 0 not busy

    public:
        MG1();
        virtual ~MG1();
        // compareFunc used to define queue, see https://doc.omnetpp.org/omnetpp/api/group__Containers.html#gabeb451b66385c18e01063cb0576ea8a0 and https://doc.omnetpp.org/omnetpp/api/classomnetpp_1_1cQueue.html#a7eae56a84f7da30c84a4b68a96783577


    protected:
        static int compareFunc(cObject *a, cObject *b);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void startPacketService(); // no parameters are needed since msgInServer is an attribute of MG1 class
        void putPacketInQueue(ClassMessage *msg);



};

#endif
