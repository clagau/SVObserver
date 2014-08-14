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

#include "stdafx.h"
#include "SVSharedValue.h"

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedValue.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:10:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
