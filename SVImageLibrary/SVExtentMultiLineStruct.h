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

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVUtilityLibrary/SVGUID.h"

#include "SVExtentLineStruct.h"
#include "SVExtentFigureStruct.h"
#pragma endregion Includes

struct SVExtentMultiLineStruct  
{
	SVExtentMultiLineStruct();
	SVExtentMultiLineStruct( const SVExtentMultiLineStruct &p_rsvValue );
	~SVExtentMultiLineStruct();

	const SVExtentMultiLineStruct &operator=( const SVExtentMultiLineStruct &p_rsvValue );

	void Assign(const SVExtentFigureStruct &rValue, COLORREF Color = 0 );
	void AssignExtentFigure(const SVExtentFigureStruct &rValue, COLORREF Color = 0 );

	void Initialize();

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

	SVPoint<double> m_StringPoint;
	std::string m_csString;

	SVExtentLineStructVector m_svLineArray;
};

typedef std::vector<SVExtentMultiLineStruct> SVExtentMultiLineStructVector;

