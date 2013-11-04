//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentMultiLineStruct
//* .File Name       : $Workfile:   SVExtentMultiLineStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:58  $
//******************************************************************************

#ifndef SVEXTENTMULTILINESTRUCT_H
#define SVEXTENTMULTILINESTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVExtentLineStruct.h"
#include "SVExtentFigureStruct.h"

struct SVExtentMultiLineStruct  
{
	SVExtentMultiLineStruct();
	SVExtentMultiLineStruct( const SVExtentMultiLineStruct &p_rsvValue );
	virtual ~SVExtentMultiLineStruct();

	const SVExtentMultiLineStruct &operator=( const SVExtentMultiLineStruct &p_rsvValue );

	void Assign( SVExtentFigureStruct &p_rsvValue, COLORREF p_Color = 0 );
	void AssignExtentFigure( SVExtentFigureStruct &p_rsvValue, COLORREF p_Color = 0 );

	virtual HRESULT Initialize();

	SVGUID m_InspectionID;
	SVGUID m_ToolID;
	SVGUID m_AnalyzerID;
	SVGUID m_ObjectID;

	long m_ToolSetDrawFlag;
	long m_ToolDrawFlag;

	bool m_Passed;
	bool m_Warned;
	bool m_Failed;

	COLORREF m_Color;

	SVExtentPointStruct m_StringPoint;
	CString m_csString;

	SVExtentLineStructCArray m_svLineArray;
};

typedef SVVector< SVExtentMultiLineStruct > SVExtentMultiLineStructCArray;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentMultiLineStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:39:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jul 2012 07:53:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Chaged the functionality of the assign method and added a new assign method that just updated the figure information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Jul 2012 13:50:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added the ability to set the color when adding a figure to the drawing data object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Jul 2012 08:22:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated attribute names and updated methods to const to make variables more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jul 2012 15:59:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Nov 2010 15:41:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:03:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
