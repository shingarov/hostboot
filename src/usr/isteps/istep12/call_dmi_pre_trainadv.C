/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/istep12/call_dmi_pre_trainadv.C $              */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2020                        */
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
#include    <stdint.h>

#include    <trace/interface.H>
#include    <initservice/taskargs.H>
#include    <initservice/isteps_trace.H>
#include    <errl/errlentry.H>
#include    <errl/errludtarget.H>
#include    <isteps/hwpisteperror.H>
#include    <util/misc.H>
#include    <util/utilmbox_scratch.H>

//  targeting support.
#include    <targeting/common/commontargeting.H>
#include    <targeting/common/utilFilter.H>

//Fapi Support
#include    <config.h>
#include    <fapi2.H>
#include    <fapi2/plat_hwp_invoker.H>


//HWP
#include    <p9_io_dmi_pre_trainadv.H>
#ifdef CONFIG_AXONE
#include    <exp_omi_setup.H>
#include    <p9a_omi_setup.H>
#include    <p9a_omi_train.H>
#endif

using   namespace   ISTEP;
using   namespace   ISTEP_ERROR;
using   namespace   ERRORLOG;
using   namespace   TARGETING;


namespace ISTEP_12
{
void* call_dmi_pre_trainadv (void *io_pArgs)
{
    IStepError l_StepError;
    errlHndl_t l_err = NULL;

    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace, "call_dmi_pre_trainadv entry" );

    TARGETING::TargetHandleList l_dmiTargetList;
    getAllChiplets(l_dmiTargetList, TYPE_DMI);

    TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace, "call_dmi_pre_trainadv: %d DMIs found",
            l_dmiTargetList.size());

    for (const auto & l_dmi_target : l_dmiTargetList)
    {
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
            "p9_io_dmi_pre_trainadv HWP target HUID %.8x",
            TARGETING::get_huid(l_dmi_target));

        //get the membuf associated with this DMI.
        TARGETING::TargetHandleList l_pChildMembufList;
        getChildAffinityTargetsByState(l_pChildMembufList,
                                       l_dmi_target,
                                       CLASS_CHIP,
                                       TYPE_MEMBUF,
                                       UTIL_FILTER_PRESENT);
        // call the HWP p9_io_dmi_pre_trainadv only if membuf connected.
        //we can't expect more than one membufs connected to a DMI
        if (l_pChildMembufList.size() == 1)
        {
            //  call the HWP with each DMI target
            fapi2::Target<fapi2::TARGET_TYPE_DMI> l_fapi_dmi_target
                (l_dmi_target);

            fapi2::Target<fapi2::TARGET_TYPE_MEMBUF_CHIP> l_fapi_membuf_target
                (l_pChildMembufList[0]);

            FAPI_INVOKE_HWP(l_err, p9_io_dmi_pre_trainadv, l_fapi_dmi_target, l_fapi_membuf_target );

            //  process return code.
            if ( l_err )
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "ERROR 0x%.8X:  p9_io_dmi_pre_trainadv HWP on target HUID %.8x",
                    l_err->reasonCode(), TARGETING::get_huid(l_dmi_target) );

                // capture the target data in the elog
                ErrlUserDetailsTarget(l_dmi_target).addToLog( l_err );

                // Create IStep error log and cross reference to error that occurred
                l_StepError.addErrorDetails( l_err );

                // Commit Error
                errlCommit( l_err, ISTEP_COMP_ID );
            }
            else
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                         "SUCCESS :  p9_io_dmi_pre_trainadv HWP on target 0x%.08X", TARGETING::get_huid(l_dmi_target));
            }
        }
        else    //No associated membuf
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                      "p9_io_dmi_pre_trainadv HWP skipped, HUID 0x%.08X has no associated membufs"
                      ,TARGETING::get_huid(l_dmi_target));
        }

    }

#ifdef CONFIG_AXONE

        TARGETING::TargetHandleList l_ocmbTargetList;
        getAllChips(l_ocmbTargetList, TYPE_OCMB_CHIP);

        for (const auto & l_ocmb_target : l_ocmbTargetList)
        {
            //  call the HWP with each target
            fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> l_fapi_ocmb_target
                (l_ocmb_target);


            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                "exp_omi_setup HWP target HUID 0x%.08x",
                TARGETING::get_huid(l_ocmb_target));

            FAPI_INVOKE_HWP(l_err, exp_omi_setup, l_fapi_ocmb_target);

            //  process return code.
            if ( l_err )
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "ERROR 0x%.8X:  exp_omi_setup HWP on target HUID 0x%.08x",
                    l_err->reasonCode(), TARGETING::get_huid(l_ocmb_target) );

                // capture the target data in the elog
                ErrlUserDetailsTarget(l_ocmb_target).addToLog( l_err );

                // Create IStep error log and cross reference to error that occurred
                l_StepError.addErrorDetails( l_err );

                // Commit Error , continue on to next OCMB
                errlCommit( l_err, ISTEP_COMP_ID );
            }
        }

        TARGETING::TargetHandleList l_omiTargetList;
        getAllChiplets(l_omiTargetList, TYPE_OMI);

        TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace, "call_dmi_pre_trainadv: %d OMIs found",
                l_omiTargetList.size());

        for (const auto & l_omi_target : l_omiTargetList)
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                "p9a_omi_setup HWP target HUID %.8x",
                TARGETING::get_huid(l_omi_target));

            //  call the HWP with each OMI target
            fapi2::Target<fapi2::TARGET_TYPE_OMI> l_fapi_omi_target(l_omi_target);

            FAPI_INVOKE_HWP(l_err, p9a_omi_setup , l_fapi_omi_target );

            //  process return code.
            if ( l_err )
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "ERROR 0x%.8X:  p9a_omi_setup HWP on target HUID %.8x",
                    l_err->reasonCode(), TARGETING::get_huid(l_omi_target) );

                // capture the target data in the elog
                ErrlUserDetailsTarget(l_omi_target).addToLog( l_err );

                // Create IStep error log and cross reference to error that occurred
                l_StepError.addErrorDetails( l_err );

                // Commit Error
                errlCommit( l_err, ISTEP_COMP_ID );
            }
            else
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                          "SUCCESS :  p9a_omi_setup HWP on 0x%.08X", TARGETING::get_huid(l_omi_target));
            }
        }

#endif

    // Set ATTR_ATTN_CHK_OCMBS to let ATTN know that we may now get attentions
    // from the OCMB, but interrupts from the OCMB are not enabled yet.
    TargetHandle_t sys = nullptr;
    targetService().getTopLevelTarget( sys );
    assert( sys != nullptr );
    sys->setAttr<ATTR_ATTN_CHK_OCMBS>(1);

    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace, "call_dmi_pre_trainadv exit" );
    // end task, returning any errorlogs to IStepDisp
    return l_StepError.getErrorHandle();
}

};
