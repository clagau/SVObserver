//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxPolarTransformStruct
//* .File Name       : $Workfile:   SVMatroxPolarTransformStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:26:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxPolarTransformStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxPolarTransformStruct::SVMatroxPolarTransformStruct(SVMatroxBuffer& p_rSource)
: m_rSource( p_rSource ),
m_dCenterX( 0.0 ),
m_dCenterY( 0.0 ),
m_dStartRadius( 0.0 ),
m_dEndRadius( 0.0 ),
m_dStartAngle( 0.0 ),
m_dEndAngle( 0.0 ),
m_dDestSizeX( 0.0 ),
m_dDestSizeY( 0.0 )
{
}

SVMatroxPolarTransformStruct::~SVMatroxPolarTransformStruct()
{
}

