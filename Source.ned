simple Source
{
    parameters:
        @display("i=block/source");
        double L @unit(s);
        double avgInterArrivalTime @unit(s);
    gates:
        output out;
}
