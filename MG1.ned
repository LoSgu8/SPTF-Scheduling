simple MG1
{
	parameters:
        int nbClasses @default2;
        //double avgServiceTime @unit(s);
        @display("i=block/queue;q=queue");
        
        @signal[queueLength](type=int);
        @signal[generalQueueingTime](type="simtime_t");
        @signal[queueingTimeClass:*](type="simtime_t");
        
        @statistic[queueLength](title="Queue length over time";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[generalQueueingTime](title="General queueing time";record=timeavg);
        @statisticTemplate[conditionalQueueingTimeTemplate](title="Conditional queueing time template of class $namePart1";record=timeavg,max);
    
    gates:
        input in[nbClasses];
        output out;
}
