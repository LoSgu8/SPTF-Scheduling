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
    char signalName [32];
    char statisticName[32];
    int i;

    int nbClasses = par("nbClasses").intValue();

    endOfServiceMsg = new cMessage("end-service");
    queue.setName("queue");
    queue.setup(&compareFunc);
    //serverBusy = false; // at start server is empty

    //signal registering
    queueLengthSignal = registerSignal("queueLength");
    generalQueueingTimeSignal = registerSignal("generalQueueingTime");

    // Setting up the conditionalQueueingTime statistic template
    cEnvir* ev = getEnvir();
    cProperty *statisticTemplate =
            getProperties()->get("statisticTemplate", "conditionalQueueingTimeTemplate");
    for (i=0; i<nbClasses; i++) {
        sprintf(signalName, "queueingTimeClass:%d", i);
        conditionalQueueingTimeSignals[i] = registerSignal(signalName);

        char statisticName[32];
        sprintf(statisticName, "queueingTimeClass:%d", i);

        ev->addResultRecorders(this, conditionalQueueingTimeSignals[i], statisticName, statisticTemplate);
    }
}

void MG1::handleMessage(cMessage *msg)
{
    ClassMessage *castedmsg;
    // --- PACKET IN SERVER HAS BEEN PROCESSED ---
    if (msg->isSelfMessage()) {
        EV << "Completed service of " << msgInServer->getName() << " of class " << msgInServer->getMsgClass() << endl;

        //Send the processed packet to the sink
        send(msgInServer, "out");

        // start next packet processing if queue not empty
        if (!queue.isEmpty()) {
            msgInServer = (ClassMessage *)queue.pop(); // take top of the queue msg and remove it from queue
            EV << "Popped out " << msgInServer->getName() << " of class " << msgInServer->getMsgClass() << " after waiting in queue for "<< simTime() - msgInServer->getStartedQueueingAt()<< " seconds." << endl;
            // STATISTICS
            // the queue length has decreased
            emit(queueLengthSignal, queue.getLength());
            // emit the generalQueueingTimeSignal
            emit(generalQueueingTimeSignal, simTime() - msgInServer->getStartedQueueingAt() );
            // emit the conditionalQueueingTimeSignal
            emit(conditionalQueueingTimeSignals[msgInServer->getMsgClass()], simTime() - msgInServer->getStartedQueueingAt() );

            //start service
            startPacketService();
        } else { // if the queue is empty -> the server goes idle
            //server is not busy anymore
            msgInServer = nullptr;

            //log idle server
            EV << "Empty queue, the server goes IDLE" <<endl;
        }
    }
    else { // PACKET FROM SOURCE HAS ARRIVED
        // cast the received cMessage into a ModifiedMessage
        castedmsg = check_and_cast<ClassMessage *>(msg);

        if (msgInServer != nullptr) {
            // STATISTICS
            // store the simtime_t in which it started queueing
            castedmsg->setStartedQueueingAt(simTime());

            // put the packet in queue
            putPacketInQueue(castedmsg);

        }
        else { //the server is idle -> start service right away
            // STATITISTICS
            //emit the generalQueueingTime signal (SIMTIME_ZERO)
            emit(generalQueueingTimeSignal, SIMTIME_ZERO);

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
    EV << msg->getName() << " of class " << msgInServer->getMsgClass() << " enters queue" << endl;
}

// compareFunc used to define queue,
// see https://doc.omnetpp.org/omnetpp/api/group__Containers.html#gabeb451b66385c18e01063cb0576ea8a0 and https://doc.omnetpp.org/omnetpp/api/classomnetpp_1_1cQueue.html#a7eae56a84f7da30c84a4b68a96783577
int MG1::compareFunc(cObject *a, cObject *b){
    ClassMessage* castedA = check_and_cast<ClassMessage *>(a);
    ClassMessage* castedB =check_and_cast<ClassMessage *>(b);
    double comparison = castedA->getMsgClass() - castedB->getMsgClass();

    if ( comparison > 0)
        comparison = ceil(comparison);
    else
        comparison = floor(comparison);
    return (int) comparison;
}
