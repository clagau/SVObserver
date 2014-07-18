//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorPpg
//* .File Name       : $Workfile:   ObjectSelectorPpg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Jul 2014 14:42:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectSelectorPpg.h"

#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::ObjectSelectorLibrary;
using namespace Seidenader::SVTreeLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(ObjectSelectorPpg, CPropertyPage)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_SIZE()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

#pragma region Constructor
ObjectSelectorPpg::ObjectSelectorPpg( ObjectTreeItems& rTreeContainer, const SVString& rTitle, bool SingleSelect )
	: CPropertyPage( ObjectSelectorPpg::IDD )
	, m_rTreeContainer( rTreeContainer )
	, m_NodeTree( *this, SingleSelect )
	, m_LeafTree( *this, SingleSelect )
{
	m_psp.pszTitle = rTitle.c_str();
	m_psp.dwFlags |= PSP_USETITLE;
}

ObjectSelectorPpg::~ObjectSelectorPpg()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void ObjectSelectorPpg::updateData( const ObjectTreeCtrl* const pFromTree )
{
	m_NodeTree.updateTree();
	m_LeafTree.updateTree();
}
#pragma endregion Public Methods

#pragma region Protected Methods
void ObjectSelectorPpg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NODES, m_NodeTree);
	DDX_Control(pDX, IDC_TREE_VALUES, m_LeafTree);
	DDX_Text(pDX, IDC_LOCATION, m_NodeLocation);
}

BOOL ObjectSelectorPpg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	setResizeControls();

	m_StateImageList.Create( 16, 16, TRUE, 16, 4 );

	for(int i = IDI_EMPTY_ENABLED; i <= IDI_TRI_STATE_DISABLED; i++)
	{
		m_StateImageList.Add( AfxGetApp()->LoadIcon( i ) );
	}

	m_NodeTree.ModifyStyle (0, TVS_CHECKBOXES);
	m_NodeTree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_NodeTree.SetImageList( &m_StateImageList, TVSIL_STATE );

	m_LeafTree.ModifyStyle (0, TVS_CHECKBOXES);
	m_LeafTree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_LeafTree.SetImageList( &m_StateImageList, TVSIL_STATE );

	m_NodeTree.loadTree();

	return TRUE;
}

void ObjectSelectorPpg::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	//Resize all controls
	m_Resizer.Resize(this);
}

void ObjectSelectorPpg::OnHelp() 
{
	AfxGetApp()->HtmlHelp(0x213B0);
}

BOOL ObjectSelectorPpg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	//@TODO[ramseier] This needs to be changed to call the correct Help IDs that have to be generated
	if( ( pHelpInfo->iCtrlId & 0xffff ) == 0xffff || 
		pHelpInfo->iCtrlId < 500)
	{
		pHelpInfo->iCtrlId = 0x213B0;
//		pHelpInfo->iCtrlId = IDD_PASSWORD_DLG + 0x60000;
	}
	else
	{
		pHelpInfo->iCtrlId = 0x213B0;
//		pHelpInfo->iCtrlId += 0x70000;
	}	
	AfxGetApp()->HtmlHelp( pHelpInfo->iCtrlId,HH_HELP_CONTEXT );
	return TRUE ; 
}

void ObjectSelectorPpg::setResizeControls()
{
	m_Resizer.Add(this, IDC_LABEL_TREE_NODES, RESIZE_LOCKLEFT | RESIZE_LOCKTOP);
	m_Resizer.Add(this, IDC_TREE_NODES, RESIZE_LOCKLEFT | RESIZE_LOCKRIGHT | RESIZE_LOCKTOP | RESIZE_LOCKBOTTOM );

	m_Resizer.Add(this, IDC_LABEL_TREE_VALUES, RESIZE_LOCKRIGHT | RESIZE_LOCKTOP);
	m_Resizer.Add(this, IDC_TREE_VALUES, RESIZE_LOCKRIGHT | RESIZE_LOCKTOP | RESIZE_LOCKBOTTOM );

	m_Resizer.Add(this, IDC_LABEL_LOCATION, RESIZE_LOCKLEFT | RESIZE_LOCKTOP);
	m_Resizer.Add(this, IDC_LOCATION, RESIZE_LOCKLEFT | RESIZE_LOCKRIGHT | RESIZE_LOCKTOP );
}

#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectSelectorPpg.cpp_v  $
 * 
 *    Rev 1.1   18 Jul 2014 14:42:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added the help methods to the dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:16:24   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
