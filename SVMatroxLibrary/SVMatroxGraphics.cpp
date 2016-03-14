//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGraphics
//* .File Name       : $Workfile:   SVMatroxGraphics.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:17:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGraphics.h"
#include "SVMatroxGraphicsInterface.h"

SVMatroxGraphics::SVMatroxGraphics()
:m_GraphicsId(0)
{
}

SVMatroxGraphics::~SVMatroxGraphics()
{
}

bool SVMatroxGraphics::empty() const
{
	return m_GraphicsId == 0;
}

void SVMatroxGraphics::LocalClear()
{
	SVMatroxGraphicsInterface l_Interface;

	l_Interface.Destroy( *this );
}


SVMatroxGraphics& SVMatroxGraphics::operator=( const SVMatroxGraphics& p_rhs )
{
	m_GraphicsId = p_rhs.m_GraphicsId;

	return *this;
}

