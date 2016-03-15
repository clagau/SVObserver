// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPipeConnection
// * .File Name       : $Workfile:   SVPipeConnection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Dec 2014 13:43:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPipeConnection.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVLibrary/SVLibrary.h"
#include "SVPipesConnectionDlg.h"
#include "SVIPC/SVConnectionInterface.h"
#include "SVMessage/SVMessage.h"
#include "SVSystemLibrary/SVCrash.h"
#include "SVSystemLibrary/SVThreadManager.h"

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
		SVThreadManager::Instance().Remove( mhThread);
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
		  SVThreadManager::Instance().Add( mhThread, _T("Pipe Thread..SVFocus"));
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
		  SVThreadManager::Instance().Add( mhThread, _T("Pipe Thread..SVFocus"));
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
			SVThreadManager::Instance().Remove( mhThread );
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
	  SVThreadManager::Instance().Add( mhThread, _T("Pipe Thread Shutdown"));
	}
	else
	{
		mhThread = CreateThread (NULL, 0, CloseThread, this, 0, NULL);
		SVThreadManager::Instance().Add( mhThread, _T("Pipe Thread Close"));
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
	SVThreadManager::Instance().Add( mhThread, _T("Pipe Thread Read"));
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
	SVThreadManager::Instance().Add( mhThread, _T("Pipe Thread Write"));
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

	szServer = l_Server.c_str();

	return l_Status;
}

BOOL SVPipeConnection::GetPipeName (CString &szConnection, CString &szPipe)
{
  SVRegistryClass reg(GetRegistryKey (szConnection));
	SVString l_Pipe;

	BOOL l_Status = reg.GetRegistryValue(_T("PipeName"), l_Pipe);

	szPipe = l_Pipe.c_str();

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

