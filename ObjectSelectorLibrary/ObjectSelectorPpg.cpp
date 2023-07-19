//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "TextDefinesSvOsl.h"
#include "SVContainerLibrary/ObjectTreeItems.h"
#include "SVOResource/ConstGlobalSvOr.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int IconNumber = 8;
constexpr int IconGrowBy = 1;
#pragma endregion Declarations

namespace SvOsl
{
	BEGIN_MESSAGE_MAP(ObjectSelectorPpg, CPropertyPage)
		ON_COMMAND(ID_HELP, OnHelp)
		ON_EN_CHANGE(IDC_FILTER, OnChangeFilter)
		ON_WM_SIZE()
		ON_WM_HELPINFO()
	END_MESSAGE_MAP()

	#pragma region Constructor
	ObjectSelectorPpg::ObjectSelectorPpg( SvCl::ObjectTreeItems& rTreeContainer, LPCTSTR Title, bool SingleSelect, LPCTSTR nodeToBeSelected)
		: CPropertyPage( ObjectSelectorPpg::IDD )
		, m_rTreeContainer( rTreeContainer )
		, m_NodeTree( *this, SingleSelect )
		, m_LeafTree( *this, SingleSelect )
		, m_HelpID(0)
		, m_nodeToBeSelected(nodeToBeSelected)
	{
		m_psp.pszTitle = Title;
		m_psp.dwFlags |= PSP_USETITLE;
	}

	ObjectSelectorPpg::~ObjectSelectorPpg()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void ObjectSelectorPpg::updateTreeData()
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
		DDX_Control(pDX, IDC_FILTER, m_FilterNameControl);
		//This is ok when std::string is only being read, input from the dialog would be lost
		CString tempStr{m_HighlightedNode.c_str()};
		DDX_Text(pDX, IDC_HIGHLIGHTED_NODE, tempStr);
	}

	BOOL ObjectSelectorPpg::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		m_FilterNameControl.setHelpText(FilterTreeNameHelp);

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

	void ObjectSelectorPpg::OnChangeFilter()
	{
		UpdateData(true);
		if(m_lastFilter != m_FilterNameControl.getEditText().GetString())
		{
			m_NodeTree.loadTree();
			m_LeafTree.loadTree();
			m_lastFilter = m_FilterNameControl.getEditText().GetString();
		}
	}

	BOOL ObjectSelectorPpg::OnSetActive()
	{
		m_NodeTree.UpdateAllNodes();
		if (m_nodeToBeSelected)
		{ 
			m_NodeTree.SelectNodeByName(m_nodeToBeSelected);
			m_nodeToBeSelected = nullptr; //only select that node once
		}

		m_LeafTree.updateTree();

		return CPropertyPage::OnSetActive();
	}

	void ObjectSelectorPpg::OnHelp()
	{
		AfxGetApp()->HtmlHelp( m_HelpID );
	}

	BOOL ObjectSelectorPpg::OnHelpInfo(HELPINFO* pHelpInfo)
	{
		pHelpInfo->iCtrlId = m_HelpID;
		AfxGetApp()->HtmlHelp( pHelpInfo->iCtrlId, HH_HELP_CONTEXT );
		return TRUE ;
	}
	#pragma endregion Protected Methods

} //namespace SvOsl
