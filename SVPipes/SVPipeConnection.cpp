// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPipeConnection
// * .File Name       : $Workfile:   SVPipeConnection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 14:01:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPipeConnection.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVLibrary/SVLibrary.h"
#include "SVPipesConnectionDlg.h"
#include "SVIPC/SVConnectionInterface.h"
#include "SVMessage/SVMessage.h"
#include "SVSystemLibrary/SVCrash.h"

SVPipeConnection::SVPipeConnection ()
{
  mpfnCompletion = NULL;
  mszConnectionKey.Empty();
  mhPipe = (HANDLE) NULL;
	mhThread = (HANDLE) NULL;
  SetState (UNINITIALIZED);
	mThreadSyncEvent.ResetEvent ();
	TRACE (_T("SVPipeConnection: SVPipeConnection Constructed (Thread = 0x%08.8x)\n"), GetCurrentThreadId());
}

SVPipeConnection::~SVPipeConnection ()
{
	TRACE (_T("SVPipeConnection: ~SVPipeConnection Destructing (Thread = 0x%08.8x)\n"), GetCurrentThreadId());
	mpfnCompletion = NULL;
	Close (TRUE);

	if ( mhThread )
	{
		TerminateThread( mhThread, 0 );
	}
}

DWORD CALLBACK ConnectThread (LPVOID lpData)
{
	SetThreadType( PIPECONNECTTHREAD );

	TRACE (_T("SVPipeConnection: ConnectThread (0x%08.8x) -- Waiting for client to connect\n"), GetCurrentThreadId());
	SVPipeConnection *pConnection = (SVPipeConnection *) lpData;
	DWORD dwRc = SVMSG_PIPES_NO_ERROR;
	
	pConnection->SetState (CONNECTWAIT);
	pConnection->GetThreadWaitEvent()->SetEvent();
	
	if (ConnectNamedPipe (pConnection->GetPipeHandle (), NULL))
	{
		pConnection->SetState (CONNECTED);
	}
	else
	{
		DWORD dwError = GetLastError();
		if( ERROR_PIPE_CONNECTED == dwError )
		{
			pConnection->SetState (CONNECTED);
		}
		else
		{
			dwRc = SVMSG_PIPES_IO_ERROR;
			pConnection->SetState (UNINITIALIZED);
		}
	}
	pConnection->SetThread (NULL);
	if (pConnection->GetCompletionProc())
	{
		(pConnection->GetCompletionProc()) (pConnection, dwRc, NULL, 0, SV_OPEN, pConnection->GetUserData ());
	}
	return dwRc;
}

DWORD CALLBACK OpenThread (LPVOID lpData)
{
	SetThreadType( PIPEOPENTHREAD );
	
	TRACE (_T("SVPipeConnection: OpenThread (0x%08.8x)-- Connecting to server\n"), GetCurrentThreadId());
	SVPipeConnection *pConnection = (SVPipeConnection *) lpData;
	DWORD dwRc = SVMSG_PIPES_NO_ERROR;
	
	pConnection->SetState (CONNECTED);
	pConnection->GetThreadWaitEvent()->SetEvent();
	pConnection->SetThread (NULL);
	if (pConnection->GetCompletionProc())
	{
		(pConnection->GetCompletionProc()) (pConnection, SVMSG_PIPES_NO_ERROR, NULL, 0, SV_OPEN, pConnection->GetUserData ());
	}
	
	return dwRc;
}

BOOL SVPipeConnection::Open (CString &szConnection, DWORD dwFlags, SV_IOCOMPLETEPROC pfnCompletion, void *pUserData, DWORD dwTimeOut)
{
	TRACE (_T("SVPipeConnection: Open %s (Thread = 0x%08.8x)\n"), szConnection, GetCurrentThreadId());
  CString szServer;
  CString szPipe;
  CString szServerPipe;
	DWORD dwOSError = ERROR_PIPE_BUSY, dwRetries = 10;

  if (GetState () != UNINITIALIZED)
  {
    return FALSE;
  }

  mpfnCompletion = pfnCompletion;
  mpUserData = pUserData;
  mdwFlags = dwFlags;

  if (!GetServerName (szConnection, szServer))
  {
    if (!SetupConnection (szConnection))
    {
      return FALSE;
    }
  }

  GetPipeName (szConnection, szPipe);

  if (dwFlags & Client)
  {
    szServerPipe.Format (_T("%s\\pipe\\%s"), szServer, szPipe);

		while ((dwOSError == ERROR_PIPE_BUSY) && (dwRetries))
		{
			mhPipe = CreateFile (szServerPipe,
													GENERIC_READ | GENERIC_WRITE,
													0,
													NULL,
													OPEN_EXISTING,
													FILE_ATTRIBUTE_NORMAL,
													NULL);
			dwOSError = GetLastError();
			dwRetries--;
			if (dwOSError == ERROR_PIPE_BUSY)
			{
				Sleep (5000);
			}
		}

    if (mhPipe == INVALID_HANDLE_VALUE)
    {
      mhPipe = NULL;
      return FALSE;
    }
    else
    {
      if (!(mhThread = CreateThread (NULL, 0, OpenThread, this, 0, NULL)))
      {
        CloseHandle (mhPipe);
        mhPipe = NULL;
        return FALSE;
      }
			WaitThreadStart ();
    }
  }
  else
  {
		SECURITY_ATTRIBUTES sa;
		PSECURITY_DESCRIPTOR psd;

    szServerPipe.Format (_T("\\\\.\\pipe\\%s"), szPipe);

		psd =  (PSECURITY_DESCRIPTOR) new BYTE[SECURITY_DESCRIPTOR_MIN_LENGTH];
		InitializeSecurityDescriptor (psd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl (psd, TRUE, NULL, FALSE);

		sa.nLength = sizeof (SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = psd;
		sa.bInheritHandle = TRUE;

    mhPipe = CreateNamedPipe (szServerPipe, 
                              PIPE_ACCESS_DUPLEX,
                              PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                              PIPE_UNLIMITED_INSTANCES,
                              4096, 
                              4096,
                              5,
                              &sa);

//JAB103008		delete (BYTE *)psd;
	delete [] psd;
    if (mhPipe == INVALID_HANDLE_VALUE)
    {
      mhPipe = NULL;
      return FALSE;
    }
    else
    {
      if (!(mhThread = CreateThread (NULL, 0, ConnectThread, this, 0, NULL)))
      {
        CloseHandle (mhPipe);
        mhPipe = NULL;
        return FALSE;
      }
			WaitThreadStart();
    }
  }
  return TRUE;
}

DWORD CALLBACK ShutDownThread (LPVOID lpData)
{
	SetThreadType( PIPESHUTDOWNTHREAD );
	
	TRACE (_T("SVPipeConnection: ShutDownThread (0x%08.8x)\n"), GetCurrentThreadId());
	SVPipeConnection *pConnection = (SVPipeConnection *) lpData;
	
	pConnection->SetState (UNINITIALIZED);
	pConnection->GetThreadWaitEvent()->SetEvent();
	pConnection->SetThread (NULL);
	
	return 0;
}

DWORD CALLBACK CloseThread (LPVOID lpData)
{
	SetThreadType( PIPECLOSETHREAD );
	
	TRACE (_T("SVPipeConnection: CloseThread (0x%08.8x)\n"), GetCurrentThreadId());
	SVPipeConnection *pConnection = (SVPipeConnection *) lpData;
	
	pConnection->SetState (CLOSING);
	pConnection->GetThreadWaitEvent()->SetEvent();
	pConnection->SetState (UNINITIALIZED);
	pConnection->SetThread (NULL);
	
	if (pConnection->GetCompletionProc())
	{
		(pConnection->GetCompletionProc()) (pConnection, SVMSG_PIPES_NO_ERROR, NULL, 0, SV_CLOSE, pConnection->GetUserData ());
	}
	return 0;
}

void SVPipeConnection::Close(BOOL bDestroy)
{
	TRACE (_T("SVPipeConnection: Close with bDestroy = %d (Thread = 0x%08.8x)\n"), bDestroy, GetCurrentThreadId());
	DWORD dwState = GetState ();

	if (dwState == READING || dwState == WRITING || dwState == CONNECTWAIT || dwState == CONNECTED)
	{
		if (mhThread)
		{
			TerminateThread( mhThread, 0 );
			mhThread = NULL;
		}

		if (mhPipe)
		{
			if ( ! (mdwFlags & Client) )
			{
				DisconnectNamedPipe (mhPipe);
			}

			CloseHandle (mhPipe);

			mhPipe = NULL;
		}
	}

	if (bDestroy)
	{
	  mhThread = CreateThread (NULL, 0, ShutDownThread, this, 0, NULL);
	}
	else
	{
		mhThread = CreateThread (NULL, 0, CloseThread, this, 0, NULL);
	}

	if (mhThread)
	{
		WaitThreadStart();
	}

}

void SVPipeConnection::Close ()
{
	Close (FALSE);
}

DWORD CALLBACK ReadThread (LPVOID lpData)
{
	SetThreadType( PIPEREADTHREAD );
	
	TRACE (_T("SVPipeConnection: ReadThread (0x%08.8x)\n"), GetCurrentThreadId());
	SVPipeConnection *pConnection = (SVPipeConnection *) lpData;
	HANDLE hPipe = pConnection->GetPipeHandle ();
	void *pDataBuffer = pConnection->GetDataBuffer ();
	int cbDataBuffer = pConnection->GetDataBufferSize ();
	DWORD dwRc = SVMSG_PIPES_NO_ERROR;
	DWORD dwBytesRead;
	
	pConnection->SetState (READING);
	pConnection->GetThreadWaitEvent()->SetEvent();
	
	if (ReadFile (hPipe, pDataBuffer, cbDataBuffer, &dwBytesRead, NULL))
	{
		pConnection->SetState (CONNECTED);
	}
	else
	{
		dwRc = SVMSG_PIPES_IO_ERROR;
		if ( ((pConnection->GetFlags () & SVPipeConnection::Client) && (GetLastError() == ERROR_PIPE_NOT_CONNECTED)) ||
			((pConnection->GetFlags () & SVPipeConnection::Server) && (GetLastError() == ERROR_BROKEN_PIPE)) )
		{
			dwRc = SVMSG_PIPES_END_OF_FILE;
		}
		pConnection->SetState (UNINITIALIZED);
	}
	
	pConnection->SetThread (NULL);
	if (pConnection->GetCompletionProc())
	{
		(pConnection->GetCompletionProc()) (pConnection, dwRc, pDataBuffer, (int) dwBytesRead, SV_READ, pConnection->GetUserData ());
	}
	
	return dwRc;
}

BOOL SVPipeConnection::Read (void *pDataBuffer, int cbDataBuffer, DWORD dwTimeOut)
{
	TRACE (_T("SVPipeConnection: Read %d bytes (Thread = 0x%08.8x)\n"), cbDataBuffer, GetCurrentThreadId());
  if (GetState() != CONNECTED)
  {
    return FALSE;
  }

  mpDataBuffer = pDataBuffer;
  mcbDataBuffer = cbDataBuffer;

  if (!(mhThread = CreateThread (NULL, 0, ReadThread, this, 0, NULL)))
  {
    return FALSE;
  }
	WaitThreadStart();

  return TRUE;
}

DWORD CALLBACK WriteThread (LPVOID lpData)
{
	SetThreadType( PIPEWRITETHREAD );
	
	TRACE (_T("SVPipeConnection: WriteThread (0x%08.8x)\n"), GetCurrentThreadId());
	SVPipeConnection *pConnection = (SVPipeConnection *) lpData;
	HANDLE hPipe = pConnection->GetPipeHandle ();
	void *pDataBuffer = pConnection->GetDataBuffer ();
	int cbDataBuffer = pConnection->GetDataBufferSize ();
	DWORD dwRc = SVMSG_PIPES_NO_ERROR;
	DWORD dwBytesWritten;
	
	pConnection->SetState (WRITING);
	pConnection->GetThreadWaitEvent()->SetEvent();
	
	if (WriteFile (hPipe, pDataBuffer, cbDataBuffer, &dwBytesWritten, NULL))
	{
		FlushFileBuffers (hPipe);
		pConnection->SetState (CONNECTED);
	}
	else
	{
		dwRc = SVMSG_PIPES_IO_ERROR;
		DisconnectNamedPipe (hPipe);
		pConnection->SetState (UNINITIALIZED);
	}
	
	pConnection->SetThread (NULL);
	if (pConnection->GetCompletionProc())
	{
		(pConnection->GetCompletionProc()) (pConnection, dwRc, pDataBuffer, (int) dwBytesWritten, SV_WRITE, pConnection->GetUserData ());
	}
	
	return dwRc;
}

BOOL SVPipeConnection::Write (void *pDataBuffer, int cbDataBuffer, DWORD dwTimeOut)
{
	TRACE (_T("SVPipeConnection: Write %d bytes (Thread = 0x%08.8x)\n"), cbDataBuffer, GetCurrentThreadId());
  if (GetState() != CONNECTED)
  {
    return FALSE;
  }

  mpDataBuffer = pDataBuffer;
  mcbDataBuffer = cbDataBuffer;

  if (!(mhThread = CreateThread (NULL, 0, WriteThread, this, 0, NULL)))
  {
    return FALSE;
  }
	WaitThreadStart();

  return TRUE;
}

CString SVPipeConnection::GetRegistryKey (CString &szConnection)
{
  mszConnectionKey = _T("\\SVIPC\\Connections\\");
  mszConnectionKey += szConnection;
  mszConnectionKey += _T("\\NamedPipes");

  return mszConnectionKey;
}

BOOL SVPipeConnection::GetServerName (CString &szConnection, CString &szServer)
{
  SVRegistryClass reg(GetRegistryKey (szConnection));
	SVString l_Server;

  BOOL l_Status = reg.GetRegistryValue(_T("Server"), l_Server);

	szServer = l_Server.ToString();

	return l_Status;
}

BOOL SVPipeConnection::GetPipeName (CString &szConnection, CString &szPipe)
{
  SVRegistryClass reg(GetRegistryKey (szConnection));
	SVString l_Pipe;

	BOOL l_Status = reg.GetRegistryValue(_T("PipeName"), l_Pipe);

	szPipe = l_Pipe.ToString();

	return l_Status;
}

BOOL SVPipeConnection::SetupConnection(CString &szConnection)
{
  SVPipesConnectionDlg dlg (szConnection);

  return (dlg.DoModal () == IDOK);
}

HANDLE SVPipeConnection::GetPipeHandle()
{
  return mhPipe;
}

SV_IOCOMPLETEPROC SVPipeConnection::GetCompletionProc()
{
  return mpfnCompletion;
}

void * SVPipeConnection::GetUserData()
{
  return mpUserData;
}

DWORD SVPipeConnection::GetState()
{
  return mdwState;
}

DWORD SVPipeConnection::SetState(DWORD dwNewState)
{
  mdwState = dwNewState;
  return mdwState;
}

void SVPipeConnection::SetThread(HANDLE hThread)
{
	CloseHandle( mhThread );
  mhThread = hThread;
}

void * SVPipeConnection::GetDataBuffer()
{
  return mpDataBuffer;
}

int SVPipeConnection::GetDataBufferSize()
{
  return mcbDataBuffer;
}

void SVPipeConnection::WaitThreadStart()
{
	mThreadSyncEvent.Lock();
	mThreadSyncEvent.Unlock();
}

CEvent* SVPipeConnection::GetThreadWaitEvent()
{
	return &mThreadSyncEvent;
}

DWORD SVPipeConnection::GetFlags()
{
	return mdwFlags;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVPipes\SVPipeConnection.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 14:01:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Dec 2009 10:51:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Sep 2009 08:36:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Nov 2008 15:58:36   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Replaced delete (BYTE *)psd with delete [] psd within
 * SVPipeConnection::Open ().
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Oct 2001 09:46:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  228
 * SCR Title:  Add context information to help the debugging of multi-threaded code
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to store thread type identifiers in the TIB block for each thread that is run.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Oct 2001 09:46:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  228
 * SCR Title:  Add context information to help the debugging of multi-threaded code
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to store thread type identifiers in the TIB block for each thread that is run.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Jul 2001 09:19:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  218
 * SCR Title:  Fix issues involved with SVFocusNT pipe communications
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   -Updated constructor to set mhThread to NULL.
 * -Updated destructor to terminate last running thread.
 * -Updated ConnectThread run function to verify if the thread is already connected.  If it is do not return an error, continue normal processing.
 * -Updated OpenThread run function and moved SetThread to before the GetCompleteProc.
 * -Updated ShutDownThread run function and moved SetThread function to after setting the thread start event.
 * -Updated Close method to clean up pipe handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Feb 2001 16:24:40   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create a non-unicode version to operate with svobserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Aug 2000 12:18:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  23
 * SCR Title:  Bug fixes 1.0
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Put retry logic in the Open function for client access.  This will cause the program to retry for up to 50 seconds if "pipe busy" is returned during the CreatePipe call.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jul 2000 17:14:28   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  25
 * SCR Title:  Download OCR files
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified the Open() function to add a NULL ACE to the named pipe when it is created so that anyone can connect to it.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 May 2000 10:11:26   mike
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
 *    Rev 1.1   Mar 23 2000 16:38:26   mike
 * Removed "SV_SHUTDOWN" callback message
 * 
 *    Rev 1.0   Mar 22 2000 17:29:28   mike
 * Initial revision.
*/
