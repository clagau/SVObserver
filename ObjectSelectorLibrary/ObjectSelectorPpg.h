//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorPpg
//* .File Name       : $Workfile:   ObjectSelectorPpg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:24  $
//* ----------------------------------------------------------------------------
//* This class is the object selector property page 
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTreeLibrary\ObjectSelectorItem.h"
#include "SVTreeLibrary\ObjectTreeItems.h"
#include "SVOResource\resource.h"
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
			/**********
			 The class constructor
			 \param rTreeContainer <in> a reference to the tree to display
			 \param rTitle <in> a reference to the page title
			 \param SingleSelect <in> true for single selection mode
			***********/
			ObjectSelectorPpg( SVTreeLibrary::ObjectTreeItems& rTreeContainer, const SVString& rTitle, bool SingleSelect );

			/**********
			 The class destructor
			***********/
			~ObjectSelectorPpg();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			 The method gets the tree container
			 \return a reference to the tree container
			***********/
			inline SVTreeLibrary::ObjectTreeItems& getTreeContainer() const;

			/**********
			 The method gets the current node location
			 \return the node location
			***********/
			inline SVString getNodeLocation() const;

			/**********
			 The method sets the current node location
			 \param rNodeLocation <in> a reference to the node location
			***********/
			inline void setNodeLocation( const SVString& rNodeLocation );

			/**********
			 The method is called to update the dialog data
			 \param pFromeTree <in> a pointer from which tree the update command originates
			***********/
			void updateData( const ObjectTreeCtrl* const pFromTree );

		#pragma endregion Public Methods

		protected:
		#pragma region Protected Methods
			DECLARE_MESSAGE_MAP()

			/**********
			 The method exchanges the data between class and page
			 \param pDX <in> pointer to the data exchange object
			***********/
			virtual void DoDataExchange(CDataExchange* pDX);
			
			/**********
			 The method initializes the dialog
			 \return True if default control receives focus
			***********/
			virtual BOOL OnInitDialog();

			/**********
			 The method is called when the size of the dialog is changed
			 \param nType <in> resizing type request
			 \param cx <in> the new x size
			 \param cy <in> the new y size
			***********/
			afx_msg void OnSize(UINT nType, int cx, int cy);

			/**********
			 The method sets the controls which are resized when the page is resized
			***********/
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
			CString m_NodeLocation;							//The currently selected node location
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
