format default

in = [1 2 3 4];
in1 = [1 2]; in2 = [3 4];

ir = [7 3 5 8];
ir1 = [7 3]; ir2 = [5 8];

direct_conv = conv(in, ir)
#disp(direct_conv);

in1_ir1 = conv(in1, ir1)
in1_ir2 = conv(in1, ir2)
in2_ir1 = conv(in2, ir1)
in2_ir2 = conv(in2, ir2)









