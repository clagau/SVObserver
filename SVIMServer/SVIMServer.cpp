// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMServer
// * .File Name       : $Workfile:   SVIMServer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:07:28  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIMServer.h"
#include "SVLibrary/SVPackedFile.h"
#include "SVMessage/SVMessage.h"
#include "SVCmnLib/utilities.h"

SV_EXPORTDLL SVIMServer::SVIMServer() : SVIMCommandServer (CString (_T("SVObserver")))
{
	mpFile = NULL;
	mhWndSVObserver = NULL;
}

SVIMServer::~SVIMServer()
{
	if (mpFile)
	{
		delete mpFile;
		mpFile = NULL;
	}
}

BOOL SVIMServer::OnPutSVIMConfig(CString& szIMCSVIMConfigName)
{
	CString szPackedFile, szTemp;

	USES_CONVERSION;

	// reject request if a previous request is still ongoing.
	if (mpFile)
		return FALSE;

	//create confirm directory c:\temp
	if(!CreateDirPath(CString(_T("c:\\temp"))))return FALSE;

	//create a temporary filename and saved file to c:\temp
	szPackedFile = CString(_T("c:\\temp\\temp")) + _T(".svf");

	//create the file
		if (mpFile = new CFile (szPackedFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			mpFile->Close();
			return TRUE;
		}

	return FALSE;
}

BOOL SVIMServer::OnSVIMData(char * pBuffer, UINT NumBytes)
{
	SVIMCommand Command;
	CString szPackedFile, szSecFile;
	int cmdSize;
	char *pTemp;

	USES_CONVERSION;
	szPackedFile = _T("c:\\temp\\temp.svf");

	if (mpFile)
	{
		if (NumBytes)
		{
			if (mpFile->Open(szPackedFile, CFile::modeWrite | CFile::typeBinary))
			{
				mpFile->SeekToEnd();
				cmdSize = strlen (Command.GetCommandText(SVDATA));
				pTemp = pBuffer + cmdSize;
				try
				{
					mpFile->Write (pTemp, NumBytes - cmdSize);
					mpFile->Close();
					return TRUE;
				}
				catch (...)
				{
					mpFile->Close ();
					delete mpFile;
					mpFile = NULL;
				}
			}
			else
			{
				delete mpFile;
				mpFile = NULL;
				return FALSE;
			}
		}
		else
		{

			// file receipt completed.
			SVPackedFile svPackedFile;

		//global function to close sec and clean up c:\run dir
		if(mpfnCloseConfiguration)
			{
			if(!mpfnCloseConfiguration())return FALSE;
			}
		else
			{
			return FALSE;
			}

		//unpack the files in the c:\run directory
		if(!svPackedFile.UnPackFiles (szPackedFile, _T("C:\\RUN")))return FALSE;
		//remove the temporary file
		CFile::Remove(szPackedFile);
		
		szSecFile = svPackedFile.GetSecFilePath();
		if(szSecFile.IsEmpty())
			{
#ifdef _DEBUG
			AfxMessageBox(_T("No SEC file found!"));
#endif
			return FALSE;
			}

		//load the config
		if(mpfnOpenConfiguration)
			{
			if(!mpfnOpenConfiguration((char*)((LPCTSTR)szSecFile)))return FALSE;
			}
		else
			{
			return FALSE;
			}
			
			delete mpFile;
			mpFile = NULL;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL SVIMServer::OnGetSVIMConfig(CString& szIMCSVIMConfigName)
{
	BOOL rc = TRUE;
	CString szPackedFile;
	SVPackedFile PackedFile;

	USES_CONVERSION;

	szPackedFile = szIMCSVIMConfigName;
	szPackedFile += _T(".svf");

	//global function to close sec and clean up c:\run dir
	if(mpfnSaveConfiguration)
		{
		if (rc = mpfnSaveConfiguration ())
	{
		try
		{
			if (PackedFile.PackFiles (szIMCSVIMConfigName, szPackedFile))
			{
				mpFile = new CFile (szPackedFile, CFile::modeRead | CFile::typeBinary);
			}
			else
			{
				mpFile = NULL;
				rc = FALSE;
			}
		}
		catch (...)
		{
			mpFile = NULL;
			rc = FALSE;
		}
	}
		else
			{
			rc = FALSE;
			}
		}
	else
		{
		rc = FALSE;
		}
  return rc;
}

BOOL SVIMServer::OnGetCurrentSVIMConfig(CString &szIMCSVIMConfigName)
{
	char szBuffer[_MAX_PATH];

	if (mpfnGetCurrentConfigPath)
	{
		if (mpfnGetCurrentConfigPath(szBuffer))
		{
			szIMCSVIMConfigName = szBuffer;
			return OnGetSVIMConfig (szIMCSVIMConfigName);
		}
	}
	return FALSE;
}

BOOL SVIMServer::OnSVIMGoOffline()
{
	if (mpfnGoOffline)
		return mpfnGoOffline();
	else
		return FALSE;
}

BOOL SVIMServer::OnSVIMGoOnline()
{
	if (mpfnGoOnline)
		return mpfnGoOnline();
	else
		return FALSE;
}

BOOL SVIMServer::OnGetSVIMState(DWORD * pdwSVIMState)
{
	if (mpfnGetSVIMState)
		return mpfnGetSVIMState (pdwSVIMState);
	else
		return FALSE;
}

void SVIMServer::OnSVIMReceiveComplete(CString& szReceiveFileName)
{
	if (mpFile->m_hFile != CFile::hFileNull)
	{
		mpFile->Close();
		delete mpFile;
		mpFile = NULL;
	}
}

void SVIMServer::OnSVIMSendComplete(CString& szSendFileName)
{
}

BOOL SVIMServer::OnWriteBlockComplete(SVException *psvException, char *pBuffer, int cbBuffer)
{
	SVIMCommand Command;
	char *pTemp;
	int size, count, cmdlen;
	CString szErrorMsg;

	if (psvException->GetErrorCode () == SVMSG_PIPES_NO_ERROR)
	{
		if (mpFile)
		{
			strcpy (pBuffer, Command.GetCommandText (SVDATA));
			cmdlen = strlen (pBuffer);
			size = cbBuffer - cmdlen;
			pTemp = pBuffer + cmdlen;

			if (count = mpFile->Read (pTemp, size))
			{
				if (!mpConnection->Write (pBuffer, count + cmdlen))
				{
					szErrorMsg.Format (_T("Write Failed"));
					psvException->SetException (SVMSG_PIPES_WRITE_FAILED, _T(__DATE__), _T(__TIME__), szErrorMsg, _T(__FILE__), __LINE__, _T(__TIMESTAMP__));
				}
				else
				{
					return TRUE;
				}
			}
		}
	}

	if (psvException->GetErrorCode () != SVMSG_PIPES_NO_ERROR)
	{
		OnSVIMError (psvException);
	}

	if (mpFile)
	{
		mpFile->Close();
		delete mpFile;
		mpFile = NULL;
	}

	return FALSE;
}

BOOL SVIMServer::OnReadBlockComplete(SVException *psvException, char *pBuffer, int cbBuffer)
{
	return FALSE;
}

void SVIMServer::SetupConnection()
{
	mpConnection->SetupConnection (CString (_T("SVObserver")));
}

void SVIMServer::SetSVObserverWnd(HWND hWnd)
{
	mhWndSVObserver = hWnd;
}

void SVIMServer::SetGoOffline (PFN_BOOLVOID pfnGoOffline)
{
	mpfnGoOffline = pfnGoOffline;
}

void SVIMServer::SetGoOnline (PFN_BOOLVOID pfnGoOnline)
{
	mpfnGoOnline = pfnGoOnline;
}

void SVIMServer::SetGetConfigurationName (PFN_BOOLCHAR pfnGetCurrentConfigPath)
{
	mpfnGetCurrentConfigPath = pfnGetCurrentConfigPath;
}

void SVIMServer::SetGetState (PFN_BOOLPDWORD pfnGetSVIMState)
{
	mpfnGetSVIMState = pfnGetSVIMState;
}

void SVIMServer::SetOpenConfiguration (PFN_BOOLCHAR pfnOpenConfiguration)
{
	mpfnOpenConfiguration = pfnOpenConfiguration;
}

void SVIMServer::SetSaveConfiguration(PFN_BOOLVOID pfnSaveConfiguration)
{
	mpfnSaveConfiguration = pfnSaveConfiguration;
}

void SVIMServer::SetCloseConfiguration(PFN_BOOLVOID pfnCloseConfiguration)
{
	mpfnCloseConfiguration = pfnCloseConfiguration;
}

BOOL SVIMServer::IsStarted ()
{
	return SVIMCommandServer::IsStarted();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIMServer\SVIMServer.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:07:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   11 Feb 2013 10:56:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method to use the new calling convention for the Packed File Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   15 Dec 2009 11:56:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   26 Jul 2001 09:26:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  218
 * SCR Title:  Fix issues involved with SVFocusNT pipe communications
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   -Updated OnBlockWriteComplete method to change attribute name.
 * -Updated SetupConnection method to change attribute name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   14 Jun 2001 13:23:10   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised OnGetSVIMConfig so that the configuration is saved and not closed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   12 Jun 2001 09:27:42   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised functionality of this pipes server to be consistent with the equivalent COM server.  
 * Modified the following functions:  OnPutSVIMConfig, OnSVIMData, OnGetSVIMConfig.
 * Revised the follwoing function names: from SetGetCurrentConfigPath to SetGetConfigurationName, from SetGetSVIMState to SetGetState, from SetOpenSECFile to SetOpenConfiguration, from SetSVIMSendConfigInit to SetSaveConfiguration.
 * Added the function SetCloseConfiguration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Feb 2001 16:22:16   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create a non-unicode version to operate with svobserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Feb 2001 08:55:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   SVIMServer will create directories if they don't already exist
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 May 2000 08:47:44   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added IsStarted() method so applications using this class can determine if the server is actively listening.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 May 2000 10:08:08   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added #include "SVMessage" to use standard error codes.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 May 2000 10:07:28   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added an #include "SVMessage.h" because this dependency was removed from SVException.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Apr 2000 09:04:02   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  9
 * SCR Title:  Baseline Development: Main Menu
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed #include search path to also search ..\SVMessage in all configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Apr 2000 07:48:12   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Corrected configuration file "grow" error caused by svimserver appending to an existing ".svf" file instead of overwriting it.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Apr 2000 15:44:58   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  2
 * SCR Title:  Baseline Development: Product Select
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix NULL file problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Apr 2000 15:09:36   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified OnSVData() to fix a bug in configuration upload (upload was appending to the .sec file instead of the .svf file.) 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Apr 2000 11:45:52   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Completed the PutConfig processing for SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Apr 2000 16:55:22   mike
 * Initial revision.
*/
