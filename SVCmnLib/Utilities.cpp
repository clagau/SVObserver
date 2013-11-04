// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Utilities.cpp
// * .File Name       : $Workfile:   Utilities.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
// ******************************************************************************

#include "stdafx.h"
#include "utilities.h"

BOOL CreateDirPath(CString szPath)
{
	int nBackCount = 0;
	int nBackIndex = 0;
	int nIndex;
	CString szTemp;

	if(szPath.IsEmpty())return FALSE;

	//find the number of backslashes
	while(nBackIndex = szPath.Find(_T("\\"),nBackIndex))
		{
		if(nBackIndex == -1)break;
		nBackIndex++;
		nBackCount++;
		}
	
	if(nBackCount > 1)
		{
		//check for all the directories, create dirs if they don't exist
		nBackIndex=0;
		for(nIndex=0; nIndex<nBackCount; nIndex++)
			{
			nBackIndex = szPath.Find(_T("\\"),nBackIndex);				
			nBackIndex++;
			if(nIndex==0)continue;
			szTemp = szPath.Left(nBackIndex-1);
			if(_tmkdir(szTemp.GetBuffer(szTemp.GetLength()))) 
				{
				if(errno != EEXIST)return FALSE;
				}
			}
		}
	else if (nBackCount == 1)
		{
		return TRUE;
		}
	else
		{
		return FALSE;
		}
	return TRUE;
}

HRESULT GetSystemErrorText(DWORD dwError, CString & szMsg)
{
	LPVOID lpMsgBuf;
	if(FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	) == 0)return S_FALSE;
	szMsg.Format(_T("%s"),lpMsgBuf);
	LocalFree(lpMsgBuf);
	return S_OK;
}

DWORD GetLastSystemErrorText(CString & szMsg)
{
	DWORD dwError = GetLastError();
	GetSystemErrorText(dwError, szMsg);
	return dwError;
}

void DisplayLastSystemError()
{
	CString szTemp, szText;
	DWORD dwError = GetLastSystemErrorText(szText);
	szTemp.Format(_T("(%04X) %s"),dwError,szText);
	MessageBox(NULL,szTemp,_T("ERROR"),MB_OK);
}

BOOL CopyDir(LPCTSTR szOrigPath, LPCTSTR szNewPath)
{

	CString			csFileToFind;
	CString			csNewFile;
	CString			csFile;
	CFileFind		cffFinder;
	BOOL			blReturn = TRUE;

	//create the temp folder
	CreateDirPath(szNewPath);

	csFileToFind	= szOrigPath;
	csFileToFind += _T("*.*");

	BOOL bWorking = cffFinder.FindFile ( csFileToFind );

	while ( bWorking )
		{
		bWorking = cffFinder.FindNextFile ( );

		// If file is directory ...
		if ( !cffFinder.IsDirectory ( ) )
			{
			csFile = (LPCTSTR) cffFinder.GetFilePath ( );
			csNewFile = szNewPath + cffFinder.GetFileName();
			if(!CopyFile(csFile,csNewFile,TRUE))
				{
				blReturn = FALSE;
				break;
				}
			}
		}

	return blReturn;
}


BOOL DeleteDir(LPCTSTR szPath)
{
	CString			csFileToFind;
	CString			csFile;
	CFileFind*		pcffFinder = NULL;
	BOOL			blReturn = TRUE;

	pcffFinder = new CFileFind(); 
	
	if(!pcffFinder)return FALSE;
	
	csFileToFind	= szPath;
	csFileToFind += _T("*.*");

	BOOL bWorking = pcffFinder->FindFile ( csFileToFind );

	while ( bWorking )
		{
		bWorking = pcffFinder->FindNextFile ( );

		// If file is directory ...
		if ( !pcffFinder->IsDirectory ( ) )
			{
			csFile = (LPCTSTR) pcffFinder->GetFilePath ( );
			if(!DeleteFile(csFile))
				{
				blReturn = FALSE;
				break;
				}
			}
		}

	if(pcffFinder)delete pcffFinder;

	CString szTemp = szPath;
	szTemp.Delete(szTemp.GetLength() - 1,1);	   
	if(blReturn)
		{
		BOOL bl = RemoveDirectory(szTemp);

		if(!bl)
			{
			CString szMsg;
			GetLastSystemErrorText(szMsg);
			}	  
		}

	return blReturn;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\Utilities.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1.1.0   22 Oct 2010 08:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jul 2001 21:20:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  215
 * SCR Title:  Fix SVObserver remote command processing problem with MS-DOS version SVFocus
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added DeleteDir and CopyDir functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jul 2001 21:20:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  215
 * SCR Title:  Fix SVObserver remote command processing problem with MS-DOS version SVFocus
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added DeleteDir and CopyDir functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
