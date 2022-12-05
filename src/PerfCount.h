// PerfCount.h: interface for the CPerfCount class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERFCOUNT_H__820BD200_7A3F_4375_89B5_675A8D186523__INCLUDED_)
#define AFX_PERFCOUNT_H__820BD200_7A3F_4375_89B5_675A8D186523__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_INST_NAME   128     // maximum size of instance name

// ------------------------ Flags determined by Counter iteself ---------------------------------------
#define PFCFLG_INSTANCE_ALIVE   0x1         // counter is alive (for processes)
#define PFCFLG_PREV_READ        0x2         // previous value is ready
#define PFCFLG_MAX_SMART        0x100       // maximum depends on MO (local variations)
#define PFCFLG_MAX_UNDEFINED    0x200       // absolute maximum value not defined and can vary

class CIndicatorSettings;
class CPerfData;

// -----------------------------------------------------------------------------------
struct CounterSample {
    DWORD   m_RawVal;
    DWORD   m_BaseVal;
    };


// -----------------------------------------------------------------------------------
//   Queue for values
// -----------------------------------------------------------------------------------
#define NUM_LAST_VALUES    8  // number of last values to store

class   CStoredValues {
public:
    CStoredValues();

    void    Put(double v);      // put value
    bool    IsLess(double v);   // check if all values less thank value
    void    Reset();

private:
    int       m_lastIdx;
    double    m_data[NUM_LAST_VALUES + 1];
};


// -----------------------------------------------------------------------------------
//      CPerfCount
// -----------------------------------------------------------------------------------
class CPerfCount  
{
public:
    CPerfCount();
    CPerfCount(LPCSTR  szCategory, LPCSTR  szCounter, LPCSTR  szInst, CIndicatorSettings* pIndSets);
    virtual ~CPerfCount();

    void        MakeValue();        // obtain next value 
    void        MakeValue(PBYTE data);

    // ------  Calculating Values --------  
    double      GetAbsoluteValue(); 
    double      GetRelativeValue(); // get value from 0 to 1.0 relative to max
    double      GetRealCounterValue();
    double      GetCurrentMaxValue();
    double      GetPreviousAbsValue();

    bool        IsActive() { return m_isActive; }
    void        SetActive(bool val) { m_isActive = val; }
    
    bool        IsCounterTypeSupported(DWORD t);        // analyze counter type

    void        AdjustMax();            //    Adjust maximum 
    DWORD       SampleCount() { return m_samples; }     // processed sample number
        
protected:
    void        InitPerfCount(); 
    void        Calculate();

    bool        m_isActive;         // false: if PerfCount can not be used
    double      m_val;              // current value
    double      m_PreviousValue;    // previous value

public:
    CPerfData*  m_cntDB;            // current Perf counters data base 
    CString     m_strCategory;      // Object Name
    CString     m_strCounter;
    CString     m_strInst;          // Counter instance name

    CIndicatorSettings* m_IndSets;
    DWORD       m_flags;

    DWORD       m_ObjectId;
    DWORD       m_CounterId;
    LONG        m_InstId;       // Can be used for search instead of name: -1 : absent.
    wchar_t     m_InstWsz [MAX_INST_NAME + 1];

    DWORD       m_CounterType;
    DWORD       m_CounterSize;
    DWORD       m_BaseType;
    DWORD       m_BaseSize;

    LARGE_INTEGER   m_RawData;          // current raw value    (LowPart, HighPart, QuadPart)
    LARGE_INTEGER   m_LastRawData;      // last raw value

    LARGE_INTEGER   m_BaseData;         // current raw value    (LowPart, HighPart, QuadPart)
    LARGE_INTEGER   m_LastBaseData;     // last raw value

    double      m_maxVal;           // maximum value as it ccured for calculating relative value
    double      m_maxValUsed;       // maximum value used for calculating relative value
    double      m_MO;               // mean value occured

private:
    DWORD       m_samples;
    DWORD       m_RecentSamples;    // samples # for local MO calc
    CStoredValues   m_LastValues;
};

#endif // !defined(AFX_PERFCOUNT_H__820BD200_7A3F_4375_89B5_675A8D186523__INCLUDED_)
