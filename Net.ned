// Giacomo Sguotti 10667547 - SPTF Scheduling
// Net.ned
network Net
{
    submodules:
        mg1: MG1 {
            @display("p=196,79");
        }
        source1: Source {
            @display("p=80,79");
        }
        sink1: Sink {
            @display("p=306,79");
        }
    connections:
        source1.out --> mg1.in;
        mg1.out --> sink1.in;
}
