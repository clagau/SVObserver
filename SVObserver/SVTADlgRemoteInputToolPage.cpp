//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRemoteInputToolPage
//* .File Name       : $Workfile:   SVTADlgRemoteInputToolPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 Dec 2014 18:01:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgRemoteInputToolPage.h"

#include "SVInspectionProcess.h"
#include "SVRemoteInputTool.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolset.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "RootObject.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVTADlgRemoteInputToolPage::SVTADlgRemoteInputToolPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id ) 
: CPropertyPage( id )
, m_pParentDialog( Parent )
, m_pTool( nullptr )
, m_InputName( _T( "" ) )
{
}

SVTADlgRemoteInputToolPage::~SVTADlgRemoteInputToolPage()
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
	if( nullptr == m_pTool ) { return; }

	SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*>( m_pTool->GetAncestor( SVToolSetObjectType ) );

	if( nullptr == pToolSet ) { return; }

	SVString InspectionName( pToolSet->GetInspection()->GetName() );

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );
	SvOsl::ObjectTreeGenerator::Instance().setAttributeFilters( SV_ARCHIVABLE );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );

	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>(pToolSet->GetInspection()->GetUniqueObjectID(), pToolSet->GetUniqueObjectID());

	if( nullptr !=  m_pTool->GetInputObject() )
	{
		SVStringSet Items;

		SVObjectReference ObjectRef( m_pTool->GetInputObject() );
		SVString Location( ObjectRef.GetCompleteOneBasedObjectName() );
		SvOsl::ObjectTreeGenerator::Instance().convertObjectArrayName( ObjectRef, Location );
		Items.insert( Location );
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems( Items );
	}

	CString ToolsetOutput;
	ToolsetOutput.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
	SVString Title;
	Title.Format( _T("%s - %s"), ToolsetOutput, m_pTool->GetName() );
	SVString mainTabTitle = ToolsetOutput;
	CString Filter;
	Filter.LoadString( IDS_FILTER );
	SVString filterTabTitle = Filter;

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, filterTabTitle, this );

	if( IDOK == Result )
	{
		m_InputName = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str();

		SVGUID ObjectGuid(SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getItemKey());
		m_pTool->SetInputObject( ObjectGuid );
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgRemoteInputToolPage.cpp_v  $
 * 
 *    Rev 1.3   16 Dec 2014 18:01:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed the OnBnClickedSelectInputButton method to specify the title of the Object Selector's Filter Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Dec 2014 04:52:24   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Result selected object can now use both DisplayLocation and Location
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jul 2014 20:30:00   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Replace ResultPicker Dialog with Object Selector Dialog
 * Changed methods: OnBnClickedSelectInputButton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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