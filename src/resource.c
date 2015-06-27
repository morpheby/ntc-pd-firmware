
#define RESOURCE_C

#include "resource.h"

const char 
        /* Interrupts */
           *_StrOscFail         = "Unhandled oscillator failure",
           *_StrAddrError       = "Internal addressing error",
           *_StrStackError      = "Stack error",
           *_StrMathError       = "Math error",
           *_StrDMAError        = "DMA error",
           *_StrUndefISR        = "Undefined interrupt",
        /* System errors */
           *_StrHPError         = "Priority mismatch",
           *_StrNoMemory        = "Not enough memory",
           *_StrMsgWaitDeadlock = "IPL deadlock in _wait_wait()",
           *_StrMsgUnstableLink = "Unstable link",
           *_StrMsgCommTimeout  = "Communication timeout",
           *_StrBootMsg         = "System ready",
           *_StrModbusCfgError  = "Invalid modbus configuration",
           *_StrResetDescr[]    = {
                                    "Power on",
                                    "Power fail",
                                    "MCLR",
                                    "Software reset",
                                    "WDT timeout",
                                    "Configuration mismatch",
                                    "Trap conflict",
                                    "ILL"
                                   };


