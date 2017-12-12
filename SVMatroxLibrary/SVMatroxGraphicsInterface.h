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

class SVMatroxGraphicsInterface
{
public:
	SVMatroxGraphicsInterface();

	virtual ~SVMatroxGraphicsInterface();

	static double CreateRGB888( unsigned char p_Red, unsigned char p_Green, unsigned char p_Blue );

	static HRESULT Clear(SVMatroxIdentifier& p_rGraphicsID, SVMatroxBuffer& p_rBuffer );

	static HRESULT RectangleFill(SVMatroxIdentifier& p_rGraphicsID, SVMatroxBuffer& p_rBuffer, const RECT& p_rRectangle );

	static HRESULT Color(SVMatroxIdentifier& p_rGraphicsID, double p_Color );
	static HRESULT BackColor(SVMatroxIdentifier& p_rGraphicsID, double p_Color );

	static HRESULT Create(SVMatroxIdentifier& p_rGraphicsID );

	static HRESULT Destroy(SVMatroxIdentifier& p_rGraphicsID );

};

