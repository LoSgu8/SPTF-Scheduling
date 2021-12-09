simple MG1
{
	parameters:
        int nbClasses @default2;
        //double avgServiceTime @unit(s);
        @display("i=block/queue;q=queue");
        
        @signal[queueLength](type="int");
        //@signal[mm1ServiceTime](type="simtime_t");
        
        @statistic[queueLength](title="Queue length over time";record=vector,timeavg,max;interpolationmode=sample-hold);
        //@statistic[mm1ServiceTime](title="M/M/1 Service Time";record=vector,timeavg, max;interpolationmode=sample-hold);
    
    gates:
        input in[nbClasses];
        output out;
}
