//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShowDependentsDialog
//* .File Name       : $Workfile:   SVShowDependentsDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:00:16  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <functional>
#include "SVShowDependentsDialog.h"
#include "GuiCommands\GetDependencies.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVTaskObject.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SV_NUMBER_DEPENDENTS_COLUMNS 2

SVShowDependentsDialog::SVShowDependentsDialog( const SVObjectPairVector& rList, LPCTSTR DisplayText, DialogType Type /*= DeleteConfirm*/, CWnd* pParent /*=nullptr*/ )
: CDialog(SVShowDependentsDialog::IDD, pParent)
, m_DisplayText( DisplayText )
, m_DialogType( Type )
{
	ConvertList(rList);
}

SVShowDependentsDialog::SVShowDependentsDialog( const GUID& rInspectionID, const GUID& rTaskObjectID, bool bOnlyImages, SVObjectTypeEnum objectType, LPCTSTR DisplayText, DialogType Type /*= DeleteConfirm*/, CWnd* pParent /*=nullptr*/ )
: CDialog(SVShowDependentsDialog::IDD, pParent)
, m_DisplayText( DisplayText )
, m_DialogType( Type )
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
, m_bOnlyImages(bOnlyImages)
, m_objectType(objectType)
{
	RetreiveList();
}

void SVShowDependentsDialog::RetreiveList()
{
	typedef GuiCmd::GetDependencies Command;
	typedef SVSharedPtr<Command> CommandPtr;

	CommandPtr commandPtr = new Command(m_TaskObjectID, m_bOnlyImages, m_objectType);
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		m_dependencyList = commandPtr->Dependencies();
	}
}

void SVShowDependentsDialog::ConvertList(const SVObjectPairVector& rList)
{
	SVObjectTypeEnum ObjectType(SVToolObjectType);

	if (DeleteConfirmWithIP_Name == m_DialogType || ShowWithIP_Name == m_DialogType)
	{
		ObjectType = SVInspectionProcessType;
	}
	for (SVObjectPairVector::const_iterator it = rList.begin(); it != rList.end(); ++it)
	{
		// Who is using
		SVString WhoName = it->first->GetCompleteObjectNameToObjectType(nullptr, ObjectType);

		// Where/What object is being used
		SVString WhatName = it->second->GetCompleteObjectNameToObjectType(nullptr, ObjectType);
		m_dependencyList.push_back(SvOi::Relation(WhoName, WhatName));
	}
}

/*static*/ INT_PTR SVShowDependentsDialog::StandardDialog( SVTaskObjectClass* pTaskObject, bool OnlyImages )
{
	INT_PTR Result( IDOK );

	if( nullptr != pTaskObject )
	{
		CString DisplayText;
		CString Name( pTaskObject->GetName() );
		DisplayText.Format( IDS_DELETE_CHECK_DEPENDENCIES, Name, Name, Name, Name);
	
		SVGUID taskObjectID = pTaskObject->GetUniqueObjectID();
		SVGUID inspectionID = pTaskObject->GetInspection()->GetUniqueObjectID();
		
		SVShowDependentsDialog Dlg(inspectionID, taskObjectID, OnlyImages, SVToolObjectType, DisplayText);

		Result = Dlg.DoModal();
	}

	return Result;
}

void SVShowDependentsDialog::addColumnHeadings()
{
	CString columnName;

	// load the Column names
	for( int i = 0; i < SV_NUMBER_DEPENDENTS_COLUMNS; i++ )
	{
		listCtrl.InsertColumn( i, columnName, LVCFMT_LEFT, -1, i );
	}
}

void SVShowDependentsDialog::addItems()
{
	int index = 0;
	CListCtrl* pCtrl = &listCtrl;
	std::for_each(m_dependencyList.begin(),m_dependencyList.end(), [&index, &pCtrl](const SvOi::Relation& rel)->void
	{
		pCtrl->InsertItem(index, rel.first.c_str()); // Who is using
		pCtrl->SetItemText(index, 1, rel.second.c_str()); // Where/What object is being used
		++index;
	}
	);
}

void SVShowDependentsDialog::setColumnWidths()
{
	CRect viewRect;
	listCtrl.GetClientRect( viewRect );

	int columnWidth = viewRect.Width() / SV_NUMBER_DEPENDENTS_COLUMNS;

	for( int i = 0; i < SV_NUMBER_DEPENDENTS_COLUMNS; i++ )
	{
		listCtrl.SetColumnWidth( i, columnWidth );
	}
}

void SVShowDependentsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVShowDependentsDialog)
	DDX_Control(pDX, IDC_DEPENDENCY_LIST, listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVShowDependentsDialog, CDialog)
	//{{AFX_MSG_MAP(SVShowDependentsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVShowDependentsDialog message handlers

BOOL SVShowDependentsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pControl(nullptr);
	if( DeleteConfirm == m_DialogType || DeleteConfirmWithIP_Name == m_DialogType)
	{
		if( 0 == m_dependencyList.size() )
		{
			EndDialog(IDOK);
			return false;
		}
		pControl = GetDlgItem( IDC_WARNING_TEXT );
		if( nullptr != pControl )
		{
			pControl->SetWindowText( m_DisplayText.c_str() );
		}
	}
	else
	{
		pControl = GetDlgItem( IDC_WARNING_TEXT );
		if( nullptr != pControl )
		{
			pControl->ShowWindow( SW_HIDE );
		}
		pControl = GetDlgItem( IDCANCEL );
		if( nullptr != pControl )
		{
			pControl->ShowWindow( SW_HIDE );
		}
	}

	// Build ListCtrl Headers
	addColumnHeadings();

	// Set the widths
	setColumnWidths();

	addItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVShowDependentsDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:00:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   04 Oct 2012 11:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   12 Oct 2004 12:59:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for filtering the list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   28 May 2003 14:36:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed an existing problem with the resources for the SVShowDependentsDialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 15:34:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   04 Oct 2001 18:24:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

