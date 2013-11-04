//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShapeEditorDlg
//* .File Name       : $Workfile:   SVMaskShapeEditorDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 09:48:54  $
//******************************************************************************

#ifndef SVMASKSHAPEEDITORDLG_H
#define SVMASKSHAPEEDITORDLG_H

#include "SVDlgImageEditor.h"
#include "SVShapeMaskHelperClass.h"
#include "SVMaskShape.h"
#include "SVMaskShapeFigureEditors.h"
#include "PropertyTree/PropTree.h"
#include "ISVCancel.h"
#include "SVTaskObjectInterfaceClass.h"

class SVMaskShapeEditorDlg : public CDialog, public ISVCancel, public ISVCancel2, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVMaskShapeEditorDlg(CWnd* pParent = NULL);   // standard constructor

	// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

	// ISVCancel2
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rMap);

	// SVTaskObjectInterfaceClass
	virtual HRESULT SetInspectionData();


// Dialog Data
	//{{AFX_DATA(SVMaskShapeEditorDlg)
	enum { IDD = IDD_MASK_SHAPE_EDITOR };
	CComboBox	m_cbZoom;
	CTabCtrl	m_tabImage;
	CComboBox	m_cbMaskShape;
	CComboBox	m_cbMaskOperator;
	CComboBox	m_cbMaskArea;
	CComboBox	m_cbFillOptions;
	CString	m_sFillColor;
	CString	m_sCoordinates;
	BOOL	m_bAutoResize;
	//}}AFX_DATA
	SVDlgImageEditor     m_dialogImage;
	SVRPropTree          m_Tree;

	SVShapeMaskHelperClass::ShapeTypeEnum m_eShapeType;

	SVMaskShape* GetCurrentShape();	// holds the properties and does the rendering
	SVMaskShapeFigureEditor* GetCurrentFigureEditor();  // handles GUI feedback ( mouse move, cursor changes, etc. )

	enum ViewImageEnum
	{
		VIEW_SOURCE = 0,
		VIEW_MASK   = 1,
		VIEW_RESULT = 2,
	};

	ViewImageEnum m_ePage;

	SVToolClass*                       m_pTool;
	SVUserMaskOperatorClass*           m_pMask;

// Implementation
protected:
	struct ShapePair
	{
		ShapePair(SVMaskShape* p_pShape, SVMaskShapeFigureEditor* p_pFigureEditor)
		{
			pShape = p_pShape;
			pFigureEditor = p_pFigureEditor;
			pFigureEditor->SetShape(pShape);
		}
		ShapePair(const ShapePair& rhs)
		{
			pShape = rhs.pShape;
			pFigureEditor = rhs.pFigureEditor;
		}
		ShapePair() {}

		SVMaskShape* pShape;
		SVMaskShapeFigureEditor* pFigureEditor;
	};
	typedef std::map<SVShapeMaskHelperClass::ShapeTypeEnum, ShapePair> ShapeMap;

	ShapeMap m_mapShapes;

	void DisplayImagePage();
	int GetPropertyID(GUID guidProperty );
	GUID GetPropertyGuid(int iPropertyID );
	HRESULT BuildPropertyList();
	HRESULT RefreshProperties();

	HRESULT UpdateMask(bool bResetObject = false);

	std::map<GUID, int> m_mapPropertyIds;

	double m_dZoom;
	CRect m_rectViewport;

	std::vector<double> m_vecZoomLevels;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVMaskShapeEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVMaskShapeEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeImageTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnFillColorMore();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnZoomInButton();
	afx_msg void OnZoomOutButton();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual void OnOK();
	afx_msg void OnSelchangeComboShape();
	afx_msg void OnSelchangeComboMaskOperator();
	afx_msg void OnSelchangeComboMaskArea();
	afx_msg void OnSelchangeComboFillOptions();
	afx_msg void OnSelchangeComboZoomScale();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEditchangeComboZoomScale();
	afx_msg void OnChangeEditFillColor();
	afx_msg void OnCheckAutoResize();
	//}}AFX_MSG
	void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	DECLARE_MESSAGE_MAP()

	static UINT_PTR CALLBACK ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );

private:
	void FillComboBox(SVEnumerateValueObjectClass& p_rValueObject, CComboBox* p_pCombo);
	static SVMaskShapeEditorDlg* m_pThis;
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
