//------------------------------------------------------------------------------
//
//    scmRTOS Project-level Target Configuration Header
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
//    Author: Harry E. Zhurov
//    -----------------------------------------------------
//    Project sources: https://github.com/z7bm
//
//------------------------------------------------------------------------------

#ifndef  scmRTOS_TARGET_CFG_H
#define  scmRTOS_TARGET_CFG_H

//------------------------------------------------------------------------------
// If the macro value is 0 (the default), the port uses SysTick as a system
// timer. It initializes the timer and starts it. The user must make sure that
// the address of the timer interrupt handler (SysTick_Handler) is in the right
// place at the interrupt vector table.
// If the macro value is 1, then the user has to implement (see docs for details):
//     1. extern "C" void __init_system_timer();
//     2. void LOCK_SYSTEM_TIMER() / void UNLOCK_SYSTEM_TIMER();
//     3. In the interrupt handler of the custom timer, the user needs to call
//        OS::system_timer_isr().
//
#define SCMRTOS_USE_CUSTOM_TIMER 0

//------------------------------------------------------------------------------
// Define SysTick clock frequency and its interrupt rate in Hz.
// It makes sense if USE_CUSTOM_TIMER = 0.
//
//#if   defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || defined(STM32F10X_HD_VL)
//# define SYSTICKFREQ     24000000UL
//#else
//# define SYSTICKFREQ     72000000UL
//#endif
//#define SYSTICKINTRATE  1000

//------------------------------------------------------------------------------
// Define number of priority bits implemented in hardware.
//
// #define CORE_PRIORITY_BITS  4


#endif // scmRTOS_TARGET_CFG_H

