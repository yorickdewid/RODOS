#include "workspace.h"
#include "gpt/gpt.h"
#include "registers/regsgpt.h"

//! @brief Options for low power mode support for the timers.
//!
//! These constants are bit masks that are or'd together to select in which low
//! power modes the timer will continue counting.
enum _timer_low_power_modes
{
    WAIT_MODE_EN = 1,   //!< Timer is enabled in wait mode.
    STOP_MODE_EN = 2    //!< Timer is enabled in stop mode.
};

//! @brief Available clock sources for the timers.
enum _timer_clock_sources
{
    CLKSRC_OFF = 0,     //!< clock source is OFF
    CLKSRC_IPG_CLK = 1, //!< clock source is peripheral clock
    CLKSRC_PER_CLK = 2, //!< clock source is high-freq reference clock
    CLKSRC_CLKIN = 3,   //!< clock source is external from a CLKIN input
    CLKSRC_CKIL = 3     //!< clock source is low-freq reference clock
};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

static inline void gpt_clear_all_events(void)
{
    HW_GPT_SR_WR(kGPTAllEvents);
}

uint32_t gpt_get_rollover_event()
{
    // clear it if found set 
    if (HW_GPT_SR.B.ROV)
    {
        HW_GPT_SR_WR(BM_GPT_SR_ROV);
        return kGPTRollover;
    }

    // return the read value before the bit was cleared 
    return 0;
}

uint32_t gpt_get_capture_event(uint8_t flag, uint32_t * capture_val)
{
    // get the capture status bit 
    flag &= kGPTInputCapture1 | kGPTInputCapture2;
    uint32_t status_register = HW_GPT_SR_RD() & flag;
    
    // Read the captured timer value.
    if (capture_val)
    {
        if (status_register == kGPTInputCapture1)
        {
            *(uint32_t *) capture_val = HW_GPT_ICR1.B.CAPT;
        }
        else if (status_register == kGPTInputCapture2)
        {
            *(uint32_t *) capture_val = HW_GPT_ICR2.B.CAPT;
        }
    }
    
    // Clear the flag.
    HW_GPT_SR_WR(status_register);

    // return the read value before the bit was cleared 
    return status_register;
}

void gpt_set_capture_event(uint8_t cap_input, uint8_t cap_input_mode)
{
    // set the new input mode 
    switch (cap_input)
    {
        case kGPTInputCapture1:
            HW_GPT_CR.B.IM1 = cap_input_mode;
            break;
            
        case kGPTInputCapture2:
            HW_GPT_CR.B.IM2 = cap_input_mode;
            break;
    }
}

uint32_t gpt_get_compare_event(uint8_t flag)
{
    // get the active compare flags 
    flag &= kGPTOutputCompare1 | kGPTOutputCompare2 | kGPTOutputCompare3;
    uint32_t status_register = HW_GPT_SR_RD() & flag;
    
    // clear flags which are active 
    if (status_register)
    {
        HW_GPT_SR_WR(status_register);
    }

    // return the read value before the flags were cleared
    return status_register;
}

void gpt_set_compare_event(uint8_t cmp_output, uint8_t cmp_output_mode, uint32_t cmp_value)
{
    // set the value to compare with 
    switch (cmp_output)
    {
        case kGPTOutputCompare1:
            BW_GPT_CR_OM1(cmp_output_mode);
            HW_GPT_OCR1_WR(cmp_value);
            break;

        case kGPTOutputCompare2:
            BW_GPT_CR_OM2(cmp_output_mode);
            HW_GPT_OCR2_WR(cmp_value);
            break;

        case kGPTOutputCompare3:
            BW_GPT_CR_OM3(cmp_output_mode);
            HW_GPT_OCR3_WR(cmp_value);
            break;
    }
}

void gpt_counter_disable()
{
    // disable the counter 
    HW_GPT_CR.B.EN = 0;

    // ensure to leave the counter in a proper state by disabling the interrupt sources
    HW_GPT_IR_WR(0);
    
    // and by clearing possible remaining events 
    gpt_clear_all_events();
}

void gpt_counter_enable(uint32_t irq_mode)
{
    // ensure to start the counter in a proper state by clearing possible remaining events
    gpt_clear_all_events();

    // enable the interrupts or clear the register for polling 
    HW_GPT_IR_WR(irq_mode & kGPTAllEvents);

    // finally, enable the counter 
    HW_GPT_CR.B.EN = 1;
}

void gpt_setup_interrupt(gpt_isr_function isr, bool enableIt, int priority)
{
    if (enableIt)
    {    
        // register the IRQ sub-routine 
        register_interrupt_routine(IMX_INT_GPT, isr);
        
        // enable the IRQ 
        enable_interrupt(IMX_INT_GPT, CPU_0, priority);
    }
    else
    {
        // disable the IRQ 
        disable_interrupt(IMX_INT_GPT, CPU_0);
    }
}

void gpt_init(uint32_t clock_src, uint32_t prescaler,
              uint32_t counter_mode, uint32_t low_power_mode)
{
    uint32_t control_reg_tmp = 0;

    // enable the source clocks to the GPT port 
    clock_gating_config(REGS_GPT_BASE, CLOCK_ON);

    // start with a known state by disabling and reseting the module 
    HW_GPT_CR_WR(BM_GPT_CR_SWR);
    
    // wait for the reset to complete 
    while (HW_GPT_CR.B.SWR != 0) ;

    // set the reference source clock for the counter 
    if (clock_src == CLKSRC_CKIL)
    {
        // CKIL source is 0x4 for GPT but 0x3 for EPIT
        clock_src++;
    }
    control_reg_tmp |= BF_GPT_CR_CLKSRC(clock_src);

    // the prescaler can be changed at any time, and 
    // this affects the output clock immediately
    HW_GPT_PR_WR(BF_GPT_PR_PRESCALER(prescaler - 1));

    // set the counter mode 
    control_reg_tmp |= BF_GPT_CR_FRR(counter_mode);

    // set behavior for low power mode 
    if (low_power_mode & WAIT_MODE_EN)
    {
        control_reg_tmp |= BM_GPT_CR_WAITEN;
    }
    if (low_power_mode & STOP_MODE_EN)
    {
        control_reg_tmp |= BM_GPT_CR_STOPEN;
    }

    // specify from where the counter starts to count when enabled 
    // this code makes it start from 0 
    control_reg_tmp |= BM_GPT_CR_ENMOD;

    // finally write the control register 
    HW_GPT_CR_WR(control_reg_tmp);
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
