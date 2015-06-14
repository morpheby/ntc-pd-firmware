#include "menu_ext.h"

void menu_err_disp_enter() {
    disp_puts(last_failure_get());
}
