//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveWarpDlg
//* .File Name       : $Workfile:   SVPerspectiveWarpDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:31:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPerspectiveWarpDlg.h"
#include "svobserver.h"
#include "SVTool.h"
#include "SVPerspectiveTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVPerspectiveWarpDlg dialog


SVPerspectiveWarpDlg::SVPerspectiveWarpDlg( const SVGUID& rInspectionId, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id ) 
: CPropertyPage( id )
, pParentDialog(Parent)
{
	
	//{{AFX_DATA_INIT(SVPerspectiveWarpDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVPerspectiveWarpDlg::~SVPerspectiveWarpDlg()
{
}

HRESULT SVPerspectiveWarpDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	if( m_pTool )
	{
		bool l_bUpdate = false;

		UpdateData( TRUE ); // get data from dialog

		int sel = m_FunctionCombo.GetCurSel();
		if( sel >= 0 )
		{
			l_bUpdate = true;

			long lValue = ( long ) m_FunctionCombo.GetItemData( sel );

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pWarpType, lValue );
			}
		}

		sel = m_cbInterpolation.GetCurSel();
		if( sel >= 0 )
		{
			l_bUpdate = true;

			long lValue = ( long ) m_cbInterpolation.GetItemData( sel );

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pInterpolationMode, lValue );
			}
		}

		if( l_bUpdate )
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequestMarker();
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = RunOnce( m_pTool );
			}
		}

		UpdateData( FALSE );
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}


void SVPerspectiveWarpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPerspectiveWarpDlg)
	DDX_Control(pDX, IDC_PERSPECTIVE_UNWARP_TYPE_COMBO, m_FunctionCombo);
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPerspectiveWarpDlg, CDialog)
	//{{AFX_MSG_MAP(SVPerspectiveWarpDlg)
	ON_CBN_SELCHANGE(IDC_PERSPECTIVE_UNWARP_TYPE_COMBO, OnSelchangePerspectiveUnwarpTypeCombo)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelchangeInterpolationModeCombo)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPerspectiveWarpDlg message handlers

BOOL SVPerspectiveWarpDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( pParentDialog && ( m_pTool = pParentDialog->GetTool() ) )
	{
		SetTaskObject( m_pTool );

		SVObjectTypeInfoStruct objectInfo;
		objectInfo.EmbeddedID = SVWarpTypeObjectGuid;
		m_pWarpType = reinterpret_cast<SVEnumerateValueObjectClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ));
		if( m_pWarpType )
		{
			
			CString l_strEnumList;

			m_pWarpType->GetEnumTypes( l_strEnumList );
			m_FunctionCombo.SetEnumTypes( l_strEnumList );

			CString strEnum;
			m_pWarpType->GetValue( strEnum );
			m_FunctionCombo.SelectString( -1, strEnum );
		}
		m_pWarpType->GetValue( m_lLastWarpType );

		// Interpolation Mode
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		m_pInterpolationMode = reinterpret_cast<SVEnumerateValueObjectClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ));
		if( m_pInterpolationMode )
		{
			CString l_strEnumList;

			m_pInterpolationMode->GetEnumTypes( l_strEnumList );
			m_cbInterpolation.SetEnumTypes( l_strEnumList );

			CString strEnum;
			m_pInterpolationMode->GetValue( strEnum );
			m_cbInterpolation.SelectString( -1, strEnum );
		}
		m_pWarpType->GetValue( m_lLastWarpType );

	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPerspectiveWarpDlg::OnSelchangePerspectiveUnwarpTypeCombo() 
{
	SetInspectionData();
}


void SVPerspectiveWarpDlg::OnDestroy() 
{
	// Set Some defaults if warp type changed...
	long l_lType ;
	m_pWarpType->GetValue( l_lType );
	if( l_lType != m_lLastWarpType )
	{
		SVImageExtentClass l_svExtents;
		HRESULT l_hr = S_OK;
		l_hr = m_pTool->GetImageExtent( l_svExtents );
		long lWarpType;
		m_pWarpType->GetValue( lWarpType );
		SVPerspectiveToolClass::WarpType eWarpType = (SVPerspectiveToolClass::WarpType) lWarpType;

		if( eWarpType == SVPerspectiveToolClass::WarpTypeVertical )
		{
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 10 );
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 0 );
		}
		else
		if( eWarpType == SVPerspectiveToolClass::WarpTypeHorizontal  )
		{
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 10 );
			l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 0 );
		}
		m_pTool->SetImageExtent( 1, l_svExtents );
	}
	CDialog::OnDestroy();
}

void SVPerspectiveWarpDlg::OnSelchangeInterpolationModeCombo() 
{
	SetInspectionData();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPerspectiveWarpDlg.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:31:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:00:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:22:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 17:30:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Mar 2011 13:48:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 07:59:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2005 15:47:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Interpolation mode to PerspectiveWarpTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Mar 2005 13:46:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in new files for the new perspective tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
