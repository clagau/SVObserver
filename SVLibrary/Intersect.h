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

