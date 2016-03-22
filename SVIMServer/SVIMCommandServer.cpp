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
//Moved to precompiled header: #include <stdio.h>
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

