network Net
{
    submodules:
        mg1: MG1 {
            @display("p=176,79");
        }
        source1[mg1.nbClasses]: Source;
        sink1: Sink {
            @display("p=306,79");
        }
    connections:
        for i=0..mg1.nbClasses-1 {
            source1[i].out --> mg1.in[i];
        }
        mg1.out --> sink1.in;
}
