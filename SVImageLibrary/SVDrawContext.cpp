//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDrawContext
//* .File Name       : $Workfile:   SVDrawContext.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:37:26  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////
#include "math.h"

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////

#include "SVDrawContext.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////
SVDrawContext::SVDrawContext( HDC Hdc, SVImageClass *p_psvImage )
{
	m_lIndex = 1;

	DC = Hdc;

	m_psvImage = p_psvImage;

	ScaleFactor = 1.0;

	ViewPortOffset.x = 0;
	ViewPortOffset.y = 0;
}

SVDrawContext::SVDrawContext( HDC Hdc, SVImageClass *p_psvImage, double dScale )
{
	m_lIndex = 1;

	DC = Hdc;

	m_psvImage = p_psvImage;

	ScaleFactor = dScale;

	ViewPortOffset.x = 0;
	ViewPortOffset.y = 0;
}

///////////////////////////////////////////////////////////
// Transform a set of points
///////////////////////////////////////////////////////////
void SVDrawContext::Transform( POINT* PInPoints, POINT* POutPoints, int Cnt )
{
	for( int i = 0;i < Cnt;i++ )
	{
		POutPoints[i] = PInPoints[i];

		ViewPortNormalize( POutPoints[i] );

		Scale( POutPoints[i], ScaleFactor );
	}
}

///////////////////////////////////////////////////////////
// Perform Inverse Transform
///////////////////////////////////////////////////////////
void SVDrawContext::InverseTransform( POINT* PInPoints, POINT* POutPoints, int Cnt )
{
	for( int i = 0;i < Cnt;i++ )
	{
		POutPoints[i] = PInPoints[i];

		Scale ( POutPoints[i], 1.0 / ScaleFactor );

		ViewPortNormalize( POutPoints[i], FromViewPort );
	}
}

///////////////////////////////////////////////////////////
// Scale a set of points
///////////////////////////////////////////////////////////
void SVDrawContext::Scale( POINT& RPoint, double ScaleValue )
{
	RPoint.x = (long)( (double)RPoint.x * ScaleValue );
	RPoint.y = (long)( (double)RPoint.y * ScaleValue );
}

///////////////////////////////////////////////////////////
// Normalize to/From ViewPort
///////////////////////////////////////////////////////////
void SVDrawContext::ViewPortNormalize( POINT& RPoint, BOOL Direction )
{
	if( Direction == FromViewPort )
	{
		RPoint.x -= ViewPortOffset.x;
		RPoint.y -= ViewPortOffset.y;
	}
	else
	{
		RPoint.x += ViewPortOffset.x;
		RPoint.y += ViewPortOffset.y;
	}
}
void SVDrawContext::Initialize()
{
	m_psvImage = NULL;

	m_lIndex = 1;

	// Default is no Viewport normalization
	ViewPortOffset.x = 0;
	ViewPortOffset.y = 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVDrawContext.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:37:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 10:20:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moving files from SVObserver project to this project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 09:16:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Nov 2001 10:37:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Corrected type casting in ::Scale
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Oct 2001 14:14:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2001 15:02:26   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified function, SVDrawContext::SVDrawContext.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Feb 2000 16:58:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Scale and ViewPortNormalize methods.
 * Revised Transform, InverseTransform, Translate and Rotate
 * method to accept in and out point arrays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 18:09:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Class for Drawing Context.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/