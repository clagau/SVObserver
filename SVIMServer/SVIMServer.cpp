// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMServer
// * .File Name       : $Workfile:   SVIMServer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Feb 2014 16:29:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIMServer.h"
#include "SVLibrary/SVPackedFile.h"
#include "SVMessage/SVMessage.h"
#include "SVCmnLib/utilities.h"
#include "SVStatusLibrary\MessageContainer.h"

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
	CString szPackedFile, configFilePath;
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
				cmdSize = static_cast<int>(strlen (Command.GetCommandText(SVDATA)));
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

			//global function to close config and clean up c:\run dir
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

			configFilePath = svPackedFile.getConfigFilePath();
			if(configFilePath.IsEmpty())
			{
#ifdef _DEBUG
				AfxMessageBox( _T( "No config file found." ) );
#endif
				return FALSE;
			}

			//load the config
			if(mpfnOpenConfiguration)
			{
				if(!mpfnOpenConfiguration((char*)((LPCTSTR)configFilePath)))return FALSE;
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

	//global function to close config and clean up c:\run dir
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

BOOL SVIMServer::OnWriteBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer)
{
	SVIMCommand Command;
	char *pTemp;
	int size, count, cmdlen;
	CString szErrorMsg;

	if ( SVMSG_PIPES_NO_ERROR == psvException->getMessage().m_MessageCode )
	{
		if (mpFile)
		{
			strcpy (pBuffer, Command.GetCommandText (SVDATA));
			cmdlen = static_cast<int>(strlen (pBuffer));
			size = cbBuffer - cmdlen;
			pTemp = pBuffer + cmdlen;

			if (count = mpFile->Read (pTemp, size))
			{
				if (!mpConnection->Write (pBuffer, count + cmdlen))
				{
					szErrorMsg.Format (_T("Write Failed"));
					psvException->setMessage (SVMSG_PIPES_WRITE_FAILED, szErrorMsg, StdMessageParams);
				}
				else
				{
					return TRUE;
				}
			}
		}
	}

	if ( SVMSG_PIPES_NO_ERROR != psvException->getMessage().m_MessageCode )
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

BOOL SVIMServer::OnReadBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer)
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

