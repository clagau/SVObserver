//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferArray
//* .File Name       : $Workfile:   SVMatroxBufferArray.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:00:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxStatusInformation.h"
#include "SVMatroxBuffer.h"
#pragma endregion Includes

class SVMatroxSystem;
struct SVMatroxBufferCreateStruct;

class SVMatroxBufferArray
{
public:
	friend class SVMatroxBufferInterface;
	friend class SVMatroxDigitizerInterface;

	SVMatroxBufferArray();
	SVMatroxBufferArray(const SVMatroxBufferArray& p_rBufArray);

	virtual ~SVMatroxBufferArray();

	void swap( SVMatroxBufferArray& p_rBufArray );

	SVMatroxBufferArray& operator=( const SVMatroxBufferArray& p_rBufArray );

	HRESULT Create(const SVMatroxSystem& rSystem, const SVMatroxBufferCreateStruct& rCreateStruct, long count);
	void Destroy();

	bool empty() const;

protected:
	typedef std::vector< __int64 > BufferIDList;
	typedef std::vector< SVMatroxBuffer > BufferList;

	BufferIDList GetList() const;

	BufferList m_BufferIdentifierList;

private:
	void LocalClear();
};
