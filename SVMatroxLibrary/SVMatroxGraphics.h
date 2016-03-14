//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGraphics
//* .File Name       : $Workfile:   SVMatroxGraphics.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:17:36  $
//******************************************************************************

#ifndef SVMATROXGRAPHICS_H
#define SVMATROXGRAPHICS_H

#include "SVMatroxTypedefs.h"

class SVMatroxGraphicsInterface;


/**
@SVObjectName Matrox Graphics

@SVObjectOverview This is a data class that holds the identifier for a Graphic.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/
class SVMatroxGraphics  
{
	friend SVMatroxGraphicsInterface;

public:
	SVMatroxGraphics();
	virtual ~SVMatroxGraphics();

	bool empty() const;
	SVMatroxGraphics& operator=( const SVMatroxGraphics& p_rhs );
	bool operator<(const SVMatroxGraphics& p_rBuf) const {return m_GraphicsId < p_rBuf.m_GraphicsId;};

protected:
	SVMatroxIdentifier m_GraphicsId;

private:
	void LocalClear();
};

#endif

