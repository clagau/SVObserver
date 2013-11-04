//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDrawContext
//* .File Name       : $Workfile:   SVDrawContext.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:37:30  $
//******************************************************************************

#ifndef SVDRAWCONTEXT_H
#define SVDRAWCONTEXT_H

extern const GUID  SVInvalidGUID;

enum { FromViewPort, ToViewPort };

class SVImageClass;

class SVDrawContext
{
public:
	HDC DC;					// GDI Device Context
	double ScaleFactor;		// Scale Factor (not used yet - always 1)
	SVImageClass *m_psvImage;// Image being displayed

	POINT ViewPortOffset;	// View Port Normalization

	long m_lIndex;

public:
	SVDrawContext( HDC Hdc, SVImageClass *p_psvImage );
	SVDrawContext( HDC Hdc, SVImageClass *p_psvImage, double dScale );
	void Transform( POINT* PInPoints, POINT* POutPoints, int Cnt );
	void InverseTransform( POINT* PInPoints, POINT* POutPoints, int Cnt );
	void Scale( POINT& RPoint, double ScaleValue );

	// Usage p_lIndex is used with the y component of the first point of a polyline
	// when drawing horizontal lines to skip lines when zoomed in.
	// called from SVDrawObjectListClass::Draw when used for drawing mask overlays.
	bool ShouldDrawPoints( long p_lIndex )
	{
		bool l_bOk = false;

		if( ScaleFactor != 0.0 )
		{
			long lFactor = static_cast<long>((1/ScaleFactor) * 2);
			if( lFactor == 0 )
			{
				l_bOk = true;
			}
			else
			if( ( p_lIndex % lFactor ) == 0 )
			{
				l_bOk = true;
			}
		}
		else
		{
			l_bOk = true;
		}

		return l_bOk;
	};

	// for Drawing/Moving/Sizing
	void ViewPortNormalize( POINT& RPoint, BOOL Direction = ToViewPort );

	void Initialize();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVDrawContext.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:37:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Aug 2005 10:57:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed compiler warning in ShouldDrawPoints
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Aug 2005 15:47:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ShouldDrawPoints, used in SVDrawObjectListClass.
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
 *    Rev 1.5   22 Apr 2003 09:16:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Oct 2001 15:03:04   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added member, SVDrawContext::GUID SecondaryImageGuid.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Feb 2000 16:58:46   sjones
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
 *    Rev 1.2   21 Feb 2000 18:13:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised from Struct to Class.
 * Moved methods to .cpp file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Feb 2000 15:41:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Transform Method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Feb 2000 16:57:26   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/