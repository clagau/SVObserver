// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMultithread
// * .File Name       : $Workfile:   SVMultithread.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:53:38  $
// ******************************************************************************

#ifndef SVMULTITHREAD_H_
#define SVMULTITHREAD_H_

class SVSyncObject
{
public:
	virtual HRESULT Unlock() = 0;
	virtual HRESULT Lock() = 0;
	virtual HRESULT Lock(DWORD dwTimeout) {return Lock();}
};

struct SVCriticalSectionAdapter
{
	SVCriticalSectionAdapter( CRITICAL_SECTION& rCritSec ) : m_rCriticalSection( rCritSec ) {}
	inline void Lock()                  { ::EnterCriticalSection( &m_rCriticalSection ); }
	inline void Unlock()                { ::LeaveCriticalSection( &m_rCriticalSection ); }

	CRITICAL_SECTION& m_rCriticalSection;
};

struct SVEmptySyncObject : public SVSyncObject	// to be used as a placeholder when synchronization isn't necessary
{
	HRESULT Unlock() {return S_OK;}
	HRESULT Lock()   {return S_OK;}
};

class SVSingleLock
{
public:
	SVSingleLock( SVSyncObject& rSync );
	~SVSingleLock();

protected:
	SVSyncObject& m_rSyncObject;
};

inline SVSingleLock::SVSingleLock( SVSyncObject& rSync ) : m_rSyncObject( rSync )
{
	m_rSyncObject.Lock();
}

inline SVSingleLock::~SVSingleLock()
{
	m_rSyncObject.Unlock();
}

// provide a template version so it can be used with any class that has Lock and Unlock, not just those derived from SVSyncObject
template <class SYNC_OBJECT>
class SVSingleLockT
{
public:
	SVSingleLockT( SYNC_OBJECT& rSync );
	~SVSingleLockT();

protected:
	SYNC_OBJECT& m_rSyncObject;
};

template <class SYNC_OBJECT>
inline SVSingleLockT<SYNC_OBJECT>::SVSingleLockT( SYNC_OBJECT& rSync ) : m_rSyncObject( rSync )
{
	m_rSyncObject.Lock();
}

template <class SYNC_OBJECT>
inline SVSingleLockT<SYNC_OBJECT>::~SVSingleLockT()
{
	m_rSyncObject.Unlock();
}

typedef SVSingleLockT<SVCriticalSectionAdapter> SVCriticalSectionSingleLock;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVMultithread.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:53:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 09:22:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2006 09:13:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVSingleLockT, SVCriticalSectionAdapter, SVEmptySyncObject, typedef SVCriticalSectionSingleLock
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jun 2005 14:06:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
