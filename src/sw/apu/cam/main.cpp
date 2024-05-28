
#include <ps7mmrs.h>
#include "z7int.h"
#include "z7qspi.h"
#include "z7uart.h"

               
const uint32_t PIN_INT = 50;

void swi_isr_handler();
void gpio_isr_handler();
void default_isr_handler();

uint8_t Buf[1024];
uint8_t bbb = 0x34;
uint8_t aaa = 0x56;
uint8_t ccc = 0x78;
Uart uart1(UART1_ADDR);

uint32_t slonick(uint32_t x)
{
    volatile static uint32_t Data[3000] = { 1, 2, 3, 4  };
    
    return Data[x]+ bbb; // +aaa + ccc;
}

__attribute__((section(".kot"))) __attribute__ ((aligned (4))) 
uint8_t slonick2 = 5;

//------------------------------------------------------------------------------
int main() 
{ 
    for(uint32_t i = 0; i < sizeof(Buf); ++i)
    {
        Buf[i] = slonick(i) + slonick2;
    }

    for(;;)
    {
        
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
    __nop();
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

