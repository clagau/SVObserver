//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectFilterPpg
//* .File Name       : $Workfile:   ObjectFilterPpg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Dec 2014 17:34:30  $
//* ----------------------------------------------------------------------------
//* This class is the object selector filter property page.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTreeLibrary\ObjectTreeItems.h"
#include "GridCtrlLibrary\GridCtrl.h"
#include "SVOMFCLibrary\SVEditControlWithHelp.h"
#include "DlgItemResizer.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectSelectorLibrary
	{
		class ObjectFilterPpg : public CPropertyPage
		{
		public:
		#pragma region Declarations
			enum { IDD = IDD_OBJECT_SELECTOR_FILTER };
		#pragma endregion Declarations

		#pragma region Constructor
			//**********
			// Descripion:  The preferred constructor
			// Parameter:  treeContainer <in>:  reference to the tree to display
			// Parameter:  title <in>:  reference to the text for the page title
			// Parameter:  singleSelect <in>:  true for single selection mode (default).  Multi-select otherwise.
			//***********
			ObjectFilterPpg( SVTreeLibrary::ObjectTreeItems& treeContainer, const SVString& title, bool singleSelect = true );

			//**********
			// Descripion:  The virtual destructor
			//***********
			virtual ~ObjectFilterPpg();

		#pragma endregion Constructor

		protected:
		#pragma region Protected Methods
			DECLARE_MESSAGE_MAP()

			virtual void DoDataExchange(CDataExchange* pDX);
			virtual BOOL OnInitDialog();
			virtual BOOL OnSetActive( );
			afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
			afx_msg void OnGridRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
			afx_msg void OnChangeFilter();
			afx_msg void OnResetFilter();
			afx_msg void OnRButtonDown( UINT Flags, CPoint Point );
			afx_msg void OnCheckAll();
			afx_msg void OnUncheckAll();

			/**********
			The method check or unchecked all current displayed items.
			\param shouldCheck <in> check (true) or uncheck (false) 
			***********/
			void checkAllItem(bool shouldCheck);

			/**********
			 The method sets the controls which are resized when the page is resized.
			***********/
			void setResizeControls();

			/**********
			 The method load the grid control from tree.
			***********/
			void loadGridCtrl();

			/**********
			The method displays the context menu.
			\param Point <in> position (left up corner) of the context menu 
			***********/
			void showContextMenu( CPoint Point );

			/**********
			The method change the check-state of the tree item. DO NOT change the state in the grid control.
			\param location <in> location of the item
			\param isChecked <in> new check-state
			\param rowNumber <in> row-number of this item in the grid control. Only used for SingleSelection, to set the current selection.
			***********/
			void changeCheckState( const CString &location, bool isChecked, int rowNumber );
#pragma endregion Protected Methods

		private:
		#pragma region Member Variables
			enum
			{
				NameColumn = 0,
				LocationColumn,
				CheckedColumn,
				TypeColumn
			} Column_Enum;

			SVTreeLibrary::ObjectTreeItems& m_rTreeContainer; //The tree container reference
			CDlgItemResizer m_Resizer;						//Object which handles the resizing of all controls
			Seidenader::GridCtrlLibrary::CGridCtrl m_Grid;
			CImageList m_StateImageList;					//The state image list
			CImageList m_ImageList;							//The image list
			bool m_SingleSelect;							//True when the Object Selector is in single selection mode
			int m_RowOfCurrentSelection;					//The row of current selection (only used for SingleSelection)
			Seidenader::SVOMFCLibary::SVEditControlWithHelp m_FilterNameControl; //Control to filter objects based on their names
			Seidenader::SVOMFCLibary::SVEditControlWithHelp m_FilterLocationControl; //Control to filter objects based on their locations
			CComboBox m_checkedControl;						//Control to filter objects based on the states of their checkboxes
			CComboBox m_TypeControl;						//Control to filter objects based on their types
		#pragma endregion Member Variables
		};
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectFilterPpg.h_v  $
 * 
 *    Rev 1.0   16 Dec 2014 17:34:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */