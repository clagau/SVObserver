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

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVExtentLineStruct.h"
#include "SVExtentFigureStruct.h"

struct SVExtentMultiLineStruct  
{
	SVExtentMultiLineStruct();
	SVExtentMultiLineStruct( const SVExtentMultiLineStruct &p_rsvValue );
	~SVExtentMultiLineStruct();

	const SVExtentMultiLineStruct &operator=( const SVExtentMultiLineStruct &p_rsvValue );

	void Assign( SVExtentFigureStruct &p_rsvValue, COLORREF p_Color = 0 );
	void AssignExtentFigure( SVExtentFigureStruct &p_rsvValue, COLORREF p_Color = 0 );

	HRESULT Initialize();

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

