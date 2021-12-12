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
    int nbClasses = par("nbClasses").intValue();

    endOfServiceMsg = new cMessage("end-service");
    queue.setName("queue");
    queue.setup(&compareFunc);

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
    for (i=0; i<nbClasses; i++) {
        sprintf(signalName, "queuingTimeClass:%d", i);
        conditionalQueuingTimeSignals[i] = registerSignal(signalName);

        char statisticName[32];
        sprintf(statisticName, "queuingTimeClass:%d", i);

        ev->addResultRecorders(this, conditionalQueuingTimeSignals[i], statisticName, statisticTemplate);
    }
}

void MG1::handleMessage(cMessage *msg)
{
    ClassMessage *castedmsg;

    // --- PACKET IN SERVER HAS BEEN PROCESSED ---
    if (msg->isSelfMessage()) {
        EV << "Completed service of " << msgInServer->getName() << " of class " << msgInServer->getMsgClass() << endl;
        // STATISTICS
        //emit its response time
        emit(responseTimeSignal, simTime() - msgInServer->getStartedQueuingAt());

        //Send the processed packet to the sink
        send(msgInServer, "out");

        // start next packet processing if queue not empty
        if (!queue.isEmpty()) {
            msgInServer = (ClassMessage *)queue.pop(); // take top of the queue msg and remove it from queue
            EV << "Popped out " << msgInServer->getName() << " of class " << msgInServer->getMsgClass() << " after waiting in queue for "<< simTime() - msgInServer->getStartedQueuingAt()<< " seconds." << endl;
            // STATISTICS
            // the queue length has decreased
            emit(queueLengthSignal, queue.getLength());
            // emit the generalQueuingTimeSignal
            emit(generalQueuingTimeSignal, simTime() - msgInServer->getStartedQueuingAt() );
            // emit the conditionalQueuingTimeSignal
            emit(conditionalQueuingTimeSignals[msgInServer->getMsgClass()], simTime() - msgInServer->getStartedQueuingAt() );

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
        castedmsg = check_and_cast<ClassMessage *>(msg);
        // assign its correct class according from which Source module it comes
        //castedmsg->setMsgClass(castedmsg->getArrivalGate()->getIndex());
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
    EV << "Starting service of " << msgInServer->getName() << " of class "<< msgInServer->getMsgClass() << ", its service will last " << msgInServer->getMsgServiceTime() << " seconds." << endl;
}

// This is where the SPTF policy is implemented, the queue is sorted by packet priority. Once the server finishes the service,
// it will pop from the queue the packet with lowest service time
void MG1::putPacketInQueue(ClassMessage *msg) {
    queue.insert(msg); // since the compareFunc has already been binded to the queue, the insertion will be sorted according to the message class
    //log new message in queue
    EV << msg->getName() << " of class " << msg->getMsgClass() << " enters queue" << endl;
}

// compareFunc used to define queue,
// see https://doc.omnetpp.org/omnetpp/api/group__Containers.html#gabeb451b66385c18e01063cb0576ea8a0 and https://doc.omnetpp.org/omnetpp/api/classomnetpp_1_1cQueue.html#a7eae56a84f7da30c84a4b68a96783577
int MG1::compareFunc(cObject *a, cObject *b){
    ClassMessage* castedA = check_and_cast<ClassMessage *>(a);
    ClassMessage* castedB =check_and_cast<ClassMessage *>(b);
    double comparison = castedA->getMsgServiceTime() - castedB->getMsgServiceTime();

    if ( comparison > 0 )
        comparison = ceil(comparison);
    else
        comparison = floor(comparison);
    return (int) comparison;
}
