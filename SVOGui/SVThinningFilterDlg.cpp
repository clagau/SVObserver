// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThinningFilterDlg
// * .File Name       : $Workfile:   SVThinningFilterDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 12:25:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVThinningFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SvOi;

namespace Seidenader
{
	namespace SVOGui
	{
		/////////////////////////////////////////////////////////////////////////////
		// SVThinningFilterDlg dialog
	SVThinningFilterDlg::SVThinningFilterDlg(SvOi::IThinningFilter& rFilterClass, CWnd* pParent): 
		CDialog(SVThinningFilterDlg::IDD, pParent)
		,m_pFilterClass(rFilterClass)
	{
		//{{AFX_DATA_INIT(SVThinningFilterDlg)
		m_bGrayScale = FALSE;
		m_lIterations = 0;
		//}}AFX_DATA_INIT
	}

	HRESULT SVThinningFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		l_hrOk = m_pFilterClass.addGrayOnRequest( TRUE == m_bGrayScale );

		if( l_hrOk == S_OK )
		{
			l_hrOk = m_pFilterClass.addIterationRequest( m_lIterations );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = m_pFilterClass.AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = m_pFilterClass.RunOnce( m_pFilterClass.GetToolInterface() );
		}

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVThinningFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVThinningFilterDlg)
		DDX_Check(pDX, IDC_GRAY_CHECK, m_bGrayScale);
		DDX_Text(pDX, IDC_ITTERATIONS_EDIT, m_lIterations);
		//}}AFX_DATA_MAP
	}
	
	BEGIN_MESSAGE_MAP(SVThinningFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVThinningFilterDlg)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVThinningFilterDlg message handlers

	BOOL SVThinningFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_bGrayScale = m_pFilterClass.isGrayOn();
		m_lIterations = m_pFilterClass.getIteration();

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVThinningFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	void SVThinningFilterDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}
	}  //end namespace SVOGUI
}  //end namespace Seidenader

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVThinningFilterDlg.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:25:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 09:51:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Aug 2005 13:32:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:32:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:03:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in of Thinning Filter files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
