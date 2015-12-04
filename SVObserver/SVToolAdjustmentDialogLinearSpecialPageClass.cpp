//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogLinearSpecialPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogLinearSpecialPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:36:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogLinearSpecialPageClass.h"

#include "SVObserver.h"
#include "SVIPDoc.h"
#include "SVLinearToolClass.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVToolAdjustmentDialogLinearSpecialPageClass::SVToolAdjustmentDialogLinearSpecialPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* PParent ) 
: CPropertyPage(SVToolAdjustmentDialogLinearSpecialPageClass::IDD)
, m_pParentDialog(PParent)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogLinearSpecialPageClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVToolAdjustmentDialogLinearSpecialPageClass::~SVToolAdjustmentDialogLinearSpecialPageClass()
{
}

HRESULT SVToolAdjustmentDialogLinearSpecialPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	SVLinearToolClass *pTool = dynamic_cast<SVLinearToolClass*>(m_pParentDialog->GetTool());

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( &( pTool->m_voUseProfileRotation ), m_ctlUseRotation.GetCheck() );

		int iCurSel = m_ctlProfileOrientation.GetCurSel();
		if( iCurSel >= 0 )
		{
			long lValue = ( long ) m_ctlProfileOrientation.GetItemData( iCurSel );

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( &( pTool->m_voProfileOrientation ), lValue );
			}
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( pTool );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogLinearSpecialPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogLinearSpecialPageClass)
	DDX_Control(pDX, IDC_CHECK_ROTATION, m_ctlUseRotation);
	DDX_Control(pDX, IDC_COMBO_PROFILE_ORIENTATION, m_ctlProfileOrientation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogLinearSpecialPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogLinearSpecialPageClass)
	ON_CBN_SELCHANGE(IDC_COMBO_PROFILE_ORIENTATION, OnSelchangeComboProfileOrientation)
	ON_BN_CLICKED(IDC_CHECK_ROTATION, OnCheckRotation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogLinearSpecialPageClass message handlers

BOOL SVToolAdjustmentDialogLinearSpecialPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ctlProfileOrientation.SetEnumTypes(g_strOrientationEnums);
	
	SVLinearToolClass *pTool = dynamic_cast<SVLinearToolClass*>(m_pParentDialog->GetTool());

	if ( pTool )
	{
		SetTaskObject( pTool );

		BOOL bRotate = pTool->GetRotation();

		m_ctlUseRotation.SetCheck(bRotate);
		
		if ( bRotate )
		{
			m_ctlProfileOrientation.EnableWindow(FALSE);
			m_ctlProfileOrientation.SelectString(-1,"Horizontal");
		}
		else
		{	
			CString sVal = pTool->GetProfileOrientation();
			m_ctlProfileOrientation.SelectString(-1,sVal);
		}

		
	}
	
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVToolAdjustmentDialogLinearSpecialPageClass::OnSelchangeComboProfileOrientation() 
{
	SetInspectionData();
}

void SVToolAdjustmentDialogLinearSpecialPageClass::OnCheckRotation() 
{
	SVLinearToolClass *pTool = dynamic_cast<SVLinearToolClass*>(m_pParentDialog->GetTool());
	
	if ( pTool )
	{
		BOOL l_bUpdateRotation = TRUE;

		BOOL bIsRotated = pTool->IsToolRotated();

		BOOL l_bValue = m_ctlUseRotation.GetCheck();
		
		if ( l_bValue ) //true
		{
			CString sTxt;

			//turning on Rotation.
			m_ctlProfileOrientation.GetWindowText(sTxt);

			if ( sTxt == "Vertical" )
			{
				l_bUpdateRotation = AfxMessageBox("Tool profile orientation is at vertical.  By turning rotaion on the profile orientation will be set to horizontal. Do you wish to continue",MB_YESNO) == IDYES;

				if( ! l_bUpdateRotation )
				{
					l_bValue = FALSE;
				}
			}
		}
		else
		{
			CString sTxt;

			m_ctlProfileOrientation.GetWindowText(sTxt);

			if ( sTxt == "Horizontal" )
			{
				if ( bIsRotated )
				{
					l_bUpdateRotation = AfxMessageBox("Tool is currently rotated.  By turning off rotation the figure will be drawn at a rotation angle of 0. Do you wish to continue",MB_YESNO) == IDYES;

					if( ! l_bUpdateRotation )
					{
						l_bValue = TRUE;
					}
				}
			}
		}

		if( l_bUpdateRotation )
		{
			SetInspectionData();
		}

		m_ctlUseRotation.SetCheck( l_bValue );

		//true - turn off combo
		m_ctlProfileOrientation.EnableWindow( ! l_bValue );
		m_ctlProfileOrientation.SelectString( -1, pTool->GetProfileOrientation() );

	}//if pTool
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolAdjustmentDialogLinearSpecialPageClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:36:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2012 17:48:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Aug 2005 13:34:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:45:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
