//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGetObjectDequeByTypeVisitor
//* .File Name       : $Workfile:   SVGetObjectDequeByTypeVisitor.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:38:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVTaskObject.h"

SVGetObjectDequeByTypeVisitor::SVGetObjectDequeByTypeVisitor( const SVObjectTypeInfoStruct& p_rObjectInfo )
: m_ObjectInfo( p_rObjectInfo ), m_Objects()
{
}

SVGetObjectDequeByTypeVisitor::~SVGetObjectDequeByTypeVisitor()
{
}

HRESULT SVGetObjectDequeByTypeVisitor::VisitElement( SVObjectClass& p_rElement )
{
	HRESULT l_Status = S_OK;

	// Find best match....EmbeddedID, Type, SubType...
	if( ( m_ObjectInfo.EmbeddedID == SVInvalidGUID         || m_ObjectInfo.EmbeddedID == p_rElement.GetEmbeddedID() ) &&
		( m_ObjectInfo.ObjectType == SVNotSetObjectType    || m_ObjectInfo.ObjectType == p_rElement.GetObjectType() ) &&
		( m_ObjectInfo.SubType    == SVNotSetSubObjectType || m_ObjectInfo.SubType    == p_rElement.GetObjectSubType() )
	  )
	{
		if( m_ObjectInfo.EmbeddedID != SVInvalidGUID         ||
			m_ObjectInfo.ObjectType != SVNotSetObjectType    ||
			m_ObjectInfo.SubType    != SVNotSetSubObjectType
		  )
		{
			m_Objects.push_back( &p_rElement );
		}
	}

	return l_Status;
}

const SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque& SVGetObjectDequeByTypeVisitor::GetObjects() const
{
	return m_Objects;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGetObjectDequeByTypeVisitor.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:38:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jul 2012 08:01:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved visitor object to main SVObserver project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:02:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/