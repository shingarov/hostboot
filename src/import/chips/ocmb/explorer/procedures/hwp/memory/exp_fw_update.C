/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/ocmb/explorer/procedures/hwp/memory/exp_fw_update.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2019,2020                        */
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
/// @file exp_fw_update.C
/// @brief Procedure definition to update explorer firmware
///
// *HWP HWP Owner: Glenn Miles <milesg@ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#include <fapi2.H>
#include <exp_fw_update.H>
#include <lib/inband/exp_inband.H>
#include <lib/shared/exp_consts.H>
#include <exp_data_structs.H>
#include <generic/memory/lib/utils/c_str.H>
#include <lib/omi/crc32.H>
#include <mmio_access.H>

namespace mss
{
namespace exp
{

constexpr uint32_t FLASH_WRITE_BLOCK_SIZE = 256;

namespace bupg
{

///
/// @brief Checks explorer response argument for a successful command
/// @param[in] i_target OCMB target
/// @param[in] i_rsp response from command
/// @param[in] i_cmd original command
/// @return FAPI2_RC_SUCCESS iff okay
///
fapi2::ReturnCode check_response(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                 const host_fw_response_struct& i_rsp,
                                 const host_fw_command_struct& i_cmd)
{
    // The byte indices of i_rsp.response_argument which contain the MCHP
    // error codes. Useful for debug purposes.
    static constexpr uint8_t MCHP_ERROR_CODE_1 = 1;
    static constexpr uint8_t MCHP_ERROR_CODE_2 = 2;
    static constexpr uint8_t MCHP_ERROR_CODE_3 = 3;

    // Check if cmd was successful
    // The CMD_WRITE subcommand provides 3 bytes of MCHP error
    // information in response_argument[1] to [3]
    FAPI_ASSERT(i_rsp.response_argument[0] == omi::response_arg::SUCCESS &&
                i_rsp.request_identifier == i_cmd.request_identifier,
                fapi2::EXP_UPDATE_CMD_FAILED().
                set_TARGET(i_target).
                set_RSP_ID(i_rsp.response_id).
                set_REQ_ID(i_rsp.request_identifier).
                set_MCHP_ERROR_CODE_1(i_rsp.response_argument[MCHP_ERROR_CODE_1]).
                set_MCHP_ERROR_CODE_2(i_rsp.response_argument[MCHP_ERROR_CODE_2]).
                set_MCHP_ERROR_CODE_3(i_rsp.response_argument[MCHP_ERROR_CODE_3]),
                "Recieved failure response for firmware update command on %s . MCHP Error codes: "
                "response_argument[1] = 0x%02X, [2] = 0x%02X, [3] = 0x%02X",
                mss::c_str(i_target),
                i_rsp.response_argument[MCHP_ERROR_CODE_1],
                i_rsp.response_argument[MCHP_ERROR_CODE_2],
                i_rsp.response_argument[MCHP_ERROR_CODE_3]);

fapi_try_exit:
    return fapi2::current_err;
}

}// ns bupg

///
/// @brief host_fw_command_struct structure setup for flash_write
/// @param[in] i_binary_size the total size of the binary image
/// @param[in] i_seq_number the sequence number of this command
/// @param[in] i_cmd_data_crc the command data CRC
/// @param[out] o_cmd the command packet to update
/// @return FAPI2_RC_SUCCESS iff ok
///
fapi2::ReturnCode setup_flash_write_cmd(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                        const uint32_t i_binary_size,
                                        const uint16_t i_seq_number,
                                        const uint32_t i_cmd_data_crc,
                                        host_fw_command_struct& o_cmd)
{
    std::vector<uint8_t> swapped32;
    std::vector<uint8_t> cmd_args;
    std::vector<uint8_t> test16_vec;
    const uint16_t test16_value = 1;

    memset(&o_cmd, 0, sizeof(host_fw_command_struct));

    // Issue EXP_FW_BINARY_UPGRADE cmd though EXP-FW REQ buffer
    o_cmd.cmd_id = mss::exp::omi::EXP_FW_BINARY_UPGRADE;
    o_cmd.cmd_flags = mss::exp::omi::ADDITIONAL_DATA;

    // Host generated id number (returned in response packet)
    uint32_t l_counter = 0;
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_OCMB_COUNTER, i_target, l_counter));
    o_cmd.request_identifier = l_counter;

    //always send a block of data
    o_cmd.cmd_length = FLASH_WRITE_BLOCK_SIZE;

    o_cmd.cmd_crc = i_cmd_data_crc;
    o_cmd.host_work_area = 0;
    o_cmd.cmd_work_area = 0;
    memset(o_cmd.padding, 0, sizeof(o_cmd.padding));

    //populate command arguments using correct endian byte ordering
    //NOTE: putCMD can not do the byte ordering of the command_arguments
    //      field for us.

    //sub-command id is single byte.  Never requires byte-swapping.
    cmd_args.push_back(bupg::SUB_CMD_WRITE);

    //forceCrctEndian can only handle 1, 2, 4, and 8 byte integers, so
    //for the flash_binary_size field, which is a 3 byte integer, treat
    //it as a 4 byte value and shift result right or left one byte
    //depending on if it is little or big endian.
    FAPI_TRY(mss::exp::ib::forceCrctEndian(i_binary_size, swapped32));

    // Test for big or little endian value on a known value (0x1)
    FAPI_TRY(mss::exp::ib::forceCrctEndian(test16_value, test16_vec));

    //if the least significant byte ended up in byte 0, then the
    //result is a little endian value.
    if(test16_vec[0] == 1)
    {
        //use first 3 bytes of 4 byte value
        cmd_args.insert(cmd_args.end(), swapped32.begin(), swapped32.end() - 1);
    }
    else //big endian
    {
        //use last 3 bytes of 4 byte value
        cmd_args.insert(cmd_args.end(), swapped32.begin() + 1, swapped32.end());
    }

    //add the sequence number
    FAPI_TRY(mss::exp::ib::forceCrctEndian(i_seq_number, cmd_args));

    //copy cmd_args vector into command_args array
    std::copy(cmd_args.begin(), cmd_args.end(), &o_cmd.command_argument[0]);

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Sets the command_argument fields for flash_commit sub-command
///        in the correct endianness.
///
/// @param[in] i_target OCMB target that will be acted upon with this command
/// @param[out] o_cmd the command packet to update
///
fapi2::ReturnCode setup_flash_commit_cmd(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    host_fw_command_struct& o_cmd)
{
    memset(&o_cmd, 0, sizeof(host_fw_command_struct));

    // Issue EXP_FW_BINARY_UPGRADE cmd though EXP-FW REQ buffer
    o_cmd.cmd_id = mss::exp::omi::EXP_FW_BINARY_UPGRADE;
    o_cmd.cmd_flags = 0;

    // Retrieve a unique sequence id for this transaction
    uint32_t l_counter = 0;
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_OCMB_COUNTER, i_target, l_counter));
    o_cmd.request_identifier = l_counter;
    o_cmd.cmd_length = 0;

    o_cmd.cmd_crc = 0xffffffff;
    o_cmd.host_work_area = 0;
    o_cmd.cmd_work_area = 0;
    memset(o_cmd.padding, 0, sizeof(o_cmd.padding));

    // Set the sub-command ID in the command argument field to FLASH_COMMIT
    o_cmd.command_argument[0] = bupg::SUB_CMD_COMMIT;

fapi_try_exit:
    return fapi2::current_err;
}

}//exp
}//mss

extern "C"
{

///
/// @brief Updates explorer firmware
/// @param[in] i_target the controller
/// @param[in] i_image_ptr pointer to the binary image
/// @param[in] i_image_sz size of the binary image
/// @return FAPI2_RC_SUCCESS if ok
///
    fapi2::ReturnCode exp_fw_update(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                    const uint8_t* i_image_ptr, const size_t i_image_sz)
    {
        uint16_t seq_num = 0;
        uint32_t block_crc = 0;
        std::vector<uint8_t> buffer;
        const uint8_t* end_ptr = i_image_ptr + i_image_sz;

        FAPI_INF("Entering exp_fw_update(%s). imageSize[0x%08x]",
                 mss::c_str(i_target), i_image_sz);

        //Check that i_image_sz value is not larger than 3 bytes, which
        //is the actual field size for this value in the packet.
        FAPI_ASSERT(((i_image_sz & 0xff000000) == 0),
                    fapi2::EXP_UPDATE_INVALID_IMAGE_SIZE()
                    .set_IMAGE_SIZE(i_image_sz),
                    "exp_fw_update: image size[0x%08x] must be less than 16MB!",
                    i_image_sz);

        // Write successive blocks until the entire image is written
        buffer.reserve(mss::exp::FLASH_WRITE_BLOCK_SIZE);

        for(uint8_t* cur_ptr = const_cast<uint8_t*>(i_image_ptr);
            cur_ptr < end_ptr;
            cur_ptr += mss::exp::FLASH_WRITE_BLOCK_SIZE)
        {
            const uint32_t bytes_left = end_ptr - cur_ptr;
            const uint32_t data_size = (bytes_left <
                                        mss::exp::FLASH_WRITE_BLOCK_SIZE) ?
                                       bytes_left :
                                       mss::exp::FLASH_WRITE_BLOCK_SIZE;

            // copy data into a vector (required by crc32_gen)
            buffer.assign(cur_ptr, cur_ptr + data_size);

            // pad end with FF's if smaller than block size
            buffer.resize(mss::exp::FLASH_WRITE_BLOCK_SIZE, 0xFF);

            // calculate the crc
            block_crc = crc32_gen(buffer);

            // endian swap
            FAPI_TRY(mss::exp::ib::correctMMIOEndianForStruct(buffer));
            FAPI_TRY(mss::exp::ib::correctMMIOword_order(buffer));

            // write block to data buffer on explorer
            FAPI_TRY(fapi2::putMMIO(i_target,
                                    mss::exp::ib::EXPLR_IB_DATA_ADDR,
                                    mss::exp::ib::BUFFER_TRANSACTION_SIZE,
                                    buffer));

            // Issue flash_write sub-command through EXP-FW request buffer
            host_fw_command_struct flash_write_cmd;
            {
                // Set up the command packet
                FAPI_TRY(mss::exp::setup_flash_write_cmd(
                             i_target,
                             i_image_sz,
                             seq_num,
                             block_crc,
                             flash_write_cmd),
                         "exp_fw_update: Failed setup_flash_write_cmd() "
                         "for %s! seq_num[%u]",
                         mss::c_str(i_target), seq_num);

                // Send the command packet
                FAPI_TRY(mss::exp::ib::putCMD(i_target, flash_write_cmd),
                         "exp_fw_update: Failed flash_write putCMD() "
                         "for %s! seq_num[%u]",
                         mss::c_str(i_target), seq_num);
            }

            // Read the response message from EXP-FW RESP buffer
            {
                host_fw_response_struct response;
                std::vector<uint8_t> rsp_data;

                // Normally we need to poll the outbound doorbell and read/check the
                // fw_response_struct to make sure the command completed. In this case,
                // we will only check the fw_response_struct for every 16th transfer to
                // speed things up.
                // We still have to poll the doorbell to make sure the command completed
                if ((seq_num % 16) == 0)
                {
                    // Read response from buffer
                    FAPI_TRY(mss::exp::ib::getRSP(i_target, response, rsp_data),
                             "exp_fw_update: getRSP() failed for flash_write "
                             "on %s! seq_num[%u]",
                             mss::c_str(i_target), seq_num);

                    // Check status in response packet
                    FAPI_TRY(mss::exp::bupg::check_response(i_target, response, flash_write_cmd),
                             "exp_fw_update: error response for flash_write "
                             "on %s! seq_num[%u]",
                             mss::c_str(i_target), seq_num);
                }
                else
                {
                    // Poll response doorbell only
                    FAPI_TRY(mss::exp::ib::poll_for_response_ready(i_target),
                             "exp_fw_update: error polling response for flash_write "
                             "on %s! seq_num[%u]",
                             mss::c_str(i_target), seq_num);

                    // Clear response doorbell
                    FAPI_TRY(mss::exp::ib::clear_outbound_doorbell(i_target));
                }
            }

            //increment sequence number after each 256 byte block is written
            seq_num++;
        }

        host_fw_command_struct flash_commit_cmd;
        // Issue the flash_commit sub-command through EXP-FW request buffer
        {
            FAPI_TRY(mss::exp::setup_flash_commit_cmd(i_target,
                     flash_commit_cmd));
            FAPI_TRY(mss::exp::ib::putCMD(i_target, flash_commit_cmd),
                     "exp_fw_update: putCMD() failed for flash_commit on %s!",
                     mss::c_str(i_target));
        }

        // Read the response message from EXP-FW RESP buffer
        {
            host_fw_response_struct response;
            std::vector<uint8_t> rsp_data;

            // Wait a little while first (value based on MCHP estimations):
            // 2 sec for image authentication
            // 7 sec to erase 600K on flash part
            // 1800 usec to program flash image
            // = 10.8 sec, so wait 15 sec to be safe
            FAPI_INF("Waiting for flash image commit to complete on %s...", mss::c_str(i_target));

            for (uint64_t l_seconds = 0; l_seconds < 15; ++l_seconds)
            {
                FAPI_TRY(fapi2::delay(mss::DELAY_1S, 200));
            }

            FAPI_TRY(mss::exp::ib::getRSP(i_target, response, rsp_data),
                     "exp_fw_update: getRSP() failed for flash_commit on %s!",
                     mss::c_str(i_target) );

            // Check if cmd was successful
            FAPI_TRY(mss::exp::bupg::check_response(i_target, response, flash_commit_cmd),
                     "exp_fw_update: error response for flash_commit on %s!",
                     mss::c_str(i_target) );
        }

    fapi_try_exit:
        FAPI_INF("Exiting exp_fw_update(%s) with return code : 0x%08x...",
                 mss::c_str(i_target), (uint64_t) fapi2::current_err);
        return fapi2::current_err;
    }

} //extern "C"
