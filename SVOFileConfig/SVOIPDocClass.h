// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIPDocClass
// * .File Name       : $Workfile:   SVOIPDocClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:34:58  $
// ******************************************************************************

#pragma once

class SVOIPDocClass : public COleServerDoc
{
protected:
	SVOIPDocClass();           // protected constructor used by dynamic creation

public:
	DECLARE_DYNCREATE(SVOIPDocClass)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIPDocClass)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	virtual COleServerItem* OnGetEmbeddedItem();
	//}}AFX_VIRTUAL

public:
	virtual ~SVOIPDocClass();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(SVOIPDocClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

