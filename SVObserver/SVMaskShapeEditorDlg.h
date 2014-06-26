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

#ifndef SVMASKSHAPEEDITORDLG_H
#define SVMASKSHAPEEDITORDLG_H

#pragma region Includes
#include "SVShapeMaskHelperClass.h"
#include "SVMaskShape.h"
#include "PictureDisplay.h"
#include "PropertyTree/PropTree.h"
#include "ISVCancel.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVMaskShapeEditorDlg : public CDialog, public ISVCancel2, public SVTaskObjectInterfaceClass
{
#pragma region Construction
public:
	SVMaskShapeEditorDlg( SVToolClass* pTool, SVUserMaskOperatorClass* pMask, CWnd* pParent = NULL);   // standard constructor
	~SVMaskShapeEditorDlg();
#pragma endregion Construction

#pragma region Public Methods
#pragma region Virtual
public:
	// ISVCancel2
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rMap);

	// SVTaskObjectInterfaceClass
	virtual HRESULT SetInspectionData();
#pragma endregion Virtual

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
	PictureDisplay       m_dialogImage;
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
	SVToolClass*                       m_pTool;
	SVUserMaskOperatorClass*           m_pMask;
#pragma endregion Member Variables
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVMASKSHAPEEDITORDLG_H__130C38BF_EF89_4F9A_B654_09012A0524CD__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMaskShapeEditorDlg.h_v  $
 * 
 *    Rev 1.3   26 Jun 2014 17:52:12   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Feb 2014 18:31:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Aug 2013 09:48:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVShapeMaskHelperClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:27:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 12:00:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Mar 2006 09:20:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Auto Resize capability to the Shape Mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Aug 2005 10:36:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   implemented Input Request Queue tool updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:28:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/