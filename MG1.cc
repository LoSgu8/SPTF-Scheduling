// Giacomo Sguotti 10667547 - SPTF Scheduling
// MG1.cc
#include "MG1.h"

Define_Module(MG1);

MG1::MG1()
{
    endOfServiceMsg = nullptr;
    msgInServer = nullptr;
}

MG1::~MG1()
{
    delete msgInServer;
    cancelAndDelete(endOfServiceMsg);
}

void MG1::initialize()
{
    endOfServiceMsg = new cMessage("end-service");
    queue.setName("queue");
    queue.setup(&compareFunc);

    // L parameter
    L = par("L").doubleValue();

    // L parameter
    nbIntervals = par("nbIntervals").intValue();

    // uniform_real_distribution used to uniform distributed service times
    std::uniform_real_distribution<double> distribution(0.0, L);

    //signal registering
    queueLengthSignal = registerSignal("queueLength");
    generalQueuingTimeSignal = registerSignal("generalQueuingTime");
    utilizationFactorSignal = registerSignal("utilizationFactor");
    responseTimeSignal = registerSignal("responseTime");

    // Setting up the conditionalQueuingTime statistic template
    char signalName [32];
    char statisticName[32];
    int i;
    cEnvir* ev = getEnvir();
    cProperty *statisticTemplate =
            getProperties()->get("statisticTemplate", "conditionalQueuingTimeTemplate");
    for (i=0; i<nbIntervals; i++) {
        sprintf(signalName, "queuingTimeInterval:%d", i);
        conditionalQueuingTimeSignals.push_back(registerSignal(signalName));
        //conditionalQueuingTimeSignals[i] = registerSignal(signalName);

        sprintf(statisticName, "queuingTimeInterval:%d", i);

        ev->addResultRecorders(this, conditionalQueuingTimeSignals.at(i), statisticName, statisticTemplate);
        //ev->addResultRecorders(this, conditionalQueuingTimeSignals[i], statisticName, statisticTemplate);
    }
}

void MG1::handleMessage(cMessage *msg)
{
    ModifiedMessage *castedmsg;

    // --- PACKET IN SERVER HAS BEEN PROCESSED ---
    if (msg->isSelfMessage()) {
        EV << "[" << simTime() << "] Completed service of " << msgInServer->getName() << " with service time " << msgInServer->getMsgServiceTime() << " seconds." << endl;
        // STATISTICS
        //emit its response time
        emit(responseTimeSignal, simTime() - msgInServer->getStartedQueuingAt());

        //Send the processed packet to the sink
        send(msgInServer, "out");

        // start next packet processing if queue not empty
        if (!queue.isEmpty()) {
            msgInServer = (ModifiedMessage *)queue.pop(); // take top of the queue msg and remove it from queue
            EV << "[" << simTime() << "] Popped out " << msgInServer->getName() << " with service time " << msgInServer->getMsgServiceTime() << " seconds after waiting in queue for "<< simTime() - msgInServer->getStartedQueuingAt()<< " seconds." << endl;
            // STATISTICS
            // the queue length has decreased
            emit(queueLengthSignal, queue.getLength());
            // emit the generalQueuingTimeSignal
            emit(generalQueuingTimeSignal, simTime() - msgInServer->getStartedQueuingAt() );
            // emit the conditionalQueuingTimeSignal on its related dx interval -> floor(MsgServiceTime/(L/nbIntervals)
            emit(conditionalQueuingTimeSignals.at(msgInServer->getDxIntervalIndex()), simTime() - msgInServer->getStartedQueuingAt() );
            //emit(conditionalQueuingTimeSignals[msgInServer->getDxIntervalIndex()], simTime() - msgInServer->getStartedQueuingAt() );

            //start service
            startPacketService();
        } else { // if the queue is empty -> the server goes idle
            //server busy --> server idle
            msgInServer = nullptr;

            // STATISTICS
            // update the totalActiveServerTime value, adding to the previous value the time elapsed from the time the server became busy
            totalActiveServerTime = totalActiveServerTime + (simTime() - startTimeForRho);
            // update the startTimeForRho value, store the time at which the server becomes idle
            startTimeForRho = simTime();
            // emit UtilizationFactor signal
            emit(utilizationFactorSignal, totalActiveServerTime / simTime());

            //log idle server
            EV << "Empty queue, the server goes IDLE" <<endl;
        }
    }
    else { // PACKET FROM SOURCE HAS ARRIVED
        // cast the received cMessage into a ModifiedMessage
        castedmsg = check_and_cast<ModifiedMessage *>(msg);

        // Fill all its fields
        // Generate and assign the Service Time to the message using a uniform distribution generator [0, L]
        std::uniform_real_distribution<double> distribution(0.0, L);
        castedmsg->setMsgServiceTime(distribution(generator));
        // Assign the actual time to startedQueuingAt field of the message (since send time and MG1receive->queue time are 0)
        castedmsg->setStartedQueuingAt(simTime());
        // Assign the dxIntervalIndex field to the message according to its service time
        castedmsg->setDxIntervalIndex(floor(castedmsg->getMsgServiceTime()/(L/nbIntervals)));

        if (msgInServer != nullptr) { // the server is busy -> put the received msg in queue

            // put the packet in queue
            putPacketInQueue(castedmsg);

        }
        else { //the server is idle -> start its service
            // STATITISTICS
            // emit the generalQueuingTime signal (SIMTIME_ZERO)
            emit(generalQueuingTimeSignal, SIMTIME_ZERO);
            // emit UtilizationFactor signal
            emit(utilizationFactorSignal, totalActiveServerTime / simTime());
            // update the startTimeForRho value, store the time at which the server becomes busy
            startTimeForRho = simTime();

            // server idle --> server busy
            // The packet just received becomes the msgInServer packet and starts its service
            msgInServer = castedmsg;
            startPacketService();
        }
    }
}

void MG1::startPacketService()
{

    //generate service time and schedule completion accordingly
    simtime_t serviceTime = msgInServer->getMsgServiceTime();
    scheduleAt(simTime()+serviceTime, endOfServiceMsg);

    //log service start
    EV << "[" << simTime() << "] Starting service of " << msgInServer->getName() << " with service time " << msgInServer->getMsgServiceTime() << " seconds." << endl;
}

// This is where the SPTF policy is implemented, the queue is sorted by packet priority. Once the server finishes the service,
// it will pop from the queue the packet with lowest service time
void MG1::putPacketInQueue(ModifiedMessage *msg) {
    // cast the received cMessage into a ModifiedMessage
    msg = check_and_cast<ModifiedMessage *>(msg);
    queue.insert(msg); // since the compareFunc has already been binded to the queue, the insertion will be sorted according to the message class
    //log new message in queue
    EV << "[" << simTime() << "]" << msg->getName() << " with service time " << msg->getMsgServiceTime() << " seconds enters queue." << endl;
}

// compareFunc used to define queue,
// see https://doc.omnetpp.org/omnetpp/api/group__Containers.html#gabeb451b66385c18e01063cb0576ea8a0 and https://doc.omnetpp.org/omnetpp/api/classomnetpp_1_1cQueue.html#a7eae56a84f7da30c84a4b68a96783577
int MG1::compareFunc(cObject *a, cObject *b){
    ModifiedMessage* castedA = check_and_cast<ModifiedMessage *>(a);
    ModifiedMessage* castedB =check_and_cast<ModifiedMessage *>(b);
    double comparison = castedA->getMsgServiceTime() - castedB->getMsgServiceTime();

    if ( comparison > 0 )
        comparison = ceil(comparison);
    else
        comparison = floor(comparison);
    return (int) comparison;
}
