//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVException
//* .File Name       : $Workfile:   SVException.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   23 Feb 2015 03:47:16  $
//******************************************************************************

#ifndef _INC_SVEXCEPTION_38BBBEC2003E_INCLUDED
#define _INC_SVEXCEPTION_38BBBEC2003E_INCLUDED

#include <comdef.h>
#include "SVUtilityLibrary/SVString.h"

// Macros
#define SET_EXCEPTION(e, ErrorCode, ProgramCode, OSError, Message) (e.SetException(ErrorCode, _T(__DATE__), _T(__TIME__), Message, _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), ProgramCode, OSError))
#define SETEXCEPTION0(e, eCode) e.SetException (eCode, _T(__DATE__), _T(__TIME__), NULL, 0, _T(__FILE__), __LINE__, _T(__TIMESTAMP__));
#define SETEXCEPTION1(e, eCode, eMsg) e.SetException (eCode, _T(__DATE__), _T(__TIME__), eMsg, _T(__FILE__), __LINE__, _T(__TIMESTAMP__));
#define SETEXCEPTION2(e, eCode, eMsg, ecbMsg) e.SetException (eCode, _T(__DATE__), _T(__TIME__), eMsg, ecbMsg, _T(__FILE__), __LINE__, _T(__TIMESTAMP__));
#define SETEXCEPTION3(e, pCode, eCode, eMsg, ecbMsg) e.SetException (eCode, _T(__DATE__), _T(__TIME__), eMsg, ecbMsg, _T(__FILE__), __LINE__, _T(__TIMESTAMP__), pCode);
#define SETEXCEPTION4(e, eCode, pCode) e.SetException (eCode, _T(__DATE__), _T(__TIME__), NULL, 0, _T(__FILE__), __LINE__, _T(__TIMESTAMP__), pCode);
#define SETEXCEPTION5(e, eCode, pCode, eMsg) e.SetException (eCode, _T(__DATE__), _T(__TIME__), eMsg, _T(__FILE__), __LINE__, _T(__TIMESTAMP__), pCode);
#define SETEXCEPTION6(e, eCode, pCode, eMsg, ecbMsg) e.SetException (eCode, _T(__DATE__), _T(__TIME__), eMsg, ecbMsg, _T(__FILE__), __LINE__, _T(__TIMESTAMP__), pCode);
#define SETEXCEPTION7(e, eCode, eMsg, oCode) e.SetException(eCode,_T(__DATE__),_T(__TIME__),(LPVOID) (LPCTSTR)eMsg,eMsg.GetLength() * sizeof (TCHAR),_T(__FILE__),__LINE__,_T(__TIMESTAMP__),0,oCode)
#define SETSECURITYEXCEPTION(e, eCode, eUser, eDialog, eFunction) e.SetException (eCode, eFunction, _T(__TIME__), NULL, 0, eUser, __LINE__, eDialog);

//The SVException Class provides a standard means of handling errors within an application.
class SVException 
{
public:
	SVException();
	SVException(const SVException& orig);
	SVException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPVOID pErrorData, UINT cbErrorData, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, DWORD dwProgramCode = 0, DWORD dwOsErrorCode = 0);
	SVException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPCTSTR sErrorText, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, DWORD dwProgramCode = 0, DWORD dwOSErrorCode = 0);

	virtual ~SVException();

	SVString what() const;
	void DeleteBinData();

	ULONG GetSourceFile(BSTR * bstrValue);
	ULONG GetSourceDateTime(BSTR * bstrValue);
	ULONG GetCompileDate(BSTR * bstrValue);
	ULONG GetCompileTime(BSTR * bstrValue);
	ULONG GetErrorData(BSTR * bstrValue);
	LONG GetSourceLine() const { return mSourceLine; }
	BOOL GetLoggedStatus() const { return mbLogged; }

	void LogException();

	void LogException( LPCTSTR szMessage );

	DWORD GetProgramCode() const;

	void SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPVOID pErrorData, UINT cbErrorData, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime , DWORD dwProgramCode = 0);

	void SetException(long ErrorCode, DWORD dwProgramCode, DWORD dwOsErrorCode );

    void SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPCTSTR szErrorText, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, DWORD dwProgramCode, DWORD dwOsErrorCode);
	void SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPVOID pErrorData, UINT cbErrorData, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, DWORD dwProgramCode, DWORD dwOsErrorCode);
	void SetException(long ErrorCode, TCHAR* szCompileDate, TCHAR* szCompileTime, LPCTSTR sErrorText, TCHAR* szSourceFile, long SourceLine, TCHAR* szSourceDateTime, DWORD dwProgramCode = 0);

	UINT GetSeverity() const;
	UINT getSeverityIcon() const;
	UINT GetFacility() const;
	UINT GetEventID() const;
	WORD GetCategory() const;
	SVString GetCategoryName() const;

	DWORD GetOSErrorCode() const;
	SVString Format (SVString& szMsg , LPCTSTR pszMessage = nullptr, LANGID Language = 0) const;
	DWORD GetErrorCode() const;

	const SVException& operator=(const SVException& rhs);

	void ResetException();

private:
	void InitException();

	//Program specific code.
	DWORD mdwProgramCode;

	//Size (in bytes) of the mpErrorData.
	UINT mcbErrorData;

	//Number identifying the error.
	DWORD mErrorCode;

	//Operating system error code.
	DWORD mOSErrorCode;

	//Name of the source file where the exception was encountered.
	TCHAR mszSourceFile[256];

	//Line number within the source file where the error was encountered.
	long mSourceLine;

	//Last modification date/time of the source file.
	TCHAR mszSourceDateTime[256];

	//Date the source file was compiled.
	TCHAR mszCompileDate[256];

	//Time the source file was compiled.
	TCHAR mszCompileTime[256];

	//Supporting information for the error.
	LPVOID mpErrorData;

	//Flag indicating if the exception was written to the event log
	BOOL mbLogged;
	SVString m_ErrorText; 
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVStatusLibrary\SVException.h_v  $
 * 
 *    Rev 1.3   23 Feb 2015 03:47:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added method getSevirityIcon
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Feb 2015 03:14:50   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Adapted Format method to display the message similar to the event log
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Nov 2014 16:55:58   sjones
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
 *    Rev 1.0   25 Apr 2013 17:43:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 11:11:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
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
 *    Rev 1.23   29 Jul 2009 14:39:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
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
 *    Rev 1.21   17 Nov 2008 08:22:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Fixed Header File - missing SVSetException
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   14 Nov 2008 15:51:02   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Reorganized constructors within SVException.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   01 Oct 2008 16:00:22   sjones
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
 *    Rev 1.18   24 Jul 2007 08:36:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetException Overload for use with Matrox errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   14 Jan 2003 10:11:50   ebeyeler
 * Project: SVSVIMInterface
 * Change Request (SCR) nbr: 13
 * SCR Title: Add the ability to stream data results to the SIAC from the SVIM
 * Checked in by: eBeyeler; Eric Beyeler
 * Change Description: no change
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.16   Jan 09 2003 11:14:06   rschock
 * Changes to fix merged out code
 * 
 *    Rev 1.15   18 Nov 2002 14:46:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   add 2 additional SetException functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   18 Jun 2001 09:47:00   ed
 * Project: SVSVIMInterface
 * Change Request (SCR) nbr: 1
 * SCR Title: ActiveX Control exposing SVObserver functionality
 * Checked in by: Ed; Ed Chobanoff
 * Change Description: Added macro number 7.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   13 Jun 2001 13:23:16   ed
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
 *    Rev 1.11   24 Oct 2000 15:25:28   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  55
 * SCR Title:  SVFocus Remote control from PLC
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added SETEXCEPTION macros
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   11 Oct 2000 12:19:20   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  57
 * SCR Title:  SVException destructor fails
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed CString member variables to TCHAR * variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   30 Aug 2000 12:09:18   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  37
 * SCR Title:  Language Translation
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed the Format() function to properly load the string based on language.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   08 Aug 2000 12:55:20   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  23
 * SCR Title:  Bug fixes 1.0
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified the Format() function to include the replacement parameters in the output string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Aug 2000 11:51:56   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  23
 * SCR Title:  Bug fixes 1.0
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added logic to handle creation and handling of security log information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   05 Aug 2000 21:29:10   mike
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
 *    Rev 1.5   23 May 2000 17:16:40   mike
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
 *    Rev 1.4   03 May 2000 09:59:08   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Eliminated the #include "SVMessage.h" dependency so that any project can use the SVException class with its own message file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Apr 2000 16:58:16   mike
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
 *    Rev 1.2   20 Apr 2000 09:06:52   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  9
 * SCR Title:  Baseline Development: Main Menu
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed #include search path to also search ..\SVMessage in all configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Apr 2000 17:10:04   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to add required functionality to SVFocus and other projects associated with SVocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:28:08   mike
 * Initial revision.
*/
