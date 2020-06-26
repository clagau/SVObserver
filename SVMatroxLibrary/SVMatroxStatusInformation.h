// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxStatusInformation
// * .File Name       : $Workfile:   SVMatroxStatusInformation.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:27:22  $
// ******************************************************************************

#pragma once
#include <mil.h>

//Moved to precompiled header: #include <string>


/**
@SVObjectName Matrox Status Information

@SVObjectOverview This class will be used to hold the status information generated by Matrox for each function call.

@SVObjectOperations This class will present empty, clear, and data extraction operators.

*/
class SVMatroxStatusInformation  
{
public:
	SVMatroxStatusInformation();
	SVMatroxStatusInformation( const SVMatroxStatusInformation& p_rObject );

	virtual ~SVMatroxStatusInformation();

	bool empty() const;

	void clear();

	const SVMatroxStatusInformation& operator=( const SVMatroxStatusInformation& p_rObject );

	std::string GetCompleteString() const;

	HRESULT m_StatusCode;
	std::string m_StatusString;

	MIL_INT m_FunctionCode;
	std::string m_FunctionString;

	MIL_INT m_StatusSubCodeCount;

	HRESULT m_StatusSubCode[ 3 ];
	std::string m_StatusSubString[ 3 ];

};

