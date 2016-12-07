// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIODocClass
// * .File Name       : $Workfile:   SVOIODocClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:33:24  $
// ******************************************************************************

#pragma once

class SVOIODocClass : public COleServerDoc
{
protected:
	SVOIODocClass();           // protected constructor used by dynamic creation

public:
	DECLARE_DYNCREATE(SVOIODocClass)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIODocClass)
	public:
	virtual void Serialize(CArchive& ar) override;   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
	protected:
	virtual BOOL OnNewDocument() override;
	virtual COleServerItem* OnGetEmbeddedItem() override;
	//}}AFX_VIRTUAL

public:
	virtual ~SVOIODocClass();

#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

protected:
	//{{AFX_MSG(SVOIODocClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


