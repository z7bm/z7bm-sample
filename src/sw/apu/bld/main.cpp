
#include <ps7mmrs.h>
#include "ps7_init.h"
#include "z7int.h"
#include "z7qspi.h"
#include "z7uart.h"

#include <string.h>
        
const uint32_t PIN_INT = 50;

void swi_isr_handler();
void gpio_isr_handler();
void default_isr_handler();

void cpy32(uint32_t *const dst, const uint32_t *src, const size_t count);

const uint32_t BUF_SIZE = 4*1024/4;

uint32_t Buf[BUF_SIZE];
uint32_t TargetBuf[BUF_SIZE];

const uint32_t QSPI_BUF_SIZE = 1024;

uint32_t QSpiBuf[QSPI_BUF_SIZE];
//TQSpi QSpi(QSpiBuf);

extern uint32_t QSpiBuf_ddr[QSPI_BUF_SIZE];

TQSpi QSpi_ddr(QSpiBuf_ddr);


const uint32_t UART1_TX_BUF_SIZE = 2048;
usr::ring_buffer<char, UART1_TX_BUF_SIZE, uint16_t> Uart1_TxBuf;
TUart Uart1(UART1_ADDR);


//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

const int TX_BUF_SIZE = 256;

char print_buf[TX_BUF_SIZE];
int print(const char *format, ...)
{
    va_list args;                                      //
    va_start(args, format);                            //
    uint16_t size = vsprintf(print_buf, format, args); //
    va_end(args);                                      //
                                                       //
    Uart1.send(print_buf);
    //TxBuf.write(print_buf, size);                      // put formatted data to port buffer
                                                       //
    return size;
}
//------------------------------------------------------------------------------
void remap_mmu_tt(uintptr_t src, uintptr_t dst)
{
    cpy32(reinterpret_cast<uint32_t*>(dst), reinterpret_cast<uint32_t*>(src), 4096);
    uint32_t ttbr0 = dst | 0x5b;


    uint32_t tmp;
    __asm__ __volatile__("mrc p15, 0, %[tmp], c1, c0, 0\n\t"
                         "bic %[tmp], %[tmp], #0x1\n\t"
                         "mcr p15, 0, %[tmp], c1, c0, 0\n\t"
                         : [tmp] "=r" (tmp)
                         :       "r"  (tmp));

    __asm__ __volatile__("mcr p15, 0, %1, c2, c0, 0"
                         : "=r" (ttbr0)
                         : "r"  (ttbr0));

    __asm__ __volatile__("mrc p15, 0, %[tmp], c1, c0, 0\n\t"
                         "orr %[tmp], %[tmp], #0x1\n\t"
                         "mcr p15, 0, %[tmp], c1, c0, 0\n\t"
                         "dsb\n\t"
                         "isb\n\t"
                         : [tmp] "=r" (tmp)
                         :       "r"  (tmp));
}
//------------------------------------------------------------------------------
int main() 
{ 
    //----------------------------------------------------------------
    //
    //    relocate MMU translate table
    //
    remap_mmu_tt(MMU_TT_INIT_ADDR, MMU_TT_ADDR);

    //-----------------------------------------------
    // set up output pins
    sbpa(GPIO_DIRM_0_REG, 1ul << 7);
    sbpa(GPIO_OEN_0_REG,  1ul << 7);
    sbpa(GPIO_DATA_0_REG, 1ul << 7);
    
    sbpa(GPIO_DIRM_0_REG, 1ul << 16);
    sbpa(GPIO_OEN_0_REG,  1ul << 16);
    sbpa(GPIO_DATA_0_REG, 1ul << 16);
    
    //  JE1
    sbpa(GPIO_DIRM_0_REG, 1ul << 13);
    sbpa(GPIO_OEN_0_REG,  1ul << 13);
    //set_bits_pa(GPIO_DATA_0_REG, 1ul << 13);
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | 0 );
    
    //  JE2
    sbpa(GPIO_DIRM_0_REG, 1ul << 10);
    sbpa(GPIO_OEN_0_REG,  1ul << 10);
    sbpa(GPIO_DATA_0_REG, 1ul << 10);

        
    //-----------------------------------------------
    // initialize interrupt handlers table
    for(uint32_t i = 0; i < PS7_MAX_IRQ_ID; ++i)
    {
        ps7_register_isr_handler(&default_isr_handler, i);
    }
    
    ps7_register_isr_handler(&swi_isr_handler,  PS7IRQ_ID_SW7);
    ps7_register_isr_handler(&gpio_isr_handler, PS7IRQ_ID_GPIO);
    
    //-----------------------------------------------
    // set up GPIO interrupt
    gpio_clr_int_sts(PIN_INT);
    gpio_int_pol(PIN_INT, GPIO_INT_POL_HIGH_RISE);
    gpio_int_en(PIN_INT);
        
    
    {
    //    TCritSect cs;
        gic_int_enable(PS7IRQ_ID_UART1);
        gic_set_target(PS7IRQ_ID_UART1, GIC_CPU0);
        
        gic_set_target(PS7IRQ_ID_GPIO, 1ul << GIC_CPU0);
        gic_set_config(PS7IRQ_ID_GPIO, GIC_EDGE_SINGLE);
        gic_int_enable(PS7IRQ_ID_GPIO);
        
        sbpa(GIC_ICCPMR, 0xff);
        gic_set_priority(PS7IRQ_ID_SW7, 0x10);
        
        
        sbpa(GIC_ICDDCR, 0x1);
        sbpa(GIC_ICCICR, 0x1);
    }

    enable_interrupts();
    
    void cpy32(uint32_t * const dst, const uint32_t *src, const uint32_t count);
    void trash();
    
   // QSpi.init();
    QSpi_ddr.init();
    Uart1.init();
    
    print("mamont12345678 go to North!\n");
    print("slon go to North!\n");
    
    extern unsigned char __ddr_code_start[];
    extern unsigned char __ddr_code_end[];
    extern unsigned char __ddr_src_start[];
    print("__ddr_code_start: 0x%x\n", __ddr_code_start); 
    print("__ddr_code_end:   0x%x\n", __ddr_code_end); 
    print("__ddr_src_start:  0x%x\n", __ddr_src_start); 

    bool load_img(const uint32_t img_addr);
    //load_img(0x20000);

    for(;;)
    {
        
        wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | (1ul << 13) );  // JE1 on
        wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | 0 );            // JE1 off
       // QSpi.run();
        
        
//      write_pa( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
//               (2 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
//                PS7IRQ_ID_SW7                                 // rise software interrupt ID15
//              );

        
        
        
//      write_pa( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
//               (0 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
//               (0x01 << GIC_ICDSGIR_CPU_TARGET_LIST_BPOS) +   //
//                PS7IRQ_ID_SW7                                 // rise software interrupt ID15
//              );
    }
}
//------------------------------------------------------------------------------
void gpio_isr_handler()
{
    //write_pa(GPIO_INT_STAT_1_REG, 1ul << 18);
    gpio_clr_int_sts(PIN_INT);

    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | (1ul << 7) );
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | 0 );

    //write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | (1ul << 13) );  // JE1 on
    //write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | 0 );            // JE1 off
}
//------------------------------------------------------------------------------
void swi_isr_handler()
{
    wrpa(GPIO_MASK_DATA_0_MSW_REG, (~(1ul << 0) << 16) | (1ul << 0) );
    wrpa(GPIO_MASK_DATA_0_MSW_REG, (~(1ul << 0) << 16) | 0 );
    
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | (1ul << 10) );  // JE2 on
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | 0 );            // JE2 off

}
//------------------------------------------------------------------------------
void default_isr_handler()
{
    __nop();
    __dsb();
    __isb();
}
//------------------------------------------------------------------------------
void cpy32(uint32_t * const dst, const uint32_t * src, const size_t count)
{
    for(size_t i = 0; i < count; ++i)
    {
        dst[i] = src[i];
    }
}
//------------------------------------------------------------------------------

