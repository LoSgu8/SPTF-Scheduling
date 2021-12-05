//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SPTFSCHEDULING_SOURCE_H_
#define __SPTFSCHEDULING_SOURCE_H_

#include <omnetpp.h>
#include <ClassMessage_m.h>
#include <random>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Source : public cSimpleModule
{
    public:
        Source();
        virtual ~Source();
    private:
        cMessage *sendMessageEvent;
        int nbGenMessages; // msg counter
        double avgInterArrivalTime;
        double L; // maximum value of the uniform distribution, ned parameter

        std::default_random_engine generator;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

#endif
