// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPatResult
// * .File Name       : $Workfile:   SVMatroxPatResult.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:25:10  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxPatResult.h"
#include "SVMatroxPatternInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxPatResult::SVMatroxPatResult()
:m_PatResultId(0)
{
}

SVMatroxPatResult::~SVMatroxPatResult()
{
//	if( m_PatResultId != 0 )
//	{
//		LocalClear();
//	}

}

bool SVMatroxPatResult::empty() const
{
	return m_PatResultId == 0;
}

void SVMatroxPatResult::LocalClear()
{
	SVMatroxPatternInterface l_Interface;
	l_Interface.Destroy( *this );
}


