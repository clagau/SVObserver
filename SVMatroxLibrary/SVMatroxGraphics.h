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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxGraphics.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:17:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 10:36:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
