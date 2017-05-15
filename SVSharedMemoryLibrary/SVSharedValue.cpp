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

namespace SvSml
{
	SVSharedValue::SVSharedValue(const void_allocator& rAlloc)
	: m_ResultType(SVSharedValue::UnknownType)
	, m_Status(-1)
	, m_Allocator(rAlloc )
	, m_ElementName("", rAlloc)
	{
		memset(m_Result, 0, sizeof(m_Result));
	}

	SVSharedValue::SVSharedValue(const SVSharedValue& rData)
	: m_ResultType( rData.m_ResultType)
	, m_Status(rData.m_Status)
	, m_Allocator(rData.m_Allocator)
	, m_ElementName(rData.m_ElementName)
	{
		strcpy_s(m_Result, sizeof(m_Result), rData.m_Result);
	}

	const SVSharedValue& SVSharedValue::operator=(const SVSharedValue& rData)
	{
		if (this != &rData)
		{
			strcpy_s(m_Result, sizeof(m_Result), rData.m_Result);
			m_ResultType = rData.m_ResultType;
			m_Status = rData.m_Status;
			m_ElementName = rData.m_ElementName;
		}
		return *this;
	}

	void SVSharedValue::SetName(const SVString& Name)
	{
		m_ElementName = bip_string(Name.c_str(), m_Allocator);
	}

	void SVSharedValue::SetData(ResultTypeEnum ResultType, const SVString& Result, int Status)
	{
		m_ResultType = ResultType;
		try
		{
			strcpy_s(m_Result, sizeof(m_Result), Result.c_str());
			m_Status = Status;
		}
		catch (...)
		{
			memset(m_Result, 0, sizeof(m_Result));
			m_Status = E_FAIL;
		}
	}


	void  CopySharedValues( SVSharedValueVector& rTo  , const SVSharedValueVector& rfrom)
	{
		SVSharedValueVector::const_iterator itFrom;
		SVSharedValueVector::iterator itTo;
		for(itFrom = rfrom.begin(),  itTo  = rTo.begin();  itFrom != rfrom.end() &&  itTo != rTo.end(); ++itFrom,++itTo)
		{
			strcpy_s( itTo->m_Result, statics::max_result_size, itFrom->m_Result);
			itTo->m_ResultType = itFrom->m_ResultType;
			itTo->m_Status = itFrom->m_Status;
		}
	}
} //namespace SvSml
