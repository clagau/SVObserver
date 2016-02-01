//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
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
#include <boost/algorithm/string.hpp>
#include "SVObjectNameInfo.h"
#pragma endregion Includes

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
HRESULT SVObjectNameInfo::ParseObjectName( SVObjectNameInfo& p_rNameInfo, const SVString& p_rObjectName )
{
	HRESULT l_Status = S_OK;

	if( !( p_rObjectName.empty() ) )
	{
		SVString::size_type l_Index = p_rObjectName.find_first_of( _T( ".[" ) );

		if( l_Index != SVString::npos )
		{
			p_rNameInfo.m_NameArray.push_back( p_rObjectName.Left( l_Index ) );

			if( p_rObjectName[ l_Index ] == _T( '.' ) )
			{
				l_Status = ParseObjectName( p_rNameInfo, p_rObjectName.Mid( l_Index + 1 ) );
			}
			else
			{
				SVString::size_type l_NextIndex = p_rObjectName.find_first_of( _T( "]{" ), l_Index + 1 );

				if( l_NextIndex != SVString::npos )
				{
					p_rNameInfo.SetIsIndexPresent(true);
					// ignore whitespace
					std::string indexStr = p_rObjectName.Mid( l_Index + 1, l_NextIndex - ( l_Index + 1 ) ).c_str();
					StripSpaces(indexStr);
					p_rNameInfo.m_Index = indexStr.c_str();

					if( p_rObjectName[ l_NextIndex ] == _T( ']' ) )
					{
						if( !( p_rObjectName.Mid( l_NextIndex + 1 ).empty() ) )
						{
							l_Status = E_FAIL;
						}
					}
					else
					{
						SVString::size_type l_BraceIndex = p_rObjectName.find_first_of( _T( "}" ), l_NextIndex + 1 );
						SVString::size_type l_BracketIndex = p_rObjectName.find_first_of( _T( "]" ), l_NextIndex + 1 );

						if( l_BraceIndex != SVString::npos && l_BracketIndex != SVString::npos )
						{
							p_rNameInfo.m_DefaultValuePresent = true;
							p_rNameInfo.m_DefaultValue = p_rObjectName.Mid( l_NextIndex + 1, l_BraceIndex - ( l_NextIndex + 1 ) );

							if( !( p_rObjectName.Mid( l_BracketIndex + 1 ).empty() ) )
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
			p_rNameInfo.m_NameArray.push_back( p_rObjectName );
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

SVObjectNameInfo::SVObjectNameInfo( const SVNameDeque& p_rNameArray )
:	m_NameArray( p_rNameArray ), 
	m_IndexPresent( false ), 
	m_Index(), 
	m_DefaultValuePresent( false ), 
	m_DefaultValue()
{
}

SVObjectNameInfo::SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex )
:	m_NameArray( p_rNameArray ), 
	m_IndexPresent( true ), 
	m_Index( p_rIndex ), 
	m_DefaultValuePresent( false ), 
	m_DefaultValue()
{
}

SVObjectNameInfo::SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex, const SVString& p_rDefaultValue )
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

void SVObjectNameInfo::clear()
{
	m_NameArray.clear();
	m_IndexPresent = false;
	m_Index.clear(); 
	m_DefaultValuePresent = false; 
	m_DefaultValue.clear();
}

HRESULT SVObjectNameInfo::ParseObjectName( const SVString& p_rObjectName )
{
	HRESULT l_Status = S_OK;

	l_Status = ParseObjectName( *this, p_rObjectName );

	return l_Status;
}

HRESULT SVObjectNameInfo::UpdateObjectNameOnly( const SVString& p_rObjectName )
{
	HRESULT l_Status = S_OK;

	SVObjectNameInfo l_Temp;

	l_Status = l_Temp.ParseObjectName( p_rObjectName );

	if( l_Status == S_OK )
	{
		m_NameArray = l_Temp.m_NameArray;
	}

	return l_Status;
}

SVString SVObjectNameInfo::GetObjectName( size_t p_StartIndex ) const
{
	SVString l_Name;

	for( size_t i = p_StartIndex; i < m_NameArray.size(); ++i )
	{
		const SVString& l_rNameItem = m_NameArray[ i ];

		if( !( l_Name.empty() ) && !( l_rNameItem.empty() ) )
		{
			l_Name += _T( "." );
		}

		l_Name += l_rNameItem;
	}

	return l_Name;
}

SVString SVObjectNameInfo::GetObjectArrayName( size_t p_StartIndex ) const
{
	SVString l_Name = GetObjectName( p_StartIndex );

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
		m_NameArray.pop_front();
	}
}

void SVObjectNameInfo::RemoveBottomName()
{
	if(m_NameArray.begin() != m_NameArray.end())
	{
		m_NameArray.pop_back();
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectNameInfo.cpp_v  $
 * 
 *    Rev 1.3   15 Aug 2014 15:47:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised ParseObjectName to strip spaces from the array specifier.
 * Added StripSpaces function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Mar 2014 14:19:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Includes region.
 *   Added methods to remove either the top or bottom part of an FQN.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jun 2013 17:42:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 May 2013 12:48:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Object Name Information class to allow for starting the name at a specified index position.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:57:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:20:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/