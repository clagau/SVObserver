// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilterDlg
// * .File Name       : $Workfile:   SVThickeningFilterDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 11:37:08  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <boost/assign/list_of.hpp>
#include "SVThickeningFilterDlg.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVOGui
	{
		static const std::string GrayOnTag("GrayOn");
		static const std::string IterationTag("Iteration");
		/////////////////////////////////////////////////////////////////////////////
		// SVThickeningFilterDlg dialog
	SVThickeningFilterDlg::SVThickeningFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent): 
		CDialog(SVThickeningFilterDlg::IDD)
		,m_filterID(rFilterID)
		,m_rInspectionID(rInspectionID)
		, m_Values(SvOg::BoundValues(rInspectionID, rFilterID, boost::assign::map_list_of
			(GrayOnTag, SVThickeningFilterGrayOnGuid)
			(IterationTag, SVThickeningFilterItterationsGuid)))
	{
		//{{AFX_DATA_INIT(SVThickeningFilterDlg)
		m_bGrayScale = FALSE;
		m_lIterations = 0;
		//}}AFX_DATA_INIT
	}

	SVThickeningFilterDlg::~SVThickeningFilterDlg()
	{
	}

	HRESULT SVThickeningFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		m_Values.Set<bool>(GrayOnTag, TRUE == m_bGrayScale);
		m_Values.Set<long>(IterationTag, m_lIterations);
		m_Values.Commit();

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVThickeningFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVThickeningFilterDlg)
		DDX_Check(pDX, IDC_GRAY_CHECK, m_bGrayScale);
		DDX_Text(pDX, IDC_ITTERATIONS_EDIT, m_lIterations);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVThickeningFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVThickeningFilterDlg)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVThickeningFilterDlg message handlers

	BOOL SVThickeningFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_Values.Init();
		m_bGrayScale = m_Values.Get<bool>(GrayOnTag);
		m_lIterations = m_Values.Get<long>(IterationTag);

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVThickeningFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	void SVThickeningFilterDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}
	}  //end namespace SVOGUI
}  //end namespace Seidenader

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVThickeningFilterDlg.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:37:08   bWalter
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
 *    Rev 1.2   11 Aug 2005 13:32:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:32:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:02:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Thickening Filter files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
