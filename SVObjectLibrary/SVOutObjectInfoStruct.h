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
#include "SVSystemLibrary/SVCriticalSection.h"
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

	HRESULT AddInput( SVInObjectInfoStruct& p_rsvInput );
	HRESULT RemoveInput( SVInObjectInfoStruct& p_rsvInput );

	HRESULT DisconnectAllInputs();
	HRESULT GetDependentsList( SVObjectClass* p_psvObject, SVObjectPairVector& rListOfDependents );

	INT_PTR GetInputSize() { return UserInfoList.GetSize(); }
	SVInObjectInfoStruct& GetInputAt( long p_lIndex ) { return UserInfoList[ p_lIndex ]; }

	BOOL Lock( DWORD p_TimeOutMilliseconds = INFINITE ) const;
	BOOL Unlock() const;

private:
	// Who are my users...
	//	If you have to send messages to the user objects ( especially in SETUP MODE ),
	//  you should use their object GUID! In RUN MODE you should normally not send 
	//  messages, but you can use the validated object pointer!
	SVInObjectInfoStructVector UserInfoList;

	mutable SVCriticalSectionPtr m_CriticalSectionPtr;

};

typedef std::set<SVOutObjectInfoStruct*> SVStdSetSVOutObjectInfoStructPtr;
typedef SVVector<SVOutObjectInfoStruct*> SVOutObjectInfoStructPtrVector;

