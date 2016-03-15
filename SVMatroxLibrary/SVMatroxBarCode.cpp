// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBarCode
// * .File Name       : $Workfile:   SVMatroxBarCode.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxBarCode.h"
#include "SVMatroxBarCodeInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBarCode::SVMatroxBarCode()
:m_BarCodeId(0)
{
}

SVMatroxBarCode::~SVMatroxBarCode()
{
//	if( m_BarCodeId != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxBarCode::empty() const
{
	return m_BarCodeId == 0;
}

void SVMatroxBarCode::LocalClear()
{
	SVMatroxBarCodeInterface l_Interface;
	l_Interface.Destroy( *this );
}


SVMatroxBarCode& SVMatroxBarCode::operator=( const SVMatroxBarCode& p_rhs )
{
//	SVMatroxBarCodeInterface l_BarCodeIntf;
//	l_BarCodeIntf.Create( *this, p_rhs );
	m_BarCodeId = p_rhs.m_BarCodeId;
	return *this;
}

