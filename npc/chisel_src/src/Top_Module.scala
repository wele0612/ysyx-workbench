import chisel3._
import chisel3.util.Mux1H
import chisel3.util.Reverse
import chisel3.util.PopCount
import chisel3.util.OHToUInt
import chisel3.util.PriorityEncoder

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

  val ptwo=Module(new Plus_two())
  ptwo.io.in:=io.sw
  io.ledr:=ptwo.io.out
}

class Plus_two extends Module{
  val io=IO(new Bundle {
    val in=Input(Uint(16.W))
    val out=Output(Uint(16.W))
  })
  io.out:=~io.in
}
