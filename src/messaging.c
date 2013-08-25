
#include "messaging.h"

#include "uart_base.h"
#include "flash_store.h"
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* Protocol description is provided in file messaging-info in the project root*/

/* Consider splitting this file */

#define MAGIC 0xA5
#define MAX_NODES 0x20

const uint8_t _msgAck  = 0x00,
              _msgNAck = 0xFF;

#define GLOBAL_ADDR UART_GLOBAL_ADDR
#define NO_ADDR     UART_NO_ADDR


typedef enum tagMSGTYPE {
    _msgReady       = 0x00,
    _msgError       = 0x01,
    _msgPreRegister = 0x02,
    _msgLock        = 0x03,
    _msgNetworkInfo = 0x04,
    _msgFind        = 0x05,
    _msgData        = 0x06,
    _msgRequest     = 0x07,
    _msgControlOn   = 0x08,
    _msgControlOff  = 0x09,
    _msgNameCheck   = 0x0A,
    _msgRegister    = 0x0B,
    _msgEnumNames   = 0x0C,
    _msgPing        = 0x0D,
    _msgSync        = 0x0E
} _MessageType;

// Lookup table
const _Bool _DataMessage [] = {
    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  */
       0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 // 0
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 7
    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  */
};
const _Bool _WantAck [] = {
    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  */
       1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0 // 0
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 7
    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  */
};

typedef struct tagPACKET {
    uint8_t targetAddr;
    _Bool wantAck :1;
    _MessageType msgType :7;

    _Address sourceAddress;

    union {
        uint16_t param;
        uint16_t dataSize;
    };

    /* packetXorSum shall be aligned on 4-byte boundary */
    uint8_t packetXorSum,
            dataXorSum;
} _Packet;

typedef struct tagMESSAGE {
    _MessageType msgType;
    _Address sourceAddress;
    union {
        uint16_t param;
        uint16_t dataSize;
    };
    void *data;
} _Message;

typedef struct tagSTATMESSAGE {
    _Bool ok;
    uint16_t param;
    _Address sourceAddress;
} _StatusMessage;


typedef void(*_statmsg_callback_t)(const _StatusMessage *msg);
typedef void(*_msg_callback_t)(const _Message *msg);

/* Local fuction forward declarations */
void        network_lock();
void        network_unlock();
uint8_t     get_ack();
void        send_packet(_Packet *packet, const unsigned char *data);
void        packet_init(_Address target, _Packet *packet, _MessageType type);
void        send_simple_packet(_MessageType type, _Address addr);
void        send_ack(_Address addr);
void        send_nack(_Address addr);
void        send_ready(_Address addr, uint16_t param);
void        send_error(_Address addr);
void        send_prereg();
void        send_netinfo();
void        send_find(_Address addr, const char *name);
void        send_data(_Address addr, const unsigned char *data, size_t size);
void        send_request(_Address addr, uint16_t resID);
void        send_control_on(_Address addr, uint16_t resID);
void        send_control_off(_Address addr, uint16_t resID);
void        send_name_check(_Address addr, const char *name);
void        send_reg();
void        send_sync();
void        reg_packet_process(const _Packet *packet, const unsigned char *data);
void        pp_init();
void        packet_process(const _Packet *packet, const unsigned char *data);
void        pp_reg_statmsg_callback(_statmsg_callback_t func);
void        pp_reg_msg_callback(_msg_callback_t func);
void        addrtable_add(_Address address);
void        addrtable_add_a(const _Address *addresses, uint8_t size);
void        addrtable_clear();
void        address_set(_Address addr);
int         addrtable_read(_Address *buffer);
_StatusMessage
            wait_status();
_Message    wait_message(_MessageType msg);
void        packet_prepare(_Packet *packet, const unsigned char *data);
void        packet_xor(_Packet *packet);
_Bool       packet_validate(_Packet *packet);
_Bool       packet_validate_data(const _Packet *packet, const unsigned char *data);
uint32_t    xorsum(const unsigned char *data, size_t size);
_Bool       is_ack(uint8_t msg);
inline
   _Bool    packet_has_data(const _Packet *packet);
inline
   _Bool    packet_want_ack(const _Packet *packet);


/* Variables declaration */

//_FLASH_ACCESS uint16_t nodeAddress _FLASH_STORE = NO_ADDR;
_FLASH_ACCESS uint16_t nodeAddress _FLASH_STORE = 0x02; // XXX test
_FLASH_ACCESS uint8_t  _addressCount _FLASH_STORE = 0;
_FLASH_ACCESS uint16_t _addressTable[MAX_NODES] _FLASH_STORE;
_PERSISTENT size_t waitingData;     // Size of the data we are waiting for
_Bool packetPresent _PERSISTENT,    // 1 if recievedPacket is a valid packet
      dataPresent   _PERSISTENT,    // 1 if recievedData contains data
      waitingPacket _PERSISTENT,    // 1 if we are to recieve a packet
      regProcess    _PERSISTENT,
      waitingReg = 0/* not _PERSISTENT*/,  // 1 if device waits for PREREG msg
      extLock       _PERSISTENT;    // Set if ext. node issued LOCK message

_PERSISTENT _Packet recievedPacket; // A packet, that was recieved
// Data to be recieved. NOTE: befeore setting waitingData, preallocate
// recievedData accordingly
_PERSISTENT unsigned char *recievedData;

_PERSISTENT _msg_callback_t     _pp_msg_callback;
_PERSISTENT _statmsg_callback_t _pp_statmsg_callback;

_Bool _msgWaitLock = 0;
_StatusMessage _msgWaitStatMsg;
_Message       _msgWaitMsg;
_MessageType   _msgWaitFor;


/******************************************************************************/
/************************** EXPORT FUNCTIONS **********************************/

/**************************** MESSAGING BASE **********************************/

void messages_check() {
    if(waitingPacket && uart_get_data_size() >= sizeof(_Packet)) {
        // Dance ;)
        waitingPacket = 0;
        uart_recieve((unsigned char *) &recievedPacket, sizeof(_Packet));

        if(packet_validate(&recievedPacket) && recievedPacket.wantAck) {
            // We've got damaged packet
            send_nack(recievedPacket.sourceAddress);
            uart_flush_rx();
        } else {
            if(recievedPacket.wantAck)
                send_ack(recievedPacket.sourceAddress);
            packetPresent = 1;
        }
    } else if(waitingData && uart_get_data_size() >= waitingData) {
        uart_recieve(recievedData, waitingData);
        if(packet_validate_data(&recievedPacket, recievedData) && 
                recievedPacket.wantAck) {
            send_nack(recievedPacket.sourceAddress); // Invalid data recieved
            packetPresent = 0;
            waitingPacket = 1; // Reget packet
        } else {
            if(recievedPacket.wantAck)
                send_ack(recievedPacket.sourceAddress);
            dataPresent = 1;
        }
        waitingData = 0;
    }

    if(!waitingData && packetPresent) {
        if(packet_has_data(&recievedPacket) && !dataPresent) {
            /* Not enough information to process packet */
            if(recievedData)
                gc_free(recievedData);
            recievedData = gc_malloc(recievedPacket.dataSize);
            waitingData = recievedPacket.dataSize;
        }
        else {
            if(regProcess)
                reg_packet_process(&recievedPacket,
                        dataPresent ? recievedData : 0);
            else
                packet_process(&recievedPacket,
                        dataPresent ? recievedData : 0);

            /* Invalidate processed packet */
            packetPresent = 0;
            dataPresent = 0;

            /* Recieve next packet */
            waitingPacket = 1;
        }

    }

}

void messages_init() {
    if(reset_is_cold()) {
        waitingData  = 0;
        packetPresent = 0;
        dataPresent   = 0;
        if(nodeAddress != NO_ADDR) {
            extLock = 0;
            waitingPacket = 1;
        } else {
            extLock = 1;
            waitingPacket = 0;
        }
        uart_set_addr(nodeAddress);
        recievedData = 0;
        pp_init();
    }
}

void messages_reg_device() {
    waitingReg = 1;
}

void messages_register() {
    if(nodeAddress == NO_ADDR)
        send_prereg();
}

void messages_unreg() {
    // Reset messaging system
    waitingPacket = 0;
    waitingData   = 0;
    waitingReg    = 0;
    packetPresent = 0;
    dataPresent   = 0;
    recievedData  = 0;
    pp_init();

    addrtable_clear();
    address_set(NO_ADDR);
}





/****************************** DIALOGS ***************************************/

_Address find_name(const char *name, uint16_t *resID) {
    int i;
    _StatusMessage res;

    int addrCount = addrtable_read(0);
    _Address *addresses = gc_malloc(addrCount * sizeof(_Address));

    addrtable_read(addresses);

    for(i = 0; i < addrCount; ++i) {
        send_find(addresses[i], name);
        do
            res = wait_status();
        while(res.sourceAddress != addresses[i]);
        if(res.ok)
            break;
    }
    if(i == addrCount)
        return 0;
    if(resID)
        *resID = res.param;
    return res.sourceAddress;
}

void * get_data(const char *name, size_t *dataSize) {
    uint16_t resID;
    _Address addr;
    _Message msg;
    void *data;

    addr = find_name(name, &resID);
    if(!addr)
        return 0;
    // XXX check if resID represents 'data'

    send_request(addr, resID);
    msg = wait_message(_msgData);

    *dataSize = msg.dataSize;
    data = gc_malloc(msg.dataSize);
    memcpy(msg.data, data, msg.dataSize);

    return data;
}

_Bool switch_status(const char *name) {
    _StatusMessage msg;
    _Address addr;
    uint16_t resID;

    addr = find_name(name, &resID);
    if(!addr)
        return 0;
    // XXX check if resID represents 'switch'

    send_request(addr, resID);
    msg = wait_status();

    return msg.ok && msg.param;
}

void switch_control(const char *name, _Bool switchOn) {
    _Address addr;
    uint16_t resID;

    addr = find_name(name, &resID);
    if(!addr)
        return;
    // XXX check if resID represents 'switch'

    if(switchOn)
        send_control_on(addr, resID);
    else
        send_control_off(addr, resID);
}

// return true if name is unique
_Bool check_name(const char *name) {
    /* Maybe sending _msgNameCheck is faster but it lacks ACK check so
       it is not reliable */
    // NOTE: _msgNameCheck is never actually used. Consider it's removal
    // send_name_check(GLOBAL_ADDR, name);
    return !find_name(name, 0);
}


// XXX
void sync_funct() {

}

void synchronize() {
    send_sync();
    uart_sync_transmit(sync_funct);
    // XXX
}








/******************************************************************************/
/*************************** INTERNAL FUNCTIONS *******************************/

/***************************** PACKET RECIEVING *******************************/

uint8_t get_ack() {
    uint8_t ack;
    int i = UART_RATE_CYCLES * 15 / 10;
    // 15 = start bit + stop bit + 9 bits + 4 as timeout
    // 10 == just a guess for number of cycles happening in the loop

    // Wait for transmission to end
    while(!uart_tx_empty());
    while(i && uart_get_data_size() < sizeof(ack))
        --i;
//    if(!i)
//        system_fail(_StrMsgCommTimeout);
//    uart_recieve((char *)&ack, sizeof(ack));
    // XXX
    return ack;
}







/***************************** PACKET SENDING *********************************/

// packet is guaranteed to be the same after call, however it may be changed
// in progress
void send_packet(_Packet *packet, const unsigned char *data) {
    _Bool ackSupressed = 0;

    // XXX Probably one should prefer buffering outgoing packets
    while(extLock);

    if(data)
        network_lock();

    if(packet->targetAddr == GLOBAL_ADDR) {
        packet->wantAck = 0; // supress ACK
        ackSupressed = 1;
    }

  sendPacket:
// XXX: give up after some count of retries
    do {
        uart_send(packet->targetAddr, (unsigned char *) packet, sizeof(_Packet));
    } while(packet->wantAck && !is_ack(get_ack()));


    if(data) {
        uart_send(packet->targetAddr, data, packet->dataSize);
        if(packet->wantAck && !is_ack(get_ack()))
            goto sendPacket;
    }

    if(data)
        network_unlock();

    if(ackSupressed)
        packet->wantAck = 1;
}

void packet_init(_Address target, _Packet *packet, _MessageType type) {
    memset(packet, 0, sizeof(_Packet));
    packet->targetAddr    = target;
    packet->sourceAddress = nodeAddress;
    packet->msgType       = type;
    packet->param         = 0;
    packet->wantAck       = packet_want_ack(packet);
}

void send_simple_packet(_MessageType type, _Address addr) {
    _Packet packet;

    packet_init(addr, &packet, type);
    packet_prepare(&packet, 0);
    send_packet(&packet, 0);
}

void _send_lock(_Bool lock) {
    _Packet lockPacket;

    packet_init(GLOBAL_ADDR, &lockPacket, _msgLock);
    lockPacket.param = lock;
    packet_prepare(&lockPacket, 0);
    send_packet(&lockPacket, 0);
}

void send_prereg() {
    regProcess = 1;
    waitingPacket = 1;

    send_simple_packet(_msgPreRegister, GLOBAL_ADDR);
}

void send_nack(_Address addr) {
    uart_send(addr, (unsigned char *) &_msgNAck, sizeof(_msgNAck));
}

void send_ack(_Address addr) {
    uart_send(addr, (unsigned char *) &_msgAck, sizeof(_msgAck));
}

void send_netinfo() {
    _Packet packet;
    int addrCount = addrtable_read(0);
    _Address *addresses = gc_malloc(addrCount * sizeof(_Address));

    addrtable_read(addresses);

    packet_init(NO_ADDR, &packet, _msgNetworkInfo);
    packet.dataSize = addrCount*sizeof(_Address);
    packet_prepare(&packet, (unsigned char *) addresses);
    send_packet(&packet, (unsigned char *)addresses);

    gc_free(addresses);
}

void send_reg() {
    send_simple_packet(_msgRegister, GLOBAL_ADDR);
}

void send_ready(_Address addr, uint16_t param) {
    _Packet packet;

    packet_init(addr, &packet, _msgReady);
    packet.param = param;
    packet_prepare(&packet, 0);
    send_packet(&packet, 0);
}

void send_error(_Address addr) {
    send_simple_packet(_msgError, addr);
}

void send_sync() {
    send_simple_packet(_msgSync, GLOBAL_ADDR);
}

void _send_data_packet(_Address addr, _MessageType type, const unsigned char *data,
                size_t size) {
    _Packet packet;

    packet_init(addr, &packet, type);
    packet.dataSize = size;
    packet_prepare(&packet, data);
    send_packet(&packet, data);
}

void send_find(_Address addr, const char *name) {
    _send_data_packet(addr, _msgFind, (unsigned char *) name, strlen(name) + 1);
}

void send_name_check(_Address addr, const char *name) {
    _send_data_packet(addr, _msgNameCheck, (unsigned char *) name, strlen(name) + 1);
}

void send_data(_Address addr, const unsigned char *data, size_t size) {
    _send_data_packet(addr, _msgData, data, size);
}

void _send_res_packet(_Address addr, _MessageType msgID, uint16_t resID) {
    _Packet packet;

    packet_init(addr, &packet, msgID);
    packet.param = resID;
    packet_prepare(&packet, 0);
    send_packet(&packet, 0);
}

void send_request(_Address addr, uint16_t resID) {
    _send_res_packet(addr, _msgRequest, resID);
}

void send_control_on(_Address addr, uint16_t resID) {
    _send_res_packet(addr, _msgControlOn, resID);
}

void send_control_off(_Address addr, uint16_t resID) {
    _send_res_packet(addr, _msgControlOff, resID);
}









/****************************** NETWORK STATE *********************************/

void network_lock() {
    _send_lock(1);
}

void network_unlock() {
    _send_lock(0);
}







/************************ RECIEVED PACKET PROCESSING **************************/

// Processes packets while device is not registered
void reg_packet_process(const _Packet *packet, const unsigned char *data) {
    // Catch only NETINFO and UNLOCK
    if(packet->msgType == _msgLock && packet->param == 0) {
        // We have to repeat registration procedure
        send_prereg();
    } else if(packet->msgType == _msgNetworkInfo) {
        // Process and store network information
        addrtable_add_a((_Address *)data, packet->dataSize);
    }
}

void pp_init() {
    regProcess = 0;
    _pp_statmsg_callback = 0;
    _pp_msg_callback = 0;
}

void _pp_call_smc(const _Packet *packet) {
    if(_pp_statmsg_callback) {
        _StatusMessage msg;
        msg.ok = packet->msgType == _msgReady;
        msg.param = packet->param;
        msg.sourceAddress = packet->sourceAddress;
        _pp_statmsg_callback(&msg);
    }
}

void _pp_call_mc(const _Packet *packet, const void *data) {
    if(_pp_msg_callback) {
        _Message msg;
        msg.msgType = packet->msgType == _msgReady;
        msg.sourceAddress = packet->sourceAddress;
        msg.param = packet->param;
        msg.data = data;
        _pp_msg_callback(&msg);
    }
}

void packet_process(const _Packet *packet, const unsigned char *data) {
    switch(packet->msgType) {
     /* Status messages */
    case _msgReady:
    case _msgError:
        _pp_call_smc(packet);
        break;
     /* Non-static input messages */
    case _msgData:
//    case _msgSet:
        _pp_call_mc(packet, data);
        break;
     /* Non-static output messages */
    case _msgControlOn:
    case _msgControlOff:
    case _msgRequest:
    case _msgFind:
    case _msgNameCheck:
    case _msgEnumNames:
        break;
     /* Static internal messages */
    case _msgLock:
        extLock = packet->param;
        break;
    case _msgPreRegister:
        // If user initiated registration, send NETINFO
        if(waitingReg)
            send_netinfo();
        break;
    case _msgRegister:
        // Ask user confirmation
        // XXX add interaction with user
        addrtable_add(packet->sourceAddress);
        break;
    case _msgNetworkInfo:
        /* Do nothing. This message is not for us */
        break;
    case _msgPing:
        led_flash(FCY);
        send_ready(packet->sourceAddress, 0);
        break;
    case _msgSync:
//        uart_sync_recieve();
        // XXX
        break;
    default:
        // Probably newer protocol version. Ignore
        break;
    }
}

void pp_reg_statmsg_callback(_statmsg_callback_t func) {
    _pp_statmsg_callback = func;
}

void pp_reg_msg_callback(_msg_callback_t func) {
    _pp_msg_callback = func;
}










/**************************** FLASH CONFIGURATION *****************************/

void _addrtable_write(_Address address, uint8_t index) {
    flash_write(FLASH_GETPAGE(_addressTable),
        FLASH_GETAOFFSET(_addressTable, index),
        address);
}

void _addrtable_store(uint8_t tableSize) {
    flash_write(FLASH_GETPAGE(&_addressCount),
            FLASH_GETOFFSET(&_addressCount),
            tableSize);
}

void addrtable_add(_Address address) {
    _addrtable_write(address, _addressCount);
    
    _addrtable_store(_addressCount + 1);
}

void addrtable_add_a(const _Address *addresses, uint8_t size) {
    uint8_t i;
    for(i = 0; i < size; ++i)
        _addrtable_write(addresses[i], _addressCount+i);
    _addrtable_store(_addressCount + size);
}

void addrtable_clear() {
    _addrtable_store(0);
}

void address_set(_Address addr) {
    flash_set(FLASH_GETPAGE(&nodeAddress),
            FLASH_GETOFFSET(&nodeAddress),
            addr);
    uart_set_addr(addr);
}

int addrtable_read(_Address *buffer) {
    int i;
    if(buffer)
        for(i = 0; i < _addressCount; ++i)
            buffer[i] = _addressTable[i];
    return _addressCount;
}









/******************************************************************************/

void _statmsg_wait_callback(const _StatusMessage *msg) {
    _msgWaitStatMsg = *msg;
    _msgWaitLock = 0;
}

void _msg_wait_callback(const _Message *msg) {
    if(msg->msgType == _msgWaitFor) {
        _msgWaitMsg = *msg;
        _msgWaitLock = 0;
    }
}

void _reg_wait(_MessageType msg) {
    _msgWaitLock = 1;
    _msgWaitFor = msg;
    if(msg == _msgReady)
        pp_reg_statmsg_callback(_statmsg_wait_callback);
    else
        pp_reg_msg_callback(_msg_wait_callback);
}

void _unreg_wait() {
    if(_msgWaitFor == _msgReady)
        pp_reg_statmsg_callback(0);
    else
        pp_reg_msg_callback(0);
}

void _wait_wait(_MessageType msg) {
    if(cpu_ipl_get() >= UART_IPL)
        system_fail(_StrMsgWaitDeadlock);

    _reg_wait(msg);
    do
        messages_check();
    while(_msgWaitLock);
    _unreg_wait();
}

_StatusMessage wait_status() {
    _wait_wait(_msgReady);
    return _msgWaitStatMsg;
}

_Message wait_message(_MessageType msg) {
    _wait_wait(msg);
    return _msgWaitMsg;
}









/**************************** UTILITY FUNCTIONS *******************************/

void packet_xor(_Packet *packet) {
    packet->packetXorSum = xorsum((unsigned char *)packet, sizeof(_Packet));
}

void packet_prepare(_Packet *packet, const unsigned char *data) {
    if(data)
        packet->dataXorSum = xorsum((unsigned char *)data, packet->dataSize);

    packet->packetXorSum = MAGIC; // Initial value
    packet_xor(packet);
}

_Bool packet_validate(_Packet *packet) {
    if(packet->targetAddr != GLOBAL_ADDR && packet->targetAddr != nodeAddress)
        return 1;
    
    packet_xor(packet);

    if(packet->packetXorSum != MAGIC)
        return 1;

    return 0;
}

_Bool packet_validate_data(const _Packet *packet, const unsigned char *data) {
    if(data && xorsum((unsigned char *)data, packet->dataSize) != packet->dataXorSum)
        return 1;

    return 0;
}

uint32_t xorsum(const unsigned char *data, size_t size) {
    uint32_t xorSum;
    for(xorSum = 0; size; --size, ++data)
        xorSum ^= *data;
    return xorSum;
}

inline _Bool packet_has_data(const _Packet *packet) {
    return _DataMessage[packet->msgType];
}

inline _Bool packet_want_ack(const _Packet *packet) {
    return _WantAck[packet->msgType];
}

_Bool is_ack(uint8_t msg) {
    int i;
    for(i = 0; msg; msg>>=1)
        if(msg&1)
            ++i;
    if(i<4)
        return 1;
    else if(i>4)
        return 0;
    else
        system_fail(_StrMsgUnstableLink);
}
