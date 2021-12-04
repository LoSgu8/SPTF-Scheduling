network Net
{
    submodules:
        source1: Source {
            @display("p=50,79");
        }
        mg1: MG1 {
            @display("p=176,79");
        }
        sink1: Sink {
            @display("p=604,79");
        }
    connections:
        source1.out --> mg1.in;
        mg1.out --> sink1.in;
}