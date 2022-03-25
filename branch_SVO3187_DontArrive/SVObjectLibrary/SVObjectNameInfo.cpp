//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectNameInfo
//* .File Name       : $Workfile:   SVObjectNameInfo.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 Aug 2014 15:47:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/algorithm/string.hpp>
#include "SVObjectNameInfo.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

static void StripSpaces(std::string& rStr)
{
	boost::algorithm::trim(rStr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// indexes are one-based and specified with []
// {} inside index block defines default value
// whitespace is ignored inside []
// an empty [] means whole array
// a value object without [] means the first index.
// the following are examples of legal parses:
//
//  Inspection_1.Tool Set.Window Tool.Blob Analyzer.Area               - same as Area[1]
//  Inspection_1.Tool Set.Window Tool.Blob Analyzer.Area[ ]            - all of the valid results in Area
//  Inspection_1.Tool Set.Window Tool.Blob Analyzer.Area[ 2 ]          - second index of Area
//  Inspection_1.Tool Set.Window Tool.Blob Analyzer.Area[ 2 {-1} ]     - second index with -1 as default value if index is not valid
//
HRESULT SVObjectNameInfo::ParseObjectName( SVObjectNameInfo& rNameInfo, const std::string& rObjectName )
{
	HRESULT l_Status = S_OK;

	if( !( rObjectName.empty() ) )
	{
		std::string::size_type l_Index = rObjectName.find_first_of( _T( ".[" ) );

		if( l_Index != std::string::npos )
		{
			rNameInfo.m_NameArray.push_back( rObjectName.substr(0, l_Index ) );

			if( rObjectName[ l_Index ] == _T( '.' ) )
			{
				l_Status = ParseObjectName( rNameInfo, SvUl::Mid( rObjectName, l_Index + 1 ) );
			}
			else
			{
				std::string::size_type l_NextIndex = rObjectName.find_first_of( _T( "]{" ), l_Index + 1 );

				if( l_NextIndex != std::string::npos )
				{
					rNameInfo.SetIsIndexPresent(true);
					// ignore whitespace
					std::string indexStr = rObjectName.substr( l_Index + 1, l_NextIndex - ( l_Index + 1 ) ).c_str();
					StripSpaces(indexStr);
					rNameInfo.m_Index = indexStr.c_str();

					if( rObjectName[ l_NextIndex ] == _T( ']' ) )
					{
						if( !( SvUl::Mid( rObjectName, l_NextIndex + 1 ).empty() ) )
						{
							l_Status = E_FAIL;
						}
					}
					else
					{
						std::string::size_type l_BraceIndex = rObjectName.find_first_of( _T( "}" ), l_NextIndex + 1 );
						std::string::size_type l_BracketIndex = rObjectName.find_first_of( _T( "]" ), l_NextIndex + 1 );

						if( l_BraceIndex != std::string::npos && l_BracketIndex != std::string::npos )
						{
							rNameInfo.m_DefaultValuePresent = true;
							rNameInfo.m_DefaultValue = rObjectName.substr( l_NextIndex + 1, l_BraceIndex - ( l_NextIndex + 1 ) );

							if( !( SvUl::Mid( rObjectName, l_BracketIndex + 1 ).empty() ) )
							{
								l_Status = E_FAIL;
							}
						}
						else
						{
							l_Status = E_FAIL;
						}
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
		}
		else
		{
			rNameInfo.m_NameArray.push_back( rObjectName );
		}
	}

	return l_Status;
}

SVObjectNameInfo::SVObjectNameInfo()
:	m_NameArray(), 
	m_IndexPresent( false ), 
	m_Index(), 
	m_DefaultValuePresent( false ), 
	m_DefaultValue()
{
}

SVObjectNameInfo::SVObjectNameInfo( const SVNameVector& p_rNameArray )
:	m_NameArray( p_rNameArray ), 
	m_IndexPresent( false ), 
	m_Index(), 
	m_DefaultValuePresent( false ), 
	m_DefaultValue()
{
}

SVObjectNameInfo::SVObjectNameInfo( const SVNameVector& p_rNameArray, const std::string& p_rIndex )
:	m_NameArray( p_rNameArray ), 
	m_IndexPresent( true ), 
	m_Index( p_rIndex ), 
	m_DefaultValuePresent( false ), 
	m_DefaultValue()
{
}

SVObjectNameInfo::SVObjectNameInfo( const SVNameVector& p_rNameArray, const std::string& p_rIndex, const std::string& p_rDefaultValue )
:	m_NameArray( p_rNameArray ), 
	m_IndexPresent( true ), 
	m_Index( p_rIndex ), 
	m_DefaultValuePresent( true ), 
	m_DefaultValue( p_rDefaultValue )
{
}

SVObjectNameInfo::SVObjectNameInfo( const SVObjectNameInfo& p_rObject )
:	m_NameArray( p_rObject.m_NameArray ), 
	m_IndexPresent( p_rObject.IsIndexPresent() ), 
	m_Index( p_rObject.m_Index ), 
	m_DefaultValuePresent( p_rObject.m_DefaultValuePresent ), 
	m_DefaultValue( p_rObject.m_DefaultValue )
{
}

SVObjectNameInfo::~SVObjectNameInfo()
{
	clear();
}

void SVObjectNameInfo::clear()
{
	m_NameArray.clear();
	m_IndexPresent = false;
	m_Index.clear();
	m_DefaultValuePresent = false; 
	m_DefaultValue.clear();
}

HRESULT SVObjectNameInfo::ParseObjectName( const std::string& p_rObjectName )
{
	HRESULT l_Status = S_OK;

	l_Status = ParseObjectName( *this, p_rObjectName );

	return l_Status;
}

std::string SVObjectNameInfo::GetObjectName( size_t p_StartIndex ) const
{
	std::string l_Name;

	for( size_t i = p_StartIndex; i < m_NameArray.size(); ++i )
	{
		const std::string& l_rNameItem = m_NameArray[ i ];

		if( !( l_Name.empty() ) && !( l_rNameItem.empty() ) )
		{
			l_Name += _T( "." );
		}

		l_Name += l_rNameItem;
	}

	return l_Name;
}

std::string SVObjectNameInfo::GetObjectArrayName( size_t p_StartIndex ) const
{
	std::string l_Name = GetObjectName( p_StartIndex );

	if( m_IndexPresent )
	{
		l_Name += _T( "[" );
		l_Name += m_Index;

		if( m_DefaultValuePresent )
		{
			l_Name += _T( "{" );
			l_Name += m_DefaultValue;
			l_Name += _T( "}" );
		}

		l_Name += _T( "]" );
	}

	return l_Name;
}

long SVObjectNameInfo::GetIndexValue() const
{
	long l_Index = -1;

	if( m_IndexPresent )
	{
		l_Index = ::atol( m_Index.c_str() ) - 1; // this is always 1-based
	}

	return l_Index;
}

void SVObjectNameInfo::RemoveTopName()
{
	if(m_NameArray.begin() != m_NameArray.end())
	{
		m_NameArray.erase(m_NameArray.begin());
	}
}

void SVObjectNameInfo::RemoveBottomName()
{
	if(m_NameArray.begin() != m_NameArray.end())
	{
		m_NameArray.pop_back();
	}
}

