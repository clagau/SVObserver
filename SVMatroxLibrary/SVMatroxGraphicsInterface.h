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

#ifndef SVMATROXGRAPHICSINTERFACE_H
#define SVMATROXGRAPHICSINTERFACE_H

#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxGraphics.h"

class SVMatroxGraphicsInterface
{
public:
	typedef SVMatroxApplicationInterface::SVStatusCode SVStatusCode;

	SVMatroxGraphicsInterface();

	virtual ~SVMatroxGraphicsInterface();

	static double CreateRGB888( unsigned char p_Red, unsigned char p_Green, unsigned char p_Blue );

	static SVStatusCode Clear( SVMatroxGraphics& p_rGraphicsID, SVMatroxBuffer& p_rBuffer );

	static SVStatusCode RectangleFill( SVMatroxGraphics& p_rGraphicsID, SVMatroxBuffer& p_rBuffer, const RECT& p_rRectangle );

	static SVStatusCode Color( SVMatroxGraphics& p_rGraphicsID, double p_Color );
	static SVStatusCode BackColor( SVMatroxGraphics& p_rGraphicsID, double p_Color );

	static SVStatusCode Create( SVMatroxGraphics& p_rGraphicsID );

	static SVStatusCode Destroy( SVMatroxGraphics& p_rGraphicsID );

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxGraphicsInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:17:48   bWalter
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
