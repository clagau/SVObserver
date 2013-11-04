//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Intersect.cpp
//* .File Name       : $Workfile:   Intersect.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:30:22  $
//******************************************************************************

#include "stdafx.h"
#include "intersect.h"

//////////////////////////////////////////////////////////////
// NOTE:
// The following macro is to determine if two numbers  
// have the same sign, is for 2's complement number        
// representation.  It will need to be modified for other  
// number systems.                                         
//////////////////////////////////////////////////////////////
#define SAME_SIGNS(a, b) \
		(((long)((unsigned long)a ^ (unsigned long)b)) >= 0)

/*static*/
Intersect::LineResultEnum Intersect::Lines(const POINT& pt1a, const POINT& pt1b, const POINT& pt2a, const POINT& pt2b, POINT& rptIntersect)
{
	return Lines(pt1a.x, pt1a.y, pt1b.x, pt1b.y, pt2a.x, pt2a.y, pt2b.x, pt2b.y, rptIntersect.x, rptIntersect.y);
}

///////////////////////////////////////////////////////////////////////
// Intersect::Lines
//
//   This function computes whether two line segments,
//   respectively joining the input points (x1,y1) -- (x2,y2)
//   and the input points (x3,y3) -- (x4,y4) intersect.
//   If the lines intersect, the output variables x, y are
//   set to coordinates of the point of intersection.
//
//   All values are in integers.  The returned value is rounded
//   to the nearest integer point.
//
//   If non-integral grid points are relevant, the function
//   can easily be transformed by substituting floating point
//   calculations instead of integer calculations.
//
//   Entry
//        x1, y1,  x2, y2   Coordinates of endpoints of one segment.
//        x3, y3,  x4, y4   Coordinates of endpoints of other segment.
//
//   Exit
//        x, y              Coordinates of intersection point.
//
//   The value returned by the function is one of:
//
//        DONT_INTERSECT    0
//        DO_INTERSECT      1
//        COLLINEAR         2
//
// Error condititions:
//
//     Depending upon the possible ranges, and particularly on 16-bit
//     computers, care should be taken to protect from overflow.
//
//     In the following code, 'long' values could have been used for this
//     purpose, instead of 'short'.
//
///////////////////////////////////////////////////////////////////////
Intersect::LineResultEnum Intersect::Lines(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long &x, long &y)
{
	long a1, a2, b1, b2, c1, c2; // Coefficients of line eqns.
	long s1, s2, s3, s4;         // 'Sign' values
	long denom, offset, num;     // Intermediate values

	///////////////////////////////////////////////////////////
	// Compute a1, b1, c1, where line joining points 1 and 2   
	// is "a1x + b1y + c1 = 0" (Implicit form)                 
	///////////////////////////////////////////////////////////
	a1 = y2 - y1;
	b1 = x1 - x2;
	c1 = x2 * y1 - x1 * y2;

	/////////////////////
	// Compute s3 and s4 
	/////////////////////
//	r3 = ((y2 - y1) * x3) + ((x1 - x2) * y3) + ((x2 * y1) - (x1 * y2));
//	r4 = ((y2 - y1) * x4) + ((x1 - x2) * y4) + ((x2 * y1) - (x1 * y2));

	s3 = a1 * x3 + b1 * y3 + c1;
	s4 = a1 * x4 + b1 * y4 + c1;

	///////////////////////////////////////////////////////////
	// Check signs of s3 and s4.                               
	// If both point 3 and point 4 lie on same side of line 1, 
	// the line segments do not intersect.                     
	///////////////////////////////////////////////////////////
	if (s3 && s4 && SAME_SIGNS(s3, s4))
		return DONT_INTERSECT;

	///////////////////////////////////////////////////////////
	// Compute a2, b2, c2, where line joining points 3 and 4   
	// is "a2x + b2y + c2 = 0" (Implicit form)                 
	///////////////////////////////////////////////////////////
	a2 = y4 - y3;
	b2 = x3 - x4;
	c2 = x4 * y3 - x3 * y4;

	/////////////////////
	// Compute s1 and s2 
	/////////////////////
//  s1 = ((y4 - y3) * x1) + ((x3 - x4) * y1) + ((x4 * y3) - (x3 * y4));
//  s2 = ((y4 - y3) * x2) + ((x3 - x4) * y2) + ((x4 * y3) - (x3 * y4));

	s1 = a2 * x1 + b2 * y1 + c2;
	s2 = a2 * x2 + b2 * y2 + c2;

	///////////////////////////////////////////////////////////////////////
	// Check signs of s1 and s2.                                           
	// If both point 1 and point 2 lie on same side of second line segment,
	// the line segments do not intersect.                                 
	///////////////////////////////////////////////////////////////////////
	if (s1 && s2 && SAME_SIGNS(s1, s2))
		return DONT_INTERSECT;

	////////////////////////////////////////////////////////////////////////
	// Line segments intersect: compute intersection point. 
	// x = (num - offset) / denom                           
	//   offset = denom / 2                                 
	//   denom = ((a1 * b2) - (a2 * b1)) / 2                
	//   num = (b1 * c2) - (b2 * c1)                        
	//                                                      
	// y = (num - offset) / denom                           
	//   offset = denom / 2                                 
	//   denom = ((a1 * b2) - (a2 * b1)) / 2                
	//   num = (a2 * c1) - (a1 * c2);                       
	///////////////////////////////////////////////////////////////////////
	denom = a1 * b2 - a2 * b1;

//  denom = ((y2 - y1) * (x3 - x4)) - ((y4 - y3) * (x1 - x2));

	if (!denom)  // COLLINEAR LINE SEGMENTS
	{
		a1 = std::max(x1, x2);
		b1 = std::min(x3, x4);
		c1 = std::min(y3, y4);
		if ((b1 - a1) < 0)
		{
			x = b1;
			y = c1;
			return DO_INTERSECT;
		} 
		return COLLINEAR;
	}
	offset = denom < 0 ? - denom / 2 : denom / 2;

	///////////////////////////////////////////////////////////////////////
	// The denom/2 is to get rounding instead of truncating.  It   
	// is added or subtracted to the numerator, depending upon the 
	// sign of the numerator.                                      
	///////////////////////////////////////////////////////////////////////

//  num = ((x1 - x2) * ((x4 * y3) - (x3 * y4))) - ((x3 - x4) * ((x2 * y1) - (x1 * y2)));
//  num = ((y4 - y3) * ((x2 * y1) - (x1 * y2))) - ((y2 - y1) * ((x4 * y3) - (x3 * y4)));

	num = b1 * c2 - b2 * c1;
	x = (num < 0 ? num - offset : num + offset) / denom;

	num = a2 * c1 - a1 * c2;
	y = (num < 0 ? num - offset : num + offset) / denom;

	return DO_INTERSECT;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
bool Intersect::LineRect(const POINT& sPos, const POINT& ePos, const RECT& rect)
{
	bool bRetVal = false;

	POINT intersectPos;
	
	int rc = Intersect::Lines(rect.left, rect.top, rect.right, rect.top, sPos.x, sPos.y, ePos.x, ePos.y, intersectPos.x, intersectPos.y);
	if (rc == DO_INTERSECT)
	{
		bRetVal = true;
	}

	if (!bRetVal)
	{
		rc = Intersect::Lines(rect.left, rect.bottom, rect.right, rect.bottom, sPos.x, sPos.y, ePos.x, ePos.y, intersectPos.x, intersectPos.y);
		if (rc == DO_INTERSECT)
		{
			bRetVal = true;
		}
	}

	if (!bRetVal)
	{
		rc = Intersect::Lines(rect.left, rect.top, rect.left, rect.bottom, sPos.x, sPos.y, ePos.x, ePos.y, intersectPos.x, intersectPos.y);
		if (rc == DO_INTERSECT)
		{
			bRetVal = true;
		}
	}

	if (!bRetVal)
	{
		rc = Intersect::Lines(rect.right, rect.top, rect.right, rect.bottom, sPos.x, sPos.y, ePos.x, ePos.y, intersectPos.x, intersectPos.y);
		if (rc == DO_INTERSECT)
		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
bool Intersect::RectRect(const RECT& rect1, const RECT& rect2)
{
	bool bRetVal = false;
	RECT iRect;
	if (::IntersectRect(&iRect, &rect1, &rect2))
		bRetVal = true;
	else
	{
		POINT pt[4];
		pt[0].x = rect2.left;
		pt[0].y = rect2.top;
		pt[1].x = rect2.right;
		pt[1].y = rect2.top;
		pt[2].x = rect2.right;
		pt[2].y = rect2.bottom;
		pt[3].x = rect2.left;
		pt[3].y = rect2.bottom;
		if (::PtInRect(&rect1, pt[0]) || ::PtInRect(&rect1, pt[1]) ||
			::PtInRect(&rect1, pt[2]) || ::PtInRect(&rect1, pt[3]))
  		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

/*
float Magnitude(POINT& pos1, POINT& pos2)
{
    float dx = abs(pos1.x - pos2.x);
    float dy = abs(pos1.y - pos2.y);

    return (float)sqrtf((dx * dx) + (dy * dy));
}

bool DistancePointToLine(POINT& pos, POINT& sPos, POINT& ePos, float& distance)
{
	bool bRetVal = true;
    POINT Intersection;
 
	float lineLen = Magnitude(sPos, ePos);
 
    float U = ((((pos.x - sPos.x) * (sPos.x - ePos.x)) +
				((pos.y - sPos.y) * (sPos.y - ePos.y))) /
                (lineLen * lineLen);
 
    if (U < 0.0f || U > 1.0f)
	{
        bRetVal = false;   // closest point does not fall within the line segment
	}
	else
	{
		Intersection.x = sPos.x + (U * (sPos.x - ePos.x));
		Intersection.x = sPos.y + (U * (sPos.y - ePos.y));
 
		Distance = Magnitude(pos.x, Intersection.y);
	}
	return bRetVal;
}

void main( void )
{
    XYZ LineStart, LineEnd, Point;
    float Distance;


    LineStart.X =  50.0f; LineStart.Y =   80.0f; LineStart.Z =  300.0f;
    LineEnd.X   =  50.0f; LineEnd.Y   = -800.0f; LineEnd.Z   = 1000.0f;
    Point.X     =  20.0f; Point.Y     = 1000.0f; Point.Z     =  400.0f;

    if( DistancePointLine( &Point, &LineStart, &LineEnd, &Distance ) )
        printf( "closest point falls within line segment, distance = %f\n", Distance );
    else
        printf( "closest point does not fall within line segment\n" );


    LineStart.X =  0.0f; LineStart.Y =   0.0f; LineStart.Z =  50.0f;
    LineEnd.X   =  0.0f; LineEnd.Y   =   0.0f; LineEnd.Z   = -50.0f;
    Point.X     = 10.0f; Point.Y     =  50.0f; Point.Z     =  10.0f;

    if( DistancePointLine( &Point, &LineStart, &LineEnd, &Distance ) )
        printf( "closest point falls within line segment, distance = %f\n", Distance );
    else
        printf( "closest point does not fall within line segment\n" );
}
*/

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\Intersect.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:30:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Oct 2010 08:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
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
