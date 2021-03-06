/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/memory/lib/phy/mss_dwl.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2018,2020                        */
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
/// @file lib/phy/mss_dwl.H
/// @brief DWL LRDIMM training
/// Training is very device specific, so there is no attempt to generalize
/// this code in any way.
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Andre Marin <aamarin@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#ifndef MSS_DWL_H
#define MSS_DWL_H

#include <lib/phy/mss_lrdimm_training.H>
#include <lib/shared/mss_const.H>

#ifdef LRDIMM_CAPABLE
    #include <lib/phy/mss_lrdimm_training_helper.H>
#endif

//
// Note: The LRDIMM support has NOT been characterized or qualified by IBM
// LRDIMM development was done in conjunction with an OpenPower partner and is being released to all of OpenPower
// As IBM has not characterized or qualified LRDIMM, IBM will not be able to offer technical expertise to debug any LRDIMM side issues
// As such, the OpenPower partner using LRDIMM assumes ownership and associated risk for using LRDIMM's in their system
// Additionally, LRDIMM will see a large IPL time increase compared to RDIMM, as all of the LRDIMM training has to be enabled in software
//

namespace mss
{

namespace training
{

namespace lrdimm
{

///
/// @brief DWL training step
/// @note Will take a long time to run as this step is done via a software algorithm
///
class dwl : public step
{
    public:
        dwl() :
            step("DWL")
        {}

        ///
        /// @brief Default virtual destructor
        ///
        virtual ~dwl() = default;

        ///
        /// @brief Sets the rank to calibrate for WR LVL in the buffer
        /// @param[in] i_target the DIMM target on which to operate
        /// @param[in] i_rank the rank target on which to operate
        /// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff ok
        ///
        fapi2::ReturnCode set_rank(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                                   const uint64_t i_rank) const;

        ///
        /// @brief Configures the given rank into WR LVL mode
        /// @param[in] i_target DIMM target on which to operate
        /// @paran[in] i_rank the rank on which to operate
        /// @paran[in] i_mode mode for the WR LVL
        /// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff ok
        ///
        fapi2::ReturnCode dram_wr_lvl( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                                       const uint64_t i_rank,
                                       const mss::states i_mode) const;

        ///
        /// @brief Executes the pre-cal step workaround
        /// @param[in] i_target - the MCA target on which to operate
        /// @param[in] i_rp - the rank pair
        /// @param[in] i_abort_on_error - whether or not we are aborting on cal error
        /// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff ok
        ///
        fapi2::ReturnCode pre_workaround( const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                                          const uint64_t i_rp,
                                          const uint8_t i_abort_on_error ) const;

        ///
        /// @brief Sets up and runs the calibration step
        /// @param[in] i_target - the MCA target on which to operate
        /// @param[in] i_rp - the rank pair
        /// @param[in] i_abort_on_error - whether or not we are aborting on cal error
        /// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff ok
        ///
        fapi2::ReturnCode run( const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                               const uint64_t i_rp,
                               const uint8_t i_abort_on_error ) const;

        ///
        /// @brief Executes the post-cal step workaround
        /// @param[in] i_target - the MCA target on which to operate
        /// @param[in] i_rp - the rank pair
        /// @param[in] i_abort_on_error - whether or not we are aborting on cal error
        /// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff ok
        ///
        fapi2::ReturnCode post_workaround( const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                                           const uint64_t i_rp,
                                           const uint8_t i_abort_on_error ) const;

        ///
        /// @brief Sets MREP Delay value
        /// @param[in] i_target the DIMM target
        /// @param[in] i_rank the rank to operate on - drives the function space select
        /// @param[in] delay value /64 Tck - MREP delay value
        /// @return FAPI2_RC_SUCCESS if okay
        /// @note Sets DA setting for buffer control word (F[3:0]BC2x, F[3:0]BC3x)
        ///
        fapi2::ReturnCode set_delay(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                                    const uint8_t i_rank,
                                    const uint8_t i_delay ) const;


        ///
        /// @brief Callsout if a rank does not see a 0->1 transition
        /// @param[in] i_target the DIMM target on which to operate
        /// @param[in] i_rank the current rank
        /// @param[in] i_recorders the recorders on which to process the data
        /// @return FAPI2_RC_SUCCESS if okay
        ///
        fapi2::ReturnCode check_errors( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                                        const uint8_t i_rank,
                                        const std::vector<std::pair<mrep_dwl_recorder, mrep_dwl_recorder>>& i_recorders) const;

        ///
        /// @brief Creates the nibble flags for the invalid data callout
        /// @param[in] i_target the DIMM target on which to operate
        /// @param[in] i_rank the current rank
        /// @param[in] i_recorders the recorders on which to process the data
        /// @param[out] o_invalid_count number of invalid data occurances seen
        /// @return invalid data nibble flags
        /// @note Invalid data is defined as not having all zeros or all ones
        ///
        uint32_t flag_invalid_data( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                                    const uint8_t i_rank,
                                    const std::vector<std::pair<mrep_dwl_recorder, mrep_dwl_recorder>>& i_recorders,
                                    uint64_t& o_invalid_count) const;

        ///
        /// @brief Calls out if invalid data is seen during this calibration step
        /// @param[in] i_target the DIMM target on which to operate
        /// @param[in] i_rank the current rank
        /// @param[in] i_recorders the recorders on which to process the data
        /// @return FAPI2_RC_SUCCESS if okay
        /// @note Invalid data is defined as not having all zeros or all ones
        ///
        fapi2::ReturnCode callout_invalid_data( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                                                const uint8_t i_rank,
                                                const std::vector<std::pair<mrep_dwl_recorder, mrep_dwl_recorder>>& i_recorders) const;

        ///
        /// @brief Creates the nibble flags for the no transition callout
        /// @param[in] i_target the DIMM target on which to operate
        /// @param[in] i_rank the current rank
        /// @param[in] i_recorders the recorders on which to process the data
        /// @return no transition nibble flags
        ///
        uint32_t flag_no_transition( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                                     const uint8_t i_rank,
                                     const std::vector<std::pair<mrep_dwl_recorder, mrep_dwl_recorder>>& i_recorders) const;

        ///
        /// @brief Calls out if a rank does not see a 0->1 transition
        /// @param[in] i_target the DIMM target on which to operate
        /// @param[in] i_rank the current rank
        /// @param[in] i_recorders the recorders on which to process the data
        /// @return FAPI2_RC_SUCCESS if okay
        ///
        fapi2::ReturnCode callout_no_transition( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                const uint8_t i_rank,
                const std::vector<std::pair<mrep_dwl_recorder, mrep_dwl_recorder>>& i_recorders) const;

        ///
        /// @brief Write the results to buffer generate PBA commands
        /// @param[in] i_target the DIMM target
        /// @param[in] i_rank the rank number
        /// @param[in] i_recorders a vector of the MREP result
        /// @param[out] o_container the PBA commands structure
        /// @return FAPI2_RC_SUCCESS if and only if ok
        /// @note a little helper to allow us to unit test that we generate the PBA commands ok
        ///
        fapi2::ReturnCode write_result_to_buffers_helper( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                const uint8_t i_rank,
                const std::vector<std::pair<mrep_dwl_recorder, mrep_dwl_recorder>>& i_recorders,
                mss::ddr4::pba::commands& o_container) const;

        ///
        /// @brief Writes the results to the buffers for a given DIMM-rank
        /// @param[in] i_target the DIMM target on which to operate
        /// @param[in] i_rank the current rank
        /// @param[in] i_recorders the recorders on which to process the data
        /// @return FAPI2_RC_SUCCESS if okay
        ///
        fapi2::ReturnCode write_result_to_buffers( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                const uint8_t i_rank,
                const std::vector<std::pair<mrep_dwl_recorder, mrep_dwl_recorder>>& i_recorders) const;


        ///
        /// @brief Calculates the number of cycles a given calibration step will take
        /// @param[in] i_target - the MCA target on which to operate
        /// @return l_cycles - the number of cycles a given calibration step wil take
        ///
        uint64_t calculate_cycles( const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target ) const override;
};

} // ns training

} // ns lrdimm

} // ns mss

#endif
