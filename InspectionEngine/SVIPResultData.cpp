//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultData
//* .File Name       : $Workfile:   SVIPResultData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPResultData.h"

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVIPResultData::SVIPResultData()
: m_ResultData(),
  m_ToolSetEndTime( 0.0 ),
  m_ToolSetAvgTime( 0.0 ),
  m_TriggerDistance( 0.0 ),
  m_ToolSetTime( 0.0 )
{
}

SVIPResultData::SVIPResultData( const SVResultDataMap& p_rResultData, const std::vector <IPResultTableData>& rResultTableData, double p_ToolSetEndTime, double p_ToolSetAvgTime, double p_TriggerDistance, double p_ToolSetTime )
: m_ResultData( p_rResultData ),
  m_ToolSetEndTime( p_ToolSetEndTime ),
  m_ToolSetAvgTime( p_ToolSetAvgTime ),
  m_TriggerDistance( p_TriggerDistance ),
  m_ToolSetTime( p_ToolSetTime )
{
}

SVIPResultData::SVIPResultData( const SVIPResultData& p_rObject )
: m_ResultData( p_rObject.m_ResultData ),
  m_ToolSetEndTime( p_rObject.m_ToolSetEndTime ),
  m_ToolSetAvgTime( p_rObject.m_ToolSetAvgTime ),
  m_TriggerDistance( p_rObject.m_TriggerDistance ),
  m_ToolSetTime( p_rObject.m_ToolSetTime )
{
}

void SVIPResultData::clear()
{
	m_ResultData.clear();

	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;

	m_TriggerDistance = 0.0;
	m_ToolSetTime = 0.0;
}

} //namespace SvIe
