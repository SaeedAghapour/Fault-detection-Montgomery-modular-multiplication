module tb_Unprotected_2048;
    
parameter input_size = 2048;
parameter word_size = 64;
    
    reg clk;
    reg reset;

    // Outputs
    reg [input_size-1:0] u_dbg, v_dbg;
    reg [input_size-1:0] Result_dbg;
    wire done;
    reg [input_size+word_size-1:0] t_dbg, temp1_dbg;
    integer i_dbg, state_dbg, clock_counter_dbg;
    reg [word_size -1 :0] mi_dbg, ui_dbg, t0_dbg;
    reg [word_size-1:0] bus;
    // Instantiate the Unit Under Test (UUT)
    Unprotected_2048 uut (
        .clk(clk),
        .reset(reset),
        .bus(bus),
        .done(done)
    );
    
always @* begin
    t_dbg = uut.t;
    mi_dbg = uut.mi;
    ui_dbg = uut.ui;
    t0_dbg = uut.t0;
    i_dbg = uut.i;
    temp1_dbg = uut.temp1;
    state_dbg = uut.state;
    clock_counter_dbg = uut.clock_counter;
    Result_dbg = uut.Result;
    u_dbg = uut.u;
    v_dbg = uut.v;

end

    // Clock generation
    always #2 clk = ~clk;

initial begin
    // Initialize all signals
    clk = 0;
    reset = 1;

    #10 reset = 0;
     //x read
     bus = 64'd12223213231345678;
     #4 // 4 is the clock cycle (2*2)
     bus = 64'd32109876543218276;
     #4 
     bus = 64'd67890125678901123;
     #4 
     bus = 64'd75634567890232341;
     #4 
     bus = 64'd45634567812332341;
     #4 
     bus = 64'd45634567890232341;
     #4 
     bus = 64'd44234567890232341;
     #4 
     bus = 64'd12234567890232341;
     #4
     bus = 64'd78120934890232341;
     #4 
     bus = 64'd45634567890232341;
     #4 
     bus = 64'd34343437890232341;
     #4 
     bus = 64'd45634567893432341;
     #4 
     bus = 64'd45644345678989881;
     #4 
     bus = 64'd12345567890232341;
     #4 
     bus = 64'd49963567890232341;
     #4 
     bus = 64'd15239329290232341;
     #4 
     bus = 64'd98471552231345678;
     #4 // 4 is the clock cycle (2*2)
     bus = 64'd87788212213218276;
     #4 
     bus = 64'd67890125108901123;
     #4 
     bus = 64'd40506017890232341;
     #4 
     bus = 64'd45634557812332341;
     #4 
     bus = 64'd45863647890232341;
     #4 
     bus = 64'd32314567890232341;
     #4 
     bus = 64'd12234560190232341;
     #4
     bus = 64'd78120934555232341;
     #4 
     bus = 64'd45634567968232341;
     #4 
     bus = 64'd34343437441232341;
     #4 
     bus = 64'd45634567100432341;
     #4 
     bus = 64'd45644345111989881;
     #4 
     bus = 64'd12345567887232341;
     #4 
     bus = 64'd49963567890232341;
     #4 
     bus = 64'd15239320000232341;
     #4
     //y read
     bus = 64'd12223213231345678;
     #4 // 4 is the clock cycle (2*2)
     bus = 64'd32109876543218276;
     #4 
     bus = 64'd67890125678901123;
     #4 
     bus = 64'd75634567890232341;
     #4 
     bus = 64'd45634567812332341;
     #4 
     bus = 64'd45634567890232341;
     #4 
     bus = 64'd44234567890232341;
     #4 
     bus = 64'd12234567890232341;
     #4
     bus = 64'd78120934890232341;
     #4 
     bus = 64'd45634567890232341;
     #4 
     bus = 64'd34343437890232341;
     #4 
     bus = 64'd45634567893432341;
     #4 
     bus = 64'd45644345678989881;
     #4 
     bus = 64'd12345567890232341;
     #4 
     bus = 64'd49963567890232341;
     #4 
     bus = 64'd15239329290232341;
     #4 
     bus = 64'd98471552231345678;
     #4 // 4 is the clock cycle (2*2)
     bus = 64'd87788212213218276;
     #4 
     bus = 64'd67890125108901123;
     #4 
     bus = 64'd40506017890232341;
     #4 
     bus = 64'd45634557812332341;
     #4 
     bus = 64'd45863647890232341;
     #4 
     bus = 64'd32314567890232341;
     #4 
     bus = 64'd12234560190232341;
     #4
     bus = 64'd78120934555232341;
     #4 
     bus = 64'd45634567968232341;
     #4 
     bus = 64'd34343437441232341;
     #4 
     bus = 64'd45634567100432341;
     #4 
     bus = 64'd45644345111989881;
     #4 
     bus = 64'd12345567887232341;
     #4 
     bus = 64'd49963567890232341;
     #4 
     bus = 64'd15239320000232341;


    // Wait and observe
    #500000;

    $finish; // Finish simulation
end


endmodule
