//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportProgress
//* .File Name       : $Workfile:   SVImportProgress.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:58:30  $
//******************************************************************************


#include "Definitions/SVUserMessage.h"

template<typename Task>
DWORD WINAPI SVImportProgress<Task>::TaskThread(LPVOID lpHost)
{
	SVImportProgress<Task>* l_pProgressTask = ( reinterpret_cast<SVImportProgress<Task> *>(lpHost));

	l_pProgressTask->m_Status = l_pProgressTask->m_rTask.Import(*l_pProgressTask);

	l_pProgressTask->Complete();

	return S_OK;
}

template<typename Task>
SVImportProgress<Task>::SVImportProgress(Task& rTask, LPCTSTR title, CWnd* pParent)
: SVIProgress()
, m_rTask(rTask)
, m_Dialog(title, pParent)
, m_Status(S_OK)
{
	m_Dialog.m_pTask = this;
}

template<typename Task>
SVImportProgress<Task>::~SVImportProgress()
{
	if (m_hThread)
	{
		::CloseHandle(m_hThread);
	}
}

template<typename Task>
void SVImportProgress<Task>::DoModal()
{
	m_Dialog.DoModal();
}

template<typename Task>
HRESULT SVImportProgress<Task>::Start()
{
	HRESULT l_Status(S_OK);

	m_hThread = ::CreateThread(nullptr, 0, &SVImportProgress<Task>::TaskThread, this, 0, nullptr);

	if (nullptr == m_hThread)
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

template<typename Task>
void SVImportProgress<Task>::Complete()
{
	m_Dialog.PostMessage(SV_END_PROGRESS_DIALOG, 0, 0);
}

template<typename Task>
HRESULT SVImportProgress<Task>::GetStatus() const
{
	return m_Status;
}

template<typename Task>
void SVImportProgress<Task>::Cancel()
{
	m_Status = E_FAIL;
}

template<typename Task>
HRESULT SVImportProgress<Task>::UpdateProgress(unsigned long p_Current, unsigned long p_Total)
{
	HRESULT l_Status(m_Status);

	if (S_OK == l_Status)
	{
		m_Dialog.SendMessage(SV_UPDATE_PROGRESS, p_Current, p_Total);
	}
	return l_Status;
}

template<typename Task>
HRESULT SVImportProgress<Task>::UpdateText(LPCTSTR text)
{
	m_Dialog.SendMessage(SV_UPDATE_PROGRESS_TEXT, 0L, reinterpret_cast<LPARAM>(text));
	return true;
}

