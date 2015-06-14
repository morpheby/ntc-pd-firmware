#include "menu_ext.h"
#include "messaging.h"

void menu_send_dummy_msg() {
    send_ready(0xAB, 0x6771);
}

void menu_send_break() {
    synchronize();
}

