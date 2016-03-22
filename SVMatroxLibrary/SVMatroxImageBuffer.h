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

#include "SVMatroxBufferTemplate.h"

class SVMatroxImageBuffer : public SVMatroxBufferTemplate
{
public:
	friend class SVMatroxBufferInterface;
	friend class SVMatroxDisplayInterface;

	SVMatroxImageBuffer();

	virtual ~SVMatroxImageBuffer();

protected:
	SVMatroxImageBuffer( SVMatroxIdentifier p_Identifier, const SVString& p_rCreatorName );

};

