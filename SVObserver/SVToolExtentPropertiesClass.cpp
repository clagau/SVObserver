//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentPropertiesClass
//* .File Name       : $Workfile:   SVToolExtentPropertiesClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:44:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolExtentPropertiesClass.h"

#include "SVValueObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVToolExtentPropertiesClass::SVToolExtentPropertiesClass()
{
	Initialize();
}

SVToolExtentPropertiesClass::~SVToolExtentPropertiesClass()
{
	Initialize();
}

HRESULT SVToolExtentPropertiesClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_svProperties.clear();

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetProperties( SVImageExtentClass &p_rsvExtents ) const
{
	HRESULT l_hrOk = S_OK;

	SVToolExtentPropertiesMap::const_iterator iter;

	for ( iter = m_svProperties.begin(); l_hrOk == S_OK && iter != m_svProperties.end(); ++iter )
	{
		SVValueObjectClass* l_psvValue = iter->second.pValueObject;

		if ( l_psvValue != NULL )
		{
			double l_dValue = 0.0;

			if ( ( l_hrOk = l_psvValue->GetValue( l_dValue ) ) == S_OK )
			{
				l_hrOk = p_rsvExtents.SetExtentProperty( iter->first, l_dValue );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetProperties( unsigned long p_ulIndex, SVImageExtentClass &p_rsvExtents ) const
{
	HRESULT l_hrOk = S_OK;

	SVToolExtentPropertiesMap::const_iterator iter;

	for ( iter = m_svProperties.begin(); l_hrOk == S_OK && iter != m_svProperties.end(); ++iter )
	{
		SVValueObjectClass* l_psvValue = iter->second.pValueObject;

		if ( l_psvValue != NULL )
		{
			double l_dValue = 0.0;

			if ( ( l_hrOk = l_psvValue->GetValue( p_ulIndex, l_dValue ) ) == S_OK )
			{
				l_hrOk = p_rsvExtents.SetExtentProperty( iter->first, l_dValue );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *&p_rpsvValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVToolExtentPropertiesMap::const_iterator iter = m_svProperties.find( p_eProperty );

	if ( iter != m_svProperties.end() )
	{
		p_rpsvValue = iter->second.pValueObject;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::SetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue )
{
	HRESULT l_hrOk = S_OK;

	if ( p_psvValue == NULL )
	{
		m_svProperties.erase( p_eProperty );
	}
	else
	{
		m_svProperties[ p_eProperty ].pValueObject = p_psvValue;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVToolExtentPropertiesMap::const_iterator iter = m_svProperties.find( p_eProperty );

	if ( iter != m_svProperties.end() )
	{
		p_rInfo = iter->second.info;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::SetPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo )
{
	HRESULT l_hrOk = S_FALSE;

	SVToolExtentPropertiesMap::iterator iter = m_svProperties.find( p_eProperty );

	if ( iter != m_svProperties.end() )
	{
		iter->second.info = p_rInfo;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolExtentPropertiesClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:44:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Mar 2011 16:28:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Mar 2011 07:37:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Mar 2011 13:59:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Mar 2011 14:33:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 08:33:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:11:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

