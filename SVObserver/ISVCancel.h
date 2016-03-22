// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : ISVCancel
// * .File Name       : $Workfile:   ISVCancel.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:13:38  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVTaskObjectInterfaceInputRequestStruct.h"
#pragma endregion Includes

struct SVCancelData
{
	SVCancelData() {}
	virtual ~SVCancelData() {}
};

class ISVCancel
{
public:
	virtual ~ISVCancel() {}
	virtual bool CanCancel() = 0;
	virtual HRESULT GetCancelData(SVCancelData*& rpData) = 0;
	virtual HRESULT SetCancelData(SVCancelData* pData) = 0;
};

// new style using the input request queue
// the caller of GetCancelData is responsible for sending it to the input request queue
// (via SVTaskObjectInterfaceClass)
class ISVCancel2
{
public:
	virtual ~ISVCancel2() {}
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rMap) = 0;
};

struct SVMultiCancelData : public SVCancelData
{
	typedef std::map<ISVCancel*, SVCancelData*> MapType;
	MapType mapData;

	SVMultiCancelData() {}
	inline const SVMultiCancelData& operator = (const SVMultiCancelData& rhs);
	inline ~SVMultiCancelData();
};

// this class assumes that when one is assigned to another, only one will be full when deleted.
// ownership of the SVCancelData (via mapData.clear() on nonowners) is left up to the client code.

inline SVMultiCancelData::~SVMultiCancelData()
{
	MapType::iterator iter;
	for ( iter = mapData.begin(); iter != mapData.end(); ++iter)
	{
		delete iter->second;
	}
	mapData.clear();
}

inline const SVMultiCancelData& SVMultiCancelData::operator = (const SVMultiCancelData& rhs)
{
	mapData = rhs.mapData;
	return *this;
}

