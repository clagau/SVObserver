//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class is a list view for the Global constants
/// Used to display, add, edit and delete the constants
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/GlobalConstantData.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#pragma endregion Includes
 
class SVIODoc;
 
class GlobalConstantView : public CListView
{
#pragma region Declarations
private:
	enum HeaderColumns
	{
		GlobalConstantCol,
		TypeCol,
		ValueCol,
		DescriptionCol
	};

	DECLARE_DYNCREATE(GlobalConstantView)
	DECLARE_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
private:
	GlobalConstantView();

	virtual ~GlobalConstantView();
#pragma endregion Constructor
	
#pragma region Public
public:
	//************************************
	//! The method updates obnly this view
	//************************************
	void updateView();

	afx_msg void OnAddItem();
	afx_msg void OnUpdateAddItem(CCmdUI* pCmdUI);
#pragma endregion Public

#pragma region Protected
protected:
	//************************************
	//! The method is called prior to creating the window
	//! \param cs <in> create structure
	//! \returns True on success
	//************************************
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs ) override;

	//************************************
	//! The method is called to update the view
	//! \param pSender <in> pointer to the view which sent the update 
	//! \param lHint <in> information to the modifications
	//! \param pHint <in> object with information to the modifications
	//************************************
	virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) override;
#pragma endregion Protected

#pragma region Private
private:
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump( CDumpContext& dc ) const override;
#endif

	//************************************
	//! The method is called to add or edit an item
	//! \param Item <in> the index of the item to edit if -1 it will add an item
	//! \returns true when successfully edited
	//************************************
	bool editItem( int Item );

	//************************************
	//! The method is called to delete an item
	//! \param Item <in> the index of the item to delete
	//! \returns true when successfully deleted
	//************************************
	bool deleteItem( int Item );

	//************************************
	//! The method is called to insert an item to the list
	//! \param rpObject <in> a reference to a pointer containing the data to insert
	//! \param Pos <in> is the position where to insert the new item
	//! \returns true when successfully deleted
	//************************************
	int insertItem( const BasicValueObjectPtr& rpObject, int Pos );

	//************************************
	//! The method inserts a new Global Constant
	//! \param rGlobalData <in> reference to the global data structure to insert
	//************************************
	void insertGlobalConstant( const SvOi::GlobalConstantData& rGlobalData ) const;

	//************************************
	//! The method edits an existing Global Constant
	//! \param rGlobalData <in> reference to the global data structure to edit
	//************************************
	void editGlobalConstant( const SvOi::GlobalConstantData& rGlobalData ) const;
 
	//************************************
	//! This method retrieves the corresponding IO document
	//! \returns a pointer to the document
	//************************************
	SVIODoc* GetDocument();

	//************************************
	//! The method is called to update all the inspection documents
	//! \param RunOnce <in> when true a runonce is called
	//************************************
	void updateAllIPDocs( bool RunOnce ) const;

	//************************************
	//! The method is called to determine which item is selected
	//! \returns the index of the selected item or -1 when none are selected
	//************************************
	int getSelectedItem() const;

	//************************************
	//! The method is called to check the dependencies of all IPs
	//! \param pObject <in> pointer to the object to check for dependencies
	//! \param ConfirmDelete <in> true when checking to delete an item
	//! \returns true if object can be deleted
	//************************************
	bool checkAllDependencies( BasicValueObject* pObject, bool ConfirmDelete ) const;

	virtual BOOL PreTranslateMessage( MSG* pMsg ) override;
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnContextMenu( CWnd* pWnd, CPoint point );
	afx_msg void OnEditItem();
	afx_msg void OnUpdateEditItem(CCmdUI* pCmdUI);
	afx_msg void OnDeleteItem();
	afx_msg void OnUpdateDeleteItem(CCmdUI* pCmdUI);
	afx_msg void OnShowDependencies();
	afx_msg void OnUpdateShowDependencies(CCmdUI* pCmdUI);

#pragma endregion Private

#pragma region Member variables
private:
	CListCtrl& m_rCtrl;								//Reference to the list control
	NameBasicValuePtrVector m_DataList;				//Data list for sorting
	SVIODoc* m_pDocument;							//Pointer to the corresponding IO document
	CMenu m_ContextMenuItem;						//Menu object for context menu
 	CImageList m_ImageList;							//List of images for the list
	CImageList m_StateImageList;					//List of state images for the list
#pragma endregion Member variables
};
