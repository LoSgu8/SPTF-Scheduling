simple Source
{
    parameters:
        double L @unit(s);
        double avgInterArrivalTime @unit(s);
        @display("i=block/source");
    gates:
        output out;
}
