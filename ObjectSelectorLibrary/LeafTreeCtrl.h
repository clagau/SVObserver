//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LeafTreeCtrl
//* .File Name       : $Workfile:   LeafTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Aug 2014 07:46:18  $
//* ----------------------------------------------------------------------------
//* This class is used to display the node tree control
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectTreeCtrl.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectSelectorLibrary
	{
		//Forward declarations
		class ObjectSelectorPpg;

		class LeafTreeCtrl : public ObjectTreeCtrl
		{
		public:
		#pragma region Constructor
			/**********
			 The class constructor
			 \param rParent <in> a reference to the parent property page
			 \param SingleSelect <in> true for single selection mode
			***********/
			LeafTreeCtrl( ObjectSelectorPpg& rParent, bool SingleSelect );

			/**********
			 The class destructor
			***********/
			virtual ~LeafTreeCtrl();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			The method loads the control with the tree items from the container
			***********/
			virtual void loadTree() override;

			/**********
			The method updates the tree control 
			***********/
			virtual void updateTree() override;
		#pragma endregion Public Methods

		protected:
		#pragma region Protected Methods
			DECLARE_MESSAGE_MAP()
			//{{AFX_MSG(LeafTreeCtrl)
			afx_msg void OnCheckAll();
			afx_msg void OnUncheckAll();
			//}}AFX_MSG

			/**********
			The method is called when the tree item selection has changed
			\param rItem <in> reference to the newly selected item
			***********/
			virtual void changeSelectedItem( const HTREEITEM& rItem ) override;

			/**********
			The method displays the context menu
			\param ItemHit <in> true if item is hit
			***********/
			virtual void showContextMenu( bool ItemHit ) override;

			/**********
			The method determines if item is checkable
			***********/
			virtual bool isCheckable() const override;
		#pragma endregion Protected Methods

		private:
		#pragma region Member Variables
			SVString m_NodeLocation;						//The current node location
		#pragma endregion Member Variables
		};
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\LeafTreeCtrl.h_v  $
 * 
 *    Rev 1.1   18 Aug 2014 07:46:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Coding guidline changes using const object for get and is methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:12:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
