//------------------------------------------------------------------------------
//
//    Application Program Main Source
//
//    Permission is hereby granted, free of charge, to any person
//    obtaining  a copy of this software and associated documentation
//    files (the "Software"), to deal in the Software without restriction,
//    including without limitation the rights to use, copy, modify, merge,
//    publish, distribute, sublicense, and/or sell copies of the Software,
//    and to permit persons to whom the Software is furnished to do so,
//    subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included
//    in all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
//    THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//    Copyright (c) 2017-2024, Zynq-7000 Bare-metal Project
//    -----------------------------------------------------
//    Project sources: https://github.com/z7bm
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <ps7mmrs.h>
#include <z7int.h>
#include <z7ptmr.h>
#include <z7gpio.h>
#include <z7qspi.h>
#include <z7uart.h>

#include <scmRTOS.h>
               
//---------------------------------------------------------------------------
//
//      Declarations
//
const uint32_t JE1 = 13;    // Zed Board
const uint32_t JE2 = 10;    // Zed Board

void swi_isr_handler();
void default_isr_handler();

Uart uart1(UART1_ADDR);

//---------------------------------------------------------------------------
//
//      Process types
//
typedef OS::process<OS::pr0, 1024> TProc1;
typedef OS::process<OS::pr1, 1024> TProc2;
typedef OS::process<OS::pr2, 1024> TProc3;
//---------------------------------------------------------------------------
//
//      Process objects
//
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;

//---------------------------------------------------------------------------
//
//      Test objects
//
struct TMamont                   //  data type for sending by message
{                                //
    enum TSource
    {
        PROC_SRC,
        ISR_SRC
    }
    src;
    int data;                    //
};                               //

OS::message<TMamont> MamontMsg;  // OS::message object

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
    uart1.send(print_buf);                             //
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
        ps7_register_isr(&default_isr_handler, i);
    }

    ps7_register_isr(&swi_isr_handler, PS7IRQ_ID_SW15);

    //------------------------------------------------------
    //
    //    Setup interrupts
    //
    gic_int_enable(PS7IRQ_ID_UART1);            //
    gic_set_target(PS7IRQ_ID_UART1, GIC_CPU0);  //
    gic_set_priority(PS7IRQ_ID_SW15, 30);       // lowest priority

    sbpa(GIC_ICCPMR, 0xff);                     //
    wrpa(GIC_ICCBPR, 2);                        //
    sbpa(GIC_ICDDCR, 0x1);                      // enable GIC Distributor
    sbpa(GIC_ICCICR, 0x1);                      // enable interruptes for CPU interfaces

    //------------------------------------------------------
    //
    //    CPU 0 Private Timer
    //
    OS::start_system_timer(200, 1000, 20);

    //------------------------------------------------------
    //
    //    Initialize peripherals
    //
    uart1.init();

    enable_interrupts();

    print("cam: program start!\n");

    OS::run();
}
//------------------------------------------------------------------------------
namespace OS
{
    template <>
    OS_PROCESS void TProc1::exec()
    {
        for(;;)
        {
            MamontMsg.wait();                                    // wait for message
            TMamont Mamont = MamontMsg;                          // read message content into local TMamont variable

            if (Mamont.src == TMamont::PROC_SRC)
            {
                gpio::pin_off(JE2);                              // show that message received from other process
            }
            else
            {
                gpio::pin_off(JE2);                              // show that message received from isr
                gpio::pin_on(JE2);
                gpio::pin_off(JE2);
            }
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
        for(;;)
        {
            sleep(100);
            
            // raise software interrupt
            wrpa( GIC_ICDSGIR,                                   // 0b10: send the interrupt on only to the CPU
                  (2 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
                   PS7IRQ_ID_SW15                                // rise software interrupt ID15
                  );
        }
    }
        
    template <>
    OS_PROCESS void TProc3::exec()
    {
        for (;;)
        {
            sleep(1);
            TMamont m;                                           // create message content
            m.src  = TMamont::PROC_SRC;
            m.data = 5;
            MamontMsg = m;                                       // put the content to the OS::message object
            gpio::pin_on(JE2);
            MamontMsg.send();                                    // send the message
        }
    }
}
//------------------------------------------------------------------------------
void OS::system_timer_user_hook()
{
    TMamont m;                                                   // create message content
    m.src  = TMamont::ISR_SRC;
    m.data = 10;
    MamontMsg = m;                                               // put the content to the OS::message object
    gpio::pin_on(JE2);
    MamontMsg.send_isr();                                        // send the message
}
//------------------------------------------------------------------------------
#if scmRTOS_IDLE_HOOK_ENABLE
void OS::idle_process_user_hook()
{
    __WFI();
}
#endif
//------------------------------------------------------------------------------
void swi_isr_handler()
{
    volatile auto slon = 0;
    gpio::pin_on(JE1);
    for(size_t i = 0; i < 1000; ++i)
    {
        ++slon;
    }
    gpio::pin_off(JE1);
}
//------------------------------------------------------------------------------
void default_isr_handler()
{
    __nop();
    __dsb();
    __isb();
}
//------------------------------------------------------------------------------

