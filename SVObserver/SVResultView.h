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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma region Declarations
enum { SV_LINE_STEP = 10 };

class SVIPDoc;
#pragma endregion Declarations

class SVResultViewClass : public CListView
{
	DECLARE_DYNCREATE( SVResultViewClass )

public:
	//{{AFX_VIRTUAL(SVResultViewClass)
	public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) override;
	virtual void OnInitialUpdate() override;
	virtual BOOL Create( LPCTSTR LPSZClassName, LPCTSTR LPSZWindowName, DWORD DwStyle, const RECT& Rect, CWnd* PParentWnd, UINT NID, CCreateContext* PContext ) override;
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;
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
		virtual void AssertValid() const override;
		virtual void Dump( CDumpContext& dc ) const override;
	#endif

protected:
	typedef std::deque< SVIPResultItemDefinition > SVResultDefinitionDeque;

	SVResultViewClass();

	SVString CalcProcessesPerSecond( double p_LastTriggerDistance );

	void addColumnHeadings();
	void addItems();
	void setColumnWidths();

	SVResultDefinitionDeque m_ResultDefinitions;
	SVIPResultData m_ResultData;

	CRect m_ClientExtent;
	CRect m_Rect;
	double m_OldValue;
	double m_OldEndTime;
	BOOL m_ColumnWidthSet;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

