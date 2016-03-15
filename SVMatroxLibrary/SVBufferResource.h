// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferResource
// * .File Name       : $Workfile:   SVBufferResource.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:14  $
// ******************************************************************************

#ifndef SVBUFFERRESOURCE_H
#define SVBUFFERRESOURCE_H

#include "mil.h"

// This class is used to provide a MIL_TEXT_CHAR buffer for use with mil functions.
class SVBufferResource
{
public:
	SVBufferResource(void);
	virtual ~SVBufferResource(void);
	void Destroy(void);
	HRESULT Resize( long l_lSize);
	HRESULT GetBuffer( MIL_TEXT_CHAR*& p_rpMilChar );
	MIL_TEXT_CHAR* m_pBuffer;
};

#endif // SVBUFFERRESOURCE_H

