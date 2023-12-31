//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataBuffer
//* .File Name       : $Workfile:   SVDataBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectAppClass.h"
#include "SVGlobalHandles.h"
#pragma endregion Includes

namespace SvIe
{

class SVDataBufferInfoClass
{
public:
	SVDataBufferInfoClass() = default;
	SVDataBufferInfoClass( const SVDataBufferInfoClass& S2 ) = default;
	SVDataBufferInfoClass& operator=( SVDataBufferInfoClass& S2 ) = default;

	SVObjectClass				*pOwnerTool{ nullptr };
	long						Length{ 0L };
	DWORD						Type{ 0L };
	SVDataBufferHandleStruct	HBuffer{};

	// Data Buffer Types
	enum 
	{
		SVProjectResult		= 0x0040,
		SVEventResult		= 0x0080,
		SVExtremeResult		= 0x0100,
		SVCountResult		= 0x0200,
		SVHistResult		= 0x0400,
		SVContainsFloatData = 0x4000
	};
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDataBufferClass 
// -----------------------------------------------------------------------------
// .Description : This class capsules a Data Buffer like the 
//              : Image Result Buffers in MIL ( refer to MimAllocResult() )
////////////////////////////////////////////////////////////////////////////////
class SVDataBufferClass : public SvOl::SVObjectAppClass
{
	SV_DECLARE_CLASS

public:
	SVDataBufferClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDATABUFFER );
	virtual ~SVDataBufferClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;
	bool Resize( int NewLength );

	SVDataBufferInfoClass&		GetDataBufferInfo();
	SVDataBufferHandleStruct&	GetDataBufferHandle();

protected:
	void init();

	SVDataBufferInfoClass m_DataInfo;
};

} //namespace SvIe
