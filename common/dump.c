/**
 * @file    dump.c
 *
 * Defines functions to print messages to log.
 */

#include <stdint.h>

#define NRF_LOG_LEVEL                   3

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "dump.h"

#define MAX_DUMP_LEN                    64

static void _internal_dump_hex(const void * const p_data, size_t length, uint16_t stride)
{
    int i;
    bool newline = true;
    uint8_t *p = (uint8_t *) p_data;

    if (p_data)
    {
        for (i = 0; i < length; i++)
        {
            NRF_LOG_RAW_INFO("%02x ", p[i]);
            newline = false;

            if (((i + 1) % 4) == 0)
            {
                NRF_LOG_RAW_INFO(" ");
            }

            if (((i + 1) % stride) == 0)
            {
                NRF_LOG_RAW_INFO("\n");
                newline = true;
            }

        }

        if (!newline)
        {
            NRF_LOG_RAW_INFO("\n");
        }
    }
}

void dump_hex(const char * p_name, const void * const p_data, size_t length, uint16_t stride)
{
    if (NULL == p_data)
    {
        NRF_LOG_RAW_INFO("NULL buffer\n");
        return;
    }

    if (p_name)
    {
        NRF_LOG_RAW_INFO("%s (%d bytes)\n", p_name, length)
    }

    if (length > MAX_DUMP_LEN)
    {
        // print the first 32 bytes and the last 32 byets and ignore the middle
        // otherwise we may run out of memory dumping larger buffers.
        _internal_dump_hex(p_data, 32, stride);
        NRF_LOG_RAW_INFO("...\n");
        _internal_dump_hex(p_data + length - 32, 32, stride);
    }
    else
    {
        _internal_dump_hex(p_data, length, stride);
    }

    NRF_LOG_FLUSH();
}

