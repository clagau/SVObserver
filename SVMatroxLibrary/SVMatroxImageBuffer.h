//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageBuffer
//* .File Name       : $Workfile:   SVMatroxImageBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:18:06  $
//******************************************************************************

#pragma once

#pragma region Includes

#include "SVMatroxBufferTemplate.h"
#pragma endregion Includes

class SVMatroxImageBuffer : public SVMatroxBufferTemplate
{
public:
	friend class SVMatroxBufferInterface;
	friend class SVMatroxDisplayInterface;

	SVMatroxImageBuffer();
	SVMatroxImageBuffer(SVMatroxIdentifier p_Identifier, const std::string& p_rCreatorName);

	virtual ~SVMatroxImageBuffer();
};

