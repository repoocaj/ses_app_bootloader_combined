/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup bootloader_secure_ble main.c
 * @{
 * @ingroup dfu_bootloader_api
 * @brief Bootloader project main file for secure DFU.
 *
 */

#include <stdint.h>
#include "boards.h"
#include "nrf_mbr.h"
#include "nrf_bootloader.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_bootloader_dfu_timers.h"
#include "nrf_dfu.h"

#define NRF_LOG_MODULE_NAME             boot

#define NRF_LOG_LEVEL                   4

#include "nrf_log.h"

NRF_LOG_MODULE_REGISTER();

#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "app_error_weak.h"
#include "nrf_bootloader_info.h"
#include "nrf_delay.h"

#include "version.h"
#include "sd_version.h"

NRF_SECTION_DEF(rtt, uint8_t);

static void on_error(void)
{
    NRF_LOG_FINAL_FLUSH();

#if NRF_MODULE_ENABLED(NRF_LOG_BACKEND_RTT)
    // To allow the buffer to be flushed by the host.
    nrf_delay_ms(100);
#endif
#ifdef NRF_DFU_DEBUG_VERSION
    NRF_BREAKPOINT_COND;
#endif
    NVIC_SystemReset();
}


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    NRF_LOG_ERROR("%s:%d", p_file_name, line_num);
    on_error();
}


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    NRF_LOG_ERROR("Received a fault! id: 0x%08x, pc: 0x%08x, info: 0x%08x", id, pc, info);
    on_error();
}


void app_error_handler_bare(uint32_t error_code)
{
    NRF_LOG_ERROR("Received an error: 0x%08x!", error_code);
    on_error();
}

/**
 * @brief Function notifies certain events in DFU process.
 */
static void dfu_observer(nrf_dfu_evt_type_t evt_type)
{
    switch (evt_type)
    {
        case NRF_DFU_EVT_DFU_FAILED:
        case NRF_DFU_EVT_DFU_ABORTED:
        case NRF_DFU_EVT_DFU_INITIALIZED:
            bsp_board_init(BSP_INIT_LEDS);
            bsp_board_led_on(BSP_BOARD_LED_0);
            bsp_board_led_off(BSP_BOARD_LED_1);
            break;
        case NRF_DFU_EVT_TRANSPORT_ACTIVATED:
            bsp_board_led_off(BSP_BOARD_LED_0);
            bsp_board_led_on(BSP_BOARD_LED_1);
            break;
        case NRF_DFU_EVT_DFU_STARTED:
            break;
        default:
            break;
    }
}


static void chip_info(void)
{
    uint8_t *p = (uint8_t *)&NRF_FICR->INFO.VARIANT;

    NRF_LOG_DEBUG("nRF%x Variant: %c%c%c%c ", NRF_FICR->INFO.PART, p[3], p[2], p[1], p[0]);
    NRF_LOG_DEBUG("RAM: %dKB Flash: %dKB", NRF_FICR->INFO.RAM, NRF_FICR->INFO.FLASH);
    NRF_LOG_DEBUG("Device ID: %x%x", NRF_FICR->DEVICEID[0], NRF_FICR->DEVICEID[1]);
}

static void mbr_info(void)
{
    uint32_t bl_primary = *(uint32_t *)MBR_BOOTLOADER_ADDR;
    uint32_t bl_secondary = *(uint32_t *)MBR_UICR_BOOTLOADER_ADDR;

    uint32_t param_primary = *(uint32_t *)MBR_PARAM_PAGE_ADDR;
    uint32_t param_secondary = *(uint32_t *)MBR_UICR_PARAM_PAGE_ADDR;

    NRF_LOG_DEBUG("Bootloader 0x%08x (0x%08x, 0x%08x)", BOOTLOADER_ADDRESS, bl_primary, bl_secondary);
    NRF_LOG_DEBUG("MBR Parameters 0x%08x (0x%08x, 0x%08x)", MBR_PARAMS_PAGE_ADDRESS, param_primary, param_secondary);
}

static uint32_t get_rtc_counter(void)
{
    return NRF_RTC1->COUNTER;
}

/**@brief Function for application main entry. */
int main(void)
{
    memset(NRF_SECTION_START_ADDR(rtt), 0, NRF_SECTION_LENGTH(rtt));

    uint32_t ret_val;

    // Protect MBR and bootloader code from being overwritten.
    ret_val = nrf_bootloader_flash_protect(0, MBR_SIZE, false);
    APP_ERROR_CHECK(ret_val);
    ret_val = nrf_bootloader_flash_protect(BOOTLOADER_START_ADDR, BOOTLOADER_SIZE, false);
    APP_ERROR_CHECK(ret_val);

    (void) NRF_LOG_INIT(get_rtc_counter);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // Immediately display app version
    NRF_LOG_RAW_INFO("\nBootloader v%d.%d.%d (%s)\n",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_SUB,
#if defined(DEBUG)
        "debug"
#else
        "release"
#endif
    );
    // Display both the version of SoftDevice that the application was built
    // against and what is actually installed.
    NRF_LOG_RAW_INFO("SoftDevice header: %s, actual: %s (0x%04x)\n",
        sd_ver_header_str(),
        sd_ver_act_str(),
        sd_ver_fwid()
    );
    chip_info();
    mbr_info();

    // This function does not return unless there is an error
    ret_val = nrf_bootloader_init(dfu_observer);
    APP_ERROR_CHECK(ret_val);

    NRF_LOG_ERROR("Should never be reached");
}

/**
 * @}
 */
