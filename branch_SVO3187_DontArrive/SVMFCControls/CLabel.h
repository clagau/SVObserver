//******************************************************************************
//* Author           : NormDroid, 3 Aug 2002
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/215/Extended-Use-of-CStatic-Class-CLabel
//******************************************************************************
//* .Module Name     : CLabel
//* .File Name       : $Workfile:     $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:     $
//* .Check In Date   : $Date:     $
//******************************************************************************

// CLabel - A fully stocked owner drawn CStatic class

#pragma once

#define	NM_LINKCLICK	WM_APP + 0x200

/////////////////////////////////////////////////////////////////////////////
// CLabel window

class CLabel : public CStatic
{
// Construction
public:
	enum LinkStyle { LinkNone, HyperLink, MailLink };
	enum FlashType { None, Text, Background };
	enum Type3D { Raised, Sunken };
	enum BackFillMode { Normal, Gradient };

	CLabel();
	virtual ~CLabel();
	CLabel& SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh = 0, BackFillMode mode = Normal);
	CLabel& SetTextColor(COLORREF crText);
	CLabel& SetText(const CString& strText);
	CLabel& SetFontBold(BOOL bBold);
	CLabel& SetFontName(const CString& strFont, BYTE byCharSet = ANSI_CHARSET);
	CLabel& SetFontUnderline(BOOL bSet);
	CLabel& SetFontItalic(BOOL bSet);
	CLabel& SetFontSize(int nSize);
	CLabel& SetSunken(BOOL bSet);
	CLabel& SetBorder(BOOL bSet);
	CLabel& SetTransparent(BOOL bSet);
	CLabel& FlashText(BOOL bActivate);
	CLabel& FlashBackground(BOOL bActivate);
	CLabel& SetLink(BOOL bLink,BOOL bNotifyParent);
	CLabel& SetLinkCursor(HCURSOR hCursor);
	CLabel& SetFont3D(BOOL bSet,Type3D type=Raised);
	CLabel& SetRotationAngle(UINT nAngle,BOOL bRotation);
	CLabel& SetText3DHiliteColor(COLORREF cr3DHiliteColor);
	CLabel& SetFont(LOGFONT lf);
	CLabel& SetMailLink(BOOL bEnable, BOOL bNotifyParent);
	CLabel& SetHyperLink(const CString& sLink);

// Attributes
protected:
	void UpdateSurface();
	void ReconstructFont();
	void DrawGradientFill(CDC* pDC, CRect* pRect, COLORREF crStart, COLORREF crEnd, int nSegments);
	COLORREF		m_crText;
	COLORREF		m_cr3DHiliteColor;
	HBRUSH			m_hwndBrush;
	HBRUSH			m_hBackBrush;
	LOGFONT			m_lf;
	CFont			m_font;
	BOOL			m_bState;
	BOOL			m_bTimer;
	LinkStyle		m_Link;
	BOOL			m_bTransparent;
	BOOL			m_bFont3d;
	BOOL			m_bToolTips;
	BOOL			m_bNotifyParent;
	BOOL			m_bRotation;
	FlashType		m_Type;
	HCURSOR			m_hCursor;
	Type3D			m_3dType;
	BackFillMode	m_fillmode;
	COLORREF		m_crHiColor;
	COLORREF		m_crLoColor;
	CString			m_sLink;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabel)
protected:
	virtual void PreSubclassWindow() override;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabel)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

