//////////////////////////////////////////////////////////////////////
// Errors.h - Erro handling 
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(ERROR_H_IS_INCLUDED_)
#define ERROR_H_IS_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//################################################################################
//#                           ERROR HANDLING                                     #
//################################################################################
extern  char    last_win32_error_txt[1000];   // buffer containing last Win32 error

extern HRESULT hr;
#define CheckHr(txt) \
if ( hr != ERROR_SUCCESS ) { sprintf(last_win32_error_txt, "ERROR in %s (%0x)", txt, hr); \
                             throw CWin32ApiException(); }

class CWin32ApiException {
};

enum EErrorCode {
    UnknownError = 0, 
    NotAbleToGetCounters,
    NoActiveIndicators,
    RegQueryValueExError,
    MaxErrorCode
};

void        Win32ErrorExit();
void        ErrorExit(EErrorCode code = UnknownError);
void        ShowErrorMsg(EErrorCode code);          // only show error message box
void        DebugErrorPrint(EErrorCode code);       // only print error to debugging log

// NOTE: Use TRACE mactro for pure debugging purposes.

#endif // !defined(ERROR_H_IS_INCLUDED_)
