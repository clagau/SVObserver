//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedValue
//* .File Name       : $Workfile:   SVSharedValue.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:10:06  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedValue.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedValue::SVSharedValue( int p_Status, const void_allocator& p_rAlloc )
	: m_ResultType( SVSharedValue::UnknownType )
	, m_Result( "", p_rAlloc )
	, m_Status( p_Status )
	, m_Allocator( p_rAlloc )
	{
	}

	SVSharedValue::SVSharedValue( SVSharedValue::ResultTypeEnum p_ResultType, const char* p_szResult, int p_Status, const void_allocator& p_rAlloc )
	: m_ResultType( p_ResultType )
	, m_Result( p_szResult, p_rAlloc )
	, m_Status( p_Status )
	, m_Allocator( p_rAlloc )
	{
	}

	SVSharedValue::SVSharedValue( const void_allocator& p_rAlloc )
	: m_ResultType( SVSharedValue::UnknownType )
	, m_Result( "", p_rAlloc )
	, m_Status( -1 )
	, m_Allocator( p_rAlloc )
	{
	}

	SVSharedValue::SVSharedValue( const SVSharedValue& p_rData )
	: m_ResultType( p_rData.m_ResultType )
	, m_Result( p_rData.m_Result.c_str(), p_rData.m_Allocator )
	, m_Status( p_rData.m_Status )
	, m_Allocator( p_rData.m_Allocator )
	{
	}

	const SVSharedValue& SVSharedValue::operator=( const SVSharedValue& p_rData )
	{
		if (this != &p_rData)
		{
			m_ResultType = p_rData.m_ResultType;
			m_Result = p_rData.m_Result;
			m_Status = p_rData.m_Status;
		}
		return *this;
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
