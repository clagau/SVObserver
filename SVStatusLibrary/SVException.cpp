//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVException
//* .File Name       : $Workfile:   SVException.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   23 Feb 2015 03:47:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVException.h"
#include "SVMessage/SVMessage.h"
#include "SVRegistry.h"

#define SVEVENTSOURCE _T("SVException")
#define SVSECURITYSOURCE _T("SVSecurity")
#define SVACCESSSOURCE _T("SVAccess")

static const UINT CategoryNr = 31;
static const UINT CategoryBase = FAC_SVPLC;
static const TCHAR CategoryUnknown[] = _T("Unknown");
static const TCHAR CategoryNone[] = _T("None");
static const TCHAR CategorySystem[] = _T("System");
static const TCHAR CategoryApplication[] = _T("Application");
static const TCHAR TaskCategory[][CategoryNr]= { _T("SVPLC"), _T("SVBatchReport"), _T("SVFocusNT"), _T("SVFocusDB"), _T("SVFocusDBManager"),
								_T("SVIMCommand"), _T("SVIPC"), _T("SVLanguageManager"), _T("SVLibrary"), _T("SVPipes"), 
								_T("SVPLCCommand"), _T("SVTCPIP"), _T("SVObserver"), _T("SVSecurity"), _T("SVPLCRSLinx"),
								_T("SVMachineMessage"), _T("SVCmdComServer"), _T("SVCmdComClient"), _T("SVDataManager"),
								_T("SVAccess"), _T("SVIntek"), _T("SVImageCompression"), _T("SVEquation"), _T("SVFileAcquisition"),
								_T("SVMatroxGige"), _T("SVTVicLpt"), _T("SVOLibrary"), _T("SVSystemLibrary"), _T("SVMatroxLibrary"),
								_T("SVCI"), _T("SVXMLLibrary") };
				
/*
This parameterized constructer is used to fill this class with all of the appropriate data to be maintained for later use.
*/				
SVException::SVException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPCTSTR sErrorText, 
						 TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, DWORD dwProgramCode, 
						 DWORD dwOSErrorCode)
{
	InitException ();
	SetException(ErrorCode, szCompileDate, szCompileTime, sErrorText, szSourceFile, SourceLine, szSourceDateTime, dwProgramCode, dwOSErrorCode);
}

/*
This parameterized constructer is used to fill this class with all of the appropriate data to be maintained for later use.
*/				
SVException::SVException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPVOID pErrorData, 
						 UINT cbErrorData, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, 
						 DWORD dwProgramCode, DWORD dwOSErrorCode)
{
	InitException ();
	SetException(ErrorCode, szCompileDate, szCompileTime, pErrorData, cbErrorData, szSourceFile, SourceLine, szSourceDateTime, dwProgramCode, dwOSErrorCode);
}

/*
Default Constructor (creates and resets an SVException object).
*/
SVException::SVException()
{
	InitException ();
}

/*
This destructor clears all local data before final destruction.
*/
SVException::~SVException()
{
	ResetException ();
}

/*
Sets values in an SVException object.
*/
void SVException::SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPCTSTR pszErrorText, 
							   TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, DWORD dwProgramCode)
{
	SVString errorText(pszErrorText);
	UINT cbErrorData = static_cast< UINT >( errorText.size() * sizeof( TCHAR ) );
	SetException(ErrorCode, szCompileDate, szCompileTime,  (LPVOID) pszErrorText, cbErrorData, szSourceFile, SourceLine, szSourceDateTime, dwProgramCode);
	m_ErrorText = errorText;
}

/*
This method get the set source file name from the internal data.
*/
ULONG SVException::GetSourceFile(BSTR * bstrValue)
{	//calling function/process must use SysFreeString when 
	//finished using bstrValue.
	_bstr_t val(mszSourceFile);

	*bstrValue = val.Detach();
	if(bstrValue == NULL)
		return 0;
	ULONG* pPtr = (ULONG*)(*bstrValue);
	return *(--pPtr);
}

/*
This method get the set source date and time from the internal data.
*/
ULONG SVException::GetSourceDateTime(BSTR * bstrValue)
{	//calling function/process must use SysFreeString when 
	//finished using bstrValue.
	_bstr_t val(mszSourceDateTime);

	*bstrValue = val.Detach();
	if(bstrValue == NULL)
		return 0;
	ULONG* pPtr = (ULONG*)(*bstrValue);
	return *(--pPtr);
}

/*
This method get the set compile date from the internal data.
*/
ULONG SVException::GetCompileDate(BSTR * bstrValue)
{	//calling function/process must use SysFreeString when 
	//finished using bstrValue.
	_bstr_t val(mszCompileDate);
	*bstrValue = val.Detach();
	if(bstrValue == NULL)
		return 0;
	ULONG* pPtr = (ULONG*)(*bstrValue);
	return *(--pPtr);
}

/*
This method get the set compile time from the internal data.
*/
ULONG SVException::GetCompileTime(BSTR * bstrValue)
{	//calling function/process must use SysFreeString when 
	//finished using bstrValue.
	_bstr_t val(mszCompileTime);
	*bstrValue = val.Detach();
	if(bstrValue == NULL)
		return 0;
	ULONG* pPtr = (ULONG*)(*bstrValue);
	return *(--pPtr);
}

/*
This method get the set error data from the internal data.
*/
ULONG SVException::GetErrorData(BSTR * bstrValue)
{	//calling function/process must use SysFreeString when 
	//finished using bstrValue.
	*bstrValue = SysAllocStringByteLen((LPCSTR)mpErrorData,mcbErrorData);
	if(bstrValue == NULL)return 0;
	ULONG* pPtr = (ULONG*)(*bstrValue);
	return *(--pPtr);
}

/*
Sets values in an SVException object.
*/
void SVException::SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPCTSTR tszErrorText, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime , DWORD dwProgramCode, DWORD dwOsErrorCode)
{
	SVString errorText(tszErrorText);
	UINT cbErrorData = static_cast< UINT >( errorText.size() * sizeof( TCHAR ) );
	SetException(ErrorCode, szCompileDate, szCompileTime, (LPVOID)m_ErrorText.ToString(), cbErrorData, szSourceFile, SourceLine, szSourceDateTime ,dwProgramCode, dwOsErrorCode);
	m_ErrorText = errorText;
}

/*
Sets values in an SVException object.
*/
void SVException::SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPVOID pErrorData, UINT cbErrorData, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime , DWORD dwProgramCode, DWORD dwOsErrorCode)
{
	ResetException();

	mErrorCode = ErrorCode;
	mOSErrorCode = dwOsErrorCode;//GetLastError();
	mSourceLine = SourceLine;

	_tcsncpy (mszCompileDate, szCompileDate, std::min< size_t >( _tcslen( szCompileDate ) + 1, 255 ) );

	_tcsncpy (mszCompileTime, szCompileTime, std::min< size_t >( _tcslen( szCompileTime ) + 1, 255 ) );

	mcbErrorData = 0;
	mpErrorData = NULL;
	if (pErrorData)
	{
		if (mpErrorData = malloc (cbErrorData))
		{
			mcbErrorData = cbErrorData;
			memcpy (mpErrorData, pErrorData, cbErrorData);
		}
	}

	_tcsncpy (mszSourceDateTime, szSourceDateTime, std::min< size_t >( _tcslen( szSourceDateTime ) + 1, 255 ) );

	_tcsncpy (mszSourceFile, szSourceFile, std::min< size_t >( _tcslen( szSourceFile ) + 1, 255 ) );

	mbLogged = FALSE;
	mdwProgramCode = dwProgramCode;

	// Automatically log severe errors
	if (GetSeverity() == SEV_FATAL)
	{
		LogException( m_ErrorText.ToString() );
	}
}

// Special for Matrox Error
void SVException::SetException(long ErrorCode, DWORD dwProgramCode, DWORD dwOsErrorCode )
{
	ResetException();

	mErrorCode = ErrorCode;
	mOSErrorCode = dwOsErrorCode;//GetLastError();

	mcbErrorData = 0;
	mpErrorData = NULL;

	mbLogged = FALSE;
	mdwProgramCode = dwProgramCode;

}

/*
Sets values in an SVException object.
*/
void SVException::SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPVOID pErrorData, UINT cbErrorData, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime , DWORD dwProgramCode)
{
	SetException(ErrorCode,szCompileDate,szCompileTime,pErrorData, cbErrorData,szSourceFile,SourceLine, szSourceDateTime ,dwProgramCode,GetLastError());
}

/*
Clears an SVException object.
*/
void SVException::ResetException()
{

	if (mpErrorData)
	{
		free (mpErrorData);
	}

	InitException();
}

/*
Writes the exception to the exception log.
*/
void SVException::LogException()
{
	LogException (m_ErrorText.ToString());
}

/*
This constructor copies the data from one SVException object to this one.
*/
SVException::SVException(const SVException& orig)
{
	InitException ();
	(*this) = orig;
}

/*
This operator copies the data from one SVException object to this one.
*/
const SVException& SVException::operator=(const SVException& rhs)
{
    ResetException ();

    mErrorCode = rhs.mErrorCode;
    mOSErrorCode = rhs.mOSErrorCode;
    mSourceLine = rhs.mSourceLine;
    _tcscpy (mszCompileDate, rhs.mszCompileDate);
    _tcscpy (mszCompileTime, rhs.mszCompileTime);

    mpErrorData = NULL;
    mcbErrorData = 0;
    if (rhs.mpErrorData)
    {
        if (mpErrorData = malloc (rhs.mcbErrorData))
        {
            memcpy (mpErrorData, rhs.mpErrorData, rhs.mcbErrorData);
            mcbErrorData = rhs.mcbErrorData;
        }
    }

    _tcscpy (mszSourceDateTime, rhs.mszSourceDateTime);
    _tcscpy (mszSourceFile, rhs.mszSourceFile);

    mbLogged = rhs.mbLogged;
    mdwProgramCode = rhs.mdwProgramCode;

	m_ErrorText = rhs.m_ErrorText;
    return rhs;
}

/*
Returns the 32-bit SVMSG error code.
*/
DWORD SVException::GetErrorCode() const
{
	return mErrorCode;
}

/*
Returns the 32-bit operating system error code.
*/
DWORD SVException::GetOSErrorCode() const
{
	return mOSErrorCode;
}

/*
Formats an SVException object into a readable sting.
*/
SVString SVException::Format(SVString &szMsg , LPCTSTR pszMessage , LANGID Language) const
{
	SVString Result;
	SVString MsgDetails;
	SVString szMessageDll;
	SVString Source;
	LPVOID pszTemp;
	HMODULE hMessageDll;
	SVString szKey;
	SVString szSourceLine, szOSErrorCode, szOSErrorCodeHex;
	SVString szProgramCode, szProgramCodeHex;
	const TCHAR *szSubstituteString[11];

	if (Language == 0)
	{
		SVRegistryClass reg(_T("\\"));
		DWORD dwLanguage;

		if (!reg.GetRegistryValue ( _T( "Language" ) , &dwLanguage))
		{
			dwLanguage = MAKELANGID (LANG_NEUTRAL, SUBLANG_NEUTRAL);
		}
		Language = (LANGID) dwLanguage;
	}

	szKey = _T("HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Services\\EventLog\\Application\\");
	switch( GetFacility() )
	{
		case FAC_SVSECURITY:
		{
			Source =  SVSECURITYSOURCE;
			break;
		}
		case FAC_SVACCESS:
		{
			Source = SVACCESSSOURCE;
			break;
		}
		default :
		{
			Source = SVEVENTSOURCE;
			break;
		}
	}
	szKey += Source;
	SVRegistryClass reg(szKey.ToString());

	szMsg.clear();

	if (!reg.CreatedNewKey())
	{
		if (reg.GetRegistryValue( _T( "EventMessageFile" ), szMessageDll))
		{
			if (hMessageDll = LoadLibraryEx( szMessageDll.ToString(), NULL, LOAD_LIBRARY_AS_DATAFILE))
			{
				Sleep(0);
				szSourceLine.Format (_T("%d"), mSourceLine);
				szOSErrorCode.Format (_T("%d"), mOSErrorCode);
				szOSErrorCodeHex.Format (_T("0x%08.8x"), mOSErrorCode);
				szProgramCode.Format (_T("%d"), mdwProgramCode);
				szProgramCodeHex.Format (_T("0x%08.8x"), mdwProgramCode);

				szSubstituteString[0] = _T("\r\n");
				szSubstituteString[1] = (LPCTSTR) mszSourceFile;
				szSubstituteString[2] = szSourceLine.ToString();
				szSubstituteString[3] = (LPCTSTR) mszSourceDateTime;
				szSubstituteString[4] = szOSErrorCode.ToString();
				szSubstituteString[5] = szOSErrorCodeHex.ToString();
				szSubstituteString[6] = (LPCTSTR) mszCompileDate;
				szSubstituteString[7] = (LPCTSTR) mszCompileTime;
				szSubstituteString[8] = szProgramCode.ToString();
				szSubstituteString[9] = szProgramCodeHex.ToString();
				if (pszMessage)
				{
					szSubstituteString[10] = pszMessage;
				}
				else
				{
					szSubstituteString[10] = m_ErrorText.ToString();
				}


				if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY |
													 FORMAT_MESSAGE_FROM_HMODULE,
													 (LPCVOID) hMessageDll, mErrorCode,
													 Language, (LPTSTR) &pszTemp, 11, (va_list *) szSubstituteString))
				{
					szMsg = (TCHAR *) pszTemp;
					size_t iIndex = szMsg.find_first_of(_T("\r\n"));
					if ( -1 != iIndex )
					{
						Result.Format( _T("Source: %s\r\nCategory: %s\r\nEventID: %d\r\n"), Source.c_str(), GetCategoryName().c_str(), GetEventID() );

						szMsg.replace(_T("\r\n\r\n"), _T("\r\n") );
						szMsg.replace(_T("\\n"), _T("\r\n") );
						MsgDetails += szMsg.Mid( iIndex );
						//Remove unnecessary new lines from the details
						size_t Pos = MsgDetails.find_first_not_of(_T("\r\n"));
						if ( -1 != Pos )
						{
							MsgDetails = MsgDetails.substr( Pos );
						}
						szMsg.erase(iIndex, (szMsg.size() - iIndex));

						Result += MsgDetails;
					}
				}
				LocalFree (pszTemp);
				FreeLibrary (hMessageDll);
				// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
				Sleep(0);
			}
		}
	}
	else
	{
		reg.DeleteKey();
	}

	if (szMsg.empty())
	{
		szMsg.Format (_T("SVException\n\nError: %d\nProgramCode: %d\nOSError: %d\nSource File: %s\nLine: %d\nLast Modified: %s\nCompiled: %s %s\n"),
									mErrorCode,
									mdwProgramCode,
									mOSErrorCode,
									mszSourceFile,
									mSourceLine,
									mszSourceDateTime,
									mszCompileDate,
									mszCompileTime);
	}

	return Result;
}

/*
Returns the severity level of an exception. Possible return values are:

  SEV_SUCCESS
  SEV_INFORMATIONAL
  SEV_WARNING
  SEV_FATAL
*/
UINT SVException::GetSeverity() const
{
	return (GetErrorCode () & 0xc0000000) >> 30;
}

UINT SVException::getSeverityIcon() const
{
	UINT Icon( 0 );

	switch (GetSeverity())
	{
	case SEV_SUCCESS :
		Icon = MB_ICONINFORMATION;
		break;

	case SEV_INFORMATIONAL :
		Icon = MB_ICONINFORMATION;
		break;

	case SEV_WARNING :
		Icon = MB_ICONWARNING;
		break;

	case SEV_FATAL :
		Icon = MB_ICONERROR;
		break;

	default:
		Icon = 0;
		break;
	}

	return Icon;
}

/*
Returns the 12 bit facility code. (see SVMessage.h for a complete list).
*/
UINT SVException::GetFacility() const
{
	return (GetErrorCode () & 0x0fff0000) >> 16;
}

UINT SVException::GetEventID() const
{
	return (GetErrorCode () & 0x0000ffff);
}

/*
Returns a category value (based upon the facility) that can be used with ReportEvent.
*/
WORD SVException::GetCategory() const
{
	return (GetFacility() & 0x00ff);
}

SVString SVException::GetCategoryName() const
{
	SVString Result( CategoryUnknown );

	WORD Facility = GetFacility();
	switch( Facility )
	{
	case FAC_NONE:
		{
			Result = CategoryNone;
		}
		break;
	case FAC_SYSTEM:
		{
			Result = CategorySystem;
		}
		break;
	case FAC_APPLICATION:
		{
			Result = CategoryApplication;
		}
		break;
	default:
		{
			int Index(0);
			Index = Facility - CategoryBase;
			if( 0 <= Index && CategoryNr > Index )
			{
				Result = TaskCategory[Index];
			}
		}
		break;
	}

	return Result;
}

/*
This method returns the program code as set by the caller of SetException.
*/
DWORD SVException::GetProgramCode() const
{
	return mdwProgramCode;
}

/*
Writes a generic exception message to the exception log.
*/
void SVException::LogException( LPCTSTR szMessage)
{
	HANDLE hEventLog;
	SVString szSourceName, szSourceLine, szOSErrorCode, szOSErrorCodeHex;
	SVString szProgramCode, szProgramCodeHex, szMessageTemplate;
	SVString l_OutputString;
	const TCHAR *szSubstituteString[11];
	UINT Severity, SubStrings;

	l_OutputString.Format( _T( "Exception: ErrorCode: %d\nMessage: %s\n" ), mErrorCode, szMessage );

	::OutputDebugString( l_OutputString.ToString() );

	szSourceLine.Format (_T("%d"), mSourceLine);
	szOSErrorCode.Format (_T("%d"), mOSErrorCode);
	szOSErrorCodeHex.Format (_T("0x%08.8x"), mOSErrorCode);
	szProgramCode.Format (_T("%d"), mdwProgramCode);
	szProgramCodeHex.Format (_T("0x%08.8x"), mdwProgramCode);

	szSubstituteString[0] = _T("\r\n");
	szSubstituteString[1] = (LPCTSTR) mszSourceFile;
	szSubstituteString[2] = szSourceLine.ToString();
	szSubstituteString[3] = (LPCTSTR) mszSourceDateTime;
	szSubstituteString[4] = szOSErrorCode.ToString();
	szSubstituteString[5] = szOSErrorCodeHex.ToString();
	szSubstituteString[6] = (LPCTSTR) mszCompileDate;
	szSubstituteString[7] = (LPCTSTR) mszCompileTime;
	szSubstituteString[8] = szProgramCode.ToString();
	szSubstituteString[9] = szProgramCodeHex.ToString();
	szSubstituteString[10] = (LPCTSTR) szMessage;

	SubStrings = 11;

	if (!mbLogged)
	{
		switch( GetFacility() )
		{
			case FAC_SVSECURITY:
			{
				szSourceName = SVSECURITYSOURCE;
				break;
			}
			case FAC_SVACCESS:
			{
				szSourceName = SVACCESSSOURCE;
				break;
			}
			default :
			{
				szSourceName = SVEVENTSOURCE;
				break;
			}
		}

		if (hEventLog = RegisterEventSource (NULL, szSourceName.ToString()))
		{
			switch (GetSeverity())
			{
				case SEV_SUCCESS :
					Severity = EVENTLOG_SUCCESS;
					break;

				case SEV_INFORMATIONAL :
					Severity = EVENTLOG_INFORMATION_TYPE;
					break;

				case SEV_WARNING :
					Severity = EVENTLOG_WARNING_TYPE;
					break;

				case SEV_FATAL :
					Severity = EVENTLOG_ERROR_TYPE;
					break;
			}
			mbLogged = ReportEvent (hEventLog, Severity, GetCategory(), mErrorCode, NULL, SubStrings, mcbErrorData, szSubstituteString, mpErrorData);
			DeregisterEventSource (hEventLog);
		}
		else
		{
			DWORD dwErr;
			dwErr = GetLastError();
		}
	}
}

/*
This method initializes the internal information of the exception class.
*/
void SVException::InitException()
{
	mbLogged = FALSE;
	mcbErrorData = 0;
	mdwProgramCode = 0;
	mErrorCode = 0;
	mOSErrorCode = 0;
	mpErrorData = NULL;
	mSourceLine = 0;
	memset (mszCompileDate, 0, 256 * sizeof (TCHAR));
	memset (mszCompileTime, 0, 256 * sizeof (TCHAR));
	memset (mszSourceDateTime, 0, 256 * sizeof (TCHAR));
	memset (mszSourceFile, 0, 256 * sizeof (TCHAR));
	m_ErrorText.clear();
}

/*
This method deletes the binary data used within this object.
*/
void SVException::DeleteBinData()
{
	if (mpErrorData)
	{
		free (mpErrorData);
		mpErrorData = NULL;
	}
	mcbErrorData = 0;
}

SVString SVException::what() const
{
	SVString msg;
	if (!m_ErrorText.empty())
	{
		msg = m_ErrorText;
	}
	if (msg.empty())
	{
		Format(msg, nullptr, 0);
	}
	return msg;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVStatusLibrary\SVException.cpp_v  $
 * 
 *    Rev 1.5   23 Feb 2015 03:47:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added method getSevirityIcon
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Feb 2015 03:14:50   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Adapted Format method to display the message similar to the event log
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Dec 2014 04:32:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Set standrad error message for logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Nov 2014 16:55:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  970
 * SCR Title:  GetConfig and PutConfig cause a crash when there is not enough disk space
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to hold the text message if it was set.
 * Added the what method to retrieve the error text (if it was set) or get the text from FormatMessage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 10:08:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:43:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Dec 2010 16:09:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2010 11:11:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Oct 2009 16:56:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to remove ATL dependency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   13 Jul 2009 12:40:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to implement new exception functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   01 Oct 2008 16:00:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SetException mthod that doesn't require CString
 * Added include for atlstr.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   30 Oct 2007 14:29:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Sleep after Load and Free Library to give time to system resource management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   24 Jul 2007 08:36:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetException Overload for use with Matrox errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   19 Jun 2007 15:12:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17.1.0   30 Oct 2007 08:10:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Sleep after Load and Free Library to give time to system resource management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   23 Aug 2005 15:03:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic for SVAccess Facility.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   Jan 09 2003 11:13:44   rschock
 * Changes to fix merged out code
 * 
 *    Rev 1.15   18 Nov 2002 14:41:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   2 new SetException functions added
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   13 Jun 2001 13:23:20   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the SetException(long , TCHAR* , TCHAR* , LPVOID , UINT , TCHAR* , long ,
 * TCHAR* , DWORD) function
 * to set the mOSErrorCode via a new passed in value rather then from the system.
 * 
 * Added the following functions to retreive and manipulate internal class data.
 * DeleteBinData, GetErrorData, GetCompileTime, GetCompileDate, GetSourceDateTime,
 * GetSourceFile.
 * 
 * Also added 1 additional set function for SVException.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Feb 2001 16:23:40   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create a non-unicode version to operate with svobserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   13 Feb 2001 13:36:46   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  100
 * SCR Title:  SVFocus crashes during product save
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed string pointer memory variables to character arrays so they would not have to be dynamically re-allocated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Oct 2000 13:33:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  57
 * SCR Title:  SVException destructor fails
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   operator= was not checking for null pointers prior to copying data to the 'this' object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   11 Oct 2000 12:19:24   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  57
 * SCR Title:  SVException destructor fails
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed CString member variables to TCHAR * variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Oct 2000 12:25:56   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  54
 * SCR Title:  Automatic timeout for dialog boxes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified the Format() member function to delete any extra characters that are not needed from the string. Using the SetAt function did not modify the length returned by GetLength().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Aug 2000 12:09:20   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  37
 * SCR Title:  Language Translation
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed the Format() function to properly load the string based on language.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Aug 2000 12:55:18   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  23
 * SCR Title:  Bug fixes 1.0
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified the Format() function to include the replacement parameters in the output string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Aug 2000 11:51:54   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  23
 * SCR Title:  Bug fixes 1.0
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added logic to handle creation and handling of security log information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Aug 2000 21:29:12   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  25
 * SCR Title:  Download OCR files
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Extended the LogException function
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 May 2000 17:16:42   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  12
 * SCR Title:  Stage 2: Error/Alarm Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Updated error codes.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 May 2000 09:59:08   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Eliminated the #include "SVMessage.h" dependency so that any project can use the SVException class with its own message file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Apr 2000 16:58:18   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  12
 * SCR Title:  Stage 2: Error/Alarm Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Updated the SVException::Format () function to read the message string from the message dll. This change required a modification to SVRegistryClass to permit opening the registry key "HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\EventLog\Application"
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Apr 2000 17:10:06   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to add required functionality to SVFocus and other projects associated with SVocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:28:06   mike
 * Initial revision.
*/
