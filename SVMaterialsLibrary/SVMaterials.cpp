//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterials
//* .File Name       : $Workfile:   SVMaterials.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:14:34  $
//******************************************************************************

#include "stdafx.h"
#include <algorithm> // for std::find_if (VS2010)
#include "SVMaterials.h"
#include "SVTreeLibrary/SVCompareKeys.h"

SVMaterials::SVMaterials()
: m_Materials()
{
}

SVMaterials::SVMaterials( const SVMaterials& p_rMaterials )
: m_Materials( p_rMaterials.m_Materials )
{
}

SVMaterials::~SVMaterials()
{
}

bool SVMaterials::empty() const
{
	return m_Materials.empty();
}

void SVMaterials::clear()
{
	m_Materials.clear();
}

SVMaterials::size_type SVMaterials::size() const
{
	return m_Materials.size();
}

SVMaterials::const_iterator SVMaterials::begin() const
{
	return m_Materials.begin();
}

SVMaterials::iterator SVMaterials::begin()
{
	return m_Materials.begin();
}

SVMaterials::const_iterator SVMaterials::end() const
{
	return m_Materials.end();
}

SVMaterials::iterator SVMaterials::end()
{
	return m_Materials.end();
}

SVMaterials::const_iterator SVMaterials::find( const SVIdentifier& p_Identifier ) const
{
	return std::find_if( m_Materials.begin(), m_Materials.end(), SVCompareKeys<SVIdentifier, SVData>( p_Identifier ) );
	//return m_Materials.find( p_Identifier );
}

SVMaterials::iterator SVMaterials::find( const SVIdentifier& p_Identifier )
{
	return std::find_if( m_Materials.begin(), m_Materials.end(), SVCompareKeys<SVIdentifier, SVData>( p_Identifier ) );
	//return m_Materials.find( p_Identifier );
}

/*
SVMaterials::size_type SVMaterials::erase( const SVIdentifier& p_rKey )
{
	return m_Materials.erase( p_rKey );
}
*/

SVMaterials::iterator SVMaterials::erase( iterator p_Where )
{
	return m_Materials.erase( p_Where );
}

HRESULT SVMaterials::GetMaterial( const SVIdentifier& p_Identifier, SVData& p_rData ) const
{
	HRESULT l_Status( S_OK );

	const_iterator l_Iter( find( p_Identifier ) );

	if( l_Iter != end() )
	{
		p_rData = l_Iter->second;
	}
	else
	{
		p_rData.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMaterials::GetMaterial( const SVIdentifier& p_Identifier, _variant_t& p_rVariant ) const
{
	HRESULT l_Status( S_OK );

	const_iterator l_Iter( find( p_Identifier ) );

	if( l_Iter != end() )
	{
		p_rVariant = static_cast< const _variant_t& >( l_Iter->second );
	}
	else
	{
		p_rVariant.Clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMaterials::AddMaterial( const SVIdentifier& p_Identifier, const _variant_t& p_rVariant )
{
	HRESULT l_Status( S_OK );

	m_Materials.push_back( SVMapData( p_Identifier, p_rVariant ) );

	return l_Status;
}

HRESULT SVMaterials::UpdateMaterial( const SVIdentifier& p_Identifier, const _variant_t& p_rVariant )
{
	HRESULT l_Status( S_OK );

	iterator l_Iter( find( p_Identifier ) );

	if( l_Iter != end() )
	{
		l_Iter->second = p_rVariant;
	}
	else
	{
		m_Materials.push_back( SVMapData( p_Identifier, p_rVariant ) );
	}

	//m_Materials[ p_Identifier ] = p_rVariant;

	return l_Status;
}

HRESULT SVMaterials::AddMaterials( iterator p_First, iterator p_Last )
{
	HRESULT l_Status( S_OK );

	iterator l_Iter( p_First );

	while( l_Iter != p_Last )
	{
		HRESULT l_Temp( AddMaterial( l_Iter->first, l_Iter->second ) );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVMaterials::UpdateMaterials( iterator p_First, iterator p_Last )
{
	HRESULT l_Status( S_OK );

	iterator l_Iter( p_First );

	while( l_Iter != p_Last )
	{
		HRESULT l_Temp( UpdateMaterial( l_Iter->first, l_Iter->second ) );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

const SVMaterials& SVMaterials::operator=( const SVMaterials& p_rMaterials )
{
	if( this != &p_rMaterials )
	{
		m_Materials = p_rMaterials.m_Materials;
	}

	return *this;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterials.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:14:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed storage from map to deque for materials class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:23:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

