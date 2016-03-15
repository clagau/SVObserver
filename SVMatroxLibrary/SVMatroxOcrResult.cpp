// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrResult
// * .File Name       : $Workfile:   SVMatroxOcrResult.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:58  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxOcrResult.h"
#include "SVMatroxOcrInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxOcrResult::SVMatroxOcrResult()
:m_OcrResultID(0)
{
}

SVMatroxOcrResult::~SVMatroxOcrResult()
{
//	if( m_OcrResultID != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxOcrResult::empty() const
{
	return m_OcrResultID == 0;
}

void SVMatroxOcrResult::LocalClear()
{
	SVMatroxOcrInterface l_Interface;
	l_Interface.Destroy( *this );
}


