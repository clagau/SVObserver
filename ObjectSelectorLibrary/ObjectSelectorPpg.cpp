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
#include "SVOResource\ConstGlobalSvOr.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int IconNumber = 8;
static const int IconGrowBy = 4;
#pragma endregion Declarations

namespace Seidenader { namespace ObjectSelectorLibrary
{
	BEGIN_MESSAGE_MAP(ObjectSelectorPpg, CPropertyPage)
		ON_COMMAND(ID_HELP, OnHelp)
		ON_WM_SIZE()
		ON_WM_HELPINFO()
	END_MESSAGE_MAP()

	#pragma region Constructor
	ObjectSelectorPpg::ObjectSelectorPpg( SvCl::ObjectTreeItems& rTreeContainer, LPCTSTR Title, bool SingleSelect )
		: CPropertyPage( ObjectSelectorPpg::IDD )
		, m_rTreeContainer( rTreeContainer )
		, m_NodeTree( *this, SingleSelect )
		, m_LeafTree( *this, SingleSelect )
		, m_HelpID(0)
	{
		m_psp.pszTitle = Title;
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

		m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR24 | ILC_MASK, IconNumber, IconGrowBy );

		for(int i = IDI_EMPTY_ENABLED; i <= IDI_TRI_STATE_DISABLED; i++)
		{
			m_StateImageList.Add( AfxGetApp()->LoadIcon( i ) );
		}
		//If single select then change the tristate icons
		if( m_NodeTree.isSingleSelect() )
		{
			int ImageIndex( IDI_TRI_STATE_ENABLED - IDI_EMPTY_ENABLED );
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
		AfxGetApp()->HtmlHelp( m_HelpID + SvOr::HELPFILE_SVORESOURCE_OFFSET );
	}

	BOOL ObjectSelectorPpg::OnHelpInfo(HELPINFO* pHelpInfo)
	{
		pHelpInfo->iCtrlId = m_HelpID + SvOr::HELPFILE_SVORESOURCE_OFFSET;
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

} /*namespace ObjectSelectorLibrary*/ } /*namespace Seidenader*/
