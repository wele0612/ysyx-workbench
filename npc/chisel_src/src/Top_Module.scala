import chisel3._
import chisel3.util.Mux1H

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
  val io=IO(new NVboard_IOs)
  val nv_sw=VecInit(io.sw.asBools)
  val nv_ledr=VecInit.fill(16)(false.B)

  io.loadDefault()

  nv_ledr(0):=Mux(nv_sw(2),nv_sw(1),nv_sw(0))
  nv_ledr(1):=Mux1H(
    VecInit(nv_sw(3),nv_sw(4),nv_sw(5)),
    VecInit(true.B,false.B,true.B))

  io.ledr:=nv_ledr.asUInt
}
