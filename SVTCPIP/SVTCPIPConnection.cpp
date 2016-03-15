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

	szHost = l_Host.c_str();

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

