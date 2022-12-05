//////////////////////////////////////////////////////////////////////
//      PerfData.cpp: implementation of the CPerfData class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "PerfData.h"
#include "PerfCount.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HRESULT hr;
char    last_win32_error_txt[1000];   // buffer containing last Win32 error

//////////////////////////////////////////////////////////////////////
//      Class CPerfLibTextData
//////////////////////////////////////////////////////////////////////


CPerfLibTextData::CPerfLibTextData(LPCSTR sz_Machine)
{
    HKEY hKeyPerflib;      // handle to registry key
    HKEY hKeyPerflib009;   // handle to registry key
    DWORD dwBuffer = 0;    // bytes to allocate for buffers
    DWORD dwBufferSize;    // size of dwBuffer
    wchar_t* lpCurrentString; // pointer for enumerating data strings
    DWORD dwCounter;       // current counter index

    m_lpNamesArray = 0;
    m_lpNameStrings = 0;
    m_lpHelpStrings = 0;
    m_MaxObjectIdx = 0;
    m_MaxCounterId = 0;

    HKEY machine_key = GetMachineKey(sz_Machine, HKEY_LOCAL_MACHINE);

    // Get the number of Counter items.
    hr = RegOpenKeyExW( machine_key,
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib",
        0,
        KEY_READ,
        &hKeyPerflib);
    CheckHr("RegOpenKeyEx");

    dwBufferSize = sizeof(dwBuffer);
    hr = RegQueryValueExW( hKeyPerflib,
                            L"Last Counter",
                            NULL,
                            NULL,
                            (LPBYTE) &dwBuffer,
                            &dwBufferSize );
    CheckHr("RegQueryValueEx");

    hr = RegCloseKey( hKeyPerflib );
    CheckHr("RegCloseKey");

    m_MaxCounterId = dwBuffer + 1;
    // Allocate memory for the names array.
    m_lpNamesArray = (wchar_t**) calloc( (m_MaxCounterId + 1) + 10, sizeof(wchar_t*));

    // Open the key containing the counter and object names.
    hr = RegOpenKeyExW( machine_key,
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\009",
        0,
        KEY_READ,
        &hKeyPerflib009);
    CheckHr("RegOpenKeyEx");

    // Get the size of the largest value in the key (Counter or Help).
    hr = RegQueryInfoKeyW( hKeyPerflib009,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &m_dwMaxValueLen,
                            NULL,
                            NULL);
    CheckHr("RegQueryInfoKey");

    // Allocate memory for the counter and object names.
    dwBuffer = m_dwMaxValueLen + 1;
    m_lpNameStrings = (wchar_t*) calloc( dwBuffer * sizeof(wchar_t) + 10, sizeof(wchar_t));
    m_lpHelpStrings = (wchar_t*) calloc( dwBuffer * sizeof(wchar_t) + 10, sizeof(wchar_t));
    ASSERT(m_lpNameStrings && m_lpHelpStrings);
    if ( m_lpNameStrings == 0 || m_lpHelpStrings == 0 )
        return;

    ////////////////////////////
    // Read the Counter value.
    ////////////////////////////
    hr = RegQueryValueExW( hKeyPerflib009,  
                        L"Counter", NULL, NULL,
                        (LPBYTE)m_lpNameStrings, 
                        &dwBuffer );
    CheckHr("RegQueryValueEx");

    // Load names into an array, by index.
    for(    lpCurrentString = m_lpNameStrings; 
            *lpCurrentString;
            lpCurrentString += (wcslen(lpCurrentString)+1) ) {

        dwCounter = _wtoi( lpCurrentString );
        ASSERT( dwCounter != 0 && dwCounter <= m_MaxCounterId );
        if( dwCounter > m_MaxCounterId )
            break;  // something wrong
        m_MaxObjectIdx = (dwCounter > m_MaxObjectIdx) ? dwCounter : m_MaxObjectIdx;
        lpCurrentString += wcslen(lpCurrentString) + 1 ;
        m_lpNamesArray[dwCounter] = (wchar_t*) lpCurrentString;
    }

    ////////////////////////////
    // Read the Help value.
    ////////////////////////////
    dwBuffer = m_dwMaxValueLen + 1;
    hr = RegQueryValueExW( hKeyPerflib009,  
                        L"Help", NULL, NULL,
                        (LPBYTE)m_lpHelpStrings, 
                        &dwBuffer );
    CheckHr("RegQueryValueEx");

    // Load names into an array, by index.
    for(    lpCurrentString = m_lpHelpStrings; 
            *lpCurrentString;
            lpCurrentString += (wcslen(lpCurrentString)+1) ) {

        dwCounter = _wtoi( lpCurrentString );
        ASSERT( dwCounter != 0 && dwCounter <= m_MaxCounterId );
        if( dwCounter > m_MaxCounterId )
            break;  // something wrong
        m_MaxObjectIdx = (dwCounter > m_MaxObjectIdx) ? dwCounter : m_MaxObjectIdx;
        lpCurrentString += wcslen(lpCurrentString) + 1 ;
        m_lpNamesArray[dwCounter] = (wchar_t*) lpCurrentString;
    }

    hr = RegCloseKey( hKeyPerflib009 );
    CheckHr("RegCloseKey");
    RegCloseKey(machine_key);
}

/////////////////////////////////////////////////////////////////////////
CPerfLibTextData::~CPerfLibTextData()
{
    if (m_lpNamesArray) free(m_lpNamesArray);
    if (m_lpNameStrings) free(m_lpNameStrings);
    if (m_lpHelpStrings) free(m_lpHelpStrings);
}

/////////////////////////////////////////////////////////////////////////
wchar_t*    CPerfLibTextData::GetText(DWORD objId)
{
    if ( objId > m_MaxObjectIdx || m_lpNamesArray == 0 )
        return 0;

    return m_lpNamesArray[objId];
}

/////////////////////////////////////////////////////////////////////////
wchar_t*    CPerfLibTextData::GetHelp(DWORD objId) 
{   
    if ( objId == 0 )
        return L"";
    return GetText(objId + 1);
}


//////////////////////////////////////////////////////////////////////
// Class CPerfData
//////////////////////////////////////////////////////////////////////

CPerfData::CPerfData(LPCSTR sz_Machine)
{
    InitPerfData(0, sz_Machine);
}

//////////////////////////////////////////////////////////////////////
CPerfData::CPerfData(const wchar_t * wszRequest, LPCSTR sz_Machine)
{
    InitPerfData(wszRequest, sz_Machine);
}

//////////////////////////////////////////////////////////////////////
CPerfData::~CPerfData()
{
    RegCloseKey(m_PerfHKey); 
    if ( m_PerfData )
        delete(m_PerfData);
}

//////////////////////////////////////////////////////////////////////
void CPerfData::InitPerfData(const wchar_t * wszRequest, LPCSTR sz_Machine)
{
    m_RegRequest[0] = 0;
    m_BufferSize = 0;
    m_AllocBufferSize = 0;
    m_PerfData = NULL;
    memset(&m_OldDataBlk, 0, sizeof(m_OldDataBlk));
    m_MachineName = ( sz_Machine) ? sz_Machine : "";

    if ( wszRequest )
        SetRequestWsz(wszRequest);
    m_PerfHKey = GetMachineKey(m_MachineName, HKEY_PERFORMANCE_DATA);

    // hr = RegOpenKeyEx(HKEY_PERFORMANCE_DATA, 0, 0, KEY_QUERY_VALUE, &m_PerfHKey);
    // CheckHr("RegOpenKey(HKEY_PERFORMANCE_DATA)");
}

//////////////////////////////////////////////////////////////////////

void CPerfData::SetRequestWsz(const wchar_t * wszRequest)
{
    memcpy( m_RegRequest, wszRequest, MAX_REG_REQUEST_SIZE);
    m_RegRequest[MAX_REG_REQUEST_SIZE] = 0;
    TRACE( "  CPerfData::SetRequestWsz(): '%ls' \n", m_RegRequest );
}

//////////////////////////////////////////////////////////////////////
DWORD   CPerfData::Read()               
{
    if ( m_RegRequest[0] == 0 )             // no request supplied yet
        return 0;

    // Allocate 1-st potion of buffer, if not yet.
    if ( m_PerfData == NULL ) 
    {
        m_AllocBufferSize = PERF_DATA_TOTALBYTES;
        m_PerfData = (PPERF_DATA_BLOCK) malloc( m_AllocBufferSize );
    }
    else 
    {
        memcpy(&m_OldDataBlk, m_PerfData, sizeof(m_OldDataBlk));    // save old Data_Block
    }
    if ( m_PerfData == NULL)
        return 0;

    m_BufferSize = m_AllocBufferSize;
    while( (hr = RegQueryValueExW( m_PerfHKey,
                                   m_RegRequest,            // L"Global", L"Costly", L"num1 num2 ..." 
                                   NULL,
                                   NULL,
                                   (LPBYTE) m_PerfData,
                                   &m_BufferSize ) )
           != ERROR_SUCCESS )
    {
        if ( hr != ERROR_MORE_DATA) 
        {
            DebugErrorPrint(RegQueryValueExError);
            free( m_PerfData );
            m_PerfData = NULL;
            return 0;
        }

        // Get a buffer that is big enough.
        m_AllocBufferSize += PERF_DATA_BYTEINCREMENT;
        m_BufferSize = m_AllocBufferSize;
        m_PerfData = (PPERF_DATA_BLOCK) realloc( m_PerfData, m_AllocBufferSize );
    }

    m_Freq = LToD(m_PerfData->PerfFreq);
    return m_BufferSize;
}


//////////////////////////////////////////////////////////////////////
//  Returns 'false' if instance not found
bool CPerfData::FindInstanceId(DWORD objId, DWORD counterId, const wchar_t * inst_name, long* res_inst_id)
{
    if (!inst_name)
        return 0;

    PBYTE pdata = FindInstanceData(objId, counterId, inst_name, 0);
    *res_inst_id = m_FoundInstanceId;
    return (pdata != 0);
}

//////////////////////////////////////////////////////////////////////
// If ID != 0 and found, then we check for 'inst_name'.
PBYTE CPerfData::FindInstanceData(DWORD objId, DWORD counterId, const wchar_t * inst_name, LONG inst_Id)
{
    PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );
    PPERF_COUNTER_DEFINITION PerfCntr = 0;
    PPERF_INSTANCE_DEFINITION PerfInst;
    PPERF_COUNTER_BLOCK PtrToCntr = 0;  // ptr to Counter(s) Data Area

    m_FoundInstanceId = 0;

    for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
              i++, PerfObj=NextObject(PerfObj) )
    {
        if ( PerfObj->ObjectNameTitleIndex != objId )
            continue;
        PerfCntr = FirstCounter( PerfObj );

        if ( PerfObj->NumInstances > 0 ) 
        {
            // Get the first instance.
            PerfInst = FirstInstance( PerfObj );
            for(int k=0; PerfInst && k < (DWORD)PerfObj->NumInstances; 
                            k++,PerfInst=NextInstance(PerfInst) ) 
            {
                wchar_t* instName = (wchar_t *)((PBYTE)PerfInst + PerfInst->NameOffset);

                if ( ( inst_Id == 0 && inst_name == 0 )                     // no search => get first instance
                     || ( inst_Id == 0 && wcsicmp(inst_name,instName) == 0 ) 
                     || ( PerfInst->UniqueID == inst_Id && ( inst_name == 0 || wcsicmp(inst_name,instName) == 0 ) ) // search by ID
                   )
                {
                    m_FoundInstanceId = PerfInst->UniqueID;
                    PtrToCntr = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + PerfInst->ByteLength);
                    goto CounterSearch;         
                }

            }
            return 0;
        }
        else 
        {
            PtrToCntr = (PPERF_COUNTER_BLOCK)((PBYTE)PerfObj + PerfObj->DefinitionLength);
            goto CounterSearch;
        }
    }
    return 0;


CounterSearch:

    for(DWORD j=0; PerfCntr && j < PerfObj->NumCounters; 
              j++, PerfCntr=NextCounter(PerfCntr) )
    {
        if ( PerfCntr->CounterNameTitleIndex == counterId ) 
        {
            return (PBYTE)((PBYTE)PtrToCntr + PerfCntr->CounterOffset);
        }
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////
//  Get # of instances in object
LONG    CPerfData::GetInstancesCount(DWORD objId)
{
    try 
    {
        PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );
        for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
                  i++, PerfObj=NextObject(PerfObj) )
        {
            if ( PerfObj->ObjectNameTitleIndex != objId )
                continue;

            return PerfObj->NumInstances; 
        }
    }
    catch (...) {
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////
// Class CPerfDataBase
//////////////////////////////////////////////////////////////////////

CPerfDataBase::CPerfDataBase(const wchar_t * wszRequest, LPCSTR machine_name) 
    : CPerfData(wszRequest, machine_name), m_Titles(machine_name)
{
    m_ObjectsCount = 0;
    Read();
}

//////////////////////////////////////////////////////////////////////
CPerfDataBase::~CPerfDataBase()
{
}

//////////////////////////////////////////////////////////////////////
DWORD   CPerfDataBase::FindObjectId(const char * obj_name)              
{
    try 
    {
        wchar_t wobj_name[MAX_OBJECT_NAME+1];
        if ( ConvertToUni(obj_name, wobj_name, MAX_OBJECT_NAME) == 0 )
            return 0;
        PPERF_OBJECT_TYPE obj = FindObject(wobj_name);
        if ( !obj )
            return 0;
        
        return obj->ObjectNameTitleIndex;
    }
    catch (...)  {
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////
PPERF_OBJECT_TYPE  CPerfDataBase::FindObject(const wchar_t * obj_name)              
{
    try 
    {
        PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );

        for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
                  i++, PerfObj=NextObject(PerfObj) )
        {
            DWORD obj_idx = PerfObj->ObjectNameTitleIndex;
            wchar_t* wobjname = m_Titles.GetText(obj_idx);
            if ( !wobjname )
                continue;
            if ( wcsicmp(wobjname, obj_name ) == 0 )
                return PerfObj;
        }
    }
    catch (...) {
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////
PPERF_COUNTER_DEFINITION  CPerfDataBase::FindCounter(const char * obj_name, const char * counter_name)
{
    try 
    {
        wchar_t wobj_name[MAX_OBJECT_NAME+1];
        wchar_t wcnt_name[MAX_OBJECT_NAME+1];
        if ( ConvertToUni(obj_name, wobj_name, MAX_OBJECT_NAME) == 0 )
            return 0;
        if ( ConvertToUni(counter_name, wcnt_name, MAX_OBJECT_NAME) == 0 )
            return 0;

        PPERF_OBJECT_TYPE obj = FindObject(wobj_name);
        if ( !obj )
            return 0;

        return FindCounter(obj, wcnt_name);
    }
    catch (...)     {
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Find appropriate instance, when we do not know it.
CString CPerfDataBase::FindDefaultInstance(DWORD objId, DWORD counterId)
{
    wchar_t* firstInstName = 0;
    CString str("");

    try 
    {
        PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );
        PPERF_COUNTER_DEFINITION PerfCntr = 0;
        PPERF_INSTANCE_DEFINITION PerfInst;


        for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
                  i++, PerfObj=NextObject(PerfObj) )
        {
            if ( PerfObj->ObjectNameTitleIndex != objId )
                continue;
            PerfCntr = FirstCounter( PerfObj );

            if ( PerfObj->NumInstances < 1 ) 
                break;

            // Get the first instance.
            PerfInst = FirstInstance( PerfObj );
            for(int k = 0; PerfInst && k < (DWORD)PerfObj->NumInstances; 
                           k++, PerfInst = NextInstance(PerfInst) ) 
            {
                wchar_t* instName = (wchar_t *)((PBYTE)PerfInst + PerfInst->NameOffset);
                if (firstInstName == 0)
                    firstInstName = instName;

                if ( wcsicmp(L"_Total", instName) == 0 )
                {
                    str = "_Total";
                    goto finish;            
                }

            }
            if ( firstInstName )
            {
                TRACE("### FindDefaultInstance()  firstInstName=%S \r\n", firstInstName);
                str = (LPCTSTR)ConvertFromUni(firstInstName);  
                goto finish;            
            }
        }
    }
    catch (...) {
        TRACE("### FindDefaultInstance() EXCEPTION.  firstInstName=%p \r\n", firstInstName);
        str = "";
    }

finish:
    return str;
}

//////////////////////////////////////////////////////////////////////
PPERF_COUNTER_DEFINITION   
    CPerfDataBase::FindCounter(PPERF_OBJECT_TYPE PerfObj, const wchar_t * counter_name)
{
    try 
    {
        PPERF_COUNTER_DEFINITION PerfCntr = FirstCounter( PerfObj );

        for(DWORD j=0; PerfCntr && j < PerfObj->NumCounters; 
                  j++, PerfCntr=NextCounter(PerfCntr) )
        {
            DWORD cntr_idx = PerfCntr->CounterNameTitleIndex;
            wchar_t* wcounter_name = m_Titles.GetText(cntr_idx);
            if ( !wcounter_name )
                continue;
            if ( wcsicmp(counter_name, wcounter_name ) == 0 )
                return PerfCntr;
        }
    }
    catch (...) {
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////
CString ConvertFromUni(const wchar_t * wsz)
{
    DWORD lenUni  = wcslen(wsz);
    char *pc = 0;
    CString str;
    int buffsize = lenUni*3; 

    pc = (char*)malloc(buffsize + 2);
    DWORD lenANSI = 0;

    lenANSI = WideCharToMultiByte(
      CP_ACP,               // code page        // CP_OEMCP, CP_ACP
      0,                    // performance and mapping flags
      wsz,                  // wide-character string
      lenUni,               // number of chars in string
      pc,                   // buffer for new string
      buffsize,             // size of buffer
      NULL,                 // default for unmappable chars
      NULL                  // set when default char used
    );

    if (lenANSI <= buffsize)
        pc[lenANSI] = 0;

    if ( lenANSI == 0)
        str = "";
    else
        str = pc;

    if ( pc )
        free(pc);
    return str;
}

///////////////////////////////////////////////////////////////////////////////
//  Returns new length or 0 if not success.
DWORD ConvertToUni(const CString str, wchar_t * wsz_out, DWORD max_out_chars)
{
    int k = MultiByteToWideChar(
          CP_OEMCP,             // code page
          MB_PRECOMPOSED,       // character-type options
          str,                  // string to map
          str.GetLength(),      // number of bytes in string
          wsz_out,              // wide-character buffer
          max_out_chars - 1     // size of buffer
    );

    wsz_out[k] = 0;
    return k;
}

///////////////////////////////////////////////////////////////////
//      Fill the help buffer
///////////////////////////////////////////////////////////////////
BOOL  CPerfDataBase::GetHelpText(DWORD objId, char* buf, DWORD buflen)
{
    DWORD lenANSI = 0;
    buf[0] = 0;

    wchar_t * wsz = m_Titles.GetHelp(objId);
    if (wsz == 0)
        return FALSE;
    DWORD lenUni  = wcslen(wsz);

    lenANSI = WideCharToMultiByte(
      CP_ACP,               // code page        // CP_OEMCP, CP_ACP
      0,                    // performance and mapping flags
      wsz,                  // wide-character string
      lenUni,               // number of chars in string
      buf,                  // buffer for new string
      buflen,               // size of buffer
      NULL,                 // default for unmappable chars
      NULL                  // set when default char used
    );

    if (lenANSI <= buflen)
        buf[lenANSI] = 0;

    if ( lenANSI == 0)
    {
        buf[0] = 0;
        return FALSE;
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////
const char* GetCounterTypeStr(DWORD t) 
{
#if OUTPUT_COUNTER_LIST

#define CHKRFCTYPE(mask,str) { if ((t & mask) == mask )  \
    {  kfld++; \
       if ( kfld > 1) strcat (sz, ", "); \
       strcat (sz, str); } }
#define CHKRFCTYPEZERO(mask,str) { if ((t & mask) == 0 )  \
    {  kfld++; \
       if ( kfld > 1) strcat (sz, ", "); \
       strcat (sz, str); } }

    static char sz[400];
    sz[0] = 0;
    int kfld = 0;

    CHKRFCTYPEZERO(PERF_SIZE_DWORD,"Num")
    CHKRFCTYPE(PERF_TYPE_COUNTER,"Counter")
    CHKRFCTYPE(PERF_TYPE_TEXT,"Text")
    CHKRFCTYPE(PERF_TYPE_ZERO,"0")

    CHKRFCTYPEZERO(PERF_SIZE_VARIABLE_LEN,"DW")
    CHKRFCTYPE(PERF_SIZE_LARGE,"Large")
    CHKRFCTYPE(PERF_SIZE_ZERO,"Zero")
    CHKRFCTYPE(PERF_SIZE_VARIABLE_LEN,"Var")

    if ( (t & PERF_TYPE_ZERO) == 0 )                            // Perf type == Number
    {
        CHKRFCTYPEZERO(0x00030000,"Hex")
        CHKRFCTYPE(PERF_NUMBER_DECIMAL,"Dec")
        CHKRFCTYPE(PERF_NUMBER_DEC_1000,"Dec/1000")
    }
    else if ( (t & PERF_TYPE_COUNTER) == PERF_TYPE_COUNTER )    // Perf type == Counter
    {
        // CHKRFCTYPEZERO(0x00070000,"Display")
        CHKRFCTYPE(PERF_COUNTER_RATE,"<rate>")
        CHKRFCTYPE(PERF_COUNTER_FRACTION,"<fraction>")
        CHKRFCTYPE(PERF_COUNTER_BASE,"<base>")
        CHKRFCTYPE(PERF_COUNTER_ELAPSED,"<elapsed>")
        CHKRFCTYPE(PERF_COUNTER_QUEUELEN,"<queuelen>")
        CHKRFCTYPE(PERF_COUNTER_HISTOGRAM,"<histogram>")
    }
    else if ( (t & PERF_TYPE_TEXT) == PERF_TYPE_TEXT )          // Perf type == Text
    {
        CHKRFCTYPEZERO(PERF_TEXT_ASCII,"UNICODE")
        CHKRFCTYPE(PERF_TEXT_ASCII,"ASCII")
    }

    CHKRFCTYPEZERO(0x00300000,"Tick")
    CHKRFCTYPE(PERF_TIMER_100NS,"100ns")
    CHKRFCTYPE(PERF_OBJECT_TIMER,"ObjTimer")

    CHKRFCTYPE(PERF_DELTA_COUNTER,"Delta")
    CHKRFCTYPE(PERF_DELTA_BASE,"DeltaBase")
    CHKRFCTYPE(PERF_INVERSE_COUNTER,"Inverse")
    CHKRFCTYPE(PERF_MULTI_COUNTER,"**MULTI**")

    CHKRFCTYPE(PERF_DISPLAY_PER_SEC,"/sec")
    CHKRFCTYPE(PERF_DISPLAY_PERCENT,"%")
    CHKRFCTYPE(PERF_DISPLAY_SECONDS,"Seconds")
    CHKRFCTYPE(PERF_DISPLAY_NOSHOW,"No-Show")
    return &(sz[0]);
#else
    return "?";
#endif //OUTPUT_COUNTER_LIST
}

////////////////////////////////////////////////////////////////////////////////
//  Checks if object is feasible to display
bool CPerfDataBase::IsObjectUsable(wchar_t *wobj_name)
{
    if ( wcsicmp(L"Thread", wobj_name) == 0  ||
         wcsicmp(L"Process Address Space", wobj_name) == 0 ||
         wcsicmp(L"Image", wobj_name) == 0 ||
         wcsicmp(L"Full Image", wobj_name) == 0 ||
         wcsicmp(L"Thread Details", wobj_name) == 0 ||
         false
        )
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

static int CalcObjectGroupNumber(wchar_t* wobject_name) 
{
    CString  objname = ConvertFromUni(wobject_name);
    int      onamelen = objname.GetLength();
    if (onamelen == 0)
        return CSettings::m_CounterGroupsNumber;    // other group


    int igroup = 0;
    for ( ; igroup < CSettings::m_CounterGroupsNumber; igroup++) 
    {
        for ( int k = 0; k < CSettings::m_CounterGroups[igroup].GetSize() ; k++ ) 
        { 
            bool at_start = false;      // true: string must be at the start of name 
            bool at_end = false;        // true: string must be at the end of name

            CString nm_elt =  CSettings::m_CounterGroups[igroup].GetAt(k);
            int nm_len = nm_elt.GetLength(); 
            if (nm_len < 1)
                continue;

            if ( nm_elt[nm_len-1] == '$' ) {
                at_end = true;
                nm_len--;
                nm_elt = nm_elt.Left(nm_len);
            }

            if ( nm_len > 0 && nm_elt[0] == '$' ) {
                at_start = true;
                nm_len--;
                nm_elt = nm_elt.Right(nm_len);
            }


            int nc = objname.Find(nm_elt);

            if ( nc >= 0 ) {

                if ( at_start && nc > 0 )
                    continue;

                if ( at_end && ( nc + nm_len < onamelen ) )
                    continue;
                ///////////////////////////////////////////////////
                // Check character before and after found string
                //    it must be a special character
                ///////////////////////////////////////////////////
                if ( nc > 0 && isalnum(  objname[nc-1] ) )
                    continue;

                if ( nc + nm_len < onamelen && isalpha(  objname[nc + nm_len] ) )
                    continue;

                // OK. Speacial chars are before and after the found string.
                return igroup;
            }

        }
    }

    return igroup;
}

////////////////////////////////////////////////////////////////////////////////
int CPerfDataBase::GetObjectsList(int modes)
{
    PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );
    int obj_count = 0;
    int kgroup = 0;

    try 
    {
        for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
                  i++, PerfObj=NextObject(PerfObj) )
        {
            DWORD idObj = PerfObj->ObjectNameTitleIndex;
            wchar_t* wobject_name = m_Titles.GetText(idObj);
            if (wobject_name == 0 || !IsObjectUsable(wobject_name))
                continue;

            if ( (modes & FILL_OBJECT_LIST) != 0 ) {
                ASSERT( m_ObjsGroups != 0);
                
                kgroup = CalcObjectGroupNumber(wobject_name);
                ASSERT(kgroup <= CSettings::m_CounterGroupsNumber);

                m_ObjsGroups->SetAt(obj_count, kgroup);
            }

            obj_count++;

        }   // end-for-NextObject

    }
    catch (...) {
        TRACE(" *** NextObject error") ;
    }
    
    return obj_count;
}
 
//////////////////////////////////////////////////////////////////
int CPerfDataBase::FillCountersTree(CTreeCtrl *tree, LPCTSTR computerName, int group_number, int modes)
{
    PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );
    PPERF_COUNTER_DEFINITION PerfCntr = 0;
    CPerfCount counter;
    int obj_count = 0;      // number od really selected objects

    if ( tree->GetFirstVisibleItem() != 0 )
        return -1;          // already filled

    // Fill group numbers array (for the first time only) 
    if ( m_ObjsGroups == 0 )
    {
        m_ObjectsCount = GetObjectsList(GET_OBJECTS_COUNT);

        m_ObjsGroups = new CWordArray;
        m_ObjsGroups->SetSize(m_ObjectsCount);

        GetObjectsList(FILL_OBJECT_LIST);      // now really fill
    }

    try 
    {
        int obj_num = -1;       // number of current object
        for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
                  i++, PerfObj=NextObject(PerfObj) )
        {
            DWORD idObj = PerfObj->ObjectNameTitleIndex;
            wchar_t* wobject_name = m_Titles.GetText(idObj);

            if ( wobject_name == 0 )
                continue;

            CString  objname = ConvertFromUni(wobject_name);
            if (!IsObjectUsable(wobject_name))
                continue;
            obj_num++;

            int k_group = m_ObjsGroups->GetAt(obj_num);
            if ( k_group != group_number )       // fill group # according to sets.m_CounterGroups
                continue;

            HTREEITEM itemObject = 0;

            try 
            {
                PerfCntr = FirstCounter( PerfObj );
                for(DWORD j=0; PerfCntr && j < PerfObj->NumCounters; 
                          j++, PerfCntr=NextCounter(PerfCntr) )
                {
                    // Check if type of the counter is appropriate.
                    if ( !counter.IsCounterTypeSupported(PerfCntr->CounterType) )
                        continue;

                    DWORD idCntr = PerfCntr->CounterNameTitleIndex;
                    wchar_t* wcounter_name = m_Titles.GetText(idCntr);
                    if ( wcounter_name == 0 )   // it can be in NT4 and Win2K Server
                        continue;   

                    if (itemObject == 0)    // delayed insertion of object
                    {
                        obj_count++;
                        if ( ( modes & GET_OBJECTS_COUNT) != 0 )   // only count real object
                            continue;

                        itemObject = tree->InsertItem( ConvertFromUni(wobject_name) );
                        ASSERT(itemObject);
                        tree->SetItemData( itemObject, idObj );
                    }
                    
                    HTREEITEM itemCntr = tree->InsertItem( ConvertFromUni(wcounter_name), itemObject );
                    ASSERT(itemCntr);
                    tree->SetItemData( itemCntr, idCntr );
                }   // end-for-NextCounter

            }
            catch (...) {
                TRACE(" *** NextCounter error") ;
            }

        }   // end-for-NextObject

    }
    catch (...) {
        TRACE(" *** NextObject error") ;
    }
    
    return obj_count;
}

//////////////////////////////////////////////////////////////////
void CPerfDataBase::FillInstancesList(CListBox *list, DWORD objId)
{
    list->ResetContent();

    PPERF_OBJECT_TYPE PerfObj = FirstObject( m_PerfData );
    PPERF_COUNTER_DEFINITION PerfCntr = 0;
    PPERF_INSTANCE_DEFINITION PerfInst;

    LPTSTR szRes = "";
    wchar_t* firstInstName = 0;

    for(DWORD i=0; PerfObj && i < m_PerfData->NumObjectTypes; 
              i++, PerfObj=NextObject(PerfObj) )
    {
        if ( PerfObj->ObjectNameTitleIndex != objId )
            continue;
        PerfCntr = FirstCounter( PerfObj );

        if ( PerfObj->NumInstances < 1 ) 
            break;

        // Get the first instance.
        PerfInst = FirstInstance( PerfObj );
        for(int k=0; PerfInst && k < (DWORD)PerfObj->NumInstances; 
                        k++,PerfInst=NextInstance(PerfInst) ) 
        {
            wchar_t* instName = (wchar_t *)((PBYTE)PerfInst + PerfInst->NameOffset);
            CString str = ConvertFromUni(instName);  

            int idx = list->AddString(str);
            SetListHorizontalExtent(list, str);
            ASSERT( idx >= 0 );
        }
    }

}

//////////////////////////////////////////////////////////////////
// std_key: HKEY_PERFORMANCE_DATA or HKEY_LOCAL_MACHINE
HKEY GetMachineKey(LPCSTR machine_name, HKEY std_key)
{
    HKEY key = std_key;

    if (machine_name != 0 && strlen(machine_name) > 0 ) 
    {
        SET_WAIT_CURSOR;

        CString     str_machine = machine_name;
        str_machine.Insert(0, "\\\\");
        wchar_t w_machine_name[MAX_OBJECT_NAME+1];
        if ( ConvertToUni( machine_name, w_machine_name, MAX_OBJECT_NAME) != 0 )
        {
            hr = RegConnectRegistryW(w_machine_name, std_key, &key);
            if (hr != ERROR_SUCCESS) 
            {
                key = std_key;
            }
        }
        RESTORE_CURSOR;
    }
    return key;
}

//

