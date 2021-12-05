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
    //serverBusy = false; // at start server is empty

    //signal registering
    queueLengthSignal = registerSignal("queueLength");
    //mm1ServiceTimeSignal = registerSignal("mm1ServiceTime");

    //get avgServiceTime parameter
    //avgServiceTime = par("avgServiceTime").doubleValue();
    //max_queue_size = par("max_queue_length").intValue();
}

void MG1::handleMessage(cMessage *msg)
{
    ClassMessage *castedmsg;
    // --- PACKET IN SERVER HAS BEEN PROCESSED ---
    if (msg->isSelfMessage()) {
        EV << "Completed service of " << msgInServer->getName() << endl;

        // STATISTICS
        /*
        // emit the Service Time signal
        emit(mm1ServiceTimeSignal, simTime()-msgInServer->getStartTime());
        // add it to the accumulatedService time field
        msgInServer->setAccumulatedServiceTime( msgInServer->getAccumulatedServiceTime() + (simTime() - msgInServer->getStartTime()) );
        */
        //Send the processed packet to the sink
        send(msgInServer, "out");

        // start next packet processing if queue not empty
        if (!queue.isEmpty()) {
            msgInServer = (ClassMessage *)queue.pop(); // take top of the queue msg and remove it from queue

            // STATISTICS
            // the queue length has decreased
            emit(queueLengthSignal, queue.getLength());
            /*
            // emit the mm1 WaitingTime signal
            emit(mm1WaitingTimeSignal, simTime() - msgInServer->getStartTime() );
            // add it to the accumulatedWaitingTime field
            msgInServer->setAccumulatedWaitingTime( msgInServer->getAccumulatedWaitingTime() + (simTime() - msgInServer->getStartTime()) );
            // update startTime field in order to calculate successively the service time
            msgInServer->setStartTime(simTime());
            */
            //start service
            startPacketService();
        } else { // if the queue is empty -> the server goes idle
            //server is not busy anymore
            msgInServer = nullptr;
            //serverBusy = false;

            //log idle server
            EV << "Empty queue, the server goes IDLE" <<endl;
        }
    }
    else { // PACKET FROM SOURCE HAS ARRIVED
        // cast the received cMessage into a ModifiedMessage
        castedmsg = check_and_cast<ClassMessage *>(msg);
        if (msgInServer != nullptr) {
            // STATISTICS
            // store the simtime_t in which it started waiting
            //castedmsg->setStartTime(simTime());

            // put the packet in queue
            putPacketInQueue(castedmsg);

        }
        else { //the server is idle -> start service right away
            // STATITISTICS
            /*//emit the WaitingTime signal (0)
            emit(mm1WaitingTimeSignal, SIMTIME_ZERO);
            //not needed to add it to the accumulated field
            // store the simtime_t in which it started being served
            castedmsg->setStartTime(simTime());
            */

            // The packet just received becomes the msgInServer packet and starts its service
            msgInServer = castedmsg;
            startPacketService();

            //server is now busy
            //serverBusy=true;
        }
    }
}

void MG1::startPacketService()
{

    //generate service time and schedule completion accordingly
    simtime_t serviceTime = msgInServer->getPriority();
    scheduleAt(simTime()+serviceTime, endOfServiceMsg);

    //log service start
    EV << "Starting service of " << msgInServer->getName() << ", its service will last " << msgInServer->getPriority() << " seconds." << endl;
}

// This is where the SPTF policy is implemented, the queue is sorted by packet priority. Once the server finishes the service,
// it will pop from the queue the packet with lowest service time
void MG1::putPacketInQueue(ClassMessage *msg) {
    queue.insert(msg); // since the compareFunc has already been binded to the queue, the insertion will be sorted according to the message class
    //log new message in queue
    EV << msg->getName() << " enters queue"<< endl;
}

int MG1::compareFunc(cObject *a, cObject *b){
    ClassMessage* castedA = check_and_cast<ClassMessage *>(a);
    ClassMessage* castedB =check_and_cast<ClassMessage *>(b);
    double comparison = castedA->getPriority() - castedB->getPriority();

    if ( comparison > 0)
        comparison = ceil(comparison);
    else
        comparison = floor(comparison);
    return (int) comparison;
}
