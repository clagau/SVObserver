// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMCommandServer
// * .File Name       : $Workfile:   SVIMCommandServer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   01 Dec 2014 12:19:38  $
// ******************************************************************************

#include "stdafx.h"
#include <stdio.h>
#include "SVIMCommandServer.h"
#include "SVIMCommand/SVIMCommand.h"
#include "SVLibrary/SVLibrary.h"
#include "SVMessage/SVMessage.h"
#include "SVCmnLib/utilities.h"
#include "SVSystemLibrary/SVCrash.h"
#include "SVSystemLibrary/SVThreadManager.h"

BOOL CALLBACK SVIMServerIOCompleteProc (void *pVoidConnection, int nErrorCode, void *pBuffer, int cbBuffer, int Operation, void *pUserData)
{
//	SVConnectionControl *pConnection = (SVConnectionControl *) pVoidConnection;
	SVPipeConnection *pConnection = (SVPipeConnection *) pVoidConnection;
	SVIMCommandServer *pServer = (SVIMCommandServer *) pUserData;
	SVIMCommand Command;
	DWORD dwSVIMState;
	BOOL rc = FALSE;
	char *pTemp, *pData;
	CString szConnection, szErrorMsg, szIMCSVIMConfigName;

	USES_CONVERSION;

	szErrorMsg.Format (_T("SVIMServerIOCompleteProc: Operation = %d"), Operation);
	pServer->GetLastSVIMError()->setMessage (nErrorCode, szErrorMsg, StdMessageParams);
	TRACE (_T("SVIMServerIOCompleteProc: Operation = %04.4x, ErrorCode = %08.8x\n"), Operation, nErrorCode);

	switch (Operation)
	{
		case SV_OPEN :
			if (nErrorCode == SVMSG_PIPES_NO_ERROR)
			{
				if (!pConnection->Read (pServer->GetReadDataBuffer(), SVIOBUFFSIZE))
				{
					szErrorMsg.Format (_T("Read Failed"));
					pServer->GetLastSVIMError()->setMessage (SVMSG_PIPES_READ_FAILED, szErrorMsg, StdMessageParams);
					pServer->OnSVIMError (pServer->GetLastSVIMError());
					pConnection->Close();
				}
			}
			else
			{
				pServer->OnSVIMError (pServer->GetLastSVIMError());
				pConnection->Close();
			}
			
			break;

		case SV_READ :
			TRACE (_T("SVIMServerIOCompleteProc (READ): Count = %d bytes\n"), cbBuffer);
			switch (nErrorCode)
			{
				case SVMSG_PIPES_NO_ERROR :
					memset (pServer->GetWriteDataBuffer(), 0, SVIOBUFFSIZE);
					switch (Command.GetCommandToken ((char *) pBuffer))
					{
						case SVPUTCONFIG :
							strcpy_s (pServer->GetWriteDataBuffer(), SVIOBUFFSIZE, (char *) pBuffer);
							if (pTemp = strstr ((char *) pBuffer, "SVIMFile="))
							{
								pTemp += 9;
								Command.unescape (pTemp, (char *) pBuffer, SVIOBUFFSIZE);
								rc = pServer->OnPutSVIMConfig (CString (A2T((char *) pBuffer)));
							}
							else
							{
								rc = FALSE;
							}
							break;

						case SVDATA :
							if (pServer->OnSVIMData ((char *) pBuffer, cbBuffer))
							{
								if (!pConnection->Read (pServer->GetReadDataBuffer(), SVIOBUFFSIZE))
								{
									szErrorMsg.Format (_T("Read Failed"));
									pServer->GetLastSVIMError()->setMessage (SVMSG_PIPES_READ_FAILED, szErrorMsg, StdMessageParams);
									pServer->OnSVIMError (pServer->GetLastSVIMError());
									pConnection->Close();
								}
							}
							else
							{
								pConnection->Close();
							}
							return TRUE;
							break;

						case SVGETCONFIG :
							strcpy_s (pServer->GetWriteDataBuffer(), SVIOBUFFSIZE, (char *) pBuffer);
							if (pTemp = strstr ((char *)pBuffer, "SVIMFile="))
							{
								pTemp += 9;
								Command.unescape (pTemp, (char *) pBuffer, SVIOBUFFSIZE);
								rc = pServer->OnGetSVIMConfig (CString (A2T((char *) pBuffer)));
							}
							else
							{
								rc = FALSE;
							}
							break;

						case SVGETCURRENTCONFIG :
							pTemp = pServer->GetWriteDataBuffer();
							strcpy_s (pTemp, SVIOBUFFSIZE, (char *) pBuffer);
							if (rc = pServer->OnGetCurrentSVIMConfig (szIMCSVIMConfigName))
							{
								strcat_s (pTemp, SVIOBUFFSIZE, " SVIMFile=");
								int i = static_cast<int>(strlen (pTemp));
								Command.escape (T2A((TCHAR *)(LPCTSTR)szIMCSVIMConfigName), &pTemp[i], SVIOBUFFSIZE - i);
							}
							break;

						case SVSETCURRENTCONFIG :
							strcpy_s (pServer->GetWriteDataBuffer(), SVIOBUFFSIZE, (char *) pBuffer);
							if (pTemp = strstr ((char *) pBuffer, "SVIMFile="))
							{
								pTemp += 9;
								Command.unescape (pTemp, (char *) pBuffer, SVIOBUFFSIZE);
//								rc = pServer->OnSetCurrentSVIMConfig (CString (A2T((char *) pBuffer)));
							}
							else
							{
								rc = FALSE;
							}
							break;

						case SVGOOFFLINE :
							if (rc = pServer->OnSVIMGoOffline ())
							{
								if (rc = pServer->OnGetSVIMState (&dwSVIMState))
								{
									pTemp = pServer->GetWriteDataBuffer();
									sprintf_s (pTemp, SVIOBUFFSIZE, "%s State=%lu", Command.GetCommandText (SVGOOFFLINE), dwSVIMState);
								}
							}
							break;

						case SVGOONLINE :
							if (rc = pServer->OnSVIMGoOnline ())
							{
								if (rc = pServer->OnGetSVIMState (&dwSVIMState))
								{
									pTemp = pServer->GetWriteDataBuffer();
									sprintf_s (pTemp, SVIOBUFFSIZE, "%s State=%lu", Command.GetCommandText (SVGOONLINE), dwSVIMState);
								}
							}
							break;

						case SVGETSTATE :
							if (rc = pServer->OnGetSVIMState (&dwSVIMState))
							{
								pTemp = pServer->GetWriteDataBuffer();
								sprintf_s (pTemp, SVIOBUFFSIZE, "%s State=%lu", Command.GetCommandText (SVGETSTATE), dwSVIMState);
							}
							break;

						case SVPUTFILE :
							strcpy_s (pServer->GetWriteDataBuffer(), SVIOBUFFSIZE, (char *) pBuffer);
							pTemp = ((char *) pBuffer) + strlen (Command.GetCommandText (SVPUTFILE));
							for (pData = pTemp; *pData && *pData != '='; pData++);
							if (*pData == '=')
							{
								*pData = 0;
								pData++;
								Command.unescape (pTemp, pTemp, static_cast<int>(strlen (pTemp)));
								Command.unescape (pData, pData, static_cast<int>(strlen (pData)));
								rc = pServer->OnPutFile (pTemp, pData);
							}
							else
							{
								rc = FALSE;
							}
							break;

						default :
							rc = FALSE;
							break;
					}

					if (!rc) // cancel command if not ready.
					{
						strcpy_s (pServer->GetWriteDataBuffer(), SVIOBUFFSIZE, Command.GetCommandText (SVCANCEL));
					}

					if (!pConnection->Write (pServer->GetWriteDataBuffer(), static_cast<int>(strlen (pServer->GetWriteDataBuffer()) + 1)))
					{
						szErrorMsg.Format (_T("Write Failed"));
						pServer->GetLastSVIMError()->setMessage (SVMSG_PIPES_WRITE_FAILED, szErrorMsg, StdMessageParams);
						pServer->OnSVIMError (pServer->GetLastSVIMError());
						pConnection->Close();
					}
					break;

				case SVMSG_PIPES_END_OF_FILE :
					pServer->OnSVIMData ((char *)pBuffer, 0);  // signal receive complete
					pConnection->Close ();
					break;

				default :
					pServer->OnSVIMError (pServer->GetLastSVIMError());
					pConnection->Close ();
					break;
			}
			break;

		case SV_WRITE :
			TRACE (_T("SVIMServerIOCompleteProc (WRITE): Count = %d bytes\n"), cbBuffer);
			switch (Command.GetCommandToken((char *) pBuffer))
			{
				case SVGETCURRENTCONFIG :
				case SVGETCONFIG :
				case SVDATA :
					if (!pServer->OnWriteBlockComplete (pServer->GetLastSVIMError(), (char *) pBuffer, SVIOBUFFSIZE))
					{
						pConnection->Close ();
					}
					break;

				case SVPUTCONFIG :
					if (nErrorCode != SVMSG_PIPES_NO_ERROR)
					{
						pServer->OnSVIMError (pServer->GetLastSVIMError());
						pConnection->Close();
					}
					else
					{
						if (!pConnection->Read (pServer->GetReadDataBuffer(), SVIOBUFFSIZE))
						{
							szErrorMsg.Format (_T("Read Failed"));
							pServer->GetLastSVIMError()->setMessage (SVMSG_PIPES_READ_FAILED, szErrorMsg, StdMessageParams);
							pServer->OnSVIMError (pServer->GetLastSVIMError());
							pConnection->Close();
						}
					}
					break;

				default :
					if (nErrorCode != SVMSG_PIPES_NO_ERROR)
					{
						pServer->OnSVIMError (pServer->GetLastSVIMError());
						pConnection->Close();
					}
					else
					{
						pConnection->Close();
					}
					break;
			}
			break;

		case SV_CLOSE :
			pServer->GetConnectionName(szConnection);
			if (!pConnection->Open (szConnection, SVPipeConnection::Server, SVIMServerIOCompleteProc, pUserData))
			{
				szErrorMsg.Format (_T("Open Failed"));
				pServer->GetLastSVIMError()->setMessage (SVMSG_PIPES_OPEN_FAILED, szErrorMsg, StdMessageParams);
				pServer->OnSVIMError (pServer->GetLastSVIMError());
			}
			break;
	}

	return TRUE;
}

//##ModelId=38B56AEC0177
SVIMCommandServer::SVIMCommandServer(CString & szConnection)
{
	mpConnection = NULL;
	mszConnection.Format (_T("%s"), szConnection);
	CString szErrorMsg;

	mbStarted = TRUE;
	mpConnection = new SVPipeConnection();
	if ( mpConnection == NULL )
	{
		szErrorMsg.Format (_T("Open of %s Failed"), mszConnection);
		mSVIMLastException.setMessage (SVMSG_PIPES_OPEN_FAILED, szErrorMsg, StdMessageParams);
		mbStarted = FALSE;
	}

	if (!mpConnection->Open (mszConnection, SVPipeConnection::Server, SVIMServerIOCompleteProc, this))
	{
		szErrorMsg.Format (_T("Open of %s Failed"), mszConnection);
		mSVIMLastException.setMessage (SVMSG_PIPES_OPEN_FAILED, szErrorMsg, StdMessageParams);
		mbStarted = FALSE;
	}
}

DWORD CALLBACK DestroyPipeThread (LPVOID lpData)
{
	SetThreadType( DESTROYPIPETHREAD );
	
	TRACE (_T("SVSVIMCommandServer: DestroyPipeThread (0x%08.8x)\n"), GetCurrentThreadId());
	SVPipeConnection *pConnection = (SVPipeConnection *) lpData;
	
	delete pConnection;
	
	return 0;
}

//##ModelId=38B56AEC0167
SVIMCommandServer::~SVIMCommandServer()
{
	TRACE (_T("SVIMCommandServer Destructing\n"));

	HANDLE mhThread = CreateThread (NULL, 0, DestroyPipeThread, mpConnection, 0, NULL);
	SVThreadManager::Instance().Add(mhThread, "SVIM Server");
	CloseHandle( mhThread );
}

//##ModelId=38B56AEC0158
BOOL SVIMCommandServer::OnGetSVIMState(DWORD *pdwSVIMState)
{
	return FALSE;
}

//##ModelId=38B56AEC0157
BOOL SVIMCommandServer::OnSVIMGoOnline()
{
	return FALSE;
}

//##ModelId=38B56AEC013A
BOOL SVIMCommandServer::OnSVIMGoOffline()
{
	return FALSE;
}

//##ModelId=38B56AEC0139
BOOL SVIMCommandServer::OnGetCurrentSVIMConfig(CString &szIMCSVIMConfigName)
{
	return FALSE;
}

//##ModelId=38B56AEC0138
BOOL SVIMCommandServer::OnGetSVIMConfig(CString &szIMCSVIMConfigName)
{
	return FALSE;
}

//##ModelId=38B56AEC00FA
BOOL SVIMCommandServer::OnPutSVIMConfig(CString &szIMCSVIMConfigName)
{
	return FALSE;
}

//##ModelId=38D67880009C
BOOL SVIMCommandServer::OnSVIMData(char * pBuffer, UINT NumBytes)
{
	return FALSE;
}

//##ModelId=38BD7DB100AB
CEvent * SVIMCommandServer::GetCommandCompleteEvent()
{
	return &mCommandCompleteEvent;
}

//##ModelId=38BE5D1E0000
BOOL SVIMCommandServer::OnSVIMError(SvStl::MessageContainer* pSVException)
{
#ifdef DEBUG
	SVString szMsg;

	pSVException->Format (szMsg);
	AfxMessageBox (szMsg.c_str(), IDOK, 0);
#endif

	pSVException->logMessage();
	return FALSE;
}

//##ModelId=38BE951D03C8
SvStl::MessageContainer* SVIMCommandServer::GetLastSVIMError()
{
	return &mSVIMLastException;
}

//##ModelId=38BE981E01B5
void SVIMCommandServer::OnSVIMReceiveComplete(CString & szReceiveFileName)
{
	// TODO: Add your specialized code here.
}

//##ModelId=38BE981E0232
void SVIMCommandServer::OnSVIMSendComplete(CString & szSendFileName)
{
	// TODO: Add your specialized code here.
}

//##ModelId=38BE992903D8
char * SVIMCommandServer::GetWriteDataBuffer()
{
	return mWriteDataBuffer;
}

//##ModelId=38C3D35A00FA
char* SVIMCommandServer::GetReadDataBuffer()
{
	return mReadDataBuffer;
}

//##ModelId=38C7E38F0399
BOOL SVIMCommandServer::GetConnectionName(CString &szConnection)
{
	szConnection.Format (_T("%s"), mszConnection);
	return (!mszConnection.IsEmpty());
}

//##ModelId=38D78F590280
BOOL SVIMCommandServer::OnWriteBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer)
{
	return FALSE;
}

//##ModelId=38D78F830290
BOOL SVIMCommandServer::OnReadBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer)
{
	return FALSE;
}

//##ModelId=392AE70B005D
BOOL SVIMCommandServer::IsStarted()
{
	return mbStarted;
}

//##ModelId=398597C30251
BOOL SVIMCommandServer::OnPutFile(char* szFileName, char * pData  )
{
	BOOL bOk = FALSE;

	CString csPathName;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s( szFileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );

	csPathName = drive;
	csPathName += dir;
	csPathName.Replace( _T( '/' ), _T( '\\' ) );

	if ( csPathName.IsEmpty() )
	{
		csPathName = _T( "C:\\RUN\\" );
	}

	if ( CreateDirPath( csPathName ) )
	{
		FILE *pFile;

		if ( fopen_s ( &pFile, szFileName, "w") == 0 )
		{
			fprintf (pFile, "%s", pData);
			fclose (pFile);

			bOk = TRUE;

			if ( csPathName.CompareNoCase( _T( "C:\\RUN\\" ) ) != 0 )
			{
				CString csRunName;

				csRunName.Format( _T( "C:\\RUN\\%s%s" ), fname, ext );

				bOk = CopyFile( szFileName, csRunName, FALSE );
			}
		}
	}

	return bOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIMServer\SVIMCommandServer.cpp_v  $
 * 
 *    Rev 1.2   01 Dec 2014 12:19:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread names for SVThreadManager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 09:05:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 12:07:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   15 Dec 2009 11:59:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   21 Sep 2009 13:46:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix a change in string type used for exception.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Jun 2008 14:47:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Moved StdAfx.h up above stdio.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   09 Jun 2008 09:49:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Replaced depricated functions to use _s safe functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   05 Oct 2001 09:56:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  228
 * SCR Title:  Add context information to help the debugging of multi-threaded code
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to store thread type identifiers in the TIB block for each thread that is run.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   26 Jul 2001 09:37:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  218
 * SCR Title:  Fix issues involved with SVFocusNT pipe communications
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   -Updated constructor to include necessary operation to construct a new instance of the SVPipeConectionClass variable.
 * -Added DestroyPipeThread run function to clean up the SVPipeConnectionClass.
 * -Updated destructor to allow a thread to destroy the SVPipeConnectionClass attribute.
 * -Updated onPutFile method to handle missing paths and new file handling system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Feb 2001 16:21:16   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create a non-unicode version to operate with svobserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   31 Jul 2000 17:06:26   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  25
 * SCR Title:  Download OCR files
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added new SVIM command "PutFile" that writes a single line of data to the specified file.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   24 May 2000 08:46:46   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added IsStarted() method so applications using this class can determine if the server is actively listening.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   23 May 2000 17:15:26   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  12
 * SCR Title:  Stage 2: Error/Alarm Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Corrected error handling in constructor.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 May 2000 10:09:46   mike
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
 *    Rev 1.4   03 May 2000 10:02:36   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Eliminated dependency on the SVConnectionControl class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Apr 2000 09:02:50   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  9
 * SCR Title:  Baseline Development: Main Menu
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed #include search path to also search ..\SVMessage in all configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Apr 2000 11:44:24   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Fixed TRACE statements that exceeded the text output limit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Mar 23 2000 16:07:40   mike
 * Removed references to "SV_SHUTDOWN".
 * 
 *    Rev 1.0   Mar 22 2000 17:23:40   mike
 * Initial revision.
*/