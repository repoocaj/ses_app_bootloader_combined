/**
 * @file    sd_version.h
 *
 * Defines an API extract information about the SoftDevice.
 */
#ifndef __X_SD_VERSION_H
#define __X_SD_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Returns a static string describing the SoftDevice version.
 *
 * The string will have the general form 'sAAA vB.C.D' Where AAA is the
 * SoftDevice type and B.C.D is the major, minor and bugfix version.  The
 * string may also be 'unknown' if the SoftDevice firmware ID can't be
 * translated to a string.
 */
char * sd_ver_act_str(void);

/**@brief   Returns a static string describing the SoftDevice version defined in the header file.
 *
 * The string will have the general form 'sAAA vB.C.D' Where AAA is the
 * SoftDevice type and B.C.D is the major, minor and bugfix version.
 */
char * sd_ver_header_str(void);

/**@brief   Returns the SoftDevice firmware ID.
 */
uint16_t sd_ver_fwid(void);

#ifdef __cplusplus
}
#endif

#endif  // __X_SD_VERSION_H

