//-------------------------------------------------------------------------------
//
//     Project: Any
//
//     Purpose: PS7 Hardware SV wrapper
//
//-------------------------------------------------------------------------------

`include "cfg_params.svh"


module automatic ps7_hw_m
#(
    localparam DATA_W = `DATA_WIDTH
)
(

);

//------------------------------------------------------------------------------
//
//    Settings
//
    
//------------------------------------------------------------------------------
//
//    Types
//
    
//------------------------------------------------------------------------------
//
//    Objects
//

//------------------------------------------------------------------------------
//
//    Functions and tasks
//

//------------------------------------------------------------------------------
//
//    Logic
//


//------------------------------------------------------------------------------
//
//    Instances
//
ps7_hw_wrapper ps7_hw_inst
(
    .DDR_addr          (  ),
    .DDR_ba            (  ),
    .DDR_cas_n         (  ),
    .DDR_ck_n          (  ),
    .DDR_ck_p          (  ),
    .DDR_cke           (  ),
    .DDR_cs_n          (  ),
    .DDR_dm            (  ),
    .DDR_dq            (  ),
    .DDR_dqs_n         (  ),
    .DDR_dqs_p         (  ),
    .DDR_odt           (  ),
    .DDR_ras_n         (  ),
    .DDR_reset_n       (  ),
    .DDR_we_n          (  ),
    .FCLK0             (  ),
    .FCLK0_RST_N       (  ),
    .FIXED_IO_ddr_vrn  (  ),
    .FIXED_IO_ddr_vrp  (  ),
    .FIXED_IO_mio      (  ),
    .FIXED_IO_ps_clk   (  ),
    .FIXED_IO_ps_porb  (  ),
    .FIXED_IO_ps_srstb (  ),
    .MMR_araddr        (  ),
    .MMR_arburst       (  ),
    .MMR_arcache       (  ),
    .MMR_arid          (  ),
    .MMR_arlen         (  ),
    .MMR_arlock        (  ),
    .MMR_arprot        (  ),
    .MMR_arqos         (  ),
    .MMR_arready       (  ),
    .MMR_arsize        (  ),
    .MMR_arvalid       (  ),
    .MMR_awaddr        (  ),
    .MMR_awburst       (  ),
    .MMR_awcache       (  ),
    .MMR_awid          (  ),
    .MMR_awlen         (  ),
    .MMR_awlock        (  ),
    .MMR_awprot        (  ),
    .MMR_awqos         (  ),
    .MMR_awready       (  ),
    .MMR_awsize        (  ),
    .MMR_awvalid       (  ),
    .MMR_bid           (  ),
    .MMR_bready        (  ),
    .MMR_bresp         (  ),
    .MMR_bvalid        (  ),
    .MMR_rdata         (  ),
    .MMR_rid           (  ),
    .MMR_rlast         (  ),
    .MMR_rready        (  ),
    .MMR_rresp         (  ),
    .MMR_rvalid        (  ),
    .MMR_wdata         (  ),
    .MMR_wid           (  ),
    .MMR_wlast         (  ),
    .MMR_wready        (  ),
    .MMR_wstrb         (  ),
    .MMR_wvalid        (  )
);
//-------------------------------------------------------------------------------
endmodule
//-------------------------------------------------------------------------------

