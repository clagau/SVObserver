//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageChildBuffer
//* .File Name       : $Workfile:   SVMatroxImageChildBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:18:18  $
//******************************************************************************

#ifndef SVMATROXIMAGECHILDBUFFER_H
#define SVMATROXIMAGECHILDBUFFER_H

#include "SVMatroxBufferTemplate.h"

class SVMatroxImageChildBuffer : public SVMatroxBufferTemplate
{
public:
	friend class SVMatroxBufferInterface;

	SVMatroxImageChildBuffer();

	virtual ~SVMatroxImageChildBuffer();

protected:
	SVMatroxImageChildBuffer( SVMatroxBufferPtr p_ParentPtr, SVMatroxIdentifier p_Identifier, const SVString& p_rCreatorName );

	SVMatroxBufferPtr m_ParentPtr;

};

#endif

