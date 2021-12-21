// Giacomo Sguotti 10667547 - SPTF Scheduling
// MG1.ned
simple MG1
{
	parameters:
        @display("i=block/queue;q=queue");
        
        double L @unit(s);
        int nbIntervals;
        
        @signal[queueLength](type=int);
        @signal[generalQueuingTime](type=simtime_t);
        @signal[queuingTimeInterval:*](type=simtime_t);
        @signal[utilizationFactor](type=double);
        @signal[responseTime](type=simtime_t);
        
        @statistic[queueLength](title="Queue length over time";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[generalQueuingTime](title="General queuing time";record=mean);
        @statistic[utilizationFactor](title="Utilization Factor of the server";record=vector,timeavg;interpolationmode=sample-hold);
        @statistic[responseTime](title="Response Time";record=mean);
        @statisticTemplate[conditionalQueuingTimeTemplate](title="Conditional queuing time template of class $namePart1";record=mean);
    
    gates:
        input in;
        output out;
}