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

