import chisel3._

/**
 * Bundle for nvboard
  */
class NVboard_IOs extends Bundle{
  val ledr=Output(UInt(16.W))
  val sw=Input(UInt(16.W))

   
  def loadDefault():Unit={
    ledr:=5.U //.asTypeOf(ledr)
  }
}
class Top_Module extends Module {
  val io=IO(new NVboard_IOs)
  val nv_sw=VecInit(io.sw.asBools)
  val nv_ledr=Vec(16,false.B)

  io.loadDefault()
  io.ledr:=nv_ledr.asUInt

  nv_ledr(0):=Mux(nv_sw(2),nv_sw(1),nv_sw(0))

  //io.ledr(0):=true.B
  //io.ledr(1):=false.B
}
