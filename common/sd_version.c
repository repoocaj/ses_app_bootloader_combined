/**
 * @file    sd_version.c
 *
 * Defines functions to extract the SoftDevice version
 */

#include <stdint.h>
#include <stdbool.h>

#include "sd_version.h"
#include "nrf_sdm.h"
#include "nrf_mbr.h"

/**@brief Maximum size of a version string in bytes with the format:
 *
 * sXXX vX.X.X\0
 */
#define MAX_SD_VER_SIZE     12

static char m_header_str[MAX_SD_VER_SIZE];
static char m_act_str[MAX_SD_VER_SIZE];

static struct
{
    uint16_t fwid;
    uint8_t major;
    uint8_t minor;
    uint8_t bug;
} m_fwid_table[] =
{
    { 0x88, 2, 0, 1 },
    { 0x8C, 3, 0, 0 },
    { 0x91, 3, 1, 0 },
    { 0x95, 4, 0, 0 },
    { 0x98, 4, 0, 2 },
    { 0x99, 4, 0, 3 },
    { 0x9E, 4, 0, 4 },
    { 0x9F, 4, 0, 5 },
    { 0x9D, 5, 0, 0 },
    { 0xA5, 5, 1, 0 },
    { 0xA8, 6, 0, 0 },
    { 0xAF, 6, 1, 0 },
    { 0xB7, 6, 1, 1 },
    { 0xC2, 7, 0, 0 },
    { 0xCB, 7, 0, 1 },

    // Hand-modified version for testing SD update
    { 0xbadf, 6, 1, 2 },

    { 0x0, 0, 0, 0 },   // End of list marker
};


/**@brief   Swap two characters via XOR.
 */
static void swap(char *x, char *y)
{
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
}


/**@brief Reverse character order in bufffer between buffer[i] and buffer[j].
 */
static char* reverse(char *buffer, int i, int j)
{
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);

    return buffer;
}


/**@brief itoa function that only handles postive base 10 numbers.
 */
static char* itoa(uint8_t value, char* buffer)
{
    uint8_t n = value;

    int i = 0;
    while (n)
    {
        int r = n % 10;
        buffer[i++] = '0' + r;
        n = n / 10;
    }

    // if number is 0
    if (i == 0)
        buffer[i++] = '0';

    buffer[i] = '\0';

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

static void create_sd_version_str(uint8_t major, uint8_t minor, uint8_t bug, char * buffer)
{
    buffer[0] = 's';
    itoa(SD_VARIANT_ID, &buffer[1]);
    buffer[4] = ' ';
    buffer[5] = 'v';
    itoa(major, &buffer[6]);
    buffer[7] = '.';
    itoa(minor, &buffer[8]);
    buffer[9] = '.';
    itoa(bug, &buffer[10]);
}

char * sd_ver_act_str(void)
{
    uint16_t fwid = sd_ver_fwid();
    int i = 0;
    bool found = false;

    while (m_fwid_table[i].fwid != 0x0)
    {
        if (m_fwid_table[i].fwid == fwid)
        {
            create_sd_version_str(
                m_fwid_table[i].major,
                m_fwid_table[i].minor,
                m_fwid_table[i].bug,
                m_act_str
            );
            found = true;
            break;
        }
        else
        {
            i++;
        }
    }

    if (found)
    {
        return m_act_str;
    }
    else
    {
        return "unknown";
    }
}

char * sd_ver_header_str(void)
{
    create_sd_version_str(SD_MAJOR_VERSION, SD_MINOR_VERSION, SD_BUGFIX_VERSION, m_header_str);
    return m_header_str;
}

uint16_t sd_ver_fwid(void)
{
    return SD_FWID_GET(MBR_SIZE);
}
