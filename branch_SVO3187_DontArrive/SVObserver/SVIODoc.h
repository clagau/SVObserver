//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

class SVIOController;
// If an io entry will be connected to the outside world or not, 
// depends on two facts:
// First the user has to assign an existing channel to a possible
// io entry or this is done by default, refer to IODoc::IODoc().
// Second in any part of this software must be an 
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
	virtual void OnCloseDocument() override;
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame) override;
	virtual void SetTitle(LPCTSTR lpszTitle) override;
	virtual CFile* GetFile( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError ) override;
	protected:
	virtual BOOL SaveModified() override;
	//}}AFX_VIRTUAL

public:
	virtual ~SVIODoc();

	void CloseDocument();

	void updateGlobalConstantsView() const;

	void ShowIOTab(DWORD dwID);
	void HideIOTab(DWORD dwID);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	SVIOController* GetIOController() const;
	void SetIOController(SVIOController* pController);

protected:
	SVIODoc();

private:
	void InitMenu();

	SVIOController* m_pIOController;
};


//{{AFX_INSERT_LOCATION}}

SVIODoc* GetTheIODoc();
SVIODoc* NewSVIODoc(LPCTSTR DocName, SVIOController& Controller);


void ShowIOTabIfPossible(DWORD dwID);
void HideIOTabIfPossible(DWORD dwID);

CDocTemplate* CreateIoDocMultiDocTemplate();


