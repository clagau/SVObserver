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

#pragma region Includes
#pragma endregion Includes

class SVMatroxBuffer;

class SVMatroxGraphicsInterface
{
public:
	SVMatroxGraphicsInterface();

	virtual ~SVMatroxGraphicsInterface();

	static double CreateRGB888( unsigned char p_Red, unsigned char p_Green, unsigned char p_Blue );

	static HRESULT Clear(__int64& p_rGraphicsID, SVMatroxBuffer& p_rBuffer );

	static HRESULT RectangleFill(__int64& p_rGraphicsID, SVMatroxBuffer& p_rBuffer, const RECT& p_rRectangle );

	static HRESULT Color(__int64& p_rGraphicsID, double p_Color );
	static HRESULT BackColor(__int64& p_rGraphicsID, double p_Color );

	static HRESULT Create(__int64& p_rGraphicsID );

	static HRESULT Destroy(__int64& p_rGraphicsID );

};

