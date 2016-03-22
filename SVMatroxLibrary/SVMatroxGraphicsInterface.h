//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGraphicsInterface
//* .File Name       : $Workfile:   SVMatroxGraphicsInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:17:48  $
//******************************************************************************

#pragma once

#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxGraphics.h"

class SVMatroxGraphicsInterface
{
public:
	typedef SVMatroxApplicationInterface::SVStatusCode SVStatusCode;

	SVMatroxGraphicsInterface();

	virtual ~SVMatroxGraphicsInterface();

	static double CreateRGB888( unsigned char p_Red, unsigned char p_Green, unsigned char p_Blue );

	static SVStatusCode Clear( SVMatroxGraphics& p_rGraphicsID, SVMatroxBuffer& p_rBuffer );

	static SVStatusCode RectangleFill( SVMatroxGraphics& p_rGraphicsID, SVMatroxBuffer& p_rBuffer, const RECT& p_rRectangle );

	static SVStatusCode Color( SVMatroxGraphics& p_rGraphicsID, double p_Color );
	static SVStatusCode BackColor( SVMatroxGraphics& p_rGraphicsID, double p_Color );

	static SVStatusCode Create( SVMatroxGraphics& p_rGraphicsID );

	static SVStatusCode Destroy( SVMatroxGraphics& p_rGraphicsID );

};

