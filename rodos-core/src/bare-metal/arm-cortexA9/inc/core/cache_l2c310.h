/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if !defined(__CACHE_L2C310_H__)
#define __CACHE_L2C310_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* ----------------------------------------------------------------------------
   -- L2C
   ---------------------------------------------------------------------------- */

#define PSP_CACHE_LINE_SIZE 32


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

//! @name Data cache operations
//@{
//! @brief
void _l2c310_cache_disable();

//! @brief Invalidates the entire data cache.
void _l2c310_cache_invalidate(void);

//! @brief Setup for the L2 data cache.
void _l2c310_cache_setup(void);

//! @brief Enables L2 data cache
void _l2c310_cache_enable(void);

//@}

#if defined(__cplusplus)
}
#endif


/**
 * @addtogroup L2C_Peripheral L2C
 * @{
 */



/** L2C - Peripheral register structure */
typedef struct L2C_MemMap {
  uint32_t reg0_cache_id;                          /**< Performance Monitor Control Register, offset: 0x0 */
  uint32_t reg0_cache_type;                        /**< Cache Type Register, offset: 0x4 */
  uint8_t RESERVED_0[248];
  uint32_t reg1_control;                           /**< Control Register, offset: 0x100 */
  uint32_t reg1_aux_control;                       /**< Auxiliary Control Register, offset: 0x104 */
  uint32_t reg1_tag_ram_control;                   /**< Tag RAM Latency Control Register, offset: 0x108 */
  uint32_t reg1_data_ram_control;                  /**< Data RAM Latency Control Register, offset: 0x10C */
  uint8_t RESERVED_1[240];
  uint32_t reg2_ev_counter_ctrl;                   /**< Event Counter Control Register, offset: 0x200 */
  uint32_t reg2_ev_counter1_cfg;                   /**< Event Counter Configuration Register, offset: 0x204 */
  uint32_t reg2_ev_counter0_cfg;                   /**< Event Counter Configuration Register, offset: 0x208 */
  uint32_t reg2_ev_counter1;                       /**< Event counter value register 1, offset: 0x20C */
  uint32_t reg2_ev_counter0;                       /**< Event counter value register 0, offset: 0x210 */
  uint32_t reg2_int_mask;                          /**< Interrupt Mask Register, offset: 0x214 */
  uint32_t reg2_int_mask_status;                   /**< Masked Interrupt Status Register, offset: 0x218 */
  uint32_t reg2_int_raw_status;                    /**< Raw Interrupt Status Register, offset: 0x21C */
  uint32_t reg2_int_clear;                         /**< Interrupt Clear Register, offset: 0x220 */
  uint8_t RESERVED_2[1292];
  uint32_t reg7_cache_sync;                        /**< Cache Sync, offset: 0x730 */
  uint8_t RESERVED_3[60];
  uint32_t reg7_inv_pa;                            /**< Invalidate Line by PA, offset: 0x770 */
  uint8_t RESERVED_4[8];
  uint32_t reg7_inv_way;                           /**< Invalidate by Way, offset: 0x77C */
  uint8_t RESERVED_5[48];
  uint32_t reg7_clean_pa;                          /**< Clean Line by PA, offset: 0x7B0 */
  uint8_t RESERVED_6[4];
  uint32_t reg7_clean_index;                       /**< Clean Line by Set/Way, offset: 0x7B8 */
  uint32_t reg7_clean_way;                         /**< Clean by Way, offset: 0x7BC */
  uint8_t RESERVED_7[48];
  uint32_t reg7_clean_inv_pa;                      /**< Clean and Invalidate Line by PA, offset: 0x7F0 */
  uint8_t RESERVED_8[4];
  uint32_t reg7_clean_inv_index;                   /**< Clean and Invalidate Line by Set/Way, offset: 0x7F8 */
  uint32_t reg7_clean_inv_way;                     /**< Clean and Invalidate by Way, offset: 0x7FC */
  uint8_t RESERVED_9[256];
  uint32_t reg9_d_lockdown0;                       /**< Data Lockdown 0 Register, offset: 0x900 */
  uint32_t reg9_i_lockdown0;                       /**< Instruction Lockdown 0 Register, offset: 0x904 */
  uint32_t reg9_d_lockdown1;                       /**< Cache lockdown by way, offset: 0x908 */
  uint32_t reg9_i_lockdown1;                       /**< Instruction Lockdown 0 Register, offset: 0x90C */
  uint32_t reg9_d_lockdown2;                       /**< Data Lockdown 0 Register, offset: 0x910 */
  uint32_t reg9_i_lockdown2;                       /**< Instruction Lockdown 0 Register, offset: 0x914 */
  uint32_t reg9_d_lockdown3;                       /**< Data Lockdown 3 Register, offset: 0x918 */
  uint32_t reg9_i_lockdown3;                       /**< Instruction Lockdown 3 Register, offset: 0x91C */
  uint32_t reg9_d_lockdown4;                       /**< Data Lockdown 4 Register, offset: 0x920 */
  uint32_t reg9_i_lockdown4;                       /**< Instruction Lockdown 4 Register, offset: 0x924 */
  uint32_t reg9_d_lockdown5;                       /**< Data Lockdown 5 Register, offset: 0x928 */
  uint32_t reg9_i_lockdown5;                       /**< Instruction Lockdown 5 Register, offset: 0x92C */
  uint32_t reg9_d_lockdown6;                       /**< Data Lockdown 6 Register, offset: 0x930 */
  uint32_t reg9_i_lockdown6;                       /**< Instruction Lockdown 6 Register, offset: 0x934 */
  uint32_t reg9_d_lockdown7;                       /**< Data Lockdown 7 Register, offset: 0x938 */
  uint32_t reg9_i_lockdown7;                       /**< Instruction Lockdown 7 Register, offset: 0x93C */
  uint8_t RESERVED_10[16];
  uint32_t reg9_lock_line_en;                      /**< , offset: 0x950 */
  uint32_t reg9_unlock_way;                        /**< , offset: 0x954 */
  uint8_t RESERVED_11[680];
  uint32_t reg12_addr_filtering_start;             /**< Address Filtering Start Register, offset: 0xC00 */
  uint32_t reg12_addr_filtering_end;               /**< , offset: 0xC04 */
  uint8_t RESERVED_12[824];
  uint32_t reg15_debug_ctrl;                       /**< Debug Control Register, offset: 0xF40 */
  uint8_t RESERVED_13[28];
  uint32_t reg15_prefetch_ctrl;                    /**< Prefetch Control Register, offset: 0xF60 */
  uint8_t RESERVED_14[28];
  uint32_t reg15_power_ctrl;                       /**< Power Control Register, offset: 0xF80 */
} volatile *L2C_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- L2C - Register accessor macros
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup L2C_Register_Accessor_Macros L2C - Register accessor macros
 * @{
 */


/* L2C - Register accessors */
#define L2C_reg0_cache_id_REG(base)              ((base)->reg0_cache_id)
#define L2C_reg0_cache_type_REG(base)            ((base)->reg0_cache_type)
#define L2C_reg1_control_REG(base)               ((base)->reg1_control)
#define L2C_reg1_aux_control_REG(base)           ((base)->reg1_aux_control)
#define L2C_reg1_tag_ram_control_REG(base)       ((base)->reg1_tag_ram_control)
#define L2C_reg1_data_ram_control_REG(base)      ((base)->reg1_data_ram_control)
#define L2C_reg2_ev_counter_ctrl_REG(base)       ((base)->reg2_ev_counter_ctrl)
#define L2C_reg2_ev_counter1_cfg_REG(base)       ((base)->reg2_ev_counter1_cfg)
#define L2C_reg2_ev_counter0_cfg_REG(base)       ((base)->reg2_ev_counter0_cfg)
#define L2C_reg2_ev_counter1_REG(base)           ((base)->reg2_ev_counter1)
#define L2C_reg2_ev_counter0_REG(base)           ((base)->reg2_ev_counter0)
#define L2C_reg2_int_mask_REG(base)              ((base)->reg2_int_mask)
#define L2C_reg2_int_mask_status_REG(base)       ((base)->reg2_int_mask_status)
#define L2C_reg2_int_raw_status_REG(base)        ((base)->reg2_int_raw_status)
#define L2C_reg2_int_clear_REG(base)             ((base)->reg2_int_clear)
#define L2C_reg7_cache_sync_REG(base)            ((base)->reg7_cache_sync)
#define L2C_reg7_inv_pa_REG(base)                ((base)->reg7_inv_pa)
#define L2C_reg7_inv_way_REG(base)               ((base)->reg7_inv_way)
#define L2C_reg7_clean_pa_REG(base)              ((base)->reg7_clean_pa)
#define L2C_reg7_clean_index_REG(base)           ((base)->reg7_clean_index)
#define L2C_reg7_clean_way_REG(base)             ((base)->reg7_clean_way)
#define L2C_reg7_clean_inv_pa_REG(base)          ((base)->reg7_clean_inv_pa)
#define L2C_reg7_clean_inv_index_REG(base)       ((base)->reg7_clean_inv_index)
#define L2C_reg7_clean_inv_way_REG(base)         ((base)->reg7_clean_inv_way)
#define L2C_reg9_d_lockdown0_REG(base)           ((base)->reg9_d_lockdown0)
#define L2C_reg9_i_lockdown0_REG(base)           ((base)->reg9_i_lockdown0)
#define L2C_reg9_d_lockdown1_REG(base)           ((base)->reg9_d_lockdown1)
#define L2C_reg9_i_lockdown1_REG(base)           ((base)->reg9_i_lockdown1)
#define L2C_reg9_d_lockdown2_REG(base)           ((base)->reg9_d_lockdown2)
#define L2C_reg9_i_lockdown2_REG(base)           ((base)->reg9_i_lockdown2)
#define L2C_reg9_d_lockdown3_REG(base)           ((base)->reg9_d_lockdown3)
#define L2C_reg9_i_lockdown3_REG(base)           ((base)->reg9_i_lockdown3)
#define L2C_reg9_d_lockdown4_REG(base)           ((base)->reg9_d_lockdown4)
#define L2C_reg9_i_lockdown4_REG(base)           ((base)->reg9_i_lockdown4)
#define L2C_reg9_d_lockdown5_REG(base)           ((base)->reg9_d_lockdown5)
#define L2C_reg9_i_lockdown5_REG(base)           ((base)->reg9_i_lockdown5)
#define L2C_reg9_d_lockdown6_REG(base)           ((base)->reg9_d_lockdown6)
#define L2C_reg9_i_lockdown6_REG(base)           ((base)->reg9_i_lockdown6)
#define L2C_reg9_d_lockdown7_REG(base)           ((base)->reg9_d_lockdown7)
#define L2C_reg9_i_lockdown7_REG(base)           ((base)->reg9_i_lockdown7)
#define L2C_reg9_lock_line_en_REG(base)          ((base)->reg9_lock_line_en)
#define L2C_reg9_unlock_way_REG(base)            ((base)->reg9_unlock_way)
#define L2C_reg12_addr_filtering_start_REG(base) ((base)->reg12_addr_filtering_start)
#define L2C_reg12_addr_filtering_end_REG(base)   ((base)->reg12_addr_filtering_end)
#define L2C_reg15_debug_ctrl_REG(base)           ((base)->reg15_debug_ctrl)
#define L2C_reg15_prefetch_ctrl_REG(base)        ((base)->reg15_prefetch_ctrl)
#define L2C_reg15_power_ctrl_REG(base)           ((base)->reg15_power_ctrl)

/**
 * @}
 */ /* end of group L2C_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- L2C Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup L2C_Register_Masks L2C Register Masks
 * @{
 */

/* reg0_cache_id Bit Fields */
#define L2C_reg0_cache_id_RTL_release_MASK       0x3Fu
#define L2C_reg0_cache_id_RTL_release_SHIFT      0
#define L2C_reg0_cache_id_RTL_release(x)         (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_id_RTL_release_SHIFT))&L2C_reg0_cache_id_RTL_release_MASK)
#define L2C_reg0_cache_id_Part_number_MASK       0x3C0u
#define L2C_reg0_cache_id_Part_number_SHIFT      6
#define L2C_reg0_cache_id_Part_number(x)         (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_id_Part_number_SHIFT))&L2C_reg0_cache_id_Part_number_MASK)
#define L2C_reg0_cache_id_CACHE_ID_MASK          0xFC00u
#define L2C_reg0_cache_id_CACHE_ID_SHIFT         10
#define L2C_reg0_cache_id_CACHE_ID(x)            (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_id_CACHE_ID_SHIFT))&L2C_reg0_cache_id_CACHE_ID_MASK)
#define L2C_reg0_cache_id_Implementer_MASK       0xFF000000u
#define L2C_reg0_cache_id_Implementer_SHIFT      24
#define L2C_reg0_cache_id_Implementer(x)         (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_id_Implementer_SHIFT))&L2C_reg0_cache_id_Implementer_MASK)
/* reg0_cache_type Bit Fields */
#define L2C_reg0_cache_type_I_L2_cache_line_length_MASK 0x3u
#define L2C_reg0_cache_type_I_L2_cache_line_length_SHIFT 0
#define L2C_reg0_cache_type_I_L2_cache_line_length(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_type_I_L2_cache_line_length_SHIFT))&L2C_reg0_cache_type_I_L2_cache_line_length_MASK)
#define L2C_reg0_cache_type_I_L2_associativity_MASK 0x40u
#define L2C_reg0_cache_type_I_L2_associativity_SHIFT 6
#define L2C_reg0_cache_type_Isize_MASK           0xF80u
#define L2C_reg0_cache_type_Isize_SHIFT          7
#define L2C_reg0_cache_type_Isize(x)             (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_type_Isize_SHIFT))&L2C_reg0_cache_type_Isize_MASK)
#define L2C_reg0_cache_type_D_L2_cache_line_length_MASK 0x3000u
#define L2C_reg0_cache_type_D_L2_cache_line_length_SHIFT 12
#define L2C_reg0_cache_type_D_L2_cache_line_length(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_type_D_L2_cache_line_length_SHIFT))&L2C_reg0_cache_type_D_L2_cache_line_length_MASK)
#define L2C_reg0_cache_type_D_L2_associativity_MASK 0x40000u
#define L2C_reg0_cache_type_D_L2_associativity_SHIFT 18
#define L2C_reg0_cache_type_Dsize_MASK           0xF80000u
#define L2C_reg0_cache_type_Dsize_SHIFT          19
#define L2C_reg0_cache_type_Dsize(x)             (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_type_Dsize_SHIFT))&L2C_reg0_cache_type_Dsize_MASK)
#define L2C_reg0_cache_type_H_MASK               0x1000000u
#define L2C_reg0_cache_type_H_SHIFT              24
#define L2C_reg0_cache_type_ctype_MASK           0x1E000000u
#define L2C_reg0_cache_type_ctype_SHIFT          25
#define L2C_reg0_cache_type_ctype(x)             (((uint32_t)(((uint32_t)(x))<<L2C_reg0_cache_type_ctype_SHIFT))&L2C_reg0_cache_type_ctype_MASK)
#define L2C_reg0_cache_type_Data_banking_MASK    0x80000000u
#define L2C_reg0_cache_type_Data_banking_SHIFT   31
/* reg1_control Bit Fields */
#define L2C_reg1_control_L2_Cache_enable_MASK    0x1u
#define L2C_reg1_control_L2_Cache_enable_SHIFT   0
/* reg1_aux_control Bit Fields */
#define L2C_reg1_aux_control_Full_Line_of_Zero_Enable_MASK 0x1u
#define L2C_reg1_aux_control_Full_Line_of_Zero_Enable_SHIFT 0
#define L2C_reg1_aux_control_High_Priority_for_SO_and_Dev_Reads_Enable_MASK 0x400u
#define L2C_reg1_aux_control_High_Priority_for_SO_and_Dev_Reads_Enable_SHIFT 10
#define L2C_reg1_aux_control_Store_buffer_device_limitation_Enable_MASK 0x800u
#define L2C_reg1_aux_control_Store_buffer_device_limitation_Enable_SHIFT 11
#define L2C_reg1_aux_control_Exclusive_cache_configuration_MASK 0x1000u
#define L2C_reg1_aux_control_Exclusive_cache_configuration_SHIFT 12
#define L2C_reg1_aux_control_Shared_Attribute_Invalidate_Enable_MASK 0x2000u
#define L2C_reg1_aux_control_Shared_Attribute_Invalidate_Enable_SHIFT 13
#define L2C_reg1_aux_control_Associativity_MASK  0x10000u
#define L2C_reg1_aux_control_Associativity_SHIFT 16
#define L2C_reg1_aux_control_Way_size_MASK       0xE0000u
#define L2C_reg1_aux_control_Way_size_SHIFT      17
#define L2C_reg1_aux_control_Way_size(x)         (((uint32_t)(((uint32_t)(x))<<L2C_reg1_aux_control_Way_size_SHIFT))&L2C_reg1_aux_control_Way_size_MASK)
#define L2C_reg1_aux_control_Event_monitor_bus_enable_MASK 0x100000u
#define L2C_reg1_aux_control_Event_monitor_bus_enable_SHIFT 20
#define L2C_reg1_aux_control_Parity_enable_MASK  0x200000u
#define L2C_reg1_aux_control_Parity_enable_SHIFT 21
#define L2C_reg1_aux_control_Shared_attribute_override_enable_MASK 0x400000u
#define L2C_reg1_aux_control_Shared_attribute_override_enable_SHIFT 22
#define L2C_reg1_aux_control_Force_write_allocate_MASK 0x1800000u
#define L2C_reg1_aux_control_Force_write_allocate_SHIFT 23
#define L2C_reg1_aux_control_Force_write_allocate(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg1_aux_control_Force_write_allocate_SHIFT))&L2C_reg1_aux_control_Force_write_allocate_MASK)
#define L2C_reg1_aux_control_Cache_replacement_policy_MASK 0x2000000u
#define L2C_reg1_aux_control_Cache_replacement_policy_SHIFT 25
#define L2C_reg1_aux_control_Non_secure_lockdown_enable_MASK 0x4000000u
#define L2C_reg1_aux_control_Non_secure_lockdown_enable_SHIFT 26
#define L2C_reg1_aux_control_Non_secure_interrupt_access_control_MASK 0x8000000u
#define L2C_reg1_aux_control_Non_secure_interrupt_access_control_SHIFT 27
#define L2C_reg1_aux_control_Data_prefetch_enable_MASK 0x10000000u
#define L2C_reg1_aux_control_Data_prefetch_enable_SHIFT 28
#define L2C_reg1_aux_control_Instruction_prefetch_enable_MASK 0x20000000u
#define L2C_reg1_aux_control_Instruction_prefetch_enable_SHIFT 29
#define L2C_reg1_aux_control_Early_BRESP_enable_MASK 0x40000000u
#define L2C_reg1_aux_control_Early_BRESP_enable_SHIFT 30
/* reg1_tag_ram_control Bit Fields */
#define L2C_reg1_tag_ram_control_RAM_setup_latency_MASK 0x7u
#define L2C_reg1_tag_ram_control_RAM_setup_latency_SHIFT 0
#define L2C_reg1_tag_ram_control_RAM_setup_latency(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg1_tag_ram_control_RAM_setup_latency_SHIFT))&L2C_reg1_tag_ram_control_RAM_setup_latency_MASK)
#define L2C_reg1_tag_ram_control_RAM_read_access_latency_MASK 0x70u
#define L2C_reg1_tag_ram_control_RAM_read_access_latency_SHIFT 4
#define L2C_reg1_tag_ram_control_RAM_read_access_latency(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg1_tag_ram_control_RAM_read_access_latency_SHIFT))&L2C_reg1_tag_ram_control_RAM_read_access_latency_MASK)
#define L2C_reg1_tag_ram_control_RAM_write_access_latency_MASK 0x700u
#define L2C_reg1_tag_ram_control_RAM_write_access_latency_SHIFT 8
#define L2C_reg1_tag_ram_control_RAM_write_access_latency(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg1_tag_ram_control_RAM_write_access_latency_SHIFT))&L2C_reg1_tag_ram_control_RAM_write_access_latency_MASK)
/* reg1_data_ram_control Bit Fields */
#define L2C_reg1_data_ram_control_RAM_setup_latency_MASK 0x7u
#define L2C_reg1_data_ram_control_RAM_setup_latency_SHIFT 0
#define L2C_reg1_data_ram_control_RAM_setup_latency(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg1_data_ram_control_RAM_setup_latency_SHIFT))&L2C_reg1_data_ram_control_RAM_setup_latency_MASK)
#define L2C_reg1_data_ram_control_RAM_read_access_latency_MASK 0x70u
#define L2C_reg1_data_ram_control_RAM_read_access_latency_SHIFT 4
#define L2C_reg1_data_ram_control_RAM_read_access_latency(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg1_data_ram_control_RAM_read_access_latency_SHIFT))&L2C_reg1_data_ram_control_RAM_read_access_latency_MASK)
#define L2C_reg1_data_ram_control_RAM_write_access_latency_MASK 0x700u
#define L2C_reg1_data_ram_control_RAM_write_access_latency_SHIFT 8
#define L2C_reg1_data_ram_control_RAM_write_access_latency(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg1_data_ram_control_RAM_write_access_latency_SHIFT))&L2C_reg1_data_ram_control_RAM_write_access_latency_MASK)
/* reg2_ev_counter_ctrl Bit Fields */
#define L2C_reg2_ev_counter_ctrl_Event_counter_enable_MASK 0x1u
#define L2C_reg2_ev_counter_ctrl_Event_counter_enable_SHIFT 0
#define L2C_reg2_ev_counter_ctrl_Counter_reset_MASK 0x6u
#define L2C_reg2_ev_counter_ctrl_Counter_reset_SHIFT 1
#define L2C_reg2_ev_counter_ctrl_Counter_reset(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg2_ev_counter_ctrl_Counter_reset_SHIFT))&L2C_reg2_ev_counter_ctrl_Counter_reset_MASK)
/* reg2_ev_counter1_cfg Bit Fields */
#define L2C_reg2_ev_counter1_cfg_Event_counter_interrupt_generation_MASK 0x3u
#define L2C_reg2_ev_counter1_cfg_Event_counter_interrupt_generation_SHIFT 0
#define L2C_reg2_ev_counter1_cfg_Event_counter_interrupt_generation(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg2_ev_counter1_cfg_Event_counter_interrupt_generation_SHIFT))&L2C_reg2_ev_counter1_cfg_Event_counter_interrupt_generation_MASK)
#define L2C_reg2_ev_counter1_cfg_Counter_event_source_MASK 0x3Cu
#define L2C_reg2_ev_counter1_cfg_Counter_event_source_SHIFT 2
#define L2C_reg2_ev_counter1_cfg_Counter_event_source(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg2_ev_counter1_cfg_Counter_event_source_SHIFT))&L2C_reg2_ev_counter1_cfg_Counter_event_source_MASK)
/* reg2_ev_counter0_cfg Bit Fields */
#define L2C_reg2_ev_counter0_cfg_Event_counter_interrupt_generation_MASK 0x3u
#define L2C_reg2_ev_counter0_cfg_Event_counter_interrupt_generation_SHIFT 0
#define L2C_reg2_ev_counter0_cfg_Event_counter_interrupt_generation(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg2_ev_counter0_cfg_Event_counter_interrupt_generation_SHIFT))&L2C_reg2_ev_counter0_cfg_Event_counter_interrupt_generation_MASK)
#define L2C_reg2_ev_counter0_cfg_Counter_event_source_MASK 0x3Cu
#define L2C_reg2_ev_counter0_cfg_Counter_event_source_SHIFT 2
#define L2C_reg2_ev_counter0_cfg_Counter_event_source(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg2_ev_counter0_cfg_Counter_event_source_SHIFT))&L2C_reg2_ev_counter0_cfg_Counter_event_source_MASK)
/* reg2_ev_counter1 Bit Fields */
#define L2C_reg2_ev_counter1_Counter_value_MASK  0xFFFFFFFFu
#define L2C_reg2_ev_counter1_Counter_value_SHIFT 0
#define L2C_reg2_ev_counter1_Counter_value(x)    (((uint32_t)(((uint32_t)(x))<<L2C_reg2_ev_counter1_Counter_value_SHIFT))&L2C_reg2_ev_counter1_Counter_value_MASK)
/* reg2_ev_counter0 Bit Fields */
#define L2C_reg2_ev_counter0_Counter_value_MASK  0xFFFFFFFFu
#define L2C_reg2_ev_counter0_Counter_value_SHIFT 0
#define L2C_reg2_ev_counter0_Counter_value(x)    (((uint32_t)(((uint32_t)(x))<<L2C_reg2_ev_counter0_Counter_value_SHIFT))&L2C_reg2_ev_counter0_Counter_value_MASK)
/* reg2_int_mask Bit Fields */
#define L2C_reg2_int_mask_ECNTR_MASK             0x1u
#define L2C_reg2_int_mask_ECNTR_SHIFT            0
#define L2C_reg2_int_mask_PARRT_MASK             0x2u
#define L2C_reg2_int_mask_PARRT_SHIFT            1
#define L2C_reg2_int_mask_PARRD_MASK             0x4u
#define L2C_reg2_int_mask_PARRD_SHIFT            2
#define L2C_reg2_int_mask_ERRWT_MASK             0x8u
#define L2C_reg2_int_mask_ERRWT_SHIFT            3
#define L2C_reg2_int_mask_ERRWD_MASK             0x10u
#define L2C_reg2_int_mask_ERRWD_SHIFT            4
#define L2C_reg2_int_mask_ERRRT_MASK             0x20u
#define L2C_reg2_int_mask_ERRRT_SHIFT            5
#define L2C_reg2_int_mask_ERRRD_MASK             0x40u
#define L2C_reg2_int_mask_ERRRD_SHIFT            6
#define L2C_reg2_int_mask_SLVERR_MASK            0x80u
#define L2C_reg2_int_mask_SLVERR_SHIFT           7
#define L2C_reg2_int_mask_DECERR_MASK            0x100u
#define L2C_reg2_int_mask_DECERR_SHIFT           8
/* reg2_int_mask_status Bit Fields */
#define L2C_reg2_int_mask_status_ECNTR_MASK      0x1u
#define L2C_reg2_int_mask_status_ECNTR_SHIFT     0
#define L2C_reg2_int_mask_status_PARRT_MASK      0x2u
#define L2C_reg2_int_mask_status_PARRT_SHIFT     1
#define L2C_reg2_int_mask_status_PARRD_MASK      0x4u
#define L2C_reg2_int_mask_status_PARRD_SHIFT     2
#define L2C_reg2_int_mask_status_ERRWT_MASK      0x8u
#define L2C_reg2_int_mask_status_ERRWT_SHIFT     3
#define L2C_reg2_int_mask_status_ERRWD_MASK      0x10u
#define L2C_reg2_int_mask_status_ERRWD_SHIFT     4
#define L2C_reg2_int_mask_status_ERRRT_MASK      0x20u
#define L2C_reg2_int_mask_status_ERRRT_SHIFT     5
#define L2C_reg2_int_mask_status_ERRRD_MASK      0x40u
#define L2C_reg2_int_mask_status_ERRRD_SHIFT     6
#define L2C_reg2_int_mask_status_SLVERR_MASK     0x80u
#define L2C_reg2_int_mask_status_SLVERR_SHIFT    7
#define L2C_reg2_int_mask_status_DECERR_MASK     0x100u
#define L2C_reg2_int_mask_status_DECERR_SHIFT    8
/* reg2_int_raw_status Bit Fields */
#define L2C_reg2_int_raw_status_ECNTR_MASK       0x1u
#define L2C_reg2_int_raw_status_ECNTR_SHIFT      0
#define L2C_reg2_int_raw_status_PARRT_MASK       0x2u
#define L2C_reg2_int_raw_status_PARRT_SHIFT      1
#define L2C_reg2_int_raw_status_PARRD_MASK       0x4u
#define L2C_reg2_int_raw_status_PARRD_SHIFT      2
#define L2C_reg2_int_raw_status_ERRWT_MASK       0x8u
#define L2C_reg2_int_raw_status_ERRWT_SHIFT      3
#define L2C_reg2_int_raw_status_ERRWD_MASK       0x10u
#define L2C_reg2_int_raw_status_ERRWD_SHIFT      4
#define L2C_reg2_int_raw_status_ERRRT_MASK       0x20u
#define L2C_reg2_int_raw_status_ERRRT_SHIFT      5
#define L2C_reg2_int_raw_status_ERRRD_MASK       0x40u
#define L2C_reg2_int_raw_status_ERRRD_SHIFT      6
#define L2C_reg2_int_raw_status_SLVERR_MASK      0x80u
#define L2C_reg2_int_raw_status_SLVERR_SHIFT     7
#define L2C_reg2_int_raw_status_DECERR_MASK      0x100u
#define L2C_reg2_int_raw_status_DECERR_SHIFT     8
/* reg2_int_clear Bit Fields */
#define L2C_reg2_int_clear_ECNTR_MASK            0x1u
#define L2C_reg2_int_clear_ECNTR_SHIFT           0
#define L2C_reg2_int_clear_PARRT_MASK            0x2u
#define L2C_reg2_int_clear_PARRT_SHIFT           1
#define L2C_reg2_int_clear_PARRD_MASK            0x4u
#define L2C_reg2_int_clear_PARRD_SHIFT           2
#define L2C_reg2_int_clear_ERRWT_MASK            0x8u
#define L2C_reg2_int_clear_ERRWT_SHIFT           3
#define L2C_reg2_int_clear_ERRWD_MASK            0x10u
#define L2C_reg2_int_clear_ERRWD_SHIFT           4
#define L2C_reg2_int_clear_ERRRT_MASK            0x20u
#define L2C_reg2_int_clear_ERRRT_SHIFT           5
#define L2C_reg2_int_clear_ERRRD_MASK            0x40u
#define L2C_reg2_int_clear_ERRRD_SHIFT           6
#define L2C_reg2_int_clear_SLVERR_MASK           0x80u
#define L2C_reg2_int_clear_SLVERR_SHIFT          7
#define L2C_reg2_int_clear_DECERR_MASK           0x100u
#define L2C_reg2_int_clear_DECERR_SHIFT          8
/* reg7_cache_sync Bit Fields */
#define L2C_reg7_cache_sync_C_MASK               0x1u
#define L2C_reg7_cache_sync_C_SHIFT              0
/* reg7_inv_pa Bit Fields */
#define L2C_reg7_inv_pa_C_MASK                   0x1u
#define L2C_reg7_inv_pa_C_SHIFT                  0
#define L2C_reg7_inv_pa_Index_MASK               0xFFE0u
#define L2C_reg7_inv_pa_Index_SHIFT              5
#define L2C_reg7_inv_pa_Index(x)                 (((uint32_t)(((uint32_t)(x))<<L2C_reg7_inv_pa_Index_SHIFT))&L2C_reg7_inv_pa_Index_MASK)
#define L2C_reg7_inv_pa_Tag_MASK                 0xFFFF0000u
#define L2C_reg7_inv_pa_Tag_SHIFT                16
#define L2C_reg7_inv_pa_Tag(x)                   (((uint32_t)(((uint32_t)(x))<<L2C_reg7_inv_pa_Tag_SHIFT))&L2C_reg7_inv_pa_Tag_MASK)
/* reg7_inv_way Bit Fields */
#define L2C_reg7_inv_way_Way_MASK                0xFFFFu
#define L2C_reg7_inv_way_Way_SHIFT               0
#define L2C_reg7_inv_way_Way(x)                  (((uint32_t)(((uint32_t)(x))<<L2C_reg7_inv_way_Way_SHIFT))&L2C_reg7_inv_way_Way_MASK)
/* reg7_clean_pa Bit Fields */
#define L2C_reg7_clean_pa_C_MASK                 0x1u
#define L2C_reg7_clean_pa_C_SHIFT                0
#define L2C_reg7_clean_pa_Index_MASK             0xFFE0u
#define L2C_reg7_clean_pa_Index_SHIFT            5
#define L2C_reg7_clean_pa_Index(x)               (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_pa_Index_SHIFT))&L2C_reg7_clean_pa_Index_MASK)
#define L2C_reg7_clean_pa_Tag_MASK               0xFFFF0000u
#define L2C_reg7_clean_pa_Tag_SHIFT              16
#define L2C_reg7_clean_pa_Tag(x)                 (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_pa_Tag_SHIFT))&L2C_reg7_clean_pa_Tag_MASK)
/* reg7_clean_index Bit Fields */
#define L2C_reg7_clean_index_C_MASK              0x1u
#define L2C_reg7_clean_index_C_SHIFT             0
#define L2C_reg7_clean_index_Index_MASK          0xFFE0u
#define L2C_reg7_clean_index_Index_SHIFT         5
#define L2C_reg7_clean_index_Index(x)            (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_index_Index_SHIFT))&L2C_reg7_clean_index_Index_MASK)
#define L2C_reg7_clean_index_Way_MASK            0xFFFF0000u
#define L2C_reg7_clean_index_Way_SHIFT           16
#define L2C_reg7_clean_index_Way(x)              (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_index_Way_SHIFT))&L2C_reg7_clean_index_Way_MASK)
/* reg7_clean_way Bit Fields */
#define L2C_reg7_clean_way_Way_MASK              0xFFFFu
#define L2C_reg7_clean_way_Way_SHIFT             0
#define L2C_reg7_clean_way_Way(x)                (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_way_Way_SHIFT))&L2C_reg7_clean_way_Way_MASK)
/* reg7_clean_inv_pa Bit Fields */
#define L2C_reg7_clean_inv_pa_C_MASK             0x1u
#define L2C_reg7_clean_inv_pa_C_SHIFT            0
#define L2C_reg7_clean_inv_pa_Index_MASK         0xFFE0u
#define L2C_reg7_clean_inv_pa_Index_SHIFT        5
#define L2C_reg7_clean_inv_pa_Index(x)           (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_inv_pa_Index_SHIFT))&L2C_reg7_clean_inv_pa_Index_MASK)
#define L2C_reg7_clean_inv_pa_Tag_MASK           0xFFFF0000u
#define L2C_reg7_clean_inv_pa_Tag_SHIFT          16
#define L2C_reg7_clean_inv_pa_Tag(x)             (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_inv_pa_Tag_SHIFT))&L2C_reg7_clean_inv_pa_Tag_MASK)
/* reg7_clean_inv_index Bit Fields */
#define L2C_reg7_clean_inv_index_C_MASK          0x1u
#define L2C_reg7_clean_inv_index_C_SHIFT         0
#define L2C_reg7_clean_inv_index_Index_MASK      0xFFE0u
#define L2C_reg7_clean_inv_index_Index_SHIFT     5
#define L2C_reg7_clean_inv_index_Index(x)        (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_inv_index_Index_SHIFT))&L2C_reg7_clean_inv_index_Index_MASK)
#define L2C_reg7_clean_inv_index_Way_MASK        0xF0000000u
#define L2C_reg7_clean_inv_index_Way_SHIFT       28
#define L2C_reg7_clean_inv_index_Way(x)          (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_inv_index_Way_SHIFT))&L2C_reg7_clean_inv_index_Way_MASK)
/* reg7_clean_inv_way Bit Fields */
#define L2C_reg7_clean_inv_way_Way_MASK          0xFFFFu
#define L2C_reg7_clean_inv_way_Way_SHIFT         0
#define L2C_reg7_clean_inv_way_Way(x)            (((uint32_t)(((uint32_t)(x))<<L2C_reg7_clean_inv_way_Way_SHIFT))&L2C_reg7_clean_inv_way_Way_MASK)
/* reg9_d_lockdown0 Bit Fields */
#define L2C_reg9_d_lockdown0_DATALOCK000_MASK    0xFFFFu
#define L2C_reg9_d_lockdown0_DATALOCK000_SHIFT   0
#define L2C_reg9_d_lockdown0_DATALOCK000(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown0_DATALOCK000_SHIFT))&L2C_reg9_d_lockdown0_DATALOCK000_MASK)
/* reg9_i_lockdown0 Bit Fields */
#define L2C_reg9_i_lockdown0_INSTRLOCK000_MASK   0xFFFFu
#define L2C_reg9_i_lockdown0_INSTRLOCK000_SHIFT  0
#define L2C_reg9_i_lockdown0_INSTRLOCK000(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown0_INSTRLOCK000_SHIFT))&L2C_reg9_i_lockdown0_INSTRLOCK000_MASK)
/* reg9_d_lockdown1 Bit Fields */
#define L2C_reg9_d_lockdown1_DATALOCK001_MASK    0xFFFFu
#define L2C_reg9_d_lockdown1_DATALOCK001_SHIFT   0
#define L2C_reg9_d_lockdown1_DATALOCK001(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown1_DATALOCK001_SHIFT))&L2C_reg9_d_lockdown1_DATALOCK001_MASK)
/* reg9_i_lockdown1 Bit Fields */
#define L2C_reg9_i_lockdown1_INSTRLOCK001_MASK   0xFFFFu
#define L2C_reg9_i_lockdown1_INSTRLOCK001_SHIFT  0
#define L2C_reg9_i_lockdown1_INSTRLOCK001(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown1_INSTRLOCK001_SHIFT))&L2C_reg9_i_lockdown1_INSTRLOCK001_MASK)
/* reg9_d_lockdown2 Bit Fields */
#define L2C_reg9_d_lockdown2_DATALOCK010_MASK    0xFFFFu
#define L2C_reg9_d_lockdown2_DATALOCK010_SHIFT   0
#define L2C_reg9_d_lockdown2_DATALOCK010(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown2_DATALOCK010_SHIFT))&L2C_reg9_d_lockdown2_DATALOCK010_MASK)
/* reg9_i_lockdown2 Bit Fields */
#define L2C_reg9_i_lockdown2_INSTRLOCK010_MASK   0xFFFFu
#define L2C_reg9_i_lockdown2_INSTRLOCK010_SHIFT  0
#define L2C_reg9_i_lockdown2_INSTRLOCK010(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown2_INSTRLOCK010_SHIFT))&L2C_reg9_i_lockdown2_INSTRLOCK010_MASK)
/* reg9_d_lockdown3 Bit Fields */
#define L2C_reg9_d_lockdown3_DATALOCK011_MASK    0xFFFFu
#define L2C_reg9_d_lockdown3_DATALOCK011_SHIFT   0
#define L2C_reg9_d_lockdown3_DATALOCK011(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown3_DATALOCK011_SHIFT))&L2C_reg9_d_lockdown3_DATALOCK011_MASK)
/* reg9_i_lockdown3 Bit Fields */
#define L2C_reg9_i_lockdown3_INSTRLOCK011_MASK   0xFFFFu
#define L2C_reg9_i_lockdown3_INSTRLOCK011_SHIFT  0
#define L2C_reg9_i_lockdown3_INSTRLOCK011(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown3_INSTRLOCK011_SHIFT))&L2C_reg9_i_lockdown3_INSTRLOCK011_MASK)
/* reg9_d_lockdown4 Bit Fields */
#define L2C_reg9_d_lockdown4_DATALOCK100_MASK    0xFFFFu
#define L2C_reg9_d_lockdown4_DATALOCK100_SHIFT   0
#define L2C_reg9_d_lockdown4_DATALOCK100(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown4_DATALOCK100_SHIFT))&L2C_reg9_d_lockdown4_DATALOCK100_MASK)
/* reg9_i_lockdown4 Bit Fields */
#define L2C_reg9_i_lockdown4_INSTRLOCK100_MASK   0xFFFFu
#define L2C_reg9_i_lockdown4_INSTRLOCK100_SHIFT  0
#define L2C_reg9_i_lockdown4_INSTRLOCK100(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown4_INSTRLOCK100_SHIFT))&L2C_reg9_i_lockdown4_INSTRLOCK100_MASK)
/* reg9_d_lockdown5 Bit Fields */
#define L2C_reg9_d_lockdown5_DATALOCK101_MASK    0xFFFFu
#define L2C_reg9_d_lockdown5_DATALOCK101_SHIFT   0
#define L2C_reg9_d_lockdown5_DATALOCK101(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown5_DATALOCK101_SHIFT))&L2C_reg9_d_lockdown5_DATALOCK101_MASK)
/* reg9_i_lockdown5 Bit Fields */
#define L2C_reg9_i_lockdown5_INSTRLOCK101_MASK   0xFFFFu
#define L2C_reg9_i_lockdown5_INSTRLOCK101_SHIFT  0
#define L2C_reg9_i_lockdown5_INSTRLOCK101(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown5_INSTRLOCK101_SHIFT))&L2C_reg9_i_lockdown5_INSTRLOCK101_MASK)
/* reg9_d_lockdown6 Bit Fields */
#define L2C_reg9_d_lockdown6_DATALOCK110_MASK    0xFFFFu
#define L2C_reg9_d_lockdown6_DATALOCK110_SHIFT   0
#define L2C_reg9_d_lockdown6_DATALOCK110(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown6_DATALOCK110_SHIFT))&L2C_reg9_d_lockdown6_DATALOCK110_MASK)
/* reg9_i_lockdown6 Bit Fields */
#define L2C_reg9_i_lockdown6_INSTRLOCK110_MASK   0xFFFFu
#define L2C_reg9_i_lockdown6_INSTRLOCK110_SHIFT  0
#define L2C_reg9_i_lockdown6_INSTRLOCK110(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown6_INSTRLOCK110_SHIFT))&L2C_reg9_i_lockdown6_INSTRLOCK110_MASK)
/* reg9_d_lockdown7 Bit Fields */
#define L2C_reg9_d_lockdown7_DATALOCK111_MASK    0xFFFFu
#define L2C_reg9_d_lockdown7_DATALOCK111_SHIFT   0
#define L2C_reg9_d_lockdown7_DATALOCK111(x)      (((uint32_t)(((uint32_t)(x))<<L2C_reg9_d_lockdown7_DATALOCK111_SHIFT))&L2C_reg9_d_lockdown7_DATALOCK111_MASK)
/* reg9_i_lockdown7 Bit Fields */
#define L2C_reg9_i_lockdown7_INSTRLOCK111_MASK   0xFFFFu
#define L2C_reg9_i_lockdown7_INSTRLOCK111_SHIFT  0
#define L2C_reg9_i_lockdown7_INSTRLOCK111(x)     (((uint32_t)(((uint32_t)(x))<<L2C_reg9_i_lockdown7_INSTRLOCK111_SHIFT))&L2C_reg9_i_lockdown7_INSTRLOCK111_MASK)
/* reg12_addr_filtering_start Bit Fields */
#define L2C_reg12_addr_filtering_start_address_filtering_enable_MASK 0x1u
#define L2C_reg12_addr_filtering_start_address_filtering_enable_SHIFT 0
#define L2C_reg12_addr_filtering_start_address_filtering_start_MASK 0xFFF00000u
#define L2C_reg12_addr_filtering_start_address_filtering_start_SHIFT 20
#define L2C_reg12_addr_filtering_start_address_filtering_start(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg12_addr_filtering_start_address_filtering_start_SHIFT))&L2C_reg12_addr_filtering_start_address_filtering_start_MASK)
/* reg12_addr_filtering_end Bit Fields */
#define L2C_reg12_addr_filtering_end_address_filtering_end_MASK 0x100000u
#define L2C_reg12_addr_filtering_end_address_filtering_end_SHIFT 20
/* reg15_debug_ctrl Bit Fields */
#define L2C_reg15_debug_ctrl_DCL_MASK            0x1u
#define L2C_reg15_debug_ctrl_DCL_SHIFT           0
#define L2C_reg15_debug_ctrl_DWB_MASK            0x2u
#define L2C_reg15_debug_ctrl_DWB_SHIFT           1
#define L2C_reg15_debug_ctrl_SPNIDEN_MASK        0x4u
#define L2C_reg15_debug_ctrl_SPNIDEN_SHIFT       2
/* reg15_prefetch_ctrl Bit Fields */
#define L2C_reg15_prefetch_ctrl_Prefetch_offset_MASK 0x1Fu
#define L2C_reg15_prefetch_ctrl_Prefetch_offset_SHIFT 0
#define L2C_reg15_prefetch_ctrl_Prefetch_offset(x) (((uint32_t)(((uint32_t)(x))<<L2C_reg15_prefetch_ctrl_Prefetch_offset_SHIFT))&L2C_reg15_prefetch_ctrl_Prefetch_offset_MASK)
#define L2C_reg15_prefetch_ctrl_Not_same_ID_on_exclusive_sequence_enable_MASK 0x200000u
#define L2C_reg15_prefetch_ctrl_Not_same_ID_on_exclusive_sequence_enable_SHIFT 21
#define L2C_reg15_prefetch_ctrl_Incr_double_Linefill_enable_MASK 0x800000u
#define L2C_reg15_prefetch_ctrl_Incr_double_Linefill_enable_SHIFT 23
#define L2C_reg15_prefetch_ctrl_Prefetch_drop_enable_MASK 0x1000000u
#define L2C_reg15_prefetch_ctrl_Prefetch_drop_enable_SHIFT 24
#define L2C_reg15_prefetch_ctrl_Double_linefill_on_WRAP_read_disable_MASK 0x8000000u
#define L2C_reg15_prefetch_ctrl_Double_linefill_on_WRAP_read_disable_SHIFT 27
#define L2C_reg15_prefetch_ctrl_Data_prefetch_enable_MASK 0x10000000u
#define L2C_reg15_prefetch_ctrl_Data_prefetch_enable_SHIFT 28
#define L2C_reg15_prefetch_ctrl_Instruction_prefetch_enable_MASK 0x20000000u
#define L2C_reg15_prefetch_ctrl_Instruction_prefetch_enable_SHIFT 29
#define L2C_reg15_prefetch_ctrl_Double_linefill_enable_MASK 0x40000000u
#define L2C_reg15_prefetch_ctrl_Double_linefill_enable_SHIFT 30
/* reg15_power_ctrl Bit Fields */
#define L2C_reg15_power_ctrl_standby_mode_en_MASK 0x1u
#define L2C_reg15_power_ctrl_standby_mode_en_SHIFT 0
#define L2C_reg15_power_ctrl_dynamic_clk_gating_en_MASK 0x2u
#define L2C_reg15_power_ctrl_dynamic_clk_gating_en_SHIFT 1

/**
 * @}
 */ /* end of group L2C_Register_Masks */


/* L2C - Peripheral instance base addresses */
/** Peripheral CA5L2C base pointer */
#define CA5L2C_BASE_PTR                          ((L2C_MemMapPtr)0x00A02000u)
/** Array initializer of L2C peripheral base pointers */
#define L2C_BASE_PTRS                            { CA5L2C_BASE_PTR }

/* ----------------------------------------------------------------------------
   -- L2C - Register accessor macros
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup L2C_Register_Accessor_Macros L2C - Register accessor macros
 * @{
 */


/* L2C - Register instance definitions */
/* CA5L2C */
#define CA5L2C_reg0_cache_id                     L2C_reg0_cache_id_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg0_cache_type                   L2C_reg0_cache_type_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg1_control                      L2C_reg1_control_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg1_aux_control                  L2C_reg1_aux_control_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg1_tag_ram_control              L2C_reg1_tag_ram_control_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg1_data_ram_control             L2C_reg1_data_ram_control_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_ev_counter_ctrl              L2C_reg2_ev_counter_ctrl_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_ev_counter1_cfg              L2C_reg2_ev_counter1_cfg_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_ev_counter0_cfg              L2C_reg2_ev_counter0_cfg_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_ev_counter1                  L2C_reg2_ev_counter1_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_ev_counter0                  L2C_reg2_ev_counter0_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_int_mask                     L2C_reg2_int_mask_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_int_mask_status              L2C_reg2_int_mask_status_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_int_raw_status               L2C_reg2_int_raw_status_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg2_int_clear                    L2C_reg2_int_clear_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_cache_sync                   L2C_reg7_cache_sync_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_inv_pa                       L2C_reg7_inv_pa_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_inv_way                      L2C_reg7_inv_way_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_clean_pa                     L2C_reg7_clean_pa_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_clean_index                  L2C_reg7_clean_index_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_clean_way                    L2C_reg7_clean_way_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_clean_inv_pa                 L2C_reg7_clean_inv_pa_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_clean_inv_index              L2C_reg7_clean_inv_index_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg7_clean_inv_way                L2C_reg7_clean_inv_way_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown0                  L2C_reg9_d_lockdown0_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown0                  L2C_reg9_i_lockdown0_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown1                  L2C_reg9_d_lockdown1_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown1                  L2C_reg9_i_lockdown1_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown2                  L2C_reg9_d_lockdown2_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown2                  L2C_reg9_i_lockdown2_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown3                  L2C_reg9_d_lockdown3_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown3                  L2C_reg9_i_lockdown3_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown4                  L2C_reg9_d_lockdown4_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown4                  L2C_reg9_i_lockdown4_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown5                  L2C_reg9_d_lockdown5_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown5                  L2C_reg9_i_lockdown5_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown6                  L2C_reg9_d_lockdown6_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown6                  L2C_reg9_i_lockdown6_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_d_lockdown7                  L2C_reg9_d_lockdown7_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_i_lockdown7                  L2C_reg9_i_lockdown7_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_lock_line_en                 L2C_reg9_lock_line_en_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg9_unlock_way                   L2C_reg9_unlock_way_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg12_addr_filtering_start        L2C_reg12_addr_filtering_start_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg12_addr_filtering_end          L2C_reg12_addr_filtering_end_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg15_debug_ctrl                  L2C_reg15_debug_ctrl_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg15_prefetch_ctrl               L2C_reg15_prefetch_ctrl_REG(CA5L2C_BASE_PTR)
#define CA5L2C_reg15_power_ctrl                  L2C_reg15_power_ctrl_REG(CA5L2C_BASE_PTR)

/**
 * @}
 */ /* end of group L2C_Register_Accessor_Macros */


/**
 * @}
 */ /* end of group L2C_Peripheral */


#endif // __CACHE_L2C310_H__
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
