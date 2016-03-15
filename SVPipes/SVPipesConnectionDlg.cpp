// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPipesConnectionDlg
// * .File Name       : $Workfile:   SVPipesConnectionDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 14:03:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPipesConnectionDlg.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVLibrary/SVLibrary.h"
#include "SVMessage/SVMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPipesConnectionDlg::SVPipesConnectionDlg(CString &szConnection, CWnd* pParent /*=NULL*/)
	: SVDialog(SVPipesConnectionDlg::IDD, pParent)
{
	SVString l_Pipe = _T("");
	SVString l_Server = _T("");

	//{{AFX_DATA_INIT(SVPipesConnectionDlg)
	mszConnection = _T("");
	mszPipe = _T("");
	mszServer = _T("");
	//}}AFX_DATA_INIT

  mszConnection.Format (_T("%s"), szConnection);
  SVRegistryClass reg(GetRegistryKey (szConnection));
  mszServerKey = _T("Server");
  mszPipeKey = _T("PipeName");
  reg.GetRegistryValue (mszServerKey, l_Server);
  reg.GetRegistryValue (mszPipeKey, l_Pipe);

	mszServer = l_Server.c_str();
	mszPipe = l_Pipe.c_str();
}

void SVPipesConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	SVDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPipesConnectionDlg)
	DDX_Text(pDX, IDC_CONNECTION, mszConnection);
	DDX_Text(pDX, IDC_PIPE, mszPipe);
	DDX_Text(pDX, IDC_PIPE_SERVER, mszServer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVPipesConnectionDlg, SVDialog)
	//{{AFX_MSG_MAP(SVPipesConnectionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SVPipesConnectionDlg::OnCancel() 
{
	SVDialog::OnCancel();
}

void SVPipesConnectionDlg::OnOK() 
{
  UpdateData (TRUE);

	if (!mszPipe.IsEmpty() && !mszServer.IsEmpty())
	{
	  SVRegistryClass reg(GetRegistryKey (mszConnection));

	  reg.SetRegistryValue (mszServerKey, mszServer);
	  reg.SetRegistryValue (mszPipeKey, mszPipe);

		SVDialog::OnOK();
	}
	else
	{
		//Not done with MessageMgr1 otherwise SVPipes.dll would need to load SVOResource.dll
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_PIPES_REQUIRED_DATA_MISSING, nullptr, StdMessageParams );
		SVString szMsg;
		Exception.getMessageContainer().Format( szMsg );
		MessageBox (szMsg.c_str());
	}
}

CString SVPipesConnectionDlg::GetRegistryKey(CString &szConnection)
{
  mszConnectionKey = _T("\\SVIPC\\Connections\\");
  mszConnectionKey += szConnection;
  mszConnectionKey += _T("\\NamedPipes");

  return mszConnectionKey;
}

