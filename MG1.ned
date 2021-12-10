simple MG1
{
	parameters:
        int nbClasses @default2;

        @display("i=block/queue;q=queue");
        
        @signal[queueLength](type=int);
        @signal[generalQueuingTime](type=simtime_t);
        @signal[queuingTimeClass:*](type=simtime_t);
        @signal[utilizationFactor](type=double);
        @signal[responseTime](type=simtime_t);
        
        @statistic[queueLength](title="Queue length over time";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[generalQueuingTime](title="General queuing time";record=timeavg);
        @statistic[utilizationFactor](title="Utilization Factor of the server";record=vector,timeavg;interpolationmode=sample-hold);
        @statistic[responseTime](title="Response Time";record=timeavg);
        @statisticTemplate[conditionalQueuingTimeTemplate](title="Conditional queuing time template of class $namePart1";record=timeavg,max);
    
    gates:
        input in[nbClasses];
        output out;
}
