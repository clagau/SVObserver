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

	mszServer = l_Server.ToString();
	mszPipe = l_Pipe.ToString();
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
		Exception.getMessageHandler().Format( szMsg );
		MessageBox (szMsg.ToString());
	}
}

CString SVPipesConnectionDlg::GetRegistryKey(CString &szConnection)
{
  mszConnectionKey = _T("\\SVIPC\\Connections\\");
  mszConnectionKey += szConnection;
  mszConnectionKey += _T("\\NamedPipes");

  return mszConnectionKey;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVPipes\SVPipesConnectionDlg.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 14:03:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 10:51:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 08:36:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 2000 16:34:46   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  55
 * SCR Title:  SVFocus Remote control from PLC
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed font, tab order of controls and enabled help
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 May 2000 08:49:28   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added logic in OnOK() to verify that data has been entered.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:29:38   mike
 * Initial revision.
*/
