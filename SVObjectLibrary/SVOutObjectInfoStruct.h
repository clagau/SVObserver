//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutObjectInfoStruct
//* .File Name       : $Workfile:   SVOutObjectInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:29:20  $
//******************************************************************************

#pragma once

#pragma warning( disable : 4786 )

//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <vector>
#include "SVInObjectInfoStruct.h"

class SVObjectClass;

typedef std::pair<SVObjectClass*, SVObjectClass*> SVObjectPair;
typedef std::vector<SVObjectPair> SVObjectPairVector;

struct SVOutObjectInfoStruct : public SVObjectInfoStruct
{
	SVOutObjectInfoStruct();
	SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue );

	virtual ~SVOutObjectInfoStruct();

	const SVOutObjectInfoStruct& operator=( const SVOutObjectInfoStruct& p_rsvValue );

	HRESULT AddInput(SvOl::SVInObjectInfoStruct& p_rsvInput);
	HRESULT RemoveInput(SvOl::SVInObjectInfoStruct& p_rsvInput);

	HRESULT DisconnectAllInputs();

	INT_PTR GetInputSize() { return static_cast<INT_PTR> (m_UserInfoList.size()); }
	SvOl::SVInObjectInfoStruct& GetInputAt( long lIndex ) { return m_UserInfoList[lIndex]; }

	void lock() { return m_mutex.lock(); };
	void unlock() { return m_mutex.unlock(); };;

private:
	// Who are my users...
	//	If you have to send messages to the user objects ( especially in SETUP MODE ),
	//  you should use their object ID In RUN MODE you should normally not send 
	//  messages, but you can use the validated object pointer!
	SvOl::SVInObjectInfoStructVector m_UserInfoList;

	mutable std::mutex m_mutex;

};

typedef std::set<SVOutObjectInfoStruct*> SVStdSetSVOutObjectInfoStructPtr;
typedef std::vector<SVOutObjectInfoStruct*> SVOutObjectInfoStructPtrVector;

