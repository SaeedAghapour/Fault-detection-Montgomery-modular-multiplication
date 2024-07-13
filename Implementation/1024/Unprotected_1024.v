module Unprotected_1024 (
    input clk,
    input reset,
    input [63:0] bus,
    output reg done
);

parameter size_input = 1024;
parameter word_size = 64;

parameter [size_input-1:0] N = 1024'd9006769293124996861255893066700334272671806485303953342518281633963239447933245010353445549039115759986500715077980567753099333142383605379457864586755100204745376410954685652164535009638029367622513689904345350522370478026032534968144604478926003487872700111583442769562479658379133504611756048449576659257;
parameter [word_size-1:0] N_prime = 64'd7243884363528293111;
parameter iteration = size_input / word_size;

reg [word_size-1:0] mi, ui, t0, v0, slice1, slice2, input_mul1;
reg [2*word_size-1:0] p;
reg [size_input-1:0] u, v, Result, input_mul2;
reg [size_input + word_size-1:0] temp1, temp2, t, output_mul, temp;
reg [7:0] state;
reg [5:0] counter_input;
integer i, j, clock_counter, flag_state, mul_length;
reg input_ready;
// Main Body
always @(posedge clk or posedge reset) begin
    if (reset) begin
        Result <= 0;
        temp <= 0;
        slice1 <= 0;
        slice2 <= 0;
        p <= 0;
        j <= 0;
        output_mul <= 0;
        temp1 <= 0;
        temp2 <= 0;
        state <= 0;
        u <= 0;
        v <= 0;
        t <= 0;
        i <= 0;
        mi <= 0;
        ui <= 0;
        v0 <= 0;
        t0 <= 0;
        done <= 0;
        clock_counter <= 0;
        input_ready <= 0;
        counter_input <= 0;
        input_mul1 <= 0;
        input_mul2 <= 0;
        flag_state <= 0;
    end else begin
        clock_counter <= clock_counter + 1;
        if (input_ready == 0) begin
            case (counter_input)
                // filling u through word_size bus
                0: u[63:0] <= bus;
                1: u[127:64] <= bus;   
                2: u[191:128] <= bus; 
                3: u[255:192] <= bus; 
                4: u[319:256] <= bus; 
                5: u[383:320] <= bus; 
                6: u[447:384] <= bus; 
                7: u[511:448] <= bus; 
                8: u[575:512] <= bus; 
                9: u[639:576] <= bus; 
                10: u[703:640] <= bus; 
                11: u[767:704] <= bus; 
                12: u[831:768] <= bus; 
                13: u[895:832] <= bus; 
                14: u[959:896] <= bus; 
                15: u[1023:960] <= bus;
                
                // filling v through word_size bus
                16: v[63:0] <= bus;   
                17: v[127:64] <= bus;  
                18: v[191:128] <= bus; 
                19: v[255:192] <= bus; 
                20: v[319:256] <= bus; 
                21: v[383:320] <= bus; 
                22: v[447:384] <= bus; 
                23: v[511:448] <= bus; 
                24: v[575:512] <= bus; 
                25: v[639:576] <= bus; 
                26: v[703:640] <= bus; 
                27: v[767:704] <= bus; 
                28: v[831:768] <= bus; 
                29: v[895:832] <= bus; 
                30: v[959:896] <= bus; 
                31: begin
                    v[1023:960] <= bus;
                    input_ready <= 1;
                end
            endcase
            if (counter_input < 31) begin
                counter_input <= counter_input + 1;
            end
        end else if (input_ready == 1) begin
            case (state)
                0: begin
                    ui <= u >> (i * word_size); // Isolate the lower 64 bits of u in each iteration
                    v0 <= v[63:0];
                    t0 <= t[63:0]; // Isolate the lower 64 bits of t
                    state <= 1;
                end
                
                1: begin 
                // uiv0
                    mul_length <= word_size;
                    if (j < mul_length)begin
                        flag_state <= 1;
                        input_mul1 <= ui;
                        input_mul2 <= v0;
                        state <= 20;
                    end else if (j >= mul_length)begin
                        temp1 <= output_mul + t0;
                        j <= 0;
                        state <= 2;
                    end
                end
                
                2: begin
                // mi = (uiv0 + t0) * N' % b = (temp1 * N') % b
                    mul_length <= 2 * word_size; // size of uiv0+t0
                    if (j < mul_length)begin
                        flag_state <= 2;
                        input_mul1 <= N_prime;
                        input_mul2 <= temp1;
                        state <= 20;
                    end else if (j >= mul_length)begin
                        mi <= output_mul;
                        j <= 0;
                        state <= 3;
                    end
                end
                
                3: begin
                // ui*v
                    mul_length <= size_input; // size of v
                    if (j < mul_length)begin
                        flag_state <= 3;
                        input_mul1 <= ui;
                        input_mul2 <= v;
                        state <= 20;
                    end else if (j >= mul_length)begin
                        temp1 <= output_mul; // temp1 = uiv
                        j <= 0;
                        state <= 4;
                    end
                end
                
                4: begin
                // mi*N
                    mul_length <= size_input; // size of N
                    if (j < mul_length)begin
                        flag_state <= 4;
                        input_mul1 <= mi;
                        input_mul2 <= N;
                        state <= 20;
                    end else if (j >= mul_length)begin
                        temp2 <= output_mul; // temp2 = mi * N
                        j <= 0;
                        state <= 5;
                    end
                end            
                
                5: begin
                    t <= (t + temp1 + temp2) >> word_size; // t = (t + temp2) / b
                    i <= i + 1;
                    if (i == iteration - 1) begin
                        state <= 6;
                    end else begin
                        state <= 0;
                    end
                end
                
                6: begin
                    if (t >= N) begin
                        t <= t - N;
                    end else begin
                        state <= 7;
                    end
                end
                
                7: begin
                    Result <= t; // Final assignment of Result
                    done <= 1;
                end
                
                
                20: begin
                    // multiplication state (Schoolbook method)
                    slice1 = input_mul1;   
                    slice2 = input_mul2 >> j;   
                    p = slice1 * slice2;        // Multiply sliced portions
                    temp = temp + (p << j); // Shift and accumulate the product
                    j = j + word_size;
                    if (j >= mul_length) begin
                        output_mul <= temp;
                        temp <= 0;
                        state <= flag_state;
                    end
                end
            endcase
        end
    end
end

endmodule
