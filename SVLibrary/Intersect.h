//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Intersect
//* .File Name       : $Workfile:   Intersect.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:30:26  $
//******************************************************************************

#ifndef INCL_INTERSECT_H
#define INCL_INTERSECT_H

class Intersect
{
public:
	enum LineResultEnum
	{
		DONT_INTERSECT    = 0,
		DO_INTERSECT      = 1,
		COLLINEAR         = 2,
	};

	static bool LineRect(const POINT& sPos, const POINT& ePos, const RECT& rect);
	static LineResultEnum Lines(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long &x, long &y);
	static LineResultEnum Lines(const POINT& pt1a, const POINT& pt1b, const POINT& pt2a, const POINT& pt2b, POINT& rptIntersect);
	static bool RectRect(const RECT& rect1, const RECT& rect2);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\Intersect.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:30:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Mar 2006 10:24:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   used const POINT& instead of POINT for the Lines function to be more consistent
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Mar 2006 09:08:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/