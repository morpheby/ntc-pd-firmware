
#include "app_connector.h"
#include "board-config.h"


/*
 * Configuration checks
 */
#if !HAS_LED_DISPLAY && APP_USE_LED_DISP
#error "Display module enabled, while no display module specified"
#endif

