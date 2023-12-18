module top(
	input clk,
	input rst,
	output reg [15:0] led);

reg [31:0] count;
always @(posedge clk)begin
	if(rst)begin
		count<=32'b0;
	end else begin
		count<=count+1;
	end
end

always @(posedge clk) begin
	if(rst)begin
		led<=16'b1;
	end else begin
		if(count[15:0]==16'b1)
		led<={led[14:0],led[15]};
	end
end
endmodule
