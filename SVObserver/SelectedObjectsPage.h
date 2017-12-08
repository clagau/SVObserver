//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class is used as a property page for different sheets 
/// It lists selected objects and uses the object selector to change the selection
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

class SelectedObjectsPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	enum { IDD = IDD_SELECTED_OBJECTS_PAGE };
#pragma region Constructor
public:
	SelectedObjectsPage( const std::string& rInspectionName, const SVGUID& rInspectionID, LPCTSTR Caption, const SvCl::SelectorItemVector& rList, UINT AttributeFilters, int id = IDD );
	virtual ~SelectedObjectsPage();
#pragma endregion Constructor

#pragma region Public Methods
public:
	// ISVPropertyPageDialog
	virtual bool QueryAllowExit() override;
	virtual BOOL OnSetActive() override;
	virtual BOOL OnKillActive() override;

	const SvCl::SelectorItemVector& getList() const { return m_List; };
	UINT getAttributeFilter() const { return m_AttributeFilter; };

#pragma endregion Public Methods

#pragma region Private Methods
private:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	afx_msg void OnDblClickListSelected(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelectObjects();
	afx_msg void OnRemoveItem();
	afx_msg void OnRemoveAllItems();

	void ReadSelectedObjects();
	void ShowObjectSelector();
#pragma endregion Private Methods

#pragma region Member variables
private:
	std::string m_InspectionName;					//Inspection name for title
	SVGUID m_InspectionID;						//Unique Instance ID for the Inspection
	SvCl::SelectorItemVector m_List;			//The selected list makes a copy of the list in the sheet
	CListCtrl   m_ItemsSelected;				//The selected list control
	CImageList m_StateImageList;				//The state image list
	CButton m_Select;							//The tree select button
	CBitmap m_TreeBitmap;						//The bitmap for the tree button
	UINT	m_AttributeFilter;					//The attribute filter for the object selector
#pragma endregion Member variables
};
