//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultView
//* .File Name       : $Workfile:   SVResultView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Jan 2015 17:51:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVResultList.h"
#include "SVIPResultData.h"
#pragma endregion Includes

#pragma region Declarations
enum { SV_LINE_STEP = 10 };

class SVIPDoc;
class SVResultsWrapperClass;
#pragma endregion Declarations

class SVResultViewClass : public CListView
{
	DECLARE_DYNCREATE( SVResultViewClass )

public:
	//{{AFX_VIRTUAL(SVResultViewClass)
	public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual void OnInitialUpdate();
	virtual BOOL Create( LPCTSTR LPSZClassName, LPCTSTR LPSZWindowName, DWORD DwStyle, const RECT& Rect, CWnd* PParentWnd, UINT NID, CCreateContext* PContext );
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVResultViewClass)
	protected:
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd( CDC* p_pDC );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	virtual ~SVResultViewClass();

	SVIPDoc* GetIPDoc() const;

	BOOL GetParameters(SVObjectWriter& rWriter);

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	BOOL CheckParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	void SetViewSize (CSize &Size);
	void OnSize( UINT nType, int cx, int cy );

	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump( CDumpContext& dc ) const;
	#endif

protected:
	typedef std::deque< SVIPResultItemDefinition > SVResultDefinitionDeque;

	SVResultViewClass();

	CString CalcProcessesPerSecond( double p_LastTriggerDistance );

	void addColumnHeadings();
	void addItems();
	void setColumnWidths();

	SVResultDefinitionDeque m_ResultDefinitions;
	SVIPResultData m_ResultData;

	CRect clientExtent;
	CRect rect;
	double oldValue;
	double oldEndTime;
	BOOL columnWidthSet;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

