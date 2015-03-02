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
#include "SVTreeLibrary\ObjectSelectorItem.h"
#include "SVTreeLibrary\ObjectTreeItems.h"
#include "DlgItemResizer.h"
#include "NodeTreeCtrl.h"
#include "LeafTreeCtrl.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectSelectorLibrary
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
			// Parameter:  rTitle <in>:  a reference to the page title
			// Parameter:  SingleSelect <in>:  true for single selection mode (default)
			//************************************
			ObjectSelectorPpg( SVTreeLibrary::ObjectTreeItems& rTreeContainer, const SVString& rTitle, bool SingleSelect = true );

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
			inline SVTreeLibrary::ObjectTreeItems& getTreeContainer() const;

			//************************************
			// Description:  The method gets the display text for the current highlighted node.
			// Returns:  the display text of the highlighted node
			//************************************
			inline SVString getHighlightedNode() const;

			//************************************
			// Description:  The method sets the current highlighted node.
			// Parameter:  rHighlightedNode <in>:  a const reference to the display text of the node to be highlighted
			//************************************
			inline void setHighlightedNode( const SVString& rHighlightedNode );

			//************************************
			// Description:  The method sets the Help ID for the dialog.
			// Parameter:  HelpID <in>:  a unique identifier for this dialog
			//************************************
			inline void setHelpID( int HelpID );

			//************************************
			// Description:  The method is called to update the dialog with new data
			// Parameter:  pFromTree <in>:  a pointer to the tree containing the data to be displayed
			//************************************
			void updateData( const ObjectTreeCtrl* const pFromTree );
		#pragma endregion Public Methods

		protected:
		#pragma region Protected Methods
			DECLARE_MESSAGE_MAP()

			virtual void DoDataExchange(CDataExchange* pDX);
			virtual BOOL OnInitDialog();
			virtual BOOL OnSetActive();
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
			SVTreeLibrary::ObjectTreeItems& m_rTreeContainer; //The tree container reference
			CDlgItemResizer m_Resizer;						//Object which handles the resizing of all controls
			NodeTreeCtrl m_NodeTree;						//The node tree control
			LeafTreeCtrl m_LeafTree;						//The leaf tree control
			CImageList m_StateImageList;					//The state image list
			CImageList m_ImageList;							//The image list
			CString m_HighlightedNode;						//The currently highlighted node location
			int m_HelpID;									//The help id used to identify the property page
		#pragma endregion Member Variables
		};

		#pragma region Inline
		#include "ObjectSelectorPpg.inl"
		#pragma endregion Inline
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectSelectorPpg.h_v  $
 * 
 *    Rev 1.4   16 Dec 2014 17:39:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method OnSetActive.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Dec 2014 03:13:50   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed field name from Location to Highlighted Node
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Sep 2014 09:44:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Object Selector property page has a variable context ID
 * Added methods: setHelpID
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