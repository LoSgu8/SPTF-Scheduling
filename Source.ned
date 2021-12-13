// Giacomo Sguotti 10667547 - SPTF Scheduling
// Source.ned
simple Source
{
    parameters:
        @display("i=block/source");
        double avgInterArrivalTime @unit(s);
    gates:
        output out;
}
