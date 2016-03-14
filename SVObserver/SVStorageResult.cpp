//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorageResult
//* .File Name       : $Workfile:   SVStorageResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVStorageResult.h"

SVStorageResult::SVStorageResult()
: m_Storage(), m_Status( S_OK ), m_TriggerCount( 0 )
{
}

SVStorageResult::SVStorageResult( const SVStorage& p_rStorage, HRESULT p_Status, unsigned long p_TriggerCount )
: m_Storage( p_rStorage ), m_Status( p_Status ), m_TriggerCount( p_TriggerCount )
{
}

