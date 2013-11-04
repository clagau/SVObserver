//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectReference
//* .File Name       : $Workfile:   SVObjectReference.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:57:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectClass.h"
#include "SVObjectReference.h"


SVObjectReference::SVObjectReference( SVObjectClass* pObject )
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SVInvalidGUID;
	m_NameInfo.clear();
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, long lArrayIndex, CString strDefaultValue )
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SVInvalidGUID;
	if( m_pObject != NULL )
	{
		m_NameInfo.ParseObjectName( m_NameInfo, static_cast< LPCTSTR >( m_pObject->GetCompleteObjectName() ) );
	}
	m_NameInfo.m_IndexPresent = true;
	m_NameInfo.m_Index = _variant_t( lArrayIndex );
	m_NameInfo.m_DefaultValuePresent = true;
	m_NameInfo.m_DefaultValue = static_cast< LPCTSTR >( strDefaultValue );
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo )
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SVInvalidGUID;
	m_NameInfo = p_rNameInfo;
}

void SVObjectReference::SetEntireArray()
{
	m_NameInfo.m_IndexPresent = true;
	m_NameInfo.m_Index.clear();
}

void SVObjectReference::SetArrayIndex( long lArrayIndex )
{
	m_NameInfo.m_IndexPresent = true;
	m_NameInfo.m_Index = _variant_t( lArrayIndex );
}

CString SVObjectReference::GetName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetName();
		strName += GetZeroBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteObjectName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectName();
		strName += GetZeroBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteObjectNameToObjectType( LPCSTR lpszCompleteName, SVObjectTypeEnum objectTypeToInclude ) const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectNameToObjectType(lpszCompleteName, objectTypeToInclude);
		strName += GetZeroBasedIndexString();
	}
	return strName;
}
CString SVObjectReference::GetOneBasedName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetName();
		strName += GetOneBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteOneBasedObjectName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectName();
		strName += GetOneBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteOneBasedObjectNameToObjectType( LPCSTR lpszCompleteName, SVObjectTypeEnum objectTypeToInclude ) const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectNameToObjectType(lpszCompleteName, objectTypeToInclude);
		strName += GetOneBasedIndexString();
	}
	return strName;
}

const SVObjectNameInfo& SVObjectReference::GetObjectNameInfo() const
{
	return m_NameInfo;
}

CString SVObjectReference::GetZeroBasedIndexString() const
{
	CString s;
	if( m_NameInfo.m_IndexPresent )
	{
		if ( !( m_NameInfo.m_Index.empty() ) )
		{
			s.Format(_T("[%s]"), m_NameInfo.m_Index.c_str());
		}
		else
		{
			s = _T("[ ]");
		}
	}
	return s;
}

CString SVObjectReference::GetOneBasedIndexString() const
{
	CString s;
	if( m_NameInfo.m_IndexPresent )
	{
		if ( !( m_NameInfo.m_Index.empty() ) )
		{
			s.Format(_T("[%d]"), atol(m_NameInfo.m_Index.c_str()) + 1);
		}
		else
		{
			s = _T("[ ]");
		}
	}
	return s;
}

SVString SVObjectReference::GetIndex() const
{
	SVString s;
	return m_NameInfo.m_Index;
}


const UINT SVObjectReference::ObjectAttributesAllowed() const
{
	ASSERT( m_pObject != NULL );
	return m_pObject->ObjectAttributesAllowed();
}

const UINT SVObjectReference::ObjectAttributesSet() const
{
	ASSERT( m_pObject != NULL );
	return m_pObject->ObjectAttributesSet(::atol( m_NameInfo.m_Index.c_str() ));
}

UINT& SVObjectReference::ObjectAttributesAllowedRef()
{
	ASSERT( m_pObject != NULL );
	return m_pObject->ObjectAttributesAllowedRef();
}

SVObjectAttributeShim SVObjectReference::ObjectAttributesSetRef()
{
	ASSERT( m_pObject != NULL );
	return m_pObject->ObjectAttributesSetRef(::atol( m_NameInfo.m_Index.c_str() ));
}

GUID SVObjectReference::GetObjectGuid( SVObjectClass* pObject )
{
	return pObject->GetUniqueObjectID();
}
/*
static GUID SVObjectReference::IsObjectArray( SVObjectClass* pObject )
{
	return pObject->IsArray();
}
*/

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectReference.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:57:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Nov 2012 08:22:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  800
 * SCR Title:  Fix Blob Array Result index string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added get one based index string functions. Renamed old functions to get zero based index string for clarity.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Jun 2012 17:25:18   jspila
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
 * 
 *    Rev 1.10   14 Sep 2005 15:40:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to handle attributes for "entire array" and first element correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Sep 2005 15:40:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to handle attributes for "entire array" and first element correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   30 Aug 2005 14:10:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized construction and copying
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Aug 2005 10:10:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed member variable initialization bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Aug 2005 15:55:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added a space to [] for visual improvement
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Aug 2005 15:55:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added a space to [] for visual improvement
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Jul 2005 15:48:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Guid information so we can store a reference to an object that doesn't exist yet (only store the guid)
 * check object existence on GetName, etc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   26 Jul 2005 13:34:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed ASSERT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Jul 2005 13:30:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed index parse (0-based vs. 1-based)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2005 13:21:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added parsing for entire array syntax []
 * added constructor taking SVObjectNameParseInfoStruct
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Jul 2005 15:25:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed automatic conversion operators
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jun 2005 09:30:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed unnecessary CString assignment
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jun 2005 14:40:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
