import chisel3._
import chisel3.util.Mux1H
import chisel3.util.Reverse
import chisel3.util.PopCount

/**
 * Bundle for nvboard
  */
class NVboard_IOs extends Bundle{
  val ledr=Output(UInt(16.W))
  val sw=Input(UInt(16.W))

   
  def loadDefault():Unit={
    ledr:=0.U(16.W)//.asTypeOf(ledr)
  }
}
class Top_Module extends Module {
  val io=IO(new NVboard_IOs())
  io.ledr:=Mux1H(io.sw,List.range(1,16).map((_).U(16.W)))
}
