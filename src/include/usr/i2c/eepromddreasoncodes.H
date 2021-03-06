/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/i2c/eepromddreasoncodes.H $                   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2011,2019                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/**
 * @file eepromddreasoncodes.H
 *
 * @brief Reason codes and module ids for the EEPROM device driver
 *
 */
#ifndef __EEPROMDDREASONCODES_H
#define __EEPROMDDREASONCODES_H
// -----------------------------------------------
// Includes
// -----------------------------------------------
#include <hbotcompid.H>

namespace EEPROM
{

/**
* @enum eepromModuleid
*
* @brief Module Ids used in created errorlogs.  Indicates which
*      functions an error log was created in.
*
*/
enum eepromModuleId
{
    EEPROM_INVALID_MODULE                   = 0x00,     // Invalid Module Id
    EEPROM_PERFORM_OP                       = 0x01,
    EEPROM_READ                             = 0x02,
    EEPROM_WRITE                            = 0x03,
    EEPROM_PREPAREADDRESS                   = 0x04,
    EEPROM_READATTRIBUTES                   = 0x05,
    EEPROM_GETI2CMASTERTARGET               = 0x06,
    EEPROM_CACHE_EEPROM                     = 0x07,
    EEPROM_CLEAR_EECACHE                    = 0x08,
    EEPROM_CACHE_PERFORM_OP                 = 0x09,
    EEPROM_INVALIDATE_CACHE                 = 0x0A,
    EEPROM_RESOLVE_SOURCE                   = 0x0B,
    EEPROM_CACHE_INIT_RT                    = 0x0C,
};

/**
 * @enum eepromReasonCode
 *
 * @brief Reasoncodes used to describe what errors are being indicated.
 *
 */
enum eepromReasonCode
{
    EEPROM_INVALID_REASONCODE               = EEPROM_COMP_ID | 0x00,   // Invalid Reasoncode
    EEPROM_INVALID_OPERATION                = EEPROM_COMP_ID | 0x01,
    EEPROM_INVALID_DEVICE_TYPE              = EEPROM_COMP_ID | 0x02,
    EEPROM_ATTR_INFO_NOT_FOUND              = EEPROM_COMP_ID | 0x03,
    EEPROM_INVALID_CHIP                     = EEPROM_COMP_ID | 0x04,
    EEPROM_I2C_MASTER_PATH_ERROR            = EEPROM_COMP_ID | 0x05,
    EEPROM_TARGET_NULL                      = EEPROM_COMP_ID | 0x06,
    EEPROM_INVALID_ADDR_OFFSET_SIZE         = EEPROM_COMP_ID | 0x07,
    EEPROM_OVERFLOW_ERROR                   = EEPROM_COMP_ID | 0x08,
    EEPROM_I2C_WRITE_PAGE_SIZE_ZERO         = EEPROM_COMP_ID | 0x09,
    EEPROM_FAILED_TO_FLUSH_CONTENTS         = EEPROM_COMP_ID | 0x0A,   // Error occured while trying to flush the mmio pages out
                                                                       // containing contents of an EEPROM's cached data in the EECACHE
                                                                       // PNOR section
    EEPROM_FAILED_TO_FLUSH_HEADER           = EEPROM_COMP_ID | 0x0B,   // Error occured while trying to flush the mmio page out
                                                                       // containing EECACHE header the to pnor
    EEPROM_I2C_MUX_PATH_ERROR               = EEPROM_COMP_ID | 0x0C,
    EEPROM_NOT_IN_CACHE                     = EEPROM_COMP_ID | 0x0D,
    EEPROM_NEW_DEVICE_DETECTED              = EEPROM_COMP_ID | 0x0E,   // While looking up a part, found that current EEPROM size does not
                                                                       // match what we have seen in previous IPLs indicating a new part has
                                                                       // been installed on the system.
    EEPROM_CACHE_NOT_FOUND_IN_MAP           = EEPROM_COMP_ID | 0x0F,   // An entry we thought would be in the global map that keeps track of
                                                                       // what eeproms have been cached was not found
    EEPROM_CACHE_NOT_FOUND_IN_PNOR          = EEPROM_COMP_ID | 0x10,   // An entry we thought would be in the the EECACHE section of PNOR seems
                                                                       // to be missing
    EEPROM_CACHE_NO_VPD_IN_RSV_MEM          = EEPROM_COMP_ID | 0x11,   // When looking up the VPD section in reserved memory no entry was found.
    EEPROM_DUPLICATE_CACHE_ENTRY            = EEPROM_COMP_ID | 0x12    // While parsing eecache found in reserved memory we found at least two
                                                                       // cache entries that had identical "unique" identifiers
                                                                       // (see structs defined in eeprom_const.H)
};

enum UserDetailsTypes
{
    EEPROM_UDT_NO_FORMAT       = 0x0,
    EEPROM_UDT_PARAMETERS      = 0x1,
};

};  // end EEPROM

#endif
