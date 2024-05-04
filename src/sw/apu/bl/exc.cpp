
#include <stdint.h>
#include "z7int.h"

extern TISRHandler PS7Handlers[PS7_MAX_IRQ_ID];

extern "C"
{

intptr_t PrefetchAbortAddr;
intptr_t DataAbortAddr;
intptr_t UndefinedExceptionAddr;

void IRQInterrupt()             
{ 
    const uint32_t INT_ID = rdpa(GIC_ICCIAR);
    if (INT_ID < PS7_MAX_IRQ_ID)
    {
        (*PS7Handlers[INT_ID])();
    }
    wrpa(GIC_ICCEOIR, INT_ID);
}
void FIQInterrupt()             { }
void UndefinedException()       { }
void SWInterrupt()              { }
void DataAbortInterrupt()       { }
void PrefetchAbortInterrupt()   { }

}



