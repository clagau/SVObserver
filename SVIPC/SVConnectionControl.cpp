// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVConnectionControl
// * .File Name       : $Workfile:   SVConnectionControl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:09:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVConnectionControl.h"
#include "resource.h"
#include "SVConnectionControlDlg.h"
#include "SVLibrary/SVLibrary.h"
#include "SVConnectionInterface.h"

SVConnectionControl::SVConnectionControl()
{
  mdwConnectionType = NoProtocol;
  mszConnectionTypeName = _T("Active Protocol");
}

SVConnectionControl::~SVConnectionControl()
{
	TRACE (_T("SVIMConnectionControl Destructing\n"));
  mdwConnectionType = NoProtocol;
}

int SVConnectionControl::GetConnectionTypeDlg(SVRegistryClass *pConnectionKey, CString &szConnectionTypeName)
{
  SVConnectionControlDlg dlg;

  if (!pConnectionKey->GetRegistryValue (szConnectionTypeName, &dlg.mSelection))
  {
    dlg.mSelection = NoProtocol;
  }

  dlg.DoModal ();

  if (dlg.mSelection != NoProtocol)
    pConnectionKey->SetRegistryValue (szConnectionTypeName, dlg.mSelection);

  return (int) dlg.mSelection;
}

int SVConnectionControl::GetConnectionType(CString &szConnection)
{
  SVRegistryClass Connections (_T("Connections\\")), *pConnectionKey;
  DWORD dwConnectionType = NoProtocol;

  pConnectionKey = Connections.OpenSubKey (szConnection);

  if (pConnectionKey->CreatedNewKey ())
  {
    dwConnectionType = (GetConnectionTypeDlg (pConnectionKey, mszConnectionTypeName));
  }
	else
	{
		if (!pConnectionKey->GetRegistryValue (mszConnectionTypeName, &dwConnectionType))
		{
			dwConnectionType = (GetConnectionTypeDlg (pConnectionKey, mszConnectionTypeName));
		}
	}

	delete pConnectionKey;
  return (int) dwConnectionType;
}

BOOL SVConnectionControl::SetupConnection(CString& szConnection)
{
  SVRegistryClass Connections (_T("Connections\\")), *pConnectionKey;
  DWORD dwConnectionType = NoProtocol;
	BOOL rc = FALSE;

  pConnectionKey = Connections.OpenSubKey (szConnection);

  Close ();

  mpfnIOCompleteProc = NULL;
  mpUserData = NULL;

  switch (mdwConnectionType = GetConnectionTypeDlg (pConnectionKey, mszConnectionTypeName))
  {
    case NoProtocol :
      break;

    case TCPIP :
      rc = mTCPIPConnection.SetupConnection (szConnection);
      break;

    case NamedPipes :
      rc = mPipeConnection.SetupConnection (szConnection);
      break;

    default :
      break;
  }

	delete pConnectionKey;
  return rc;
}

BOOL CALLBACK IOComplete (void *pVoidConnection, int nErrorCode, void *pBuffer, int cbBuffer, int Operation, void *pUserData)
{
  SVConnectionControl *pConnection = (SVConnectionControl *) pUserData;

  return (pConnection->GetIOCompleteProc()) (pUserData, nErrorCode, pBuffer, cbBuffer, Operation, pConnection->GetUserData ());
}

BOOL SVConnectionControl::Open(CString& szConnection, DWORD dwFlags, SV_IOCOMPLETEPROC pfnCompletion, void *pUserData, DWORD dwTimeOut)
{
  mpfnIOCompleteProc = pfnCompletion;
  mpUserData = pUserData;
  mdwConnectionType = GetConnectionType (szConnection);
  switch (mdwConnectionType)
  {
    case NoProtocol :
      return SetupConnection (szConnection);
      break;

    case TCPIP :
      return mTCPIPConnection.Open (szConnection, dwFlags, IOComplete, this);
      break;

    case NamedPipes :
      return mPipeConnection.Open (szConnection, dwFlags, IOComplete, this);
      break;

    default :
      break;
  }

  return FALSE;
}

void SVConnectionControl::Close( )
{
  switch (mdwConnectionType)
  {
    case NoProtocol :
      break;

    case TCPIP :
      mTCPIPConnection.Close ();
      break;

    case NamedPipes :
      mPipeConnection.Close ();
      break;

    default :
      break;
  }

  return;
}

BOOL SVConnectionControl::Read(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut)
{
  switch (mdwConnectionType)
  {
    case NoProtocol :
      break;

    case TCPIP :
      return mTCPIPConnection.Read (pDataBuffer, cbDataBuffer);
      break;

    case NamedPipes :
      return mPipeConnection.Read (pDataBuffer, cbDataBuffer);
      break;

    default :
      break;
  }
  return FALSE;
}

BOOL SVConnectionControl::Write(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut)
{
  switch (mdwConnectionType)
  {
    case NoProtocol :
      break;

    case TCPIP :
      return mTCPIPConnection.Write (pDataBuffer, cbDataBuffer);
      break;

    case NamedPipes :
      return mPipeConnection.Write (pDataBuffer, cbDataBuffer);
      break;

    default :
      break;
  }
  return FALSE;
}

SV_IOCOMPLETEPROC SVConnectionControl::GetIOCompleteProc()
{
  return mpfnIOCompleteProc;
}

void * SVConnectionControl::GetUserData()
{
  return mpUserData;
}

DWORD SVConnectionControl::GetState()
{
  switch (mdwConnectionType)
  {
    case NoProtocol :
      break;

    case TCPIP :
      return mTCPIPConnection.GetState();
      break;

    case NamedPipes :
      return mPipeConnection.GetState();
      break;

    default :
      break;
  }
  return 0;
}

