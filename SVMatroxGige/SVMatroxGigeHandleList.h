//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeHandleList
//* .File Name       : $Workfile:   SVMatroxGigeHandleList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:24  $
//******************************************************************************
#ifndef SVMATROXGIGEHANDLELIST_H
#define SVMATROXGIGEHANDLELIST_H

#include <map>
#include "SVUtilityLibrary/SVHandleManager.h"

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
class SVMatroxGigeHandleList
{
	typedef std::map<HandleType, value_type> Collection;
	typedef SVHandleManager<MaxHandles, HandleType, InvalidHandle> HandleManager;

private:
	Collection m_list;
	HandleManager m_handleMgr;
	value_type m_defaultValue;

public:
	typedef typename value_type value_type;
	typedef typename HandleManager::HandleType HandleType;
	typedef typename Collection::iterator iterator;
	typedef typename Collection::const_iterator const_iterator;
	
	HRESULT Add(value_type& rValue);
	HRESULT Remove(HandleType handle);

	value_type& Get(HandleType handle, HRESULT& status);
	const value_type& Get(HandleType handle, HRESULT& status) const;

	bool IsValidHandle(HandleType handle) const;
	bool IsValidComponentHandle(HandleType handle, HandleType componentHandle) const;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	size_t size() const;
	void clear();
};

#include "SVMatroxGigeHandleList.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeHandleList.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:44:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:18:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

