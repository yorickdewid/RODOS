/**HEADER********************************************************************
*
* Copyright (c) 2008-2009 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: cache_l2c310.c$
* $Version : 3.8.3.0$
* $Date    : Aug-24-2012$
*
* Comments:
*
*   This file contains functions for use with the l2c-310 cache controller.
*   This controller can work only with physical adresses.
*
*
*END************************************************************************/
#include "workspace.h"

/* Function mmu_virtual_to_physical is used for translation from virtual to physical address */
//uint32_t mmu_virtual_to_physical(void * va, void * * pa);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_setup
* Returned Value   : none
* Comments         :
*   This function sets up reg1_tag_ram_control & reg1_data_ram_control
*   and enables double line fill in the L2 cache controller
*
*END*----------------------------------------------------------------------*/
void _l2c310_cache_setup()
{
    CA5L2C_reg1_tag_ram_control = 0x00000132u;
    CA5L2C_reg1_data_ram_control = 0x00000132u;
    CA5L2C_reg15_prefetch_ctrl = 0x40800000u;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_enable
* Returned Value   : none
* Comments         :
*   This function enables data cache at any available cache level.
*   Works only if MMU is enabled!
*
*END*----------------------------------------------------------------------*/
void _l2c310_cache_enable()
{
    CA5L2C_reg1_control |= L2C_reg1_control_L2_Cache_enable_MASK;
    /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
    while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
}
    /*FUNCTION*-------------------------------------------------------------------
* Function Name    : _l2c310_cache_disable
* Returned Value   : none
* Comments         :
*   This function disables the data cache at any available cache level.
*
*END*----------------------------------------------------------------------*/
void _l2c310_cache_disable()
{
    CA5L2C_reg1_control &= ~L2C_reg1_control_L2_Cache_enable_MASK;
    /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
    while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_invalidate
* Returned Value   : none
* Comments         :
*   This function invalidates the entire data cache
*
*END*----------------------------------------------------------------------*/
void _l2c310_cache_invalidate()
{
    CA5L2C_reg7_inv_way |= L2C_reg7_inv_way_Way_MASK;
    while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_invalidate_line
* Comments         :
*    This function is called to invalidate a line of data cache.
*
*END*------------------------------------------------------------------------*/
void _l2c310_cache_invalidate_line(void * addr)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void * pa = NULL; /* physical address void * */
    if (mmu_virtual_to_physical((uint32_t)addr, (uint32_t *)&pa) == true)
    {
        CA5L2C_reg7_inv_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
        /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
        while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_invalidate_mlines
* Returned Value   : none
* Comments         :
*
*   This function is called to invalidate number of lines of data cache.
*   Number of lines depends on length parameter and size of line.
*   Size of line for A5 L2 cache is 32B.
*
*END*-----------------------------------------------------------------------*/
void _l2c310_cache_invalidate_mlines(void * addr, uint32_t length)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void * pa = NULL; /* physical address void * */
    void * end_addr = (void *)((uint32_t)addr + length);
    do
    {
        if (mmu_virtual_to_physical((uint32_t)addr, (uint32_t *)&pa) == true)
        {
            CA5L2C_reg7_inv_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
            /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
            while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
        }
        else
        {
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
        }
    } while (addr < end_addr);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_flush
* Returned Value   : none
* Comments         :
*   This function FLUSH (CLEAN) all lines of cachce (all sets in all ways)
*   Size of line for A5 L2 cache is 32B.
*
*END*----------------------------------------------------------------------*/
void _l2c310_cache_flush()
{
    //read count of ways and clean only them
    CA5L2C_reg7_clean_way |= L2C_reg7_clean_way_Way_MASK;
    /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
    while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_flush_line
* Comments         :
*   This function FLUSH (CLEAN)  line of cachce.
*   Size of line for A5 L2 cache is 32B.
*
*END*------------------------------------------------------------------------*/
void _l2c310_cache_flush_line(void * addr)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void * pa = NULL; /* physical address void * */
    if (mmu_virtual_to_physical((uint32_t)addr, (uint32_t *)&pa) == true)
    {
        CA5L2C_reg7_clean_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
        /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
        while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _l2c310_cache_flush_mlines
* Returned Value   : none
* Comments         :
*
*   This function is called to flush number of lines of data cache.
*   Number of lines depends on length parameter and size of line.
*   Size of line for A5 L2 cache is 32B.
*
*END*-----------------------------------------------------------------------*/
void _l2c310_cache_flush_mlines(void * addr, uint32_t length)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void * pa = NULL; /* physical address void * */
    void * end_addr = (void *)((uint32_t)addr + length);
    do
    {
        if (mmu_virtual_to_physical((uint32_t)addr, (uint32_t *)&pa) == true)
        {
            CA5L2C_reg7_clean_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
            /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
            while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
        }
        else
        {
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
        }
    } while (addr < end_addr);
}
