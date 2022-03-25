//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageChildBuffer
//* .File Name       : $Workfile:   SVMatroxImageChildBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:18:18  $
//******************************************************************************

#pragma once

#pragma region Includes

#include "SVMatroxBufferTemplate.h"
#pragma endregion Includes

class SVMatroxImageChildBuffer : public SVMatroxBufferTemplate
{
public:
	friend class SVMatroxBufferInterface;

	SVMatroxImageChildBuffer();
	SVMatroxImageChildBuffer(SVMatroxBufferPtr p_ParentPtr, __int64 p_Identifier, const std::string& p_rCreatorName);

	virtual ~SVMatroxImageChildBuffer();

protected:
	SVMatroxBufferPtr m_ParentPtr;
};

