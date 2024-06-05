
#include <stddef.h>
#include <stdint.h>
#include <array>
#include "z7int.h"

extern TISRHandler PS7Handlers[PS7_MAX_IRQ_ID];

//------------------------------------------------------------------------------
class IntIdStack
{
    static const size_t SIZE = 8;
public:
    IntIdStack() : top(0) { }

    uint32_t save()
    {
        CritSect cs;
        uint32_t id = rdpa(GIC_ICCIAR);
        pool[top++] = id;
        return id;
    }

    void restore()
    {
        CritSect cs;
        wrpa(GIC_ICCEOIR, pool[--top]);
    }

private:
    std::array<uint32_t, SIZE> pool;
    size_t                     top;
    uint32_t                   mask;
};
//------------------------------------------------------------------------------

IntIdStack int_id_stack;

extern "C"
{

intptr_t PrefetchAbortAddr;
intptr_t DataAbortAddr;
intptr_t UndefinedExceptionAddr;

//------------------------------------------------------------------------------
void IRQInterrupt()             
{ 
    const uint32_t INT_ID = int_id_stack.save();
    if (INT_ID < PS7_MAX_IRQ_ID)
    {
        (*PS7Handlers[INT_ID])();
    }
    int_id_stack.restore();
}
//------------------------------------------------------------------------------

void FIQInterrupt()             { }
void UndefinedException()       { }
void SWInterrupt()              { }
void DataAbortInterrupt()       { }
void PrefetchAbortInterrupt()   { }

}  // extern "C"
//------------------------------------------------------------------------------

