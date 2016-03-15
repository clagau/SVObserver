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

