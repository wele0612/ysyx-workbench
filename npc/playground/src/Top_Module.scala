import chisel3._

/**
 * Bundle for nvboard
  */
class NVboard_IOs extends Bundle{
  val ledr=Output(Vec(16,Bool()))
}
class Top_Module extends Module {
  val io=IO(new NVboard_IOs)

  io.ledr(0)=true.B
  io.ledr(1)=false.B
}
