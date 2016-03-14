//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageRotateStruct
//* .File Name       : $Workfile:   SVMatroxImageRotateStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:21:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxImageRotateStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxImageRotateStruct::SVMatroxImageRotateStruct(SVMatroxBuffer& p_rSource)
: m_rSource(p_rSource),
m_dAngle(0.0),
m_dSrcCenX( 0.0 ),
m_dSrcCenY( 0.0 ),
m_dDstCenX( 0.0 ),
m_dDstCenY( 0.0 )
{
}

SVMatroxImageRotateStruct::~SVMatroxImageRotateStruct()
{
}

