//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRemoteInputToolPage
//* .File Name       : $Workfile:   SVTADlgRemoteInputToolPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:17:12  $
//******************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "SVTADlgRemoteInputToolPage.h"

#include "SVInspectionProcess.h"
#include "SVRemoteInputTool.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolset.h"
#include "SVToolsetOutputSelectionDialog.h"

SVTADlgRemoteInputToolPage::SVTADlgRemoteInputToolPage( SVToolAdjustmentDialogSheetClass* Parent, int id ) 
: CPropertyPage( id )
, m_pParentDialog( Parent )
, m_pTool( NULL )
, m_InputName( _T( "" ) )
{
}

void SVTADlgRemoteInputToolPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMOTE_INPUT_NAME, m_RemoteInputNameCtrl);
}


BEGIN_MESSAGE_MAP(SVTADlgRemoteInputToolPage, CDialog)
	ON_BN_CLICKED(IDC_CLEAR_INPUT_BUTTON, &SVTADlgRemoteInputToolPage::OnBnClickedClearInputButton)
	ON_BN_CLICKED(IDC_SELECT_INPUT_BUTTON, &SVTADlgRemoteInputToolPage::OnBnClickedSelectInputButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVCylindricalWarpDlg message handlers

BOOL SVTADlgRemoteInputToolPage::OnInitDialog() 
{
	BOOL l_Status = CPropertyPage::OnInitDialog();

	l_Status = l_Status && ( m_pParentDialog != NULL );

	if( l_Status )
	{
		m_pTool = dynamic_cast< SVRemoteInputTool* >( m_pParentDialog->GetTool() );
	}

	l_Status = l_Status && ( m_pTool != NULL );

	if( l_Status )
	{
		SVObjectClass* l_pObject = m_pTool->GetInputObject();

		l_Status = ( l_pObject != NULL );

		if( l_Status )
		{
			m_InputName = l_pObject->GetCompleteObjectName();
		}
	}

	RefreshSelectedInputName();

	UpdateData( FALSE );

	return l_Status;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgRemoteInputToolPage::OnBnClickedClearInputButton()
{
	m_InputName.Empty();

	if( m_pTool != NULL )
	{
		m_pTool->ClearInputObject();
	}

	RefreshSelectedInputName();
}

void SVTADlgRemoteInputToolPage::OnBnClickedSelectInputButton()
{
	if( m_pTool != NULL )
	{
		SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*> ( m_pTool->GetAncestor( SVToolSetObjectType ) );

		SVToolsetOutputSelectionDialog dlg;
		dlg.PTaskObjectList = pToolSet;
		dlg.uAttributesDesired = SV_ARCHIVABLE;

		SVObjectReference input( m_pTool->GetInputObject() );
		SVObjectReferenceVector selectedInputs;
		selectedInputs.push_back(input);

		dlg.m_treeOutputList.SetSelection(selectedInputs);

		if( dlg.DoModal() == IDOK )
		{
			if( !( dlg.m_sSelectedOutputName.IsEmpty() ) )
			{
				m_InputName = pToolSet->GetInspection()->GetName() + CString( _T( "." ) ) + dlg.m_sSelectedOutputName;
			}
			else
			{
				m_InputName.Empty();
			}

			m_pTool->SetInputObject( dlg.m_refSelectedObject.Guid() );
		}
	}

	RefreshSelectedInputName();
}

void SVTADlgRemoteInputToolPage::RefreshSelectedInputName()
{
	m_RemoteInputNameCtrl.SetWindowText( m_InputName );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgRemoteInputToolPage.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:17:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2012 17:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Feb 2012 13:44:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated input selection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jan 2012 10:03:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated source code based on gui changes desired by application users.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added tool adjustment functionality for the new remote input tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

