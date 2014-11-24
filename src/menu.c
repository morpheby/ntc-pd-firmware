#include "menu.h"
#include "list.h"
#include "cn_inputs_reg.h"

typedef void (*_menuHitFunc)();
typedef void (*_menuHitWorker)(_MWrkCallReason reason);

typedef struct tagMENU {
    char *itemName;
    const void *nextItem;
    _Bool hasSubMenu;
    const void *childItem;
    _menuHitFunc on_enter;
    _menuHitWorker worker;
} _MenuItem;

typedef struct tagMENUSTATE {
    _Bool menuActive, menuItemActive;
    const _MenuItem *sel;
    _ListHandle parent, parPrev;
    _menuHitWorker selWorker;
} _MenuState_t;

void _menu_prevlist_free(_ListHandle list);
_ListHandle _menu_prevlist_copy(const _ListHandle list);
void menu_select(const _MenuItem *item);

#include "menu_ext.h"

    const _MenuItem menuDoFail = {"Do fail", 0, 0, 0, system_reset, 0};
    const _MenuItem menuSendBrk = {"Send break", &menuDoFail, 0, 0, menu_send_break, 0};
    const _MenuItem menuSendMsg = {"Send msg", &menuSendBrk, 0, 0, menu_send_dummy_msg, 0};
    const _MenuItem menuErrDisp = {"Last fail", &menuSendMsg, 0, 0, menu_err_disp_enter, 0};
        const _MenuItem menuSetSave = {"Save cfg", 0, 0, 0, 0, 0};
            const _MenuItem menuSetCommRegDev  = {"Reg new", 0, 0, 0, 0, 0};
            const _MenuItem menuSetCommRegSelf = {"Reg self", &menuSetCommRegDev, 0, 0, 0, 0};
        const _MenuItem menuSetComm = {"Conn cfg", &menuSetSave, 1, &menuSetCommRegSelf, 0, 0};
    const _MenuItem menuSettings = {"Settings", &menuErrDisp, 1, &menuSetComm, 0, 0};
    const _MenuItem menuMode = {"Mode", &menuSettings, 0, 0, 0, 0};
const _MenuItem *menuStart = &menuMode;

_PERSISTENT _MenuState_t menuState;
_Bool menuReset = 1;

void menu_worker() {
    if(menuReset) {
        menuReset = 0;
        if(menuState.menuActive) {
            if(menuState.menuItemActive) {
                if(menuState.selWorker)
                    menuState.selWorker(_mWrkReset);
                else
                    disp_puts(last_failure_get());
//                list_clear(menuState.parent);
//                list_clear(menuState.parPrev);
            } //else
                //menu_select(&menuErrDisp);
        }
    }
    if(menuState.menuItemActive && menuState.selWorker)
        menuState.selWorker(_mWrkTimer);
}

void menu_init() {
    if(reset_is_cold()) {
        menuState.menuActive = 0;
        menuState.menuItemActive = 0;
        menuState.sel = 0;
        menuState.selWorker = 0;
        menuState.parent = list_create();
        menuState.parPrev = list_create();
        list_set_free(menuState.parPrev, _menu_prevlist_free);
        list_set_copy(menuState.parPrev, _menu_prevlist_copy);
    }
}

void menu_show() {
    menuState.selWorker = 0;
    menuState.menuItemActive = 0;
    menu_return();
}

void menu_close() {
    list_clear(menuState.parent);
    list_clear(menuState.parPrev);
    menuState.menuActive = 0;
    menuState.menuItemActive = 0;
    menuState.selWorker = 0;
    disp_cls();
}

void menu_select(const _MenuItem *item) {
    if(!item->hasSubMenu) {
        if(menuState.menuActive && menuState.sel) {
            list_push_back(menuState.parent, menuState.sel);
            list_push_back(menuState.parPrev, list_create());
        }
        menuState.sel = 0;
        menuState.menuItemActive = 1;
        if(item->on_enter)
            item->on_enter();
        menuState.selWorker = item->worker;
    }
}

void menu_enter() {
    if(menuState.menuItemActive) {
        if(menuState.selWorker)
            menuState.selWorker(_mWrkBtnEnter);
    } else if(!menuState.menuActive || !menuState.sel) {
        menuState.menuActive = 1;
        menuState.sel = menuStart;
        list_push_back(menuState.parent, 0);
        list_push_back(menuState.parPrev, list_create());
        disp_puts(menuState.sel->itemName);
    } else {
        list_push_back(menuState.parent, menuState.sel);
        list_push_back(menuState.parPrev, list_create());
        if(!menuState.sel->hasSubMenu) {
            _MenuItem *pItem = menuState.sel;
            menuState.menuItemActive = 1;
            menuState.sel = 0;
            menuState.selWorker = 0;
            if(pItem->on_enter)
                pItem->on_enter();
            menuState.selWorker = pItem->worker;
        } else {
            menuState.sel = menuState.sel->childItem;
            disp_puts(menuState.sel->itemName);
        }
    }
}

void menu_return() {
    if(menuState.menuItemActive && menuState.selWorker)
            menuState.selWorker(_mWrkBtnReturn);
    else if(!menuState.menuActive)
        return;
    else if(list_back(menuState.parent)) {
        menuState.menuItemActive = 0;
        menuState.sel = (_MenuItem*) list_back(menuState.parent);
        list_pop_back(menuState.parent);
        list_pop_back(menuState.parPrev);
        disp_puts(menuState.sel->itemName);
    } else {
        menuState.menuActive = 0;
        menuState.menuItemActive = 0;
        menuState.sel = 0;
//        list_clear(menuState.parent);
//        list_clear(menuState.parPrev);
        list_pop_back(menuState.parent);
        list_pop_back(menuState.parPrev);
        disp_cls();
    }
}

void menu_move(_Bool next) {
    if(menuState.menuItemActive) {
        if(menuState.selWorker)
            menuState.selWorker(next ? _mWrkBtnRight : _mWrkBtnLeft);
    } else if(!menuState.menuActive)
        return;
    else if(next && menuState.sel->nextItem && list_back(menuState.parPrev)) {
        list_push_back(list_back(menuState.parPrev), (_ValueType)menuState.sel);
        menuState.sel = menuState.sel->nextItem;
        disp_puts_ani(menuState.sel->itemName, 1);
    } else if(!next && list_back(menuState.parPrev) && list_back(list_back(menuState.parPrev))) {
        menuState.sel = list_back(list_back(menuState.parPrev));
        list_pop_back(list_back(menuState.parPrev));
        disp_puts_ani(menuState.sel->itemName, -1);
    }
}

CNI_DECL_PROC_FN(CNI_BTN_ENTER) {
    if(!__on)
        menu_enter();
}

CNI_DECL_PROC_FN(CNI_BTN_LEFT) {
    if(!__on)
        menu_move(0);
}

CNI_DECL_PROC_FN(CNI_BTN_RIGHT) {
    if(!__on)
        menu_move(1);
}

CNI_DECL_PROC_FN(CNI_BTN_RETURN) {
    if(!__on)
        menu_return();
}


void _menu_prevlist_free(_ListHandle list) {
    list_free(list);
}

_ListHandle _menu_prevlist_copy(const _ListHandle list) {
    return list_copy(list);
}
