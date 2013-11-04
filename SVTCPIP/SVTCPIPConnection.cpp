// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTCPIPConnection
// * .File Name       : $Workfile:   SVTCPIPConnection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 18:11:54  $
// ******************************************************************************

#include "stdafx.h"
#include "SVTCPIPConnection.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVLibrary/SVLibrary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVTCPIPConnection::SVTCPIPConnection()
{
  mPrimaryRC = 0;
}

SVTCPIPConnection::~SVTCPIPConnection()
{
	TRACE (_T("SVTCPIPConnection: ~SVTCPIPConnection Destructing\n"));
}

BOOL SVTCPIPConnection::SetupConnection(CString& szConnection)
{
  return FALSE;
}

BOOL SVTCPIPConnection::Open(CString& szConnection, DWORD dwFlags, SV_IOCOMPLETEPROC pfnCompletion, void *pUserData, DWORD dwTimeOut)
{
  int Port;
  CString szHost;

  if (!GetHost (szConnection, szHost))
  {
    if (!SetupConnection (szConnection))
    {
      return FALSE;
    }
  }

  Port = GetPort (szConnection);

  if (dwFlags & Client)
  {
    if (!Create (0))
    {
      mPrimaryRC = GetLastError ();
      return FALSE;
    }

    if (!Connect (szHost, Port))
    {
      mPrimaryRC = GetLastError ();
      return FALSE;
    }
  }
  else
  {
    if (!Create (Port))
    {
      mPrimaryRC = GetLastError ();
      return FALSE;
    }

    if (!Listen ())
    {
      mPrimaryRC = GetLastError ();
      return FALSE;
    }
  }

  if (dwFlags & Asynchronous)
  {
    if (!AsyncSelect ())
    {
      mPrimaryRC = GetLastError ();
      return FALSE;
    }
  }
  return TRUE;
}

void SVTCPIPConnection::Close()
{
  CAsyncSocket::Close ();
  return;
}

BOOL SVTCPIPConnection::Read(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut)
{
  Receive (pDataBuffer, cbDataBuffer);

  return TRUE;
}

BOOL SVTCPIPConnection::Write(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut)
{
  Send (pDataBuffer, cbDataBuffer);

  return TRUE;
}

int SVTCPIPConnection::GetPort(CString &szConnection)
{
  SVRegistryClass reg(GetRegistryKey (szConnection));
  DWORD dwPort;
  CString szPort = _T("Port");

  if (reg.GetRegistryValue (szPort, &dwPort))
  {
    return (int) dwPort;
  }
  else
  {
    return 0;
  }
}

BOOL SVTCPIPConnection::GetHost(CString &szConnection, CString &szHost)
{
  SVRegistryClass reg(GetRegistryKey (szConnection));
	SVString l_Host;

  BOOL l_Status = reg.GetRegistryValue( _T("Host"), l_Host );

	szHost = l_Host.ToString();

	return l_Status;
}

CString SVTCPIPConnection::GetRegistryKey(CString &szConnection)
{
  szConnectionKey = _T("\\SVIPC\\Connections\\");
  szConnectionKey += szConnection;
  szConnectionKey += _T("\\TCPIP");

  return szConnectionKey;
}

void SVTCPIPConnection::OnReceive(int nErrorCode)
{
  mPrimaryRC = nErrorCode;
}

void SVTCPIPConnection::OnSend (int nErrorCode)
{
  mPrimaryRC = nErrorCode;
}

void SVTCPIPConnection::OnAccept (int nErrorCode)
{
  mPrimaryRC = nErrorCode;
}

void SVTCPIPConnection::OnConnect (int nErrorCode)
{
  mPrimaryRC = nErrorCode;
}

void SVTCPIPConnection::OnClose (int nErrorCode)
{
  mPrimaryRC = nErrorCode;
}

DWORD SVTCPIPConnection::GetState()
{
  return 0;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTCPIP\SVTCPIPConnection.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:11:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 10:58:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 08:41:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jun 2001 13:54:22   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised Include directives.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Mar 23 2000 16:40:58   mike
 * Removed "SV_SHUTDOWN" callback message
 * 
 *    Rev 1.0   Mar 22 2000 17:33:12   mike
 * Initial revision.
*/
