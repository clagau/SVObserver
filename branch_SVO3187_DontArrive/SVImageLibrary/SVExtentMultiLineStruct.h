//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions\ObjectDefines.h"
#include "SVExtentLineStruct.h"
#pragma endregion Includes


struct SVExtentFigureStruct;

struct SVExtentMultiLineStruct  
{
	SVExtentMultiLineStruct();
	SVExtentMultiLineStruct( const SVExtentMultiLineStruct &p_rsvValue );
	~SVExtentMultiLineStruct();

	const SVExtentMultiLineStruct &operator=( const SVExtentMultiLineStruct &p_rsvValue );

	void Assign(const SVExtentFigureStruct &rValue, COLORREF Color = 0 );
	void AssignExtentFigure(const SVExtentFigureStruct &rValue, COLORREF Color = 0 );

	void Initialize();

	uint32_t m_InspectionID = SvDef::InvalidObjectId;
	uint32_t m_ToolID = SvDef::InvalidObjectId;
	uint32_t m_AnalyzerID = SvDef::InvalidObjectId;
	uint32_t m_ObjectID = SvDef::InvalidObjectId;

	long m_ToolSetDrawFlag;
	long m_ToolDrawFlag;

	bool m_Passed;
	bool m_Warned;
	bool m_Failed;
	bool m_bDrawFigureHatched = false;

	COLORREF m_Color;

	SVPoint<double> m_StringPoint;
	std::string m_csString;

	SVExtentLineStructVector m_svLineArray;
};

typedef std::vector<SVExtentMultiLineStruct> SVExtentMultiLineStructVector;

