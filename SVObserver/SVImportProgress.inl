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

//#include "stdafx.h"
//#include "SVImportProgress.h"
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/SVUserMessage.h"

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

	//m_hThread = ::CreateThread(NULL, 0, SVImportTaskThread, this, CREATE_SUSPENDED, NULL);
	m_hThread = ::CreateThread(NULL, 0, &SVImportProgress<Task>::TaskThread, this, 0, NULL);

	if (m_hThread == NULL)
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

	if (l_Status == S_OK)
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImportProgress.inl_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:58:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 15:30:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
