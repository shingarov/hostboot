/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/io/p9_io_scom.H $          */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2019                        */
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
///
/// @file p9_io_scom.H
/// @brief Register Access.
///----------------------------------------------------------------------------
/// *HWP HWP Owner        : Chris Steffen <cwsteffen@us.ibm.com>
/// *HWP HWP Backup Owner : Gary Peterson <garyp@us.ibm.com>
/// *HWP FW Owner         : Jamie Knight <rjknight@us.ibm.com>
/// *HWP Team             : IO
/// *HWP Level            : 3
/// *HWP Consumed by      : FSP:HB
///----------------------------------------------------------------------------
///
/// @verbatim
/// Register Access via scom.
///
/// @endverbatim
///----------------------------------------------------------------------------

#ifndef P9_IO_SCOM_H_
#define P9_IO_SCOM_H_

//* *****************************
//* Defines
//* *****************************

// @brief IO Base Addresses
#define P9_PSI_PHY_BASE_0x00000000       0x00000000
#define P9_XBUS_PHY_BASE_0x06010C00      0x06010C00
#define P9_ABUS0_PHY_BASE_0x09011000     0x09011000
#define P9_ABUS3_PHY_BASE_0x0C011000     0x0C011000
#define P9_OBUS0_PHY_BASE_0x09010C00     0x09010C00
#define P9_OBUS1_PHY_BASE_0x0A010C00     0x0A010C00
#define P9_OBUS2_PHY_BASE_0x0B010C00     0x0B010C00
#define P9_OBUS3_PHY_BASE_0x0C010C00     0x0C010C00
#define P9_DMI0_PHY_BASE_0x07011000      0x07011000
#define P9A_OMI_PHY_BASE_0x07011000      0x07011000
#define CEN_PHY_BASE_0x02010400          0x02010400

//-----------------------------------------------------------------------------
//  FAPI Includes
//-----------------------------------------------------------------------------
#include <fapi2.H>

/**
 * @namespace io
 * This class provides a generic class for a register to read/write SCOMs.
 */
namespace io
{

/**
 * @brief Parses the register data by the i_start and i_width of a field and returns
 *        a right aligned value.
 * @param[in] i_scom_addr     Not used
 * @param[in] i_start         Represents the start bit of the field within the reg
 * @param[in] i_width         Represents the width of the field within the reg
 * @param[in] i_register_data Scom Data from previous io::read()
 * @return Field Data
 */
inline uint64_t get(
    const uint64_t i_scom_addr,
    const uint8_t  i_start,
    const uint8_t  i_width,
    const uint64_t i_register_data )
{
    const uint8_t REGISTER_WIDTH = 64;
    const uint8_t SHIFT          = REGISTER_WIDTH - i_start - i_width;
    const uint64_t MASK          = ( ( (uint64_t)0x1 << i_width ) - 1 ) << SHIFT;
    return ( ( i_register_data & MASK ) >> SHIFT );
}

/**
 * @brief Sets the field specified by the i_start and i_width of the register data to the right
 *        alighted i_data.
 * @param[in]     i_scom_addr      Is not used in this function.
 * @param[in]     i_start          Represents the start bit of the field within the reg
 * @param[in]     i_width          Represents the width of the field within the reg
 * @param[in]     i_data           Data to be set
 * @param[in,out] io_register_data Scom Data
 * @return void
 */
inline void set(
    const uint64_t  i_scom_addr,
    const uint8_t   i_start,
    const uint8_t   i_width,
    const uint64_t  i_data,
    uint64_t&   io_register_data )
{
    const uint8_t REGISTER_WIDTH = 64;
    const uint8_t SHIFT          = REGISTER_WIDTH - i_start - i_width;
    const uint64_t MASK          = ( ( (uint64_t)0x1 << i_width ) - 1 ) << SHIFT;
    io_register_data = ( ( io_register_data & ~MASK ) | ( ( i_data << SHIFT ) & MASK ) );
    return;
}

/**
 * @brief Builds the base scom address
 * @tparam[in] K       fapi2::TargetType
 * @param[in] i_target FAPI2 Target
 * @return 32bit encoded base scom address
 */
template < fapi2::TargetType K >
inline uint32_t get_base_address( const fapi2::Target < K > i_target, uint32_t& o_base_addr )
{
    switch( i_target.getType() )
    {
        case fapi2::TargetType::TARGET_TYPE_XBUS:
            o_base_addr = P9_XBUS_PHY_BASE_0x06010C00;
            break;

        case fapi2::TargetType::TARGET_TYPE_DMI:
            o_base_addr = P9_DMI0_PHY_BASE_0x07011000;
            break;

        case fapi2::TargetType::TARGET_TYPE_OMI:
            o_base_addr = P9A_OMI_PHY_BASE_0x07011000;
            break;

        case fapi2::TargetType::TARGET_TYPE_OMIC:
            o_base_addr = P9A_OMI_PHY_BASE_0x07011000;
            break;

        case fapi2::TargetType::TARGET_TYPE_MC:
            o_base_addr = P9_DMI0_PHY_BASE_0x07011000;
            break;

        case fapi2::TargetType::TARGET_TYPE_ABUS:
        case fapi2::TargetType::TARGET_TYPE_OBUS:
            o_base_addr = P9_OBUS0_PHY_BASE_0x09010C00;
            break;

        case fapi2::TargetType::TARGET_TYPE_MEMBUF_CHIP:
            o_base_addr = CEN_PHY_BASE_0x02010400;
            break;

        default:
            fapi2::IO_GCR_TARGET_TYPE_NOT_FOUND().set_TARGET( i_target ).execute();
            FAPI_ERR("TargetType not found(0x%X)", i_target.getType());
    }

    return fapi2::current_err;
}

/**
 * @brief Builds the base scom address, group, and lane into the full
 *   scom address
 * @tparam[in] K          fapi2::TargetType
 * @param[in] i_target    FAPI2 Target
 * @param[in] i_scom_addr Register scom address information
 * @param[in] i_group     Group encoded into the scom address
 * @param[in] i_lane      Lane encoded into the scom address
 * @return Full 64bit encoded scom address
 */
template < fapi2::TargetType K >
inline uint64_t build_address(
    const fapi2::Target < K > i_target,
    const uint64_t            i_scom_addr,
    const uint8_t             i_group,
    const uint8_t             i_lane,
    uint64_t& o_addr )
{
    const uint8_t ADDRESS_SIZE = 64;
    const uint8_t GROUP_START  = 22;
    const uint8_t GROUP_WIDTH  = 5;
    const uint8_t LANE_START   = 27;
    const uint8_t LANE_WIDTH   = 5;
    const uint8_t GROUP_SHIFT  = ADDRESS_SIZE - GROUP_START - GROUP_WIDTH;
    const uint8_t LANE_SHIFT   = ADDRESS_SIZE - LANE_START  - LANE_WIDTH;

    uint32_t l_base_addr = 0;
    FAPI_TRY( get_base_address( i_target, l_base_addr ) )
    o_addr = ( i_scom_addr | l_base_addr
               | ( (uint64_t) i_group << GROUP_SHIFT )
               | ( (uint64_t) i_lane  << LANE_SHIFT  ) );
fapi_try_exit:
    return fapi2::current_err;
}


/**
 * @brief SCOM read function
 * @tparam[in] K           fapi2::TargetType
 * @param[in]  i_scom_addr Register Scom Information
 * @param[in]  i_start     Represents the start bit of the field within the reg
 * @param[in]  i_width     Represents the width of the field within the reg
 * @param[in]  i_target    Target to operate on
 * @param[in]  i_group     Group to operate on
 * @param[in]  i_lane      Lane to operate on
 * @param[out] o_data      Scom Data
 * @return return code. Zero on success, else platform specified error
 */
template < fapi2::TargetType K >
inline fapi2::ReturnCode read(
    const uint64_t            i_scom_addr,
    const uint8_t             i_start,
    const uint8_t             i_width,
    const fapi2::Target < K > i_target,
    const uint8_t             i_group,
    const uint8_t             i_lane,
    uint64_t&                 o_data )
{
    fapi2::buffer<uint64_t> l_register_data;

    // Stitch together the base scom address, register info, group, lane
    uint64_t l_address = 0;
    FAPI_TRY( build_address( i_target, i_scom_addr, i_group, i_lane, l_address ) );

    // Execute fapi2 getscom
    FAPI_TRY( fapi2::getScom( i_target, l_address, l_register_data ),
              "getScom Failed(0x%X): Addr(0x%016llX) Data(0x%016llX)",
              (uint64_t) fapi2::current_err, l_address, (uint64_t)l_register_data );

    o_data = l_register_data;
fapi_try_exit:
    return fapi2::current_err;
}

/**
 * @brief SCOM write function
 * @tparam[in] K           fapi2::TargetType
 * @param[in]  i_scom_addr Register Scom Information
 * @param[in]  i_start     Represents the start bit of the field within the reg
 * @param[in]  i_width     Represents the width of the field within the reg
 * @param[in]  i_target    Target to operate on
 * @param[in]  i_group     Group to operate on
 * @param[in]  i_lane      Lane to operate on
 * @param[in]  i_data      Scom Data
 * @return return code. Zero on success, else platform specified error
 */
template < fapi2::TargetType K >
inline fapi2::ReturnCode write(
    const uint64_t            i_scom_addr,
    const uint8_t             i_start,
    const uint8_t             i_width,
    const fapi2::Target < K > i_target,
    const uint8_t             i_group,
    const uint8_t             i_lane,
    const uint64_t            i_data )
{
    fapi2::buffer<uint64_t> l_register_data( i_data );

    // Stitch together the base scom address, register info, group, lane
    uint64_t l_address = 0;
    FAPI_TRY( build_address( i_target, i_scom_addr, i_group, i_lane, l_address ) );

    // Execute fapi2 putscom
    FAPI_TRY( fapi2::putScom( i_target, l_address, l_register_data ),
              "putScom Failed(0x%X): Addr(0x%016llX) Data(0x%016llX)",
              (uint64_t) fapi2::current_err, l_address, i_data );
fapi_try_exit:
    return fapi2::current_err;
}

/**
 * @brief SCOM read modify write function
 * @tparam[in] K           fapi2::TargetType
 * @param[in]  i_scom_addr Register Scom Information
 * @param[in]  i_start     Represents the start bit of the field within the reg
 * @param[in]  i_width     Represents the width of the field within the reg
 * @param[in]  i_target    Target to operate on
 * @param[in]  i_group     Group to operate on
 * @param[in]  i_lane      Lane to operate on
 * @param[in]  i_data      Field Data
 * @return return code. Zero on success, else platform specified error
 */
template < fapi2::TargetType K >
inline fapi2::ReturnCode rmw(
    const uint64_t            i_scom_addr,
    const uint8_t             i_start,
    const uint8_t             i_width,
    const fapi2::Target < K > i_target,
    const uint8_t             i_group,
    const uint8_t             i_lane,
    const uint64_t            i_data )
{
    uint64_t l_register_data = 0;

    FAPI_TRY( read(
                  i_scom_addr,
                  i_start,
                  i_width,
                  i_target,
                  i_group,
                  i_lane,
                  l_register_data ) );

    set(i_scom_addr, i_start, i_width, i_data, l_register_data );

    FAPI_TRY( write(
                  i_scom_addr,
                  i_start,
                  i_width,
                  i_target,
                  i_group,
                  i_lane,
                  l_register_data ) );
fapi_try_exit:
    return fapi2::current_err;
}
};


#endif /* P9_IO_SCOM_H_ */
