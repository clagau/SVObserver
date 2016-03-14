//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFailStatusStream
//* .File Name       : $Workfile:   SVFailStatusStream.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Feb 2015 10:51:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "RemoteMonitorNamedList.h"
#include "SVInfoStructs.h"
#include "SVObjectCommandDataJson.h"
#pragma endregion

class SVFailStatusStream
{
public:
	#pragma region Constructor
	SVFailStatusStream();
	SVFailStatusStream(const SVFailStatusStream& rhs);
	#pragma endregion

	#pragma region Public Methods
	~SVFailStatusStream();
	SVFailStatusStream& operator=(const SVFailStatusStream& rhs);

	//**********************************************
	/// Add - Adds fail status items to a named stream
	/// Note: does not append only inserts or replaces
	/// \param rName [in] - Name of the Stream to Add
	/// \param rList [in] - List of the items to Add
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT Add(const SVString& rName, const MonitoredObjectList& rList);

	void Clear();

	//**********************************************
	/// BuildJsonStream - Builds the Fail Status Stream Json for the Product
	/// and sends it to the client
	/// \param rData [in] - Reference to the Product Info Struct
	/// \param server [ref] - Reference to the Json Command Sevrer
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT BuildJsonStream(const SVProductInfoStruct& rData, SVJsonCommandServer& server) const;
	#pragma endregion

private:
	#pragma region Private Methods
	LPCTSTR SVFailStatusStream::GetName() const;
	#pragma endregion

	#pragma region Private members
	mutable SVCriticalSection m_CriticalSection;
	SVString m_name;
	SVObjectReferenceVector m_list;
	#pragma endregion
};

