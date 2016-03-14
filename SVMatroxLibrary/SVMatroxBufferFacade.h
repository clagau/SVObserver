//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferFacade
//* .File Name       : $Workfile:   SVMatroxBufferFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:02:04  $
//******************************************************************************

#ifndef SVMATROXBUFFERFACADE_H
#define SVMATROXBUFFERFACADE_H

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVMatroxTypedefs.h"

class SVMatroxBufferFacade
{
public:
	friend class SVMatroxBuffer;
	friend class SVMatroxDisplayBuffer;
	friend class SVMatroxImageChildBuffer;

	virtual ~SVMatroxBufferFacade();

	virtual bool empty() const = 0;

protected:
	SVMatroxBufferFacade();

	virtual SVMatroxIdentifier GetIdentifier() const = 0;

	// Do not implement this method
	SVMatroxBufferFacade( const SVMatroxBufferFacade& p_rObject );

	// Do not implement this method
	const SVMatroxBufferFacade& operator=( const SVMatroxBufferFacade& p_rObject );

};

typedef SVSharedPtr< SVMatroxBufferFacade > SVMatroxBufferPtr;

#endif

