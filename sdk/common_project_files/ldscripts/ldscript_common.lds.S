/**
 ****************************************************************************************
 *
 * @file ldscript_common.lds.S
 *
 * @brief Common GNU LD linker script file.
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

/* Definitions tested by da1458x_scatter_config.h */
#undef ARM_SCATTERFILE

#include "da1458x_config_basic.h"
#include "da1458x_config_advanced.h"
#include "da1458x_scatter_config.h"

#if   defined ( __DA14531__ )
    #include "mem_DA14531.lds"
#else
    #include "mem_DA14585_586.lds"
#endif

/* Provide symbols with retained code section base and it's length. */
__ER_IROM3_BASE__   = ORIGIN(LR_IROM3);
__ER_IROM3_LENGTH__ = LENGTH(LR_IROM3);

/* Set the entry instruction */
ENTRY(Reset_Handler)

/* Sections layout */
SECTIONS
{
    /* Reset vector at the beginning of RAM */
    ER_IROM1 :
    {
        KEEP(*(.vectors))
        . = BOOT_VECTOR_AREA_SZ;
    } > LR_IROM1

    /* Patched ROM functions space. Patching is done in patch.c:patch_func() */
    ER_IROM2 :
    {
        KEEP(*(.patch_table))
        . = PATCH_TABLE_AREA_SZ;
    } > LR_IROM2

#if CODE_LOCATION_EXT
#if !defined(__DA14531__)
    LR_TRNG_ZI :
    {
        /* The TRNG buffer area must be located lower than the 64K boundary. */
        *(trng_buffer_area_zi)
        /* . = TRNG_BUFFER_AREA_SZ; */
    } > LR_TRNG_ZI

    ASSERT(SIZEOF(LR_TRNG_ZI) <= TRNG_BUFFER_AREA_SZ, "LR_TRNG_ZI to small!")
#endif    
#endif

    ER_IROM3 :
    {
        /* Move ISR implementations after patch_table */
        KEEP(*(.isr_impl))
        KEEP(*(.text.disp_heaplog))
        *(.text*)
        
        . = ALIGN(4);

        KEEP(*(.init))
        KEEP(*(.fini))

        *(.rodata*)

         
    } > LR_IROM3
    
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > LR_IROM3
    
    __exidx_start = .;
    .ARM.exidx :
    {
       *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > LR_IROM3
    __exidx_end = .;
    
    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S
     */
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG (__data_end__ - __data_start__)
        __copy_table_end__ = .;
    } > LR_IROM3

    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup.S
     */
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
#if CODE_LOCATION_OTP
        LONG (__trng_buffer_area_zi_start__)
        LONG (__trng_buffer_area_zi_end__ - __trng_buffer_area_zi_start__)
#endif
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        LONG (__heap_mem_area_not_ret_start__)
        LONG (__heap_mem_area_not_ret_end__ - __heap_mem_area_not_ret_start__)
        LONG (__ret_data_start__)
        LONG (__ret_data_end__ - __ret_data_start__)

        __zero_table_end__ = .;
    } > LR_IROM3

    /* Location counter can end up 2byte aligned with narrow Thumb code but
	 * __etext is assumed by startup code to be the LMA of a section in RAM
     * which must be 4byte aligned
     */
    __etext = ALIGN(4);

    /* The initialised data section is stored immediately
     * at the end of the text section.
     */
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
    } > LR_IROM3

    __code_area_end__ = .;

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > LR_IROM3

    ER_PRODTEST (NOLOAD):
    {
        . = ALIGN(4);
        *(prodtest_uninit)
    } > LR_IROM3

#if CODE_LOCATION_OTP
    ER_TRNG_ZI :
    {
        /* Must be aligned to 4 bytes due to __STARTUP_CLEAR_BSS_MULTIPLE zeroing code */
        . = ALIGN(4);
        /* The TRNG buffer area must be located lower than the 64K boundary. */
        /* This execution region starts at most 2K before the 64K boundary. */
        __trng_buffer_area_zi_start__ = .;
        *(trng_buffer_area_zi)
        . = ALIGN(4);
        __trng_buffer_area_zi_end__ = .;
    } > LR_IROM3

    ASSERT(SIZEOF(ER_TRNG_ZI) <= TRNG_BUFFER_AREA_SZ, "ER_TRNG_ZI to small!")
#endif

    ER_ZI (NOLOAD) :
    {
        /* Must be aligned to 4 bytes due to __STARTUP_CLEAR_BSS_MULTIPLE zeroing code */
        . = ALIGN(4);
        __heap_mem_area_not_ret_start__ = .;
        *jump_table.o(heap_mem_area_not_ret)    /* not retained HEAP */
        /* Must be aligned to 4 bytes due to __STARTUP_CLEAR_BSS_MULTIPLE zeroing code */
        . = ALIGN(4);
        __heap_mem_area_not_ret_end__ = .;
    } > LR_IROM3

    /* After the zero-init sections goes the non retained heap and stack */
    .heap (NOLOAD):
    {
        __end__ = .;
        PROVIDE(end = .);
        *(.heap*)
        __HeapLimit = .;
    } > LR_IROM3

    /* .stack_dummy section does not contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later.
     * COPY type does not allocate memory.
     */
    .stack_dummy (COPY):
    {
       KEEP(*(.stack*))
    } > LR_IROM3

    /* Set stack top to end of LR_IROM3 before the retained section,
     * and stack limit moves down by size of stack_dummy section.
     */
    __StackTop = ORIGIN(LR_IROM3) + LENGTH(LR_IROM3);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

    /* Exact address placement to force first section in LR_RETAINED_RAM0 alignment to single word boundary.
     * By default linker aligns it in two-word boundary even when LR_RETAINED_RAM0 starts at a word boundary.
     */
    RET_DATA_UNINIT RET_MEM_BASE_ADDR (NOLOAD) :
    {
        . = ALIGN(4);
        __ret_data_start__ = .;
        __retention_mem_area_uninit_start__ = .;

        *(retention_mem_area_uninit)            /* uninitialized application data */

        . = ALIGN(4);
        __retention_mem_area_uninit_end__ = .;
    } > LR_RETAINED_RAM0

    ASSERT(SIZEOF(RET_DATA_UNINIT) <= CFG_RET_DATA_UNINIT_SIZE, "RET_DATA_UNINIT to small!")

    RET_DATA (NOLOAD) :
    {
        . = ALIGN(4);
        *(retention_mem_area0)                  /* zero initialized SDK + application data */

        . = ALIGN(4);
        __ret_data_end__ = .;
    } > LR_RETAINED_RAM0

    ASSERT(SIZEOF(RET_DATA) <= CFG_RET_DATA_SIZE, "RET_DATA to small!")

    RET_HEAP (NOLOAD):
    {
        . = ALIGN(4);
        *jump_table.o(heap_env_area)
        *jump_table.o(heap_db_area)
        *jump_table.o(heap_msg_area)
    } > LR_RETAINED_RAM0

    ASSERT(SIZEOF(RET_HEAP) <= RET_HEAP_SIZE, "RET_HEAP to small!")
}

/* Include ROM symbol definitions */
#if defined(__DA14531__)
    INCLUDE da14531_symbols.lds
#else
    INCLUDE da14585_symbols.lds
#endif
