// PerfData.h: interface for the CPerfData class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERFDATA_H__AB082182_8EB3_4D07_A606_7E603EF21D94__INCLUDED_)
#define AFX_PERFDATA_H__AB082182_8EB3_4D07_A606_7E603EF21D94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PERF_DATA_TOTALBYTES    1024*192
#define PERF_DATA_BYTEINCREMENT 1024*64
#define MAX_OBJECT_NAME         256
#define MAX_REG_REQUEST_SIZE    500     // size of obj list for request 

class CIndicatorSettings;

////////////////////////////////////////////////////////////////////////////
//  Perf Counters Titles and Helps Data 
////////////////////////////////////////////////////////////////////////////
class CPerfLibTextData  
{
    wchar_t*        m_lpNameStrings;
    wchar_t*        m_lpHelpStrings;
    wchar_t**       m_lpNamesArray;

    DWORD           m_dwMaxValueLen;    
    DWORD           m_MaxCounterId;     // max id as it was provided in registry
    DWORD           m_MaxObjectIdx;     // max object id as it was found in database

public:
    CPerfLibTextData(LPCSTR sz_Machine);
    virtual ~CPerfLibTextData();

    DWORD  MaxCounterId()   { return m_MaxCounterId; }
    DWORD  MaxObjectIdx()   { return m_MaxObjectIdx; }


    wchar_t*        GetText(DWORD objId);
    wchar_t*        GetHelp(DWORD objId);   // actuall GetText + 1

};

////////////////////////////////////////////////////////////////////////////
//  Perf Counters Data input buffer
////////////////////////////////////////////////////////////////////////////
class CPerfData  
{
public:
    CPerfData(LPCSTR sz_Machine);
    CPerfData(const wchar_t * wszRequest, LPCSTR sz_Machine);
    virtual ~CPerfData();

    void    SetRequestWsz(const wchar_t * wszRequest);
    DWORD   Read(); // read data from system. Returns data length.          
            // request is: "Global" , "Costly" or numbers

    bool IsDataReady()      // returns true, if data is ready to calculate
        { return m_OldDataBlk.Signature[0] != 0 && m_PerfData != 0 ; }

    bool    FindInstanceId( DWORD objId, DWORD counterId, 
                            const wchar_t * inst_name,
                            long* res_inst_id); // resulting Id
    LONG    GetInstancesCount(DWORD objId);

    // Find Counter instance data area
    PBYTE FindInstanceData(DWORD objId, DWORD counterId, const wchar_t * inst_name, LONG inst_Id);
    // If ID != 0 and found, then we check for 'inst_name'.

    PPERF_DATA_BLOCK PerfDataBlk() { return m_PerfData; };

public:     // DATA
    PERF_DATA_BLOCK m_OldDataBlk;
    double          m_Freq;     // current frequence converted

protected:

    void InitPerfData(const wchar_t * wszRequest, LPCSTR sz_Machine);
    
    PPERF_OBJECT_TYPE  FirstObject( PPERF_DATA_BLOCK PerfData ) {
        return( (PPERF_OBJECT_TYPE)((PBYTE)PerfData + PerfData->HeaderLength) );
    }

    PPERF_OBJECT_TYPE NextObject( PPERF_OBJECT_TYPE PerfObj )  {
        return( (PPERF_OBJECT_TYPE)((PBYTE)PerfObj + PerfObj->TotalByteLength) );
    }

    PPERF_INSTANCE_DEFINITION FirstInstance( PPERF_OBJECT_TYPE PerfObj ) {
        return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfObj + PerfObj->DefinitionLength) );
    }

    PPERF_INSTANCE_DEFINITION NextInstance(PPERF_INSTANCE_DEFINITION PerfInst ) {
        PPERF_COUNTER_BLOCK PerfCntrBlk;
        PerfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + PerfInst->ByteLength);
        return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfCntrBlk + PerfCntrBlk->ByteLength) );
    }

    PPERF_COUNTER_DEFINITION FirstCounter( PPERF_OBJECT_TYPE PerfObj ) {
        return( (PPERF_COUNTER_DEFINITION) ((PBYTE)PerfObj + PerfObj->HeaderLength) );
    }

    PPERF_COUNTER_DEFINITION NextCounter(PPERF_COUNTER_DEFINITION PerfCntr ) {
        return( (PPERF_COUNTER_DEFINITION)((PBYTE)PerfCntr + PerfCntr->ByteLength) );
    }

protected:
    HKEY             m_PerfHKey;
    PPERF_DATA_BLOCK m_PerfData;
    DWORD            m_AllocBufferSize;
    DWORD            m_BufferSize;          // real amount of data stored
    wchar_t          m_RegRequest[MAX_REG_REQUEST_SIZE + 2]; // request to registry: L"Global", L"Costly", L"num1 num2 ..." 

    DWORD            m_FoundInstanceId;     // output value from FindInstanceData() func
    CString          m_MachineName;
    int              m_ObjectsCount;        // total number of objects 

};


////////////////////////////////////////////////////////////////////////////
//  Perf Data buffer with Title database
////////////////////////////////////////////////////////////////////////////
class CPerfDataBase :  public CPerfData 
{
public:
    
    CPerfDataBase(const wchar_t * wszRequest, LPCSTR machine_name);
    virtual ~CPerfDataBase();

    int  GetObjectsList(int modes);     // counts objects and fills it's list
        #define FILL_OBJECT_LIST    0x0001
        #define GET_OBJECTS_COUNT   0x0002
        #define FILL_COUNTERS_TREE  0x0004

    void FillInstancesList(CListBox *list, DWORD objId);
    int  FillCountersTree(CTreeCtrl* tree, LPCTSTR computerName, int group_number, int modes); // return real objects count
    bool IsObjectUsable(wchar_t* wobj_name);

    DWORD  MaxObjectIdx()   { return m_Titles.MaxObjectIdx(); }
    DWORD  MaxCounterId()   { return m_Titles.MaxCounterId(); }

    /////////////////////
    //   Find IDs 
    ////////////////////
    DWORD   FindObjectId(const char * obj_name);                
    PPERF_OBJECT_TYPE           FindObject(const wchar_t * obj_name);

    PPERF_COUNTER_DEFINITION    FindCounter(PPERF_OBJECT_TYPE obj, const wchar_t * counter_name);
    PPERF_COUNTER_DEFINITION    FindCounter(const char * obj_name, const char * counter_name);
    CString FindDefaultInstance(DWORD objId, DWORD counterId);

    BOOL  GetHelpText(DWORD objId, char* buf, DWORD buflen);    // fill the help buffer

protected:
    CPerfLibTextData    m_Titles;

    AutoPtr<CWordArray> m_ObjsGroups;   // array of counters group number for each object.
    WORD                m_Objects;      // total # of objects.
};

////////////////////////////////////////////////////////////

DWORD GetCounterId(CPerfDataBase* pfcDB, CIndicatorSettings& indset);
HKEY  GetMachineKey(LPCSTR machine_name, HKEY std_key);

#endif // !defined(AFX_PERFDATA_H__AB082182_8EB3_4D07_A606_7E603EF21D94__INCLUDED_)
