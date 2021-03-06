/**
 ****************************************************************************************
 *
 * @file mem_DA14585_586.lds
 *
 * @brief Common GNU LD linker script file - memory configuration.
 *
 * Copyright (c) 2018-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

/*
 * This file needs to be run nagainst C preprocessor before it is used by the linker program.
 *
 * ==============================================================================================================================
 * |                                                         System RAM                                                         |
 * ------------------------------------------------------------------------------------------------------------------------------
 * |+ 1st RAM block (32KB)         + 2nd RAM block (16KB)         + 3rd RAM block (16KB)         + 4th RAM block (32KB)         |
 * ------------------------------------------------------------------------------------------------------------------------------
 * |                                                              ^                 ^            ^                   ^          |
 * |                                                              |                 |            |                   |          |
 * |                                                              |        RET_MEM_BASE_ADDR     |                   |          |
 * |                                                              |                       RAM_4_BASE_ADDR            |          |
 * |                                                       RAM_3_BASE_ADDR                                      __SCT_BLE_BASE  |
 * ==============================================================================================================================
 */

/* Macro to align val on the multiple of 4 equal or nearest higher */
#define ALIGN4_HI(val) (((val)+3) & (~(3)))

#if !defined(CFG_RET_DATA_SIZE)
    #error "CFG_RET_DATA_SIZE is not defined!"
#endif

#if !defined(CFG_RET_DATA_UNINIT_SIZE)
    #error "CFG_RET_DATA_UNINIT_SIZE is not defined!"
#endif

/* Retained memory should be pushed at the end of SRAM4 leaving only place for BLE Exchange Memory at the very end
 * Must be aligned to 4 bytes due to __STARTUP_CLEAR_BSS_MULTIPLE enabled zeroing code
 */
#define RET_MEM_BASE_ADDR    ALIGN4_HI((__SCT_BLE_BASE - (CFG_RET_DATA_UNINIT_SIZE + CFG_RET_DATA_SIZE + RET_HEAP_SIZE)))
#define RET_MEM_MAX_SIZE     ALIGN4_HI((CFG_RET_DATA_UNINIT_SIZE + CFG_RET_DATA_SIZE + RET_HEAP_SIZE))

#if defined(CFG_CODE_LOCATION_OTP) && defined(CFG_CODE_LOCATION_EXT)
    #error "Only one of CFG_CODE_LOCATION_OTP and CFG_CODE_LOCATION_EXT must be defined!"
#elif defined(CFG_CODE_LOCATION_OTP)
    #define CODE_LOCATION_OTP   1
    #define CODE_LOCATION_EXT   0
#elif defined(CFG_CODE_LOCATION_EXT)
    #define CODE_LOCATION_OTP   0
    #define CODE_LOCATION_EXT   1
#else
    #error "One of CFG_CODE_LOCATION_OTP and CFG_CODE_LOCATION_EXT must be defined!"
#endif

/* These defines are specific to DA14585_586, do not alter. */
#define SRAM_BASE_ADDR      0x07fc0000

#if defined(CFG_TRNG)
    #define TRNG_BUFFER_AREA_SZ CFG_TRNG
#else
    #define TRNG_BUFFER_AREA_SZ 0x400
#endif
#define BOOT_VECTOR_AREA_SZ 0xC0
#define PATCH_TABLE_AREA_SZ 0x50

/* Code area in SRAM starts after the boot vector and patch table. */
#if CODE_LOCATION_OTP
    #define CODE_AREA_BASE      (SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ)
#elif CODE_LOCATION_EXT
    #define CODE_AREA_BASE      (SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ + TRNG_BUFFER_AREA_SZ)
#endif

/* We can utilise this memory up to the retained memory region for RET_DATA* and RET_HEAP */
#define CODE_AREA_MAX_SIZE  (RET_MEM_BASE_ADDR - CODE_AREA_BASE)

MEMORY
{
    LR_IROM1          (rwx) : ORIGIN = SRAM_BASE_ADDR,                                              LENGTH = BOOT_VECTOR_AREA_SZ
    LR_IROM2          (rwx) : ORIGIN = SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ,                        LENGTH = PATCH_TABLE_AREA_SZ
#if CODE_LOCATION_EXT
    LR_TRNG_ZI        (rwx) : ORIGIN = SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ,  LENGTH = TRNG_BUFFER_AREA_SZ
#endif
    LR_IROM3          (rwx) : ORIGIN = CODE_AREA_BASE,                                              LENGTH = CODE_AREA_MAX_SIZE
    LR_RETAINED_RAM0  (rw)  : ORIGIN = RET_MEM_BASE_ADDR,                                           LENGTH = RET_MEM_MAX_SIZE
    /* After this there's only BLE Exchange Memory, externally defined by the __SCT_BLE_BASE address and with custom zeroing ode in arch_rom.c*/
}
