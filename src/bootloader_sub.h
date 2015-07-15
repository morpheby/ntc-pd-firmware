/* 
 * File:   bootloader_sub.h
 * Author: morpheby
 *
 * Created on серада, 15, ліпеня 2015, 19.05
 */

#ifndef BOOTLOADER_SUB_H
#define	BOOTLOADER_SUB_H

#include "system.h"
#include "app_connector.h"
#include "flash_store.h"

#if APP_USE_BOOTLOADER

// List methods are not secure, but they are used exclusively for RTSP and in
// a way, that makes their temporary absence irrelevant. Bootloader mode doesn't
// invoke any of these
#define sub_list_front list_front
#define sub_list_begin list_begin
#define sub_list_iterate_fwd list_iterate_fwd
#define sub_list_iterator_free list_iterator_free
#define sub_list_begin list_begin
#define sub_list_iterator_value list_iterator_value
#define sub_list_iterate_fwd list_iterate_fwd
// Same about GC/MP functions - their temporary absence is irrelevant
#define sub_gc_free gc_free
#define sub_gc_malloc gc_malloc

uint8_t sub_cpu_ipl_set(int ipl);

SYSHANDLE sub_high_priority_enter();

void sub_high_priority_exit(SYSHANDLE hp);

#endif

#endif	/* BOOTLOADER_SUB_H */

