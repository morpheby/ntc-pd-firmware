/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "oscillators.h"
#include "uart_base.h"
#include "messaging.h"
#include "flash_store.h"
#include "display.h"
#include "menu-base.h"
#include "timing.h"
#include "wdt.h"
#include "menu.h"
#include "cn_inputs.h"
#include "mem_pool.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void) {

    /* Initialize reset parser */
    system_init();

    mempool_init();
//
//    if(reset_is_cold())
//        gc_malloc(200);

    cpu_ipl_set(0);

    /* Initialize display */
    disp_init();
    disp_config();
    menu_init();

    /* Initialize Change-notification inputs */
    cni_init();

    /* Initialize oscillator switcher */
    osc_init();

    osc_select(oscillatorPrimaryPLL);
    
    /* Initialize UART */
    uart_init();

    /* Initialize RTSP */
    flash_init();

    /* Initialize messaging */
    messages_init();

    // ??? consider removing unnecessary messages
    if(!reset_is_cold())
        disp_puts(last_failure_get());
    else
        disp_puts(_StrBootMsg);

    timing_init();

//    flash_write();

//    disp_set_fixed(2);

//    menu_move(1);
//    disp_putf(0, 12.34);

    led_off();
    led_flash(FCY/16);
    if(reset_is_cold())
        led_flash(FCY/16);

    while(1) {
        display_update();
        wdt_clr();
    }
}
