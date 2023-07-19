//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorPpg
//* .File Name       : $Workfile:   ObjectSelectorPpg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   16 Dec 2014 17:39:38  $
//* ----------------------------------------------------------------------------
//* This class is the object selector property page 
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource\resource.h"
#include "NodeTreeCtrl.h"
#include "LeafTreeCtrl.h"
#include "SVMFCControls/SVEditControlWithHelp.h"
#pragma endregion Includes

namespace SvCl
{
class ObjectTreeItems;
}

namespace SvOsl
{
	class ObjectSelectorPpg : public CPropertyPage
	{
	public:
	#pragma region Declarations
		enum { IDD = IDD_OBJECT_SELECTOR_PPG };
	#pragma endregion Declarations

	public:
	#pragma region Constructor
		ObjectSelectorPpg(SvCl::ObjectTreeItems& rTreeContainer, LPCTSTR Title, bool SingleSelect = true, LPCTSTR nodeToBeSelected = nullptr);

		virtual ~ObjectSelectorPpg();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		SvCl::ObjectTreeItems& getTreeContainer() const { return m_rTreeContainer; }

		std::string getHighlightedNode() const { return m_HighlightedNode; }

		void setHighlightedNode( const std::string& rHighlightedNode )
		{
			m_HighlightedNode = rHighlightedNode;
			UpdateData(false);
		}

		void setHelpID(int HelpID) { m_HelpID = HelpID; }

		void updateTreeData();

		LPCTSTR getFilter() { return m_FilterNameControl.getEditText().GetString(); }
	#pragma endregion Public Methods

	protected:
	#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;
		virtual BOOL OnInitDialog() override;
		afx_msg void OnChangeFilter();
		virtual BOOL OnSetActive() override;
		afx_msg void OnHelp();
		afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	#pragma endregion Protected Methods

	private:
	#pragma region Member Variables
		SvCl::ObjectTreeItems& m_rTreeContainer;
		SvMc::SVEditControlWithHelp m_FilterNameControl;
		NodeTreeCtrl m_NodeTree;
		LeafTreeCtrl m_LeafTree;
		CImageList m_StateImageList;
		CImageList m_ImageList;
		std::string m_HighlightedNode;
		std::string m_lastFilter;
		int m_HelpID;
		LPCTSTR m_nodeToBeSelected;
	#pragma endregion Member Variables
	};
} //namespace SvOsl


