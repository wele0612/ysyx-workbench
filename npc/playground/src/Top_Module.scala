import chisel3._

/**
 * Bundle for nvboard
  */
class NVboard_IOs extends Bundle{
  val ledr=Output(UInt(16.W))

  def loadDefault():Unit={
    ledr:=0.U.asTypeOf(ledr)
  }
}
class Top_Module extends Module {
  val io=IO(new NVboard_IOs)

  io.loadDefault()

  io.ledr(0):=true.B
  io.ledr(1):=false.B
}
