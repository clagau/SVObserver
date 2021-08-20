//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
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
#include "SVMFCControls/DlgItemResizer.h"
#include "NodeTreeCtrl.h"
#include "LeafTreeCtrl.h"
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
		//************************************
		// Description:  The class constructor
		// Parameter:  rTreeContainer <in>:  a reference to the tree to display
		// Parameter:  Title <in>:  const string for page title
		// Parameter:  SingleSelect <in>:  true for single selection mode (default)
		//************************************
		ObjectSelectorPpg( SvCl::ObjectTreeItems& rTreeContainer, LPCTSTR Title, bool SingleSelect = true );

		//************************************
		// Description:  The virtual destructor
		//************************************
		virtual ~ObjectSelectorPpg();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		//************************************
		// Description:  The method gets the tree container
		// Returns:  a reference to the tree container
		//************************************
		SvCl::ObjectTreeItems& getTreeContainer() const { return m_rTreeContainer; }

		//************************************
		// Description:  The method gets the display text for the current highlighted node.
		// Returns:  the display text of the highlighted node
		//************************************
		std::string getHighlightedNode() const { return m_HighlightedNode; }

		//************************************
		// Description:  The method sets the current highlighted node.
		// Parameter:  rHighlightedNode <in>:  a const reference to the display text of the node to be highlighted
		//************************************
		void setHighlightedNode( const std::string& rHighlightedNode )
		{
			m_HighlightedNode = rHighlightedNode;
			UpdateData(false);
		}


		//************************************
		// Description:  The method sets the Help ID for the dialog.
		// Parameter:  HelpID <in>:  a unique identifier for this dialog
		//************************************
		void setHelpID(int HelpID) { m_HelpID = HelpID; }

		//************************************
		// Description:  The method is called to update the dialog with new data
		// Parameter:  pFromTree <in>:  a pointer to the tree containing the data to be displayed
		//************************************
		void updateData( const ObjectTreeCtrl* const pFromTree );
	#pragma endregion Public Methods

	protected:
	#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;
		virtual BOOL OnInitDialog() override;
		virtual BOOL OnSetActive() override;
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnHelp();
		afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

		//************************************
		// Description:  The method sets the controls which are resized when the page is resized
		//************************************
		void setResizeControls();
	#pragma endregion Protected Methods

	private:
	#pragma region Member Variables
		SvCl::ObjectTreeItems& m_rTreeContainer; //The tree container reference
		SvMc::DlgItemResizer m_Resizer;				//Object which handles the resizing of all controls
		NodeTreeCtrl m_NodeTree;						//The node tree control
		LeafTreeCtrl m_LeafTree;						//The leaf tree control
		CImageList m_StateImageList;					//The state image list
		CImageList m_ImageList;							//The image list
		std::string m_HighlightedNode;						//The currently highlighted node location
		int m_HelpID;									//The help id used to identify the property page
	#pragma endregion Member Variables
	};
} //namespace SvOsl


