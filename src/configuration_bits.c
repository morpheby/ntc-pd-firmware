
#include "system.h"
#include "board-config.h"

/******************************************************************************/
/* Configuration Bits                                                         */
/*                                                                            */
/* This is not all available configuration bits for all dsPIC devices.        */
/* Refer to the dsPIC device specific .h file in the compiler                 */
/* support\dsPIC33F\h directory for complete options specific to the device   */
/* selected.  For additional information about what hardware configurations   */
/* mean in terms of device operation, refer to the device datasheet           */
/* 'Special Features' chapter.                                                */
/*                                                                            */
/******************************************************************************/

/* Selects internal oscillator */
_FOSCSEL(FNOSC_FRC);

/* Enable Clock Switching and Configure */
#if HAS_EXTERNAL_OSCILLATOR
_FOSC(FCKSM_CSECME & OSCIOFNC_OFF & POSCMD_XT);
#else
_FOSC(FCKSM_CSECME & OSCIOFNC_ON & POSCMD_NONE);
#endif

/* Turns off JTAG and selects debug channel */
_FICD(JTAGEN_OFF & ICS_PGD2);

/* Enable and configure WDT */
_FWDT(FWDTEN_ON & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS2048)

