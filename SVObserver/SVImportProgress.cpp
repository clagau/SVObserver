//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportProgress
//* .File Name       : $Workfile:   SVImportProgress.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:58:30  $
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVImportProgress.h"
#include "SVInspectionImporter.h"
#include "Definitions/SVUserMessage.h"
#pragma endregion Includes

DWORD WINAPI SVImportProgress::TaskThread(LPVOID lpHost)
{
	SVImportProgress* pProgressTask = ( reinterpret_cast<SVImportProgress*> (lpHost));

	pProgressTask->m_Status = pProgressTask->m_rTask.Import(*pProgressTask);

	pProgressTask->Complete();

	return S_OK;
}

SVImportProgress::SVImportProgress(SVInspectionImportHelper& rTask, LPCTSTR title, CWnd* pParent)
: SVIProgress()
, m_rTask(rTask)
, m_Dialog(title, pParent)
, m_Status(S_OK)
{
	m_Dialog.m_pTask = this;
}

SVImportProgress::~SVImportProgress()
{
	if (m_hThread)
	{
		::CloseHandle(m_hThread);
	}
}

void SVImportProgress::DoModal()
{
	m_Dialog.DoModal();
}

HRESULT SVImportProgress::Start()
{
	HRESULT l_Status(S_OK);

	m_hThread = ::CreateThread(nullptr, 0, &SVImportProgress::TaskThread, this, 0, nullptr);

	if (nullptr == m_hThread)
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

void SVImportProgress::Complete()
{
	m_Dialog.PostMessage(SV_END_PROGRESS_DIALOG, 0, 0);
}

HRESULT SVImportProgress::GetStatus() const
{
	return m_Status;
}

void SVImportProgress::Cancel()
{
	m_Status = E_FAIL;
}

HRESULT SVImportProgress::UpdateProgress(unsigned long p_Current, unsigned long p_Total)
{
	HRESULT l_Status(m_Status);

	if (S_OK == l_Status)
	{
		m_Dialog.SendMessage(SV_UPDATE_PROGRESS, p_Current, p_Total);
	}
	return l_Status;
}

HRESULT SVImportProgress::UpdateText(LPCTSTR text)
{
	m_Dialog.SendMessage(SV_UPDATE_PROGRESS_TEXT, 0L, reinterpret_cast<LPARAM>(text));
	return true;
}

