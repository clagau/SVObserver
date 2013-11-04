//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEdgeControlClass
//* .File Name       : $Workfile:   SVEdge.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:50  $
//******************************************************************************

#ifndef SVEDGE_H
#define SVEDGE_H

class SVLineClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVEdgeListClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVEdgeListClass : public CDWordArray  
{
public:
	SVEdgeListClass();

public:
	virtual ~SVEdgeListClass();
};


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVEdgeControlClass 
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :06.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVEdgeControlClass : public CObject
{
public:
	SVEdgeControlClass();
	SVEdgeControlClass( SVEdgeControlClass& S2 );

	virtual ~SVEdgeControlClass();

	SVEdgeControlClass& operator=( SVEdgeControlClass& S2 );

	void ConvertStringToParams( LPCTSTR pszStrValue );

	SVEdgeListClass& GetEdgeList();

	BOOL CalculateEdgeList( SVLineClass* PLine, BOOL BCalcForDisplaying, BOOL BCurrentDisplayedEdgeIsA = TRUE );
	DWORD CalculateCurrentEdgePosition( SVLineClass* PLine );
	double CalculateSubPixelEdgePosition( SVLineClass *pLine, long lEdge );

	BOOL findEdge( double dCurrentValue, BOOL First, double dMaxValue, double dMinValue );

public:
	//edge values - updated from the analyzers valueobjects
	DWORD	m_dwDirection;
	DWORD	m_dwPolarisation;
	DWORD	m_dwEdgeSelect;
	double	m_dEdgeSelectThisValue;

	BOOL	m_bIsFixedEdgeMarker;
	DWORD	m_dwPosition;
	double	m_dPositionOffsetValue;

	DWORD	m_dwLowerThresholdValue;
	DWORD	m_dwUpperThresholdValue;

	DWORD	m_dwUserFlags;
	BOOL    m_bIsEdgeA;

protected:
	SVEdgeListClass				edgeList;
	double						dLastValue;

};

#endif	// SVEDGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEdge.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:23:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   27 Oct 2010 09:32:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Feb 2005 14:01:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   16 Feb 2005 14:06:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed enums
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   04 Dec 2003 12:15:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   18 Nov 2003 08:52:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   enum for edge direction was incorrect from previous versions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   13 Nov 2003 09:22:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   enums were incorrect for Head to Tail and Tail to Head.  The values were mismatched.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Oct 2003 16:23:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   08 Oct 2003 10:55:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new enum for direction not used
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 09:16:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   08 Nov 2001 13:11:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated findEdge method and changed parameters to include sub-pixel resolution.
 * 
 * Added CalculateSubPixelEdgePosition method and changed parameters to include sub-pixel resolution.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:29:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Sep 30 1999 13:44:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  41
 * SCR Title:  Version 3.00 Beta 5 Versioning.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to script edge control parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
