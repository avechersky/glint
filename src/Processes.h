//////////////////////////////////////////////////////////////////////
// Processes.h: interface for the CProcesses class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSES_H__BB09A309_6C96_43A7_AD3C_0C7A208D1AA5__INCLUDED_)
#define AFX_PROCESSES_H__BB09A309_6C96_43A7_AD3C_0C7A208D1AA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PerfData.h"
#include "ValueTrack.h"

class CIndicatorSettings;
class CPerfCount;

#define MAX_PROC_SORT_IDX       105     // should be bigger than 100

#define  CPU_PERCENT_TRACK_SCALE   1000000.0
#define  IO_RATE_TRACK_SCALE       10.0
#define  MAX_NUM_TRACK_PROC_SORT   50

// ======================================================================================================
struct CProcessTrackingInfo
{
    CValueTrack     m_ValueTrackCpu;   // Tracking CPU
    CValueTrack     m_ValueTrackIO; // trackin IO operations
    CString         m_ProcName;
    double          m_Value;        // real calculated value
    int             m_Falgs;

    CProcessTrackingInfo() 
        : m_Value(0.0), m_Falgs(0)
        { }
};

#define     TRCKINFO_FLG_CPU_UPDATED   0x00000001   // flag: is CPU history updated on last round
#define     TRCKINFO_FLG_IO_UPDATED    0x00000002   // flag: is IO history  updated on last round

// ======================================================================================================
enum EProcessDataType 
{
    EProcData_Unknown = 0,
    EProcData_CPU,
    EProcData_IO,
    EProcData_MAX,
};

// ======================================================================================================
class CProcesses : public CPerfData 
{
public:
    CProcesses(const wchar_t * wszRequest, LPCSTR sz_Machine);
    virtual ~CProcesses();

    bool    GetData();          // get new CPU usage and new proccesses
    bool    GetInstanceData( PPERF_OBJECT_TYPE& PerfObj, DWORD cntOffset, EProcessDataType type );      // Get Perf. Data for specific instance

    void    UpdateProcessCounter(const wchar_t* instName, PBYTE pdata, EProcessDataType type);   // update individual proc CPU usage data
    CMapStringToOb*     GetProcCounterMapPtr(EProcessDataType type);

    void    GetSortedData();                        // Sort processes according to max CPU usage & delete vanished

    bool    RemoveUnused(EProcessDataType type);    // Delete one vanished process at a time. Returns 'true' if it was a deletion.  
    
    void    UpdateProcessHistory(LPCSTR procNm,  double value, EProcessDataType dataType);
    
    void    UpdateToolTipText();    // Update Tool Tip text from CPU usage history    

    void    DeleteProcessTracking( LPCSTR procName );   //  Delete process with a given name from usage history tracking

// DATA
    CMapStringToOb              m_ProcCpuCnts;
    CMapStringToOb              m_ProcIOCnts;

    AutoPtr<CIndicatorSettings> m_ModelSettingsCpu;
    AutoPtr<CPerfCount>         m_ModelCounterCpu;
    AutoPtr<CIndicatorSettings> m_ModelSettingsIO;
    AutoPtr<CPerfCount>         m_ModelCounterIO;

    CPerfCount* m_SortedProcs[MAX_PROC_SORT_IDX + 2];     // approximately sorted processes refs

    CMapStringToOb              m_ProcessHist;          // history of CPU usage by processes

protected:
    void    ClearUsage();   // clear counters usage flags  

    int     m_MaxValueTrackCount;    // max available CPU history in ValueTrack. 
    int     m_ValueTrackCount;       // current count of available CPU history samples in ValueTrack. 
};

#endif // !defined(AFX_PROCESSES_H__BB09A309_6C96_43A7_AD3C_0C7A208D1AA5__INCLUDED_)
