// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPatternModel
// * .File Name       : $Workfile:   SVMatroxPatternModel.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:26:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxPatternModel.h"
#include "SVMatroxPatternInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxPatternModel::SVMatroxPatternModel()
:m_ModelId(0)
{
}

SVMatroxPatternModel::~SVMatroxPatternModel()
{
//	if( m_ModelId != 0 )
//	{
//		LocalClear();
//	}

}

bool SVMatroxPatternModel::empty() const
{
	return m_ModelId == 0;
}

void SVMatroxPatternModel::LocalClear()
{
	SVMatroxPatternInterface l_Interface;
	l_Interface.Destroy( *this );
}

