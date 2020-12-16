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
#include "SVObjectInfoStruct.h"

class SVObjectClass;

typedef std::pair<SVObjectClass*, SVObjectClass*> SVObjectPair;
typedef std::vector<SVObjectPair> SVObjectPairVector;

struct SVOutObjectInfoStruct : public SVObjectInfoStruct
{
	SVOutObjectInfoStruct();
	SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue );

	virtual ~SVOutObjectInfoStruct();

	const SVOutObjectInfoStruct& operator=( const SVOutObjectInfoStruct& p_rsvValue );

	

private:
	// Who are my users...
	//	If you have to send messages to the user objects ( especially in SETUP MODE ),
	//  you should use their object ID In RUN MODE you should normally not send 
	//  messages, but you can use the validated object pointer!
	
};

typedef std::set<SVOutObjectInfoStruct*> SVStdSetSVOutObjectInfoStructPtr;
typedef std::vector<SVOutObjectInfoStruct*> SVOutObjectInfoStructPtrVector;

