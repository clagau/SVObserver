//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorPpg
//* .File Name       : $Workfile:   ObjectSelectorPpg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 17:39:38  $
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

static const int IconSize = 16;
static const int IconNumber = 8;
static const int IconGrowBy = 4;
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
	, m_HelpID(0)
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
	DDX_Text(pDX, IDC_HIGHLIGHTED_NODE, m_HighlightedNode);
}

BOOL ObjectSelectorPpg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	setResizeControls();

	m_StateImageList.Create( IconSize, IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );

	for(int i = IDI_EMPTY_ENABLED; i <= IDI_TRI_STATE_DISABLED; i++)
	{
		m_StateImageList.Add( AfxGetApp()->LoadIcon( i ) );
	}
	//If single select then change the tristate icons
	if( m_NodeTree.isSingleSelect() )
	{
		int ImageIndex( 0 );
		ImageIndex = IDI_TRI_STATE_ENABLED - IDI_EMPTY_ENABLED;
		m_StateImageList.Replace( ImageIndex, AfxGetApp()->LoadIcon( IDI_DOT_ENABLED ) );
		ImageIndex = IDI_TRI_STATE_DISABLED - IDI_EMPTY_ENABLED;
		m_StateImageList.Replace( ImageIndex, AfxGetApp()->LoadIcon( IDI_DOT_DISABLED ) );
	}
	m_NodeTree.ModifyStyle( 0, TVS_CHECKBOXES | TVS_SHOWSELALWAYS );
	m_NodeTree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_NodeTree.SetImageList( &m_StateImageList, TVSIL_STATE );

	m_LeafTree.ModifyStyle( 0, TVS_CHECKBOXES );
	m_LeafTree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_LeafTree.SetImageList( &m_StateImageList, TVSIL_STATE );

	m_NodeTree.loadTree();

	return TRUE;
}

BOOL ObjectSelectorPpg::OnSetActive()
{
	m_NodeTree.UpdateAllNodes();
	m_LeafTree.updateTree();

	return CPropertyPage::OnSetActive();
}

void ObjectSelectorPpg::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	//Resize all controls
	m_Resizer.Resize(this);
}

void ObjectSelectorPpg::OnHelp()
{
	AfxGetApp()->HtmlHelp( m_HelpID + HELPFILE_SVORESOURCE_OFFSET );
}

BOOL ObjectSelectorPpg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	pHelpInfo->iCtrlId = m_HelpID + HELPFILE_SVORESOURCE_OFFSET;
	AfxGetApp()->HtmlHelp( pHelpInfo->iCtrlId, HH_HELP_CONTEXT );
	return TRUE ;
}

void ObjectSelectorPpg::setResizeControls()
{
	m_Resizer.Add(this, IDC_LABEL_TREE_NODES, RESIZE_LOCKLEFT | RESIZE_LOCKTOP);
	m_Resizer.Add(this, IDC_TREE_NODES, RESIZE_LOCKLEFT | RESIZE_LOCKRIGHT | RESIZE_LOCKTOP | RESIZE_LOCKBOTTOM );

	m_Resizer.Add(this, IDC_LABEL_TREE_VALUES, RESIZE_LOCKRIGHT | RESIZE_LOCKTOP);
	m_Resizer.Add(this, IDC_TREE_VALUES, RESIZE_LOCKRIGHT | RESIZE_LOCKTOP | RESIZE_LOCKBOTTOM );

	m_Resizer.Add(this, IDC_LABEL_HIGHLIGHTED_NODE, RESIZE_LOCKLEFT | RESIZE_LOCKTOP);
	m_Resizer.Add(this, IDC_HIGHLIGHTED_NODE, RESIZE_LOCKLEFT | RESIZE_LOCKRIGHT | RESIZE_LOCKTOP );
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectSelectorPpg.cpp_v  $
 * 
 *    Rev 1.5   16 Dec 2014 17:39:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method OnSetActive.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Dec 2014 03:16:52   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   In Single Selection mode icons are dotted for highlighted nodes otherwise no icons are displayed
 * Changed field name from Location to Highlighted Node
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Sep 2014 09:45:52   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Object Selector property page has a variable context ID
 * Changed methods: OnHelp, OnHelpInfo
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Aug 2014 12:27:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Selection in tree node remains also when it does not have the focus
 * Changed methods: OnInitDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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