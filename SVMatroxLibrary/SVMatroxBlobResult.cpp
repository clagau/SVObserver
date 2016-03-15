// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobResult
// * .File Name       : $Workfile:   SVMatroxBlobResult.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:59:36  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxBlobResult.h"
#include "SVMatroxBlobInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBlobResult::SVMatroxBlobResult()
{
	m_BlobResultId = 0;
}

SVMatroxBlobResult::~SVMatroxBlobResult()
{
//	if( m_BlobResultId != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxBlobResult::empty() const
{
	return m_BlobResultId == 0;
}

void SVMatroxBlobResult::LocalClear()
{
	SVMatroxBlobInterface l_Interface;
	l_Interface.Destroy( *this );
}


