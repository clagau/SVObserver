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

#ifndef INCL_SVIMPORTPROGRESS_H
#define INCL_SVIMPORTPROGRESS_H

#include "SVIProgress.h"
#include "SVProgressDialog.h"

class SVImportTask;

template<typename Task>
class SVImportProgress : public SVIProgress
{
public:
	SVImportProgress(Task& rTask, LPCTSTR title, CWnd* pParent=NULL);
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImportProgress.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:55:26   bWalter
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
