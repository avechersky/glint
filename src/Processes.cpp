// Processes.cpp: implementation of the CProcesses class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "Processes.h"
#include "PerfCount.h"
#include "Panel.h"
#include "ProcessWindow.h"
#include "IndicatorSettings.h"
#include "Settings.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static int s_updateTTip = 0;
static CProcessTrackingInfo*   s_SortTrackArr[MAX_NUM_TRACK_PROC_SORT + 2];
char  g_ProcessesToolTipTitle[200] = {" Recent Activity "};                                  //  "Processes Activity: "
char  g_ProcessesToolTipText[MAX_PROC_TOOLTIP_TEXT + 100] = {" ( not yet collected ) "};          // " ( not yet collected ) "
 
// ================================================================================================================
const char* GetProcessDataTypeStr(EProcessDataType dataType)
{
    switch (dataType)
    {
    case EProcData_CPU: return "CPU";
    case EProcData_IO: return "IO";
    }
    return "???";
}

// ================================================================================================================
CProcesses::CProcesses(const wchar_t * wszRequest, LPCSTR sz_Machine)
    : CPerfData(wszRequest, sz_Machine)
{
    FUNCTIONLOG(7, "CProcesses::constr");
    m_MaxValueTrackCount = sets.m_ProcessTrackingTime / (sets.m_ProcessTimeInterval / 1000.0 ) ;
    if ( m_MaxValueTrackCount > MAX_TRACK_VALUES )
        m_MaxValueTrackCount = MAX_TRACK_VALUES;

    strcpy( g_ProcessesToolTipTitle, " Recent Activity ");
    strcpy( g_ProcessesToolTipText, " ( not yet collected ) " );
    //TRACE( "=== Processes created ==== m_MaxValueTrackCount: %d \r\n", m_MaxValueTrackCount);
    m_ValueTrackCount = 0;
}

// ================================================================================================================
CProcesses::~CProcesses()
{
    FUNCTIONLOG(7, "CProcesses::destr");
    // Delete current counters
    CString     key;
    CPerfCount* pcnt;
    for(POSITION pos = m_ProcCpuCnts.GetStartPosition(); pos != NULL; )
    {
        m_ProcCpuCnts.GetNextAssoc( pos, key, (CObject*&)pcnt );
        delete pcnt;
    }
    for(POSITION pos = m_ProcIOCnts.GetStartPosition(); pos != NULL; )
    {
        m_ProcIOCnts.GetNextAssoc( pos, key, (CObject*&)pcnt );
        delete pcnt;
    }

    CString         procName;
    CProcessTrackingInfo*   trackInfo = 0;
    for(POSITION pos = m_ProcessHist.GetStartPosition(); pos != NULL; )
    {
        m_ProcessHist.GetNextAssoc( pos, procName, (CObject*&)trackInfo );
        ASSERT(trackInfo != 0);
        delete trackInfo;
    }

}

// ================================================================================================================
void CProcesses::ClearUsage()   // clear counters usage flags  
{
    CString     key;
    CPerfCount* pcnt;

    for(POSITION pos = m_ProcCpuCnts.GetStartPosition(); pos != NULL; )
    {
        m_ProcCpuCnts.GetNextAssoc( pos, key, (CObject*&)pcnt );
        FLAG_CLEAR(pcnt->m_flags, PFCFLG_INSTANCE_ALIVE);
    }

    for(POSITION pos = m_ProcIOCnts.GetStartPosition(); pos != NULL; )
    {
        m_ProcIOCnts.GetNextAssoc( pos, key, (CObject*&)pcnt );
        FLAG_CLEAR(pcnt->m_flags, PFCFLG_INSTANCE_ALIVE);
    }
}   


// ================================================================================================================
bool CProcesses::GetData()          // get new CPU usage and new proccesses
{
    bool    retc = false;
    if (! IsDataReady() )
        return false;

    FUNCTIONLOG(7, "CProcesses::GetData()");
    ClearUsage();

    PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );
    PPERF_COUNTER_DEFINITION PerfCntr = 0;
    DWORD cntOffset = 0;    // offset of counter in instance data

    // Iterate through objects
    for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
              i++, PerfObj=NextObject(PerfObj) )
    {
        if ( PerfObj->ObjectNameTitleIndex != m_ModelCounterCpu->m_ObjectId )
            continue;

        if ( PerfObj->NumInstances < 1 ) 
            return false;

        // Iterating through counters to find data offset
        PerfCntr = FirstCounter( PerfObj );
        for(DWORD j=0; PerfCntr && j < PerfObj->NumCounters; 
                  j++, PerfCntr=NextCounter(PerfCntr) )
        {
            cntOffset = PerfCntr->CounterOffset;
            if ( PerfCntr->CounterNameTitleIndex == m_ModelCounterCpu->m_CounterId )   
            {
                retc = GetInstanceData(PerfObj, cntOffset, EProcData_CPU);
            }
            else if ( PerfCntr->CounterNameTitleIndex == m_ModelCounterIO->m_CounterId )   
            {

                retc = GetInstanceData(PerfObj, cntOffset, EProcData_IO);
            }

        }
        return retc;           
    } // end for objects

    return retc;
}


// ================================================================================================================
bool    CProcesses::GetInstanceData( PPERF_OBJECT_TYPE& PerfObj, DWORD cntOffset, EProcessDataType type )
{
    bool    retc = false;
    // Iterate through instances
    PPERF_COUNTER_BLOCK PtrToCntr = 0;  // ptr to Counter(s) Data Area
    PPERF_INSTANCE_DEFINITION  PerfInst = FirstInstance( PerfObj );
    PBYTE   pdata;          // addr od data field

    for(int k=0; PerfInst && k < (DWORD)PerfObj->NumInstances; 
                    k++,PerfInst=NextInstance(PerfInst) ) 
    {
        wchar_t* instName = (wchar_t *)((PBYTE)PerfInst + PerfInst->NameOffset);
        if ( wcsicmp(instName, L"Idle" ) == 0 || wcsicmp(instName, L"_Total" ) == 0 ) // always skeep these two
            continue;

        #ifdef PRELIMINARY_EXCLUDE_NAMES
        if ( wcscmp(instName, L"System" ) == 0 ||
             wcsicmp(instName, L"csrss" ) == 0 ||
             wcsicmp(instName, L"glint" ) == 0 )
            continue;
        if ( // (g_OSVersion <= OS_VERSION_2000 ) &&    // Win2K only
            ( wcscmp(instName, L"svchost" ) == 0 ||
             wcsicmp(instName, L"services" ) == 0 ||
             wcscmp(instName, L"cidaemon" ) == 0  ) 
           )
            continue;
        if ( ( g_OSVersion == OS_VERSION_XP ) &&    // WinXP only
             ( wcscmp(instName, L"mdm" ) == 0  )    
           )
            continue;
        #endif //PRELIMINARY_EXCLUDE_NAMES

        PtrToCntr = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + PerfInst->ByteLength);
        pdata = ((PBYTE)PtrToCntr + cntOffset);

        //-------------------------------           
        UpdateProcessCounter(instName, pdata, type);
        //-------------------------------    
        retc = true;
    }

    return retc;
}

// ================================================================================================================
CMapStringToOb*     CProcesses::GetProcCounterMapPtr(EProcessDataType type)
{
    if ( type == EProcData_CPU )
    {
        return &m_ProcCpuCnts;
    }
    else if ( type == EProcData_IO )
    {
        return &m_ProcIOCnts;
    }
    return 0;
}

// ================================================================================================================
//      Update individual proc CPU usage data
// ================================================================================================================
void CProcesses::UpdateProcessCounter(const wchar_t* instName, PBYTE pdata, EProcessDataType type)
{
    CString strName = ConvertFromUni(instName);
    // TRACE("[glint] Process: '%s' \n", (LPCSTR)strName );
    if ( !sets.m_ShowSystemTasksActivity && !CheckSystemProcessName(strName) )
        return;

    CPerfCount* pcnt = 0;
    CPerfCount*         pcntModel = 0;
    CIndicatorSettings* settingsModel = 0;
    
    CMapStringToOb* procCnts = GetProcCounterMapPtr(type);
    if ( procCnts == 0 )
        return;

    if ( type == EProcData_CPU )
    {
        pcntModel = m_ModelCounterCpu;
        settingsModel = m_ModelSettingsCpu;
    }
    else if ( type == EProcData_IO )
    {
        pcntModel = m_ModelCounterIO;
        settingsModel = m_ModelSettingsIO;
    }
     
    while ( pcnt == 0) 
    {
        if ( !procCnts->Lookup( strName, ( CObject*& ) pcnt) )
        {
            // -------------------------
            //     Add new instance
            // -------------------------
            pcnt = new CPerfCount(pcntModel->m_strCategory, pcntModel->m_strCounter, (LPCSTR)strName, settingsModel);

            pcnt->m_CounterType = pcntModel->m_CounterType; 
            pcnt->m_CounterSize = pcntModel->m_CounterSize; 
            pcnt->m_BaseSize = pcntModel->m_BaseSize; 
            pcnt->m_maxVal   = pcntModel->m_maxVal; 
            pcnt->SetActive(true); 
            pcnt->m_cntDB = this;

            procCnts->SetAt(strName, (CObject*)pcnt);
            OUTLOG(9, "<+++>  Added process counter for: '%s'", (LPCSTR)strName);
            //TRACE("   +++ Added process '%s'. \n", pcnt->m_strInst);
        }
        else 
        {
            // -------------------------
            //     Find another instance
            // -------------------------
            if ( FLAG_CHECK(pcnt->m_flags, PFCFLG_INSTANCE_ALIVE))
            {
                // Duplicate instance name. 
                // TRACE("[glint] Duplicate process '%s'. \n", pcnt->m_strInst);
                strName += " ";     // add space for next process with the same name                                
                pcnt = 0;
            }
        }
    }

    pcnt->MakeValue(pdata);
    FLAG_SET(pcnt->m_flags, PFCFLG_INSTANCE_ALIVE);
}


// ================================================================================================================
bool CProcesses::RemoveUnused(EProcessDataType type) // delete vanished processes  
{
    CMapStringToOb* procCnts = GetProcCounterMapPtr(type);
    if ( procCnts == 0 )
        return false;
    
    bool  retc = false;
    CString     key;
    CPerfCount* pcnt;
    for(POSITION pos = procCnts->GetStartPosition(); pos != NULL; )
    {
        procCnts->GetNextAssoc( pos, key, (CObject*&)pcnt );
        if ( !FLAG_CHECK(pcnt->m_flags, PFCFLG_INSTANCE_ALIVE) )
        {
            retc = true;
            delete pcnt;
            break;
        }
    }

    if ( retc )
    {
        //TRACE("  ---- Deleting Process '%s' \n", key);
        OUTLOG(9, "<--->  Deleting process counter for: '%s'", (LPCSTR)key);
        if ( !procCnts->RemoveKey(key) ) 
        {
            retc = false;
            OUTLOG(1, "*** Process counter: '%s'  not deleted ***", (LPCSTR)key);
            TRACE("***** Can not delete process: '%s' !\n", key);
        }
    }

    return retc;
}   

// ================================================================================================================
bool    CheckSystemProcessName(LPCSTR instName )
{
    //  ======== Skip system rocesses from show ========
    if ( strcmp(instName, "Idle" ) == 0 || 
         strcmp(instName, "System" ) == 0 ||
         strcmp(instName, "_Total" ) == 0 ||
         stricmp(instName, "csrss" ) == 0 ||
         stricmp(instName, "lsass" ) == 0 ||
         stricmp(instName, "glint" ) == 0 )
        return false;
    if ( // (g_OSVersion <= OS_VERSION_2000 ) &&    // Win2K only
        ( strcmp(instName, "svchost" ) == 0 ||
          stricmp(instName, "services" ) == 0 ||
          strcmp(instName, "cidaemon" ) == 0  ) 
       )
        return false;
    if ( ( g_OSVersion == OS_VERSION_XP ) &&    // WinXP only
         ( strcmp(instName, "mdm" ) == 0  )    
       )
        return false;

    return true;
}


// ================================================================================================================
void CProcesses::GetSortedData()        // Sort processes according to max CPU usage & delete vanished
{
    FUNCTIONLOG(7, "CProcesses::GetSortedData()");
    RemoveUnused(EProcData_CPU);
    RemoveUnused(EProcData_IO);
    // TRACE("[glint]  Processes Counters: %d \n", procCnts->GetCount() );

    memset(m_SortedProcs, 0, sizeof(m_SortedProcs) );

    // Cleaning Trackinfo update flag.
    CString         procName;
    CProcessTrackingInfo*   trackInfo = 0;
    for(POSITION pos = m_ProcessHist.GetStartPosition(); pos != NULL; )
    {
        m_ProcessHist.GetNextAssoc( pos, procName, (CObject*&)trackInfo );
        ASSERT(trackInfo != 0);
        FLAG_CLEAR(trackInfo->m_Falgs, TRCKINFO_FLG_CPU_UPDATED | TRCKINFO_FLG_IO_UPDATED );
    }


    // --------------------------------------
    //      Handle processes CPU usage 
    // --------------------------------------
    CPerfCount* pcnt;
    for(POSITION pos = m_ProcCpuCnts.GetStartPosition(); pos != NULL; )
    {
        m_ProcCpuCnts.GetNextAssoc( pos, procName, (CObject*&)pcnt );
        if( !FLAG_CHECK(pcnt->m_flags, PFCFLG_INSTANCE_ALIVE) )
            continue;

        double fuse = pcnt->GetAbsoluteValue() / 100.0 ;

        //TRACE("    CPU   Process '%s'  v: %lf \n", (LPCSTR)procName, fuse );
        if ( fuse < 0.0 || fuse > 100.1 )
        {
            CString str;
            str.Format("***** Wrong process '%s'  CPU per cent value is calculated : %g ****  Zero value used.", (LPCSTR)procName, fuse );
            DATA_ERR_MESSAGE_BOX(str, MB_OK | MB_ICONSTOP);
            fuse = 0.0;
            continue;
        }


#define CPU_THRESHOLD       0.000001
#define IO_RATE_THRESHOLD   100.0
#define CPU_THRESHOLD_LOG   (-log10(CPU_THRESHOLD))                                         // (equals 6 for 0.000001)
#define CPU_THRESHOLD_LOG2  (CPU_THRESHOLD_LOG * CPU_THRESHOLD_LOG )                        // square
#define CPU_THRESHOLD_LOG3  (CPU_THRESHOLD_LOG * CPU_THRESHOLD_LOG * CPU_THRESHOLD_LOG)     // cube (equals 216 for 0.000001)

        if ( fuse < CPU_THRESHOLD ) 
            continue;

        procName.Trim();
        bool isSystemProcess = !CheckSystemProcessName(pcnt->m_strInst); 
        if ( !isSystemProcess || sets.m_ShowSystemTasksActivity )  
        {
            UpdateProcessHistory(procName,  fuse, EProcData_CPU);
        }

        if ( isSystemProcess ) 
            continue;

        //
        //  Translate "% CPU Time" value to index in m_SortedProcs array.
        //  If slot is already occupied put element ro next non-zero.
        //
        double vLog = ( log10(fuse) + CPU_THRESHOLD_LOG );     // 0.0 - CPU_THRESHOLD_LOG
        double v1 = (vLog * vLog * vLog / CPU_THRESHOLD_LOG3 ) * 100.0;     // 0.0 - 100.0
        //fuse = v1 * v1 * / CPU_THRESHOLD_LOG2 ) * 100.0;         // 0.0 - 100.0
        int proc = (v1 + 0.499999);                   // 0 - 100
        

        ASSERT(proc >= 0);
        if ( proc < 0 ) proc = 0;       // should not be so!
        if (proc > 100)  proc = 100;

        // === direct search for the free place to place our data ===
        bool isSlotFound = false;

        while( proc < MAX_PROC_SORT_IDX )
        {
            if ( m_SortedProcs[proc] == 0 ) 
            {
                m_SortedProcs[proc] = pcnt;
                isSlotFound = true;
                break;
            }
            proc++;
        }

        // === backward search for the free place to place our data ===
        if ( !isSlotFound )
        {
            proc = MAX_PROC_SORT_IDX - 1;
            while( proc >= 0 )
            {
                if ( m_SortedProcs[proc] == 0 ) 
                {
                    m_SortedProcs[proc] = pcnt;
                    isSlotFound = true;
                    break;
                }
                proc--;
            }
        }

        //TRACE("    CPU   Process '%s'  v: %lf   vLog: %lf   idx:%d  \n", (LPCSTR)procName, fuse, vLog, (isSlotFound ? proc : -1) );
    }

    // --------------------------------------
    //      Handle processes IO 
    // --------------------------------------

    for(POSITION pos = m_ProcIOCnts.GetStartPosition(); pos != NULL; )
    {
        m_ProcIOCnts.GetNextAssoc( pos, procName, (CObject*&)pcnt );
        if( !FLAG_CHECK(pcnt->m_flags, PFCFLG_INSTANCE_ALIVE) )
            continue;


        double fuse = pcnt->GetAbsoluteValue();
        //TRACE("    I/O   Process '%s'   v: %lf \n", (LPCSTR)procName, fuse );
        if ( fuse < IO_RATE_THRESHOLD ) 
            continue;
        
        procName.Trim();
        bool isSystemProcess = !CheckSystemProcessName(pcnt->m_strInst); 
        if ( !isSystemProcess || sets.m_ShowSystemTasksActivity )  
        {
            UpdateProcessHistory(procName,  fuse, EProcData_IO);
        }
    }

     UpdateToolTipText();
}

// ================================================================================================================
void CProcesses::UpdateProcessHistory(LPCSTR procNm,  double value, EProcessDataType dataType)
{
    OUTLOG(9, "UpdateProcessHistory[%s] for '%s'  v:%g  ", GetProcessDataTypeStr(dataType), (LPCSTR)procNm, value );
    CProcessTrackingInfo*   trackInfo = 0;
    if ( !m_ProcessHist.Lookup( procNm, ( CObject*& ) trackInfo) )
    {

        // Add new instance
        trackInfo = new CProcessTrackingInfo;
        trackInfo->m_ValueTrackCpu.SetTrackSize( m_MaxValueTrackCount );
        trackInfo->m_ValueTrackIO.SetTrackSize( m_MaxValueTrackCount );
        trackInfo->m_ProcName = procNm;
        trackInfo->m_Falgs = 0;
        m_ProcessHist.SetAt(procNm, (CObject*)trackInfo);
        //TRACE("  <+++>  Added values track for process '%s'. \n", (LPCSTR)process );

        OUTLOG(9, "+++ Added process tracking[%s] for: '%s'  v:%g", GetProcessDataTypeStr(dataType), (LPCSTR)procNm, value);
    }

    if ( dataType == EProcData_CPU )    // value is from 0.0 to 1.0
    {
        //ASSERT( value <= 1.0 );
        TValueTrack percent = (TValueTrack)(value * CPU_PERCENT_TRACK_SCALE ) / g_NumberCPUs;   // get total percentage instead of one CPU
        if ( !FLAG_CHECK(trackInfo->m_Falgs, TRCKINFO_FLG_CPU_UPDATED ) )
        {
            trackInfo->m_ValueTrackCpu.PutValue(percent);
            FLAG_SET(trackInfo->m_Falgs, TRCKINFO_FLG_CPU_UPDATED );
        }
        else
        {
            // Add value to the process with the same name.
            trackInfo->m_ValueTrackCpu.AddToLastValue(percent);
        }
    }
    else if ( dataType == EProcData_IO )
    {
        //TRACE(" ____IO_____ UpdateProcessHistory %f \r\n", value );
        // Calculate # of bytes transfered.
        TValueTrack  scaledIObytes = (TValueTrack)( value / IO_RATE_TRACK_SCALE ) * ( sets.m_ProcessTimeInterval / 1000.0 );
        if ( !FLAG_CHECK(trackInfo->m_Falgs, TRCKINFO_FLG_IO_UPDATED ) )
        {
            trackInfo->m_ValueTrackIO.PutValue( scaledIObytes );
            FLAG_SET(trackInfo->m_Falgs, TRCKINFO_FLG_IO_UPDATED );
        }
        else
        {
            // Add value to the process with the same name.
            trackInfo->m_ValueTrackIO.AddToLastValue(scaledIObytes);
        }
    }
}


// =========================================================================================
//      Sorting function (IN REVERSE ORDER)
// =========================================================================================
int CompareProcessTrackingInfoRecs(const void * rec1, const void * rec2)
{
    CProcessTrackingInfo* r1 = *(CProcessTrackingInfo**)rec1;
    CProcessTrackingInfo* r2 = *(CProcessTrackingInfo**)rec2;
    
    if ( r1->m_Value > r2->m_Value )
        return -1;
    else if ( r1->m_Value < r2->m_Value )
        return 1;
    return 0;
}

// ================================================================================================================
//      Update Tool Tip text from CPU usage history
// ================================================================================================================
void CProcesses::UpdateToolTipText()
{
    FUNCTIONLOG(7, "CProcesses::UpdateToolTipText()");
    int ksort = 0; // # of sort records
    int k = 0;
    CString  procNameToDelete;

    // increase count of process activity updates.
    ++m_ValueTrackCount;
    if ( m_ValueTrackCount > m_MaxValueTrackCount ) 
        m_ValueTrackCount = m_MaxValueTrackCount;

    // ---------------------------------------------------------------------
    //     Fill Sort array and put zero values to inactive processes.
    // ---------------------------------------------------------------------
    CString         procName;
    CProcessTrackingInfo*   trackInfo = 0;
    for(POSITION pos = m_ProcessHist.GetStartPosition(); pos != NULL; )
    {
        m_ProcessHist.GetNextAssoc( pos, procName, (CObject*&)trackInfo );
        ASSERT(trackInfo != 0);

        if ( procName.GetLength() == 0 || trackInfo == 0 )
        {
            DATA_ERR_MESSAGE_BOX("***** UpdateToolTipText ERROR  (PprocName.GetLength() == 0 || trackInfo == 0) ****");
            continue;
        }

        if ( !FLAG_CHECK(trackInfo->m_Falgs, TRCKINFO_FLG_CPU_UPDATED) ) 
        {
            trackInfo->m_ValueTrackCpu.PutValue(0);       // put 0, if not provided
        }
        if ( !FLAG_CHECK(trackInfo->m_Falgs, TRCKINFO_FLG_IO_UPDATED) ) 
        {
            trackInfo->m_ValueTrackIO.PutValue(0);       // put 0, if not provided
        }

        // -----------------------------------------------------------
        //      Calculate sorting key
        // -----------------------------------------------------------
        trackInfo->m_Value = 0;
        if ( trackInfo->m_ValueTrackCpu.GetSum() / (double)m_ValueTrackCount > 0.5  ) // check if GT 0.0001%
        {
            double  cpuValueAdd = (double)trackInfo->m_ValueTrackCpu.GetSum() * 100.0 / CPU_PERCENT_TRACK_SCALE;
            trackInfo->m_Value += cpuValueAdd ;   // %CPU/sec
            OUTLOG(20,"  (+) Process:'%s'  Adding CPU value %g to sort", (LPCSTR)procName, cpuValueAdd); 
        }

        if ( trackInfo->m_ValueTrackIO.GetSum()  > (100.0 / IO_RATE_TRACK_SCALE ) )    // check if GT 0.1K 
        {
            double  ioValueAdd = (double)trackInfo->m_ValueTrackIO.GetSum() * IO_RATE_TRACK_SCALE / (10.0*1000.0*1000.0);
            trackInfo->m_Value += ioValueAdd;   // 10Mb I/O ==eq 1% CPU
            OUTLOG(20,"  (+) Process:'%s'  Adding I/O value %g to sort", (LPCSTR)procName, ioValueAdd); 
        }

        if ( trackInfo->m_Value < EPSILON )
        {
            OUTLOG(20,"  (*) Process:'%s'  m_Value:%g  SumCPU:%llu SumCPU:%llu  too small and ready for delete", (LPCSTR)procName, trackInfo->m_Value, 
                             trackInfo->m_ValueTrackCpu.GetSum(), trackInfo->m_ValueTrackIO.GetSum() ); 

            if ( m_ValueTrackCount == m_MaxValueTrackCount && procNameToDelete.GetLength() == 0 )
            {
                procNameToDelete = procName;    // remember delete candidate
            }
            continue;
        }

        if ( ksort < MAX_NUM_TRACK_PROC_SORT )
        {
            s_SortTrackArr[ksort] = trackInfo;
            ++ksort;
        }

    }


    // -------------------------------
    //      Sort by Sum value.
    // -------------------------------
    if ( ksort > 0 && m_ValueTrackCount > 0 )
    {
        qsort(s_SortTrackArr, ksort, sizeof(void*), CompareProcessTrackingInfoRecs );
    }

    // ----------------------------------
    //    Output sorted CPU usage list
    // ---------------------------------
    int kout = 0;
    for (int k = 0; k < ksort; ++k)
    {
        trackInfo = s_SortTrackArr[k];
        double cpuValue = ((double)trackInfo->m_ValueTrackCpu.GetSum() * 100.0 / (CPU_PERCENT_TRACK_SCALE * m_ValueTrackCount) );   // per cent
        double ioValue  = (double)trackInfo->m_ValueTrackIO.GetSum() * IO_RATE_TRACK_SCALE;                                         // bytes

        if ( cpuValue < 0.001 && ioValue < 99.9 )   // ignore too small values: 0.0001% and 0.5K b/sec
            continue;

        if ( k > 0 ) 
            kout += sprintf(&g_ProcessesToolTipText[kout], "\r\n"); 

        if ( kout > (MAX_PROC_TOOLTIP_TEXT - 100) )
        {
            kout += sprintf( &g_ProcessesToolTipText[kout], " .... "); 
            break;
        }

        char sCPUvalue[20];
        const char* svalue_format = "%.4f"; 
        if ( cpuValue  > 0.99 )         svalue_format = "%.1f";
        else if ( cpuValue > 0.099 )    svalue_format = "%.2f";
        else if ( cpuValue > 0.0099 )   svalue_format = "%.3f";
        else if ( cpuValue > 0.00099 )  svalue_format = "%.4f";
        else if ( cpuValue < 0.0001 )   svalue_format = "0.0001";
        sprintf( sCPUvalue, svalue_format, cpuValue );

        kout += sprintf( &g_ProcessesToolTipText[kout], "%-17.17s %8s%%", (LPCSTR)trackInfo->m_ProcName, sCPUvalue );
        
        if ( ioValue > 99.9 )
        {
            //TRACE(" ____IO_____ %d \r\n", trackInfo->m_ValueTrackIO.GetSum() );
            kout += sprintf( &g_ProcessesToolTipText[kout], "  %7sb", GetSmallValueStr(ioValue, false /*small integers */, " ") );
        }
    }
    g_ProcessesToolTipText[kout] = 0;
    
    if ( kout == 0 )
    {
        strcpy( g_ProcessesToolTipText, " < no activity > ");
    }

    if ( (s_updateTTip++) % 2 == 0 )
    {
        int k_secs = (sets.m_ProcessTimeInterval / 1000.0) * m_ValueTrackCount ;
        if ( m_ValueTrackCount == m_MaxValueTrackCount )    k_secs = sets.m_ProcessTrackingTime;    // make output a little bit more logical
        sprintf(g_ProcessesToolTipTitle, "Recent %d secs activity (CPU, I/O)", k_secs);
        if ( theApp.m_panel && theApp.m_panel->m_ProcessesWin && theApp.m_panel->m_ProcessesWin->m_hWnd )
        {
            theApp.m_panel->m_ProcessesWin->UpdateToolTip();
        }
    }

    if ( procNameToDelete.GetLength() > 0 )
    {
        DeleteProcessTracking(procNameToDelete);
    }

    //OUTLOG(20, "[Processes-Tool-Tip]  kout:%d  ValueTrackCount:%d  Proc to delete: '%s' ", kout, m_ValueTrackCount, (LPCSTR)procNameToDelete);
}


// ================================================================================================================
//      Delete process with a given name from usage history tracking
// ================================================================================================================
void    CProcesses::DeleteProcessTracking( LPCSTR procName )
{

    CProcessTrackingInfo*   trackInfo = 0;
    if ( !m_ProcessHist.Lookup( procName, ( CObject*& ) trackInfo) )
        return;

    delete trackInfo;

    OUTLOG(9, " -- Removing Process History for '%s'", procName );
    if ( !m_ProcessHist.RemoveKey(procName) )   // remove delete candidate
    {
        OUTLOG(1, "*** Process tracking: '%s'  not deleted ***", procName);
    }
}