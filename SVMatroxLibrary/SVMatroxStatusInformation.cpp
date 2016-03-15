// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxStatusInformation
// * .File Name       : $Workfile:   SVMatroxStatusInformation.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:27:16  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxStatusInformation.h"

#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxImagingLibrary.h"
#include "SVMatroxErrorEnum.h"

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxStatusInformation::SVMatroxStatusInformation()
:	m_StatusCode( M_NULL_ERROR ), m_StatusString(),
m_FunctionCode( 0 ), m_FunctionString(),
m_StatusSubCodeCount( 0 )
{
}

/**
@SVOperationName Copy Constructor

@SVOperationDescription

*/
SVMatroxStatusInformation::SVMatroxStatusInformation( const SVMatroxStatusInformation& p_rObject )
:	m_StatusCode( M_NULL_ERROR ), m_StatusString(),
m_FunctionCode( 0 ), m_FunctionString(),
m_StatusSubCodeCount( 0 )
{
	*this = p_rObject;
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxStatusInformation::~SVMatroxStatusInformation()
{
	clear();
}

/**
@SVOperationName Empty

@SVOperationDescription

*/
bool SVMatroxStatusInformation::empty() const
{
	return m_StatusCode == M_NULL_ERROR && m_StatusSubCodeCount == 0;
}



/**
@SVOperationName Clear

@SVOperationDescription

*/
void SVMatroxStatusInformation::clear()
{
	m_StatusCode = 0;
	m_StatusString.clear();

	m_FunctionCode = 0;
	m_FunctionString.clear();

	m_StatusSubCodeCount = 0;

	for( int i = 0; i < 3; ++i )
	{
		m_StatusSubCode[ i ] = 0;
		m_StatusSubString[ i ].clear();
	}
}

/**
@SVOperationName Copy Assignment Operator

@SVOperationDescription

*/
const SVMatroxStatusInformation& SVMatroxStatusInformation::operator=( const SVMatroxStatusInformation& p_rObject )
{
	if( this != &p_rObject )
	{
		m_StatusCode = p_rObject.m_StatusCode;
		m_StatusString = p_rObject.m_StatusString;

		m_FunctionCode = p_rObject.m_FunctionCode;
		m_FunctionString = p_rObject.m_FunctionString;

		m_StatusSubCodeCount = p_rObject.m_StatusSubCodeCount;

		for( int i = 0; i < 3; ++i )
		{
			m_StatusSubCode[ i ] = p_rObject.m_StatusSubCode[ i ];
			m_StatusSubString[ i ] = p_rObject.m_StatusSubString[ i ];
		}
	}

	return *this;
}

SVMatroxStatusInformation::SVStatusString SVMatroxStatusInformation::GetCompleteString() const
{
	SVStatusString l_Temp;
	SVString l_Code;

	l_Temp += _T( "Status Code=" );
	l_Code = SvUl_SF::Format( _T( "%ld" ), m_StatusCode & ~( SVMEE_MATROX_ERROR ) );
	l_Temp += l_Code.c_str();

	l_Temp += _T( "-Function Code=" );
	l_Code = SvUl_SF::Format( _T( "%ld" ), m_FunctionCode );
	l_Temp += l_Code.c_str();

	l_Temp += _T( "-Status String=" );
	l_Temp += m_StatusString;
	l_Temp += _T( "-Function String=" );
	l_Temp += m_FunctionString;
	l_Temp += _T( "-Status Sub String 1=" );
	l_Temp += m_StatusSubString[ 0 ];
	l_Temp += _T( "-Status Sub String 2=" );
	l_Temp += m_StatusSubString[ 1 ];
	l_Temp += _T( "-Status Sub String 3=" );
	l_Temp += m_StatusSubString[ 2 ];

	return l_Temp;
}


