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
#include "SVShapeMaskHelperClass.h"
#include "SVMaskShape.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#include "PropertyTree/PropTree.h"
#include "ISVCancel.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

class SVMaskShapeEditorDlg : public CDialog, public ISVCancel2, public SVTaskObjectInterfaceClass
{
#pragma region Construction
public:
	SVMaskShapeEditorDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGUID& rMaskOperatorID, CWnd* pParent = NULL);   // standard constructor
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

	SVMaskShape* GetCurrentShape(); // holds the properties and does the rendering
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	// ISVCancel2
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rMap);

	// SVTaskObjectInterfaceClass
	virtual HRESULT SetInspectionData();

#pragma region AFX Methods
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVMaskShapeEditorDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
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
	void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
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
	int GetPropertyID(GUID guidProperty );
	GUID GetPropertyGuid(int iPropertyID );
	HRESULT BuildPropertyList();
	HRESULT RefreshProperties();

	HRESULT UpdateMask(bool bResetObject = false);
	void FillComboBox(SVEnumerateValueObjectClass& p_rValueObject, CComboBox* p_pCombo);

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
	void setShapeType(SVShapeMaskHelperClass::ShapeTypeEnum shapeType);

	//************************************
	// Method:    resetShapeOverlay
	// Description: Reset (e.g. resize) the overlays to the image control.
	// Access:    public
	//************************************
	void resetShapeOverlay();
#pragma endregion Private Methods

#pragma region Member Variables
	// Dialog Data
	//{{AFX_DATA(SVMaskShapeEditorDlg)
	enum { IDD = IDD_MASK_SHAPE_EDITOR };
	CComboBox	m_cbMaskShape;
	CComboBox	m_cbMaskOperator;
	CComboBox	m_cbMaskArea;
	CComboBox	m_cbFillOptions;
	CString	m_sFillColor;
	CString	m_sCoordinates;
	BOOL	m_bAutoResize;
	SvOg::PictureDisplay m_dialogImage;
	SVRPropTree          m_Tree;
	//}}AFX_DATA

	static const long m_numberOfTabs = 3;
	typedef std::map<SVShapeMaskHelperClass::ShapeTypeEnum, SVMaskShape*> ShapeMap;

	ShapeMap m_mapShapes;
	std::map<GUID, int> m_mapPropertyIds;
	static SVMaskShapeEditorDlg* m_pThis;
	bool m_isInit;
	long m_currentTabNumber; //only use until m_isInit is true
	long m_handleToActiveObjects[m_numberOfTabs];
	SVShapeMaskHelperClass::ShapeTypeEnum m_eShapeType;
	SVToolClass* m_pTool;
	SVUserMaskOperatorClass* m_pMask;
	SVInputRequestStructMap m_cancelData;
#pragma endregion Member Variables
};
