#include "sfdp.h"

void NMI_Handler(void) {
    SFDP_DEBUG("NMI has occured in Flashloader.");
    while(1);
}

void DefaultISR(void) {
    SFDP_DEBUG("Undefined Interrupt has occured in Flashloader.");
    while(1);
}
