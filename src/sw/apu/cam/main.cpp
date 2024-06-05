
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <ps7mmrs.h>
#include "z7int.h"
#include "z7qspi.h"
#include "z7uart.h"

               
const uint32_t PIN_INT = 50;

void swi_isr_handler();
void gpio_isr_handler();
void default_isr_handler();

Uart uart1(UART1_ADDR);

//------------------------------------------------------------------------------
const int TX_BUF_SIZE = 256;

char print_buf[TX_BUF_SIZE];
int print(const char *format, ...)
{
    va_list args;                                      //
    va_start(args, format);                            //
    uint16_t size = vsprintf(print_buf, format, args); //
    va_end(args);                                      //
                                                       //
    uart1.send(print_buf);
    //TxBuf.write(print_buf, size);                      // put formatted data to port buffer
                                                       //
    return size;
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
int main() 
{ 
    //----------------------------------------------------------------
    //
    //    Setup interrupts
    //

    //------------------------------------------------------
    //
    //    Initialize interrupt handlers table
    //
    for(uint32_t i = 0; i < PS7_MAX_IRQ_ID; ++i)
    {
        ps7_register_isr_handler(&default_isr_handler, i);
    }

    ps7_register_isr_handler(&swi_isr_handler,  PS7IRQ_ID_SW15);
    ps7_register_isr_handler(&gpio_isr_handler, PS7IRQ_ID_GPIO);

    //------------------------------------------------------
    //
    //    Setup GPIO interrupt
    //
    gpio_clr_int_sts(PIN_INT);
    gpio_int_pol(PIN_INT, GPIO_INT_POL_HIGH_RISE);
    gpio_int_en(PIN_INT);

    gic_int_enable(PS7IRQ_ID_UART1);
    gic_set_target(PS7IRQ_ID_UART1, GIC_CPU0);

    gic_set_target(PS7IRQ_ID_GPIO, 1ul << GIC_CPU0);
    gic_set_config(PS7IRQ_ID_GPIO, GIC_EDGE_SINGLE);
    gic_int_enable(PS7IRQ_ID_GPIO);

    sbpa(GIC_ICCPMR, 0xff);
    gic_set_priority(PS7IRQ_ID_SW15, 30);    // lowest priority

    sbpa(GIC_ICDDCR, 0x1);  // enable GIC Distributor
    sbpa(GIC_ICCICR, 0x1);  // enable interruptes for CPU interfaces

    //------------------------------------------------------
    //
    //    Initialize peripherals
    //
    uart1.init();

    enable_interrupts();

    print("cam: program start!\n");

    uint32_t n = 100;

    for(;;)
    {
        wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | (1ul << 13) );  // JE1 on
        wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | 0 );            // JE1 off


        if(--n == 0)
        {
            n = 100;
            wrpa( GIC_ICDSGIR,                                   // 0b10: send the interrupt on only to the CPU
                  (2 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
                   PS7IRQ_ID_SW15                                // rise software interrupt ID15
                  );
            

        }

//      wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | (1ul << 10) );  // JE2 on
//      wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | 0 );            // JE2 off
                                                                              //


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
//  __nop();
//  wrpa(GPIO_MASK_DATA_0_MSW_REG, (~(1ul << 0) << 16) | (1ul << 0) );
//  wrpa(GPIO_MASK_DATA_0_MSW_REG, (~(1ul << 0) << 16) | 0 );
    
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | (1ul << 10) );  // JE2 on
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | 0 );            // JE2 off
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | (1ul << 10) );  // JE2 on
    wrpa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | 0 );            // JE2 off
                                                                          //
//  uint32_t sgi_num = rdpa(GIC_ICCIAR);
//  wrpa(GIC_ICCEOIR, sgi_num);

}
//------------------------------------------------------------------------------
void default_isr_handler()
{
    __nop();
    __dsb();
    __isb();
}
//------------------------------------------------------------------------------

