//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShapeEditorDlg
//* .File Name       : $Workfile:   SVMaskShapeEditorDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 17:52:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Operators/SVShapeMaskHelperClass.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOGui/CDSVPictureDisplay.h"
#include "SVOGui/DataController.h"
#include "SVOGui/LinkedValue.h"
#include "SVOGui/MaskController.h"
#pragma endregion Includes

namespace SvOp
{
class SVUserMaskOperatorClass;
}

class SVMaskShapeEditorDlg : public CDialog
{
#pragma region Construction
public:
	SVMaskShapeEditorDlg(uint32_t inspectionId, uint32_t taskObjectId, uint32_t maskOperatorId, uint32_t shapeMaskHelperId, CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVMaskShapeEditorDlg();
#pragma endregion Construction

#pragma region Public Methods
	void CheckPoint();
	void Revert();

	//************************************
	// Method:    getSelectedTab
	// Description: Return the current selected tab in the image control.
	// Access:    public 
	// Returns:   long: handle of the selected tab
	//************************************
	long getSelectedTab();

	//************************************
	// Method:    setSelectedTab
	// Description: Set the selected tab in the image control.
	// Access:    public 
	// Parameter: long tabNumber: handle of the tab
	//************************************
	void setSelectedTab(long tabNumber);

	SvOg::ValueController& GetValues() { return m_Values; }
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
#pragma region AFX Methods
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVMaskShapeEditorDlg)
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVMaskShapeEditorDlg)
	afx_msg void OnBtnFillColorMore();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelChangeComboShape();
	afx_msg void OnSelChangeComboMaskOperator();
	afx_msg void OnSelChangeComboMaskArea();
	afx_msg void OnSelChangeComboFillOptions();
	afx_msg void OnChangeEditFillColor();
	afx_msg void OnCheckAutoResize();
	afx_msg void OnCheckContRecalc();
	void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	void OnItemQueryShowButton(NMHDR*, LRESULT* plResult);
	void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion AFX Methods
	DECLARE_EVENTSINK_MAP()
	//************************************
	// Method:    ObjectChangedDialogimage
	// Description: Event-methods, called if overlay-object is changed. Set the new values to the mask properties.
	// Access:    public
	//************************************
	void ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue);

	//************************************
	// Method:    MouseMovedImDialogimage
	// Description: Event-methods, called if mouse is moving. Set the coordinate to the control for displaying.
	// Access:    public
	// Parameter: long Tab: tab handle of the use image, not used for this methods.
	// Parameter: long X: X-coordinate of the mouse in the images.
	// Parameter: long Y: Y-coordinate of the mouse in the images.
	//************************************
	void MouseMovedImDialogImage(long Tab, long X, long Y);

	static UINT_PTR CALLBACK ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	unsigned int GetPropertyID(SvPb::EmbeddedIdEnum embeddedId);
	SvPb::EmbeddedIdEnum GetPropertyEmbeddedId(int iPropertyID);
	HRESULT BuildPropertyList();

	void setPropertyToList(SvPb::EmbeddedIdEnum embeddedId, SVRPropertyItem* pRoot);

	HRESULT RefreshProperties();

	void SetInspectionData();
	void UpdateMask();
	void FillComboBox(const SvOg::ValueController& rValueController, SvPb::EmbeddedIdEnum embeddedId, CComboBox& rCombo);

	//************************************
	// Method:    setImages
	// Description: Set the images to the image control.
	// Access:    public
	//************************************
	void setImages();

	//************************************
	// Method:    setShapeType
	// Description: Set a shape type to the control. Renew the overlay to the image control.
	// Access:    public
	// Parameter: SVShapeMaskHelperClass::ShapeTypeEnum shapeType: the shape type enum.
	//************************************
	void setShapeType(SvOp::SVShapeMaskHelperClass::ShapeTypeEnum shapeType);

	//************************************
	// Method:    resetShapeOverlay
	// Description: Reset (e.g. resize) the overlays to the image control.
	// Access:    public
	//************************************
	void resetShapeOverlay();

	RECT getRect() const;

	CDSVPictureDisplay::AllowType getDrawChangeType() const;
	void setValueColumn(SvPb::EmbeddedIdEnum embeddedId, SVRPropertyItemEdit& rEdit);
	bool isChangable(SvPb::EmbeddedIdEnum) const;
#pragma endregion Private Methods

#pragma region Member Variables
private:
	// Dialog Data
	//{{AFX_DATA(SVMaskShapeEditorDlg)
	enum { IDD = IDD_MASK_SHAPE_EDITOR };
	CComboBox	m_cbMaskShape;
	CComboBox	m_cbMaskOperator;
	CComboBox	m_cbMaskArea;
	CComboBox	m_cbFillOptions;
	CString	m_sFillColor;
	CString	m_sCoordinates;
	BOOL	m_bContRecalc;
	BOOL	m_bAutoResize;
	SvOg::PictureDisplay m_dialogImage;
	SVRPropTree          m_Tree;
	//}}AFX_DATA

	static const long m_numberOfTabs = 3;
	std::vector<SvPb::EmbeddedIdEnum> m_propertyIds;
	static SVMaskShapeEditorDlg* m_pThis;
	bool m_isInit;
	long m_currentTabNumber; //only use until m_isInit is true
	long m_handleToActiveObjects[m_numberOfTabs];
	SvOp::SVShapeMaskHelperClass::ShapeTypeEnum m_eShapeType;
	SvOg::MaskController m_maskController;
	std::vector<std::pair<SvPb::EmbeddedIdEnum, std::variant<_variant_t, SvOg::LinkedValueData>>> m_ValuesSaved;
	std::vector<std::pair<SvPb::EmbeddedIdEnum, std::variant<_variant_t, SvOg::LinkedValueData>>> m_ShapeHelperValuesSaved;
	
	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	SvOg::ValueController m_Values;
	SvOg::ValueController m_ShapeHelperValues;
#pragma endregion Member Variables
};
