//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIODoc
//* .File Name       : $Workfile:   sviodoc.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 20:06:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

class SVIOController;
// If an io entry will be connected to the outside world or not, 
// depends on two facts:
// First the user has to assign an existing channel to a possible
// io entry or this is done by default, refer to IODoc::IODoc().
// Second in any part of this software must be an 
// IOControl::CreateIOEntry call to open this io entry for 
// reading or writing.
class SVIODoc : public CDocument
{
	DECLARE_DYNCREATE( SVIODoc )

public:
	//{{AFX_MSG(SVIODoc)
	afx_msg void OnExtrasTestoutputs();
	afx_msg void OnExtrasEditRemoteInputs();
	afx_msg void OnEditGlobalConstantAdd();
	afx_msg void OnUpdateFileExit(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVIODoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual CFile* GetFile( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError );
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

public:
	virtual ~SVIODoc();

	void InitIO();

	void CloseDocument();

	void updateGlobalConstantsView() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	SVIOController* GetIOController() const;

	SVIOController* m_pIOController;

protected:
	SVIODoc();

private:
	void InitMenu();

	SVFileNameClass msvFileName;

};

//{{AFX_INSERT_LOCATION}}

