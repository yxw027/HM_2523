/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_nvic.h"

#ifdef HAL_NVIC_MODULE_ENABLED
#include "hal_nvic_internal.h"
//#include "hal_flash_disk_internal.h"
#include "memory_attribute.h"
#include "hal_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*nvic_callback)(hal_nvic_irq_t irq_number);
    uint32_t irq_pending;
} nvic_function_t;

static const uint32_t defualt_irq_priority[IRQ_NUMBER_MAX] = {
	DMA_IRQ_PRIORITY,
    SPI_MST_IRQ_PRIORITY,
    SPI_SLV_IRQ_PRIORITY,
    SDIO_SLV_IRQ_PRIORITY,
    SDIO_MST_IRQ_PRIORITY,
    TRNG_IRQ_PRIORITY,
    CRYPTO_IRQ_PRIORITY,
    UART0_IRQ_PRIORITY,
    UART1_IRQ_PRIORITY,
    UART2_IRQ_PRIORITY,
    UART3_IRQ_PRIORITY,
    I2S_IRQ_PRIORITY,
    I2C0_IRQ_PRIORITY,
    I2C1_IRQ_PRIORITY,
    RTC_IRQ_PRIORITY,
    GPTimer_IRQ_PRIORITY,
    SPM_IRQ_PRIORITY,
    RGU_IRQ_PRIORITY,
    PMU_DIG_IRQ_PRIORITY,
    EINT_IRQ_PRIORITY,
    SFC_IRQ_PRIORITY,
    BTIF_IRQ_PRIORITY,
    CONNSYS0_IRQ_PRIORITY,
    CONNSYS1_IRQ_PRIORITY,
    RESERVED_IRQ_PRIORITY,
    RESERVED_IRQ_PRIORITY,
    SW_ISR0_IRQ_PRIORITY,
    SW_ISR1_IRQ_PRIORITY,
    SW_ISR2_IRQ_PRIORITY,
    SW_ISR3_IRQ_PRIORITY,
    RESERVED_IRQ_PRIORITY,
    RESERVED_IRQ_PRIORITY,
};

nvic_function_t nvic_function_table[IRQ_NUMBER_MAX];

static uint32_t get_pending_irq()
{
    return ((SCB->ICSR & SCB_ICSR_ISRPENDING_Msk) >> SCB_ICSR_ISRPENDING_Pos);
}

hal_nvic_status_t hal_nvic_init(void)
{
    static uint32_t priority_set = 0;
    uint32_t i;

    if (priority_set == 0) {
        /* Set defualt priority only one time */
        for (i = 0; i < IRQ_NUMBER_MAX; i++) {
            NVIC_SetPriority((hal_nvic_irq_t)i, defualt_irq_priority[i]);
        }
        priority_set = 1;
    }
    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_enable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_EnableIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_disable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_DisableIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

uint32_t hal_nvic_get_pending_irq(hal_nvic_irq_t irq_number)
{
    uint32_t ret = 0xFF;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        return ret;
    } else {
        ret = NVIC_GetPendingIRQ(irq_number);
    }

    return ret;
}

hal_nvic_status_t hal_nvic_set_pending_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_SetPendingIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_clear_pending_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_ClearPendingIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_set_priority(hal_nvic_irq_t irq_number, uint32_t priority)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_SetPriority(irq_number, priority);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

uint32_t hal_nvic_get_priority(hal_nvic_irq_t irq_number)
{
    uint32_t ret = RESERVED_IRQ_PRIORITY;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        return ret;
    } else {
        ret = NVIC_GetPriority(irq_number);
    }

    return ret;
}

static uint32_t get_current_irq()
{
    uint32_t irq_num = ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
    return (irq_num - 16);
}


extern void Flash_ReturnReady(void);
ATTR_TEXT_IN_TCM hal_nvic_status_t isrC_main()
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;
    hal_nvic_irq_t irq_number;

    Flash_ReturnReady();

    irq_number = (hal_nvic_irq_t)(get_current_irq());

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else if (nvic_function_table[irq_number].nvic_callback == NULL) {
        status = HAL_NVIC_STATUS_ERROR_NO_ISR;
#ifdef READY
        log_hal_error("ERROR: no IRQ handler! \n");
#endif
        return status;
    } else {
        nvic_function_table[irq_number].irq_pending = get_pending_irq();
        nvic_function_table[irq_number].nvic_callback(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_register_isr_handler(hal_nvic_irq_t irq_number, hal_nvic_isr_t callback)
{
    uint32_t mask;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX || callback == NULL) {
        return HAL_NVIC_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    NVIC_ClearPendingIRQ(irq_number);
    nvic_function_table[irq_number].nvic_callback = callback;
    nvic_function_table[irq_number].irq_pending = 0;
    restore_interrupt_mask(mask);

    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask(uint32_t *mask)
{
    *mask = save_and_set_interrupt_mask();
    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_restore_interrupt_mask(uint32_t mask)
{
    restore_interrupt_mask(mask);
    return HAL_NVIC_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_NVIC_MODULE_ENABLED */

