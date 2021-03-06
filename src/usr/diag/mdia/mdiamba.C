/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/mdia/mdiamba.C $                                 */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2020                        */
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
 * @file mdiamba.C
 * @brief mdia mba specific functions
 */

#include "mdiafwd.H"
#include "mdiaglobals.H"
#include "mdiasm.H"
#include "mdiatrace.H"
#include "targeting/common/utilFilter.H"

using namespace TARGETING;

namespace MDIA
{

bool __nimbusDD1Workaround( TargetHandle_t i_trgt )
{
    // In Nimbus DD1.x, 1-rank DIMMs must use a slow read as opposed to a super
    // fast read, as such, this causes mdia to run too slowly to complete nine
    // patterns before the istep times out. In this case, we will just do four
    // patterns instead. This function will return true if we need to apply the
    // Nimbus DD1 1-rank DIMM workaround or false if we don't.

    bool slowRead = false;

    TARGETING::Target* masterProc = nullptr;
    TARGETING::targetService().masterProcChipTargetHandle(masterProc);

    // The workaround only applies to Nimbus
    if ( MODEL_NIMBUS == masterProc->getAttr<ATTR_MODEL>() )
    {
        ConstTargetHandle_t parent = getParentChip( i_trgt );

        // check if DD1.x
        if ( 0x20 > parent->getAttr<ATTR_EC>() )
        {
            // check if there is a 1-rank DIMM
            TargetHandleList mcsList;
            getChildAffinityTargets( mcsList, i_trgt, CLASS_UNIT, TYPE_MCS );

            for ( auto &mcs : mcsList )
            {
                ATTR_EFF_NUM_RANKS_PER_DIMM_type attr;
                if ( !mcs->tryGetAttr<ATTR_EFF_NUM_RANKS_PER_DIMM>(attr) )
                {
                    MDIA_FAST( "tryGetAttr<ATTR_EFF_NUM_RANKS_PER_DIMM> failed:"
                               " i_trgt=0x%08x", get_huid(i_trgt) );
                    break;
                }
                for ( uint8_t pos = 0; pos < mss::PORTS_PER_MCS; pos++ )
                {
                    for ( uint8_t ds = 0; ds < mss::MAX_DIMM_PER_PORT; ds++ )
                    {
                        // If there is a DIMM with only 1 rank we must apply the
                        // workaround.
                        if ( 1 == attr[pos][ds] )
                        {
                            slowRead = true;
                            break;
                        }
                    }
                    if ( slowRead ) break;
                }
                if ( slowRead ) break;
            }
        }
    }

    return slowRead;
}

errlHndl_t getDiagnosticMode(
        const Globals & i_globals,
        TargetHandle_t i_trgt,
        DiagMode & o_mode)
{
    o_mode = ONE_PATTERN;

    do
    {

        if(MNFG_FLAG_ENABLE_EXHAUSTIVE_PATTERN_TEST
           & i_globals.mfgPolicy)
        {
            o_mode = NINE_PATTERNS;
        }

        else if(MNFG_FLAG_ENABLE_STANDARD_PATTERN_TEST
                & i_globals.mfgPolicy)
        {
            o_mode = FOUR_PATTERNS;
        }

        else if(MNFG_FLAG_ENABLE_MINIMUM_PATTERN_TEST
                & i_globals.mfgPolicy)
        {
            o_mode = ONE_PATTERN;
        }

        else if(i_globals.simicsRunning)
        {
            o_mode = ONE_PATTERN;
        }

        // Only need to check hw changed state attributes
        // when not already set to exhaustive and not in simics
        if( ( NINE_PATTERNS != o_mode ) &&
            ( FOUR_PATTERNS != o_mode ) &&
            ( ! i_globals.simicsRunning ) )
        {
            if(isHWStateChanged(i_trgt))
            {
                // To reduce IPL times without broadcast mode, we will just run
                // 4 patterns instead of 9.
                o_mode = FOUR_PATTERNS;
            }
        }

        // Check to see if we must apply the Nimbus DD1 1-rank DIMM workaround
        if ( (NINE_PATTERNS == o_mode) && __nimbusDD1Workaround(i_trgt) )
        {
            o_mode = FOUR_PATTERNS;
        }

    } while(0);

    MDIA_FAST("getDiagnosticMode: trgt: %x, o_mode: 0x%x, "
              "simics: %d",
              get_huid(i_trgt), o_mode, i_globals.simicsRunning);

    return 0;
}

errlHndl_t getWorkFlow(
                DiagMode i_mode,
                WorkFlow & o_wf,
                const Globals & i_globals)
{
    // add the correct sequences for the mba based
    // on the mode

    // every mba does restore dram repairs

    o_wf.push_back(RESTORE_DRAM_REPAIRS);

    switch (i_mode)
    {
        case NINE_PATTERNS:

            o_wf.push_back(START_PATTERN_7);
            o_wf.push_back(START_SCRUB);
            o_wf.push_back(START_PATTERN_6);
            o_wf.push_back(START_SCRUB);
            o_wf.push_back(START_PATTERN_5);
            o_wf.push_back(START_SCRUB);
            o_wf.push_back(START_PATTERN_4);
            o_wf.push_back(START_SCRUB);
            o_wf.push_back(START_PATTERN_3);
            o_wf.push_back(START_SCRUB);

            // fall through

        case FOUR_PATTERNS:

            o_wf.push_back(START_RANDOM_PATTERN);
            o_wf.push_back(START_SCRUB);
            o_wf.push_back(START_PATTERN_2);
            o_wf.push_back(START_SCRUB);
            o_wf.push_back(START_PATTERN_1);
            o_wf.push_back(START_SCRUB);

            // fall through

        case ONE_PATTERN:

            o_wf.push_back(START_PATTERN_0); // 0's pattern, must be last
            o_wf.push_back(START_SCRUB);

            break;

        default:
            break;
    }

    if(MNFG_FLAG_IPL_MEMORY_CE_CHECKING
            & i_globals.mfgPolicy)
    {
        o_wf.push_back(ANALYZE_IPL_MNFG_CE_STATS);
    }

    o_wf.push_back(CLEAR_HW_CHANGED_STATE);

    return 0;
}

/*
 *  Local helper function to return a list of DIMMs, MCS, and
 *  MCA/Centaur associated with the input MBA/MCBIST target
 *
 *  If i_queryOnly = true (Query) or MCBIST case
 *    - Return a list of DIMMs, Centaur/MCA, and
 *      MCS connected to this MBA/MCBIST
 *
 *  Else (Clear) - Centaur case only
 *   - Return a list of DIMMs and
 *     (Centaur + MCS) if all the DIMMs behind this
 *     Centaur have hwchangedState flags cleared
 *     or about to be cleared by this MBA
 */
TargetHandleList getMemTargetsForQueryOrClear(
                    TargetHandle_t i_trgt, bool i_queryOnly)
{
    #define FUNC "getMemTargetsForQueryOrClear: "
    TargetHandleList o_list;

    do
    {
        // add associated DIMMs
        TargetHandleList dimmList;
        getChildAffinityTargets(dimmList,
                                i_trgt,
                                CLASS_NA,
                                TYPE_DIMM);

        if( ! dimmList.empty() )
        {
            o_list.insert(o_list.begin(), dimmList.begin(), dimmList.end());
        }

        TYPE trgtType = i_trgt->getAttr<ATTR_TYPE>();

        // mcbist target
        if ( TYPE_MCBIST == trgtType )
        {
            // add associated MCBIST
            o_list.push_back( i_trgt );

            // add associated MCSs
            TargetHandleList mcsList;
            getChildAffinityTargets( mcsList, i_trgt, CLASS_UNIT, TYPE_MCS );
            if ( !mcsList.empty() )
                o_list.insert( o_list.end(), mcsList.begin(), mcsList.end() );

            // add associated MCAs
            TargetHandleList mcaList;
            getChildAffinityTargets( mcaList, i_trgt, CLASS_UNIT, TYPE_MCA );
            if ( !mcaList.empty() )
                o_list.insert( o_list.end(), mcaList.begin(), mcaList.end() );

        }
        // MBA target
        else if ( TYPE_MBA == trgtType )
        {
            // add associated Centaur
            TargetHandleList targetList;
            getParentAffinityTargets(targetList,
                    i_trgt,
                    CLASS_CHIP,
                    TYPE_MEMBUF);

            if( targetList.empty() )
            {
                MDIA_FAST(FUNC "no connected centaur "
                        "for mba: %x", get_huid(i_trgt));
                break;
            }

            TargetHandle_t centaur = targetList[0];

            // if query flag is not set, check to make sure
            // all of the dimms connected to this centaur
            // have cleared hw chagned state attributes
            // before adding this centaur/mcs to the list.
            // This is needed because we only clear
            // the centaur/mcs attribute when all of the
            // dimms' attributes from both mbas have cleared.
            if(false == i_queryOnly)
            {
                targetList.clear();
                getChildAffinityTargets(targetList,
                        centaur,
                        CLASS_NA,
                        TYPE_DIMM);

                if( ! targetList.empty() )
                {
                    TargetHandleList::iterator target;

                    for(target = targetList.begin();
                            target != targetList.end(); ++target)
                    {
                        // exclude dimms belong to the current mba
                        // because their attributes will be cleared
                        if(dimmList.end() !=
                                std::find(dimmList.begin(),
                                    dimmList.end(), *target))
                        {
                            continue;
                        }

                        ATTR_HWAS_STATE_CHANGED_FLAG_type hwChangeFlag;
                        hwChangeFlag =
                            (*target)->getAttr<ATTR_HWAS_STATE_CHANGED_FLAG>();

                        if(HWAS_CHANGED_BIT_MEMDIAG & hwChangeFlag)
                        {
                            MDIA_FAST(FUNC "hwChangedState is not cleared "
                                    "for dimm: %x", get_huid(*target));
                            centaur = NULL; // don't add centaur and mcs and mba
                            break;
                        }
                    }
                }
            }

            if(NULL == centaur)
            {
                break;
            }

            o_list.push_back(centaur);

            // get connected dmi target
            TargetHandleList dmiList;
            getParentAffinityTargets(dmiList,
                                     centaur,
                                     CLASS_UNIT,
                                     TYPE_DMI);

            if( !dmiList.empty() )
            {
                o_list.push_back(dmiList[0]);
            }

            // add associated MI
            TargetHandleList miList;
            getParentAffinityTargets( miList, dmiList[0], CLASS_UNIT, TYPE_MI );
            if ( miList.size() == 1 )
            {
                o_list.push_back( miList[0] );
            }
            else
            {
                MDIA_FAST( FUNC "Could not find parent MI." );
                break;
            }

            // add associated MC
            TargetHandleList mcList;
            getParentAffinityTargets( mcList, miList[0], CLASS_UNIT, TYPE_MC );
            if ( mcList.size() == 1 )
            {
                o_list.push_back( mcList[0] );
            }
            else
            {
                MDIA_FAST( FUNC "Could not find parent MC." );
                break;
            }

            // add associated MBAs
            targetList.clear();

            getChildAffinityTargets(targetList, i_trgt, CLASS_UNIT, TYPE_MBA);

            if ( !targetList.empty() )
            {
                o_list.insert( o_list.end(), targetList.begin(),
                               targetList.end() );
            }

        }
        // OCMB target
        else if ( TYPE_OCMB_CHIP == trgtType )
        {
            // add associated OCMB
            o_list.push_back( i_trgt );

            // add associated OMI
            TargetHandleList omiList;
            getParentAffinityTargets( omiList, i_trgt, CLASS_UNIT, TYPE_OMI );
            if ( omiList.size() == 1 )
            {
                o_list.push_back( omiList[0] );
            }
            else
            {
                MDIA_FAST( FUNC "Could not find parent OMI." );
                break;
            }

            // add associated OMIC
            TargetHandleList omicList;
            getParentOmicTargetsByState( omicList, omiList[0], CLASS_NA,
                                         TYPE_OMIC, UTIL_FILTER_FUNCTIONAL );
            if ( omicList.size() == 1 )
            {
                o_list.push_back( omicList[0] );
            }
            else
            {
                MDIA_FAST( FUNC "Could not find parent OMIC." );
                break;
            }

            // add associated MCC
            TargetHandleList mccList;
            getParentAffinityTargets( mccList, omiList[0], CLASS_UNIT,
                                      TYPE_MCC );
            if ( mccList.size() == 1 )
            {
                o_list.push_back( mccList[0] );
            }
            else
            {
                MDIA_FAST( FUNC "Could not find parent MCC." );
                break;
            }

            // add associated MI
            TargetHandleList miList;
            getParentAffinityTargets( miList, mccList[0], CLASS_UNIT, TYPE_MI );
            if ( miList.size() == 1 )
            {
                o_list.push_back( miList[0] );
            }
            else
            {
                MDIA_FAST( FUNC "Could not find parent MI." );
                break;
            }

            // add associated MC
            TargetHandleList mcList;
            getParentAffinityTargets( mcList, miList[0], CLASS_UNIT, TYPE_MC );
            if ( mcList.size() == 1 )
            {
                o_list.push_back( mcList[0] );
            }
            else
            {
                MDIA_FAST( FUNC "Could not find parent MC." );
                break;
            }

        }
        else
        {
            assert( false, "getMemTargetsForQueryOrClear: Invalid target "
                    "type from i_trgt: %x", get_huid(i_trgt) );
            break;
        }

    } while(0);

    MDIA_DBG(FUNC "i_trgt HUID: %x, size: %d",
             get_huid(i_trgt), o_list.size());

    return o_list;

    #undef FUNC
}


bool isHWStateChanged(TargetHandle_t i_trgt)
{
    bool hwChanged = false;
    ATTR_HWAS_STATE_CHANGED_FLAG_type hwChangeFlag;

    // Get a list of associated targets for attribute query
    TargetHandleList targetList =
        getMemTargetsForQueryOrClear(i_trgt, true);

    for(TargetHandleList::iterator target = targetList.begin();
        target != targetList.end(); ++target )
    {
        hwChangeFlag =
            (*target)->getAttr<ATTR_HWAS_STATE_CHANGED_FLAG>();

        if(HWAS_CHANGED_BIT_MEMDIAG & hwChangeFlag)
        {
            MDIA_DBG("isHWStateChanged: set for target: %x",
                     get_huid(*target));
            hwChanged = true;
            break;
        }
    }

    return hwChanged;
}

void clearHWStateChanged(TargetHandle_t i_trgt)
{
    TargetHandleList targetList;

    // Get a list of associated targets for attribute clearing
    targetList = getMemTargetsForQueryOrClear(i_trgt, false);

    for(TargetHandleList::iterator target = targetList.begin();
        target != targetList.end(); ++target)
    {
        MDIA_DBG("clearHWStateChanged: mba: %x, target: %x",
                 get_huid(i_trgt), get_huid(*target));

        clear_hwas_changed_bit( *target,
                                HWAS_CHANGED_BIT_MEMDIAG);
    }
}


}
