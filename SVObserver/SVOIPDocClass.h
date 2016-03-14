//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIPDocClass
//* .File Name       : $Workfile:   SVOIPDocClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:07:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

class SVOIPDocClass : public COleServerDoc
{
	SVFileNameClass msvFileName;

protected:
	SVOIPDocClass();           // protected constructor used by dynamic creation

public:
	DECLARE_DYNCREATE(SVOIPDocClass)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIPDocClass)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	protected:
	virtual BOOL OnNewDocument();
	virtual COleServerItem* OnGetEmbeddedItem();
	//}}AFX_VIRTUAL

public:
	virtual void CloseDocument();
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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

