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
#include "SVOResource\resource.h"
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
			// Parameter:  title <in>: const string for page title
			// Parameter:  singleSelect <in>:  true for single selection mode (default).  Multi-select otherwise.
			//***********
			ObjectFilterPpg( SvTrl::ObjectTreeItems& treeContainer, LPCTSTR title, bool singleSelect = true );

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
			\param rLocation <in> reference to the location
			\param isChecked <in> new check-state
			\param rowNumber <in> row-number of this item in the grid control. Only used for SingleSelection, to set the current selection.
			***********/
			void changeCheckState( const SVString& rLocation, bool isChecked, int rowNumber );
#pragma endregion Protected Methods

		private:
		#pragma region Member Variables
			enum
			{
				NameColumn = 0,
				LocationColumn,
				CheckColumn,
				TypeColumn
			} Column_Enum;

			SvTrl::ObjectTreeItems& m_rTreeContainer; //The tree container reference
			CDlgItemResizer m_Resizer;						//Object which handles the resizing of all controls
			SvGcl::CGridCtrl m_Grid;						//The grid control to display the filtered items
			CImageList m_StateImageList;					//The state image list
			CImageList m_ImageList;							//The image list
			bool m_SingleSelect;							//True when the Object Selector is in single selection mode
			SVString m_CheckedLocation;						//The currently checked location (only used for SingleSelection)
			int m_CheckedRow;								//The currently chacked row (only used for SingleSelection)
			SvOml::SVEditControlWithHelp m_FilterNameControl; //Control to filter objects based on their names
			SvOml::SVEditControlWithHelp m_FilterLocationControl; //Control to filter objects based on their locations
			CComboBox m_checkedControl;						//Control to filter objects based on the states of their checkboxes
			CComboBox m_TypeControl;						//Control to filter objects based on their types
		#pragma endregion Member Variables
		};
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

namespace SvOsl = Seidenader::ObjectSelectorLibrary;

