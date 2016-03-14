//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIODocClass
//* .File Name       : $Workfile:   SVOIODocClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:06:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

class SVOIODocClass : public COleServerDoc
{
	SVFileNameClass msvFileName;

protected:
	SVOIODocClass();           // protected constructor used by dynamic creation

public:
	DECLARE_DYNCREATE(SVOIODocClass)

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIODocClass)
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
	virtual ~SVOIODocClass();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(SVOIODocClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

