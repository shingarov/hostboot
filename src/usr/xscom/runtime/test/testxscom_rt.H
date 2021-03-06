/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/xscom/runtime/test/testxscom_rt.H $                   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2013,2014              */
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
#include <cxxtest/TestSuite.H>
#include <errl/errlmanager.H>
#include <errl/errlentry.H>
#include <devicefw/userif.H>
#include <xscom/xscomreasoncodes.H>

extern trace_desc_t* g_trac_xscom;

using namespace TARGETING;

// Address and data to read/write
struct testXscomAddrData
{
    uint32_t addr;
    uint64_t data;
};

// Test table values
const testXscomAddrData g_xscomAddrTable[] =
{
        // Write data to be ORed with read value
        {0x0006000B, 0x0000040000000000}, // PORE_GPE0_SCRATCH1_0x0006000B
        {0x0006000C, 0xC000000000000000}, // PORE_GPE0_SCRATCH2_0x0006000C
        {0x00040001, 0xE000000000000000}, //TP Chiplet
        // TODO - more tests
};
const uint32_t g_xscomAddrTableSz =
                    sizeof(g_xscomAddrTable)/sizeof(testXscomAddrData);

class XscomTestSuite : public CxxTest::TestSuite
{
    public:
    /**
     * @brief XSCOM test #1
     *        Write value and read back to verify
     */
    void testXscom1(void)
    {

        TARGETING::TargetService& l_targetService = TARGETING::targetService();
        TARGETING::Target* l_testTarget = NULL;
        l_targetService.masterProcChipTargetHandle( l_testTarget );
        assert(l_testTarget != NULL);

        size_t l_size = sizeof(uint64_t);

        // Loop thru table
        errlHndl_t l_err = NULL;
        for( uint32_t l_num=0; l_num < g_xscomAddrTableSz; l_num++)
        {
             testXscomAddrData l_testEntry = g_xscomAddrTable[l_num];

             // Perform XSComOM read
             uint64_t l_readData = 0;
             uint64_t l_writeData = 0;
             uint64_t l_savedData = 0;
             l_err = deviceRead(l_testTarget,
                                &l_readData,
                                l_size,
                                DEVICE_SCOM_ADDRESS(l_testEntry.addr));
             if (l_err)
             {
                 TS_FAIL("testXscom1: XSCom read: deviceRead() fails! Error committed.");
                 break;
             }
             else
             {
                 TS_TRACE("testXscom1: XSCom read, Address 0x%.8X, Data %llx",
                                                      l_testEntry.addr,
                                                      (long long unsigned)l_readData);
             }

             // Perform an XSCom write
             l_savedData = l_readData;
             l_writeData = (l_readData | l_testEntry.data);
             l_err = deviceWrite(l_testTarget,
                     &l_writeData,
                     l_size,
                     DeviceFW::SCOM,
                     l_testEntry.addr);

             if (l_err)
             {
                 TS_FAIL("testXscom1: XSCom write: deviceWrite() fails!");
                 break;
             }
             else
             {
                 TS_TRACE("testXscom1: XSCom write, Address 0x%.8X, Data %llx",
                         l_testEntry.addr,
                         (long long unsigned)l_writeData);
             }

             // Read back
             l_readData = 0;
             l_err = deviceRead(l_testTarget,
                                &l_readData,
                                l_size,
                                DEVICE_SCOM_ADDRESS(l_testEntry.addr));
             if (l_err)
             {
                 TS_FAIL("testXscom1: XSCom read back: deviceRead() fails!");
                 break;
             }

             if( l_readData != l_writeData )
             {
                 TS_FAIL("testXscom1: XSCom read back doesn't match write!");
                 break;
             }

             // Write back original value
             l_err = deviceWrite(l_testTarget,
                      &l_savedData,
                      l_size,
                      DeviceFW::SCOM,
                      l_testEntry.addr);

              if (l_err)
              {
                  TS_FAIL("testXscom1: XSCom write back original fails!");
                  break;
              }
        }

        if (l_err)
        {
            TS_FAIL("testXscom1 failed!  Error committed.");
            errlCommit(l_err,XSCOM_COMP_ID);
        }
        else
        {
            TS_TRACE("testXscom1 runs successfully!");
        }
        return;
  }
};
