// Giacomo Sguotti 10667547 - SPTF Scheduling
// MG1.h
#ifndef __SPTFSCHEDULING_MG1_H_
#define __SPTFSCHEDULING_MG1_H_

#include <omnetpp.h>
#include <ModifiedMessage_m.h>
#include <random>
#include <vector>

using namespace omnetpp;

class MG1 : public cSimpleModule
{
    private:
        ModifiedMessage *msgInServer; //message from source and now in the server
        cMessage *endOfServiceMsg; // event that tell that the message has been processed

        double L; // maximum value of the uniform distribution, ned parameter

        int nbIntervals; // nb of dx intervals

        std::default_random_engine generator;

        simsignal_t queueLengthSignal;
        simsignal_t generalQueuingTimeSignal;
        std::vector<simsignal_t> conditionalQueuingTimeSignals; // it will have nbIntervals elements
        //simsignal_t conditionalQueuingTimeSignals[5000];
        simsignal_t utilizationFactorSignal;
        simsignal_t responseTimeSignal;

        cQueue queue; // list of pkts in the queue

        // variables for UtilizactionFactor statistic
        simtime_t startTimeForRho; // used to compute utilization factor values, store the time at which the server becomes busy or idle
        simtime_t totalActiveServerTime; // used to compute utilization factor values, store the total time the server is active (busy)

    public:
        MG1();
        virtual ~MG1();

    protected:
        static int compareFunc(cObject *a, cObject *b);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void startPacketService(); // no parameters are needed since msgInServer is an attribute of MG1 class
        void putPacketInQueue(ModifiedMessage *msg);
};

#endif
