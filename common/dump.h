/**
 * @file    dump.h
 *
 * Defines an API to log data.
 */
#ifndef __X_DUMP_H
#define __X_DUMP_H

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Function to print an area of memory in hex.
 *
 * The NRF_...HEXDUMP... functions don't always print all of the data.
 *
 *@param[in]    p_name      Description of data being printed, may be NULL.
 *@param[in]    p_data      Pointer to data to print.
 *@param[in]    length      Amount of data to print in bytes.
 *@param[in]    stride      Number of bytes to print before a newline.
 */
void dump_hex(const char * p_name, const void * const p_data, size_t length, uint16_t stride);

#ifdef __cplusplus
}
#endif

#endif  // __X_DUMP_H

