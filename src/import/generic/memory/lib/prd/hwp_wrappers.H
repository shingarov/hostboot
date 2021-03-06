/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/generic/memory/lib/prd/hwp_wrappers.H $            */
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
/// @file hwp_wrappers.H
/// @brief Main wrapper file for PRD calling memory procedure code
///
// *HWP HWP Owner: Matthew Hickman <Matthew.Hickman@ibm.com>
// *HWP HWP Backup: Andre Marin <aamarin@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: HB:FSP

#ifndef _HWP_WRAPPERS_H_
#define _HWP_WRAPPERS_H_

#include <fapi2.H>
#include <generic/memory/lib/utils/mcbist/gen_mss_mcbist.H>
#include <generic/memory/lib/utils/mcbist/gen_mss_memdiags.H>
#include <generic/memory/lib/utils/mcbist/gen_mss_mcbist_traits.H>
#include <chips/ocmb/explorer/procedures/hwp/memory/lib/mcbist/exp_mcbist_traits.H>

///
/// @brief Memdiags stop command wrapper for Nimbus
/// @param[in] i_target the target
/// @return FAPI2_RC_SUCCESS iff ok
///
fapi2::ReturnCode nim_stop( const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target );

///
/// @brief Memdiags stop command wrapper for Explorer
/// @param[in] i_target the target
/// @return FAPI2_RC_SUCCESS iff ok
///
fapi2::ReturnCode exp_stop( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target );

///
/// @brief Memdiags Super Fast Init command wrapper for Nimbus
/// @param[in] i_target the target behind which all memory should be initialized
/// @param[in] i_pattern an index representing a pattern to use to init memory (defaults to 0)
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode nim_sf_init( const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
                               const uint64_t i_pattern );

///
/// @brief Memdiags Super Fast Init command wrapper for Nimbus
/// @param[in] i_target the target behind which all memory should be initialized
/// @param[in] i_pattern an index representing a pattern to use to init memory (defaults to 0)
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode exp_sf_init( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                               const uint64_t i_pattern );

///
/// @brief Memdiags Super Fast Read command wrapper for Nimbus
/// @param[in] i_target the target behind which all memory should be read
/// @param[in] i_stop stop conditions
/// @param[in] i_address mcbist::address representing the address from which to start.
//    Defaults to the first address behind the target
/// @param[in] i_end whether to end, and where
///   Defaults to stop after slave rank
/// @param[in] i_end_address mcbist::address representing the address to end.
//    Defaults to TT::LARGEST_ADDRESS
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode nim_sf_read( const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
                               const mss::mcbist::stop_conditions<mss::mc_type::NIMBUS>& i_stop,
                               const mss::mcbist::address& i_address = mss::mcbist::address(),
                               const mss::mcbist::end_boundary i_end = mss::mcbist::end_boundary::STOP_AFTER_SLAVE_RANK,
                               const mss::mcbist::address& i_end_address = mss::mcbist::address(
                                       mss::mcbistTraits<mss::mc_type::NIMBUS, fapi2::TARGET_TYPE_MCBIST>::LARGEST_ADDRESS) );

///
/// @brief Memdiags Super Fast Read command wrapper for Explorer
/// @param[in] i_target the target behind which all memory should be read
/// @param[in] i_stop stop conditions
/// @param[in] i_address mcbist::address representing the address from which to start.
//    Defaults to the first address behind the target
/// @param[in] i_end whether to end, and where
///   Defaults to stop after slave rank
/// @param[in] i_end_address mcbist::address representing the address to end.
//    Defaults to TT::LARGEST_ADDRESS
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode exp_sf_read( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                               const mss::mcbist::stop_conditions<mss::mc_type::EXPLORER>& i_stop,
                               const mss::mcbist::address& i_address = mss::mcbist::address(),
                               const mss::mcbist::end_boundary i_end = mss::mcbist::end_boundary::STOP_AFTER_SLAVE_RANK,
                               const mss::mcbist::address& i_end_address = mss::mcbist::address(
                                       mss::mcbistTraits<mss::mc_type::EXPLORER, fapi2::TARGET_TYPE_OCMB_CHIP>::LARGEST_ADDRESS) );

///
/// @brief Continuous background scrub command wrapper for Nimbus
/// @param[in] i_target the target behind which all memory should be scrubbed
/// @param[in] i_stop stop conditions
/// @param[in] i_speed the speed to scrub
/// @param[in] i_address mcbist::address representing the address from which to start.
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode nim_background_scrub( const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
                                        const mss::mcbist::stop_conditions<mss::mc_type::NIMBUS>& i_stop,
                                        const mss::mcbist::speed i_speed,
                                        const mss::mcbist::address& i_address );

///
/// @brief Continuous background scrub command wrapper for Explorer
/// @param[in] i_target the target behind which all memory should be scrubbed
/// @param[in] i_stop stop conditions
/// @param[in] i_speed the speed to scrub
/// @param[in] i_address mcbist::address representing the address from which to start.
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode exp_background_scrub( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                        const mss::mcbist::stop_conditions<mss::mc_type::EXPLORER>& i_stop,
                                        const mss::mcbist::speed i_speed,
                                        const mss::mcbist::address& i_address );

///
/// @brief Targeted scrub command wrapper for Nimbus
/// @param[in] i_target the target behind which all memory should be scrubbed
/// @param[in] i_stop stop conditions
/// @param[in] i_speed the speed to scrub
/// @param[in] i_start_address mcbist::address representing the address from which to start.
/// @param[in] i_end_address mcbist::address representing the address at which to end.
/// @param[in] i_end whether to end, and where
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode nim_targeted_scrub( const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
                                      const mss::mcbist::stop_conditions<mss::mc_type::NIMBUS>& i_stop,
                                      const mss::mcbist::address& i_start_address,
                                      const mss::mcbist::address& i_end_address,
                                      const mss::mcbist::end_boundary i_end );

///
/// @brief Targeted scrub command wrapper for Explorer
/// @param[in] i_target the target behind which all memory should be scrubbed
/// @param[in] i_stop stop conditions
/// @param[in] i_speed the speed to scrub
/// @param[in] i_start_address mcbist::address representing the address from which to start.
/// @param[in] i_end_address mcbist::address representing the address at which to end.
/// @param[in] i_end whether to end, and where
/// @return FAPI2_RC_SUCCESS iff everything ok
///
fapi2::ReturnCode exp_targeted_scrub( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                      const mss::mcbist::stop_conditions<mss::mc_type::EXPLORER>& i_stop,
                                      const mss::mcbist::address& i_start_address,
                                      const mss::mcbist::address& i_end_address,
                                      const mss::mcbist::end_boundary i_end );

///
/// @brief Continue current command wrapper for Nimbus
/// @param[in] i_target the target
/// @param[in] i_end whether to end, and where (default - don't stop at end of rank)
/// @param[in] i_stop stop conditions (default - 0 meaning 'don't change conditions')
/// @param[in] i_speed the speed to scrub (default - SAME_SPEED meaning leave speed untouched)
/// @return FAPI2_RC_SUCCESS iff ok
///
fapi2::ReturnCode nim_continue_cmd( const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
                                    const mss::mcbist::end_boundary i_end = mss::mcbist::end_boundary::DONT_CHANGE,
                                    const mss::mcbist::stop_conditions<mss::mc_type::NIMBUS>& i_stop = mss::mcbist::stop_conditions<mss::mc_type::NIMBUS>
                                            (mss::mcbist::stop_conditions<mss::mc_type::NIMBUS>::DONT_CHANGE),
                                    const mss::mcbist::speed i_speed = mss::mcbist::speed::SAME_SPEED );

///
/// @brief Continue current command wrapper for Explorer
/// @param[in] i_target the target
/// @param[in] i_end whether to end, and where (default - don't stop at end of rank)
/// @param[in] i_stop stop conditions (default - 0 meaning 'don't change conditions')
/// @param[in] i_speed the speed to scrub (default - SAME_SPEED meaning leave speed untouched)
/// @return FAPI2_RC_SUCCESS iff ok
///
fapi2::ReturnCode exp_continue_cmd( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                    const mss::mcbist::end_boundary i_end = mss::mcbist::end_boundary::DONT_CHANGE,
                                    const mss::mcbist::stop_conditions<mss::mc_type::EXPLORER>& i_stop =
                                        mss::mcbist::stop_conditions<mss::mc_type::EXPLORER>
                                        (mss::mcbist::stop_conditions<mss::mc_type::EXPLORER>::DONT_CHANGE),
                                    const mss::mcbist::speed i_speed = mss::mcbist::speed::SAME_SPEED );

///
/// @brief Restore DRAM repairs wrapper for Nimbus
/// @param[in] i_target A target representing a port
/// @param[in,out] io_repairs_applied bit mask, where a bit set means a rank had repairs applied (bit0 = rank0, etc)
/// @param[in,out] io_repairs_exceeded bit mask, where a bit set means a DIMM had more bad bits than could be repaired (bit0 = DIMM0 etc)
/// @return FAPI2_RC_SUCCESS iff ok
///
fapi2::ReturnCode nim_restore_repairs( const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                                       fapi2::buffer<uint8_t>& io_repairs_applied,
                                       fapi2::buffer<uint8_t>& io_repairs_exceeded );

///
/// @brief Restore DRAM repairs wrapper for Explorer
/// @param[in] i_target A target representing a port
/// @param[in,out] io_repairs_applied bit mask, where a bit set means a rank had repairs applied (bit0 = rank0, etc)
/// @param[in,out] io_repairs_exceeded bit mask, where a bit set means a DIMM had more bad bits than could be repaired (bit0 = DIMM0 etc)
/// @return FAPI2_RC_SUCCESS iff ok
///
fapi2::ReturnCode exp_restore_repairs( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                       fapi2::buffer<uint8_t>& io_repairs_applied,
                                       fapi2::buffer<uint8_t>& io_repairs_exceeded );

///
/// @brief Broadcast mode check wrapper for Nimbus
/// @param[in] i_target the target to effect
/// @return o_capable - yes iff these vector of targets are broadcast capable
///
const mss::states nim_is_broadcast_capable(const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target);


///
/// @brief Broadcast mode check wrapper for Nimbus
/// @param[in] i_targets the vector of targets to analyze
/// @return o_capable - yes iff these vector of targets are broadcast capable
///
const mss::states nim_is_broadcast_capable(const std::vector<fapi2::Target<fapi2::TARGET_TYPE_MCBIST>>& i_targets);

///
/// @brief Broadcast mode check wrapper for Nimbus
/// @param[in] i_kinds the dimms to effect
/// @return o_capable - yes iff these vector of targets are broadcast capable
///
const mss::states nim_is_broadcast_capable(const std::vector<mss::dimm::kind<>>& i_kinds);

///
/// @brief Broadcast mode check wrapper for Explorer
/// @param[in] i_target the target to effect
/// @return o_capable - yes iff these vector of targets are broadcast capable
///
const mss::states exp_is_broadcast_capable(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target);


///
/// @brief Broadcast mode check wrapper for Explorer
/// @param[in] i_targets the vector of targets to analyze
/// @return o_capable - yes iff these vector of targets are broadcast capable
///
const mss::states exp_is_broadcast_capable(const std::vector<fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>>& i_targets);

///
/// @brief Broadcast mode check wrapper for Explorer
/// @param[in] i_kinds the dimms to effect
/// @return o_capable - yes iff these vector of targets are broadcast capable
///
const mss::states exp_is_broadcast_capable(const std::vector<mss::dimm::kind<>>& i_kinds);

#endif
