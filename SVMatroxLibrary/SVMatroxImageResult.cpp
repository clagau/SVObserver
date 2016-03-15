// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageResult
// * .File Name       : $Workfile:   SVMatroxImageResult.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:17:24  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxImageResult.h"
#include "SVMatroxImageInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxImageResult::SVMatroxImageResult()
: m_ImageResultID(0)
{
}

SVMatroxImageResult::~SVMatroxImageResult()
{
//	if( m_ImageResultID != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxImageResult::empty() const
{
	return m_ImageResultID == 0;
}

void SVMatroxImageResult::LocalClear()
{
	SVMatroxImageInterface l_Interface;
	l_Interface.Destroy( *this );
}

