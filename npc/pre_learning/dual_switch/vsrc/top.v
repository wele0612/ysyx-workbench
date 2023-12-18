module top(
	input ina,
	input inb,
	output outled);

assign outled=ina^inb;

endmodule
