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
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/SVString.h"
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

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( &( pTool->m_voProfileOrientation ), lValue );
			}
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( pTool->GetUniqueObjectID() );
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
			SVString Value = pTool->GetProfileOrientation();
			m_ctlProfileOrientation.SelectString( -1, Value.c_str() );
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
			CString Text;
			//turning on Rotation.
			m_ctlProfileOrientation.GetWindowText(Text);
			if ( _T("Vertical") == Text  )
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_LinearSpecial_OrientationVertical, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10213, SV_GUID_NULL, MB_YESNO );
				l_bUpdateRotation = (IDYES == result);

				if( ! l_bUpdateRotation )
				{
					l_bValue = FALSE;
				}
			}
		}
		else
		{
			CString Text;
			m_ctlProfileOrientation.GetWindowText(Text);

			if ( "Horizontal" == Text )
			{
				if ( bIsRotated )
				{
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_LinearSpecial_IsRotated, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10214, SV_GUID_NULL, MB_YESNO );
					l_bUpdateRotation = (IDYES == result);
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
		m_ctlProfileOrientation.SelectString( -1, pTool->GetProfileOrientation().c_str() );

	}//if pTool
}

