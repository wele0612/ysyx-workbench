import chisel3._
import chisel3.util.Mux1H
import chisel3.util.Reverse
import chisel3.util.PopCount
import chisel3.util.OHToUInt
import chisel3.util.PriorityEncoder
import chisel3.util.Counter

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

  
  withClockAndReset(io.sw.asBools(0).asClock,io.sw.asBools(1)){
    val (countvalue,wrap)=Counter(3 until 16 by 2)
    io.ledr:=countvalue
  }
}

