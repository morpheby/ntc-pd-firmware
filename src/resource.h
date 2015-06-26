/* 
 * File:   resource.h
 * Author: Morphe
 *
 * Created on 10 June 2012, 9:40
 */

#ifndef RESOURCE_H
#define	RESOURCE_H

#ifdef RESOURCE_C
#  define RESOURCE
#else
#  define RESOURCE extern
#endif

RESOURCE const char
        /* Interrupts */
           *_StrOscFail,
           *_StrAddrError,
           *_StrStackError,
           *_StrMathError,
           *_StrDMAError,
           *_StrUndefISR,
        /* System errors */
           *_StrHPError,
           *_StrNoMemory,
           *_StrMsgWaitDeadlock,
           *_StrMsgUnstableLink,
           *_StrMsgCommTimeout,
           *_StrBootMsg,
           *_StrModbusCfgError,
           *_StrResetDescr[];

#endif	/* RESOURCE_H */

