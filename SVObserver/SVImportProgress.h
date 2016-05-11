//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportProgress
//* .File Name       : $Workfile:   SVImportProgress.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:55:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVIProgress.h"
#include "SVProgressDialog.h"
#pragma endregion Includes

class SVImportTask;

template<typename Task>
class SVImportProgress : public SVIProgress
{
public:
	SVImportProgress(Task& rTask, LPCTSTR title, CWnd* pParent=nullptr);
	virtual ~SVImportProgress();

	void DoModal();

	virtual HRESULT Start();
	void Cancel();
	void Complete();

	virtual HRESULT UpdateProgress(unsigned long p_Current, unsigned long p_Total);
	virtual HRESULT UpdateText(LPCTSTR text);

	HRESULT GetStatus() const;

private:
	static DWORD WINAPI TaskThread(LPVOID lpHost);

	SVProgressDialog m_Dialog;
	Task& m_rTask;
	HRESULT m_Status;
	HANDLE m_hThread;
};

#include "SVImportProgress.inl"
