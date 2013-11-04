//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vission, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryHostStruct
//* .File Name       : $Workfile:   SVIOEntryHostStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:09:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVIOEntryHostStruct.h"
#include "SVValueObjectClass.h"

SVIOEntryHostStruct::SVIOEntryHostStruct()
: m_Enabled( false )
, m_PPQIndex( -1 )
, m_ObjectType( IO_INVALID_OBJECT )
, m_IOId()
, m_DeleteValueObject( true )
, m_pValueObject( NULL )
, m_pValueParent( NULL )
{
}

SVIOEntryHostStruct::~SVIOEntryHostStruct()
{
	clear();
}

void SVIOEntryHostStruct::clear()
{
	if( m_DeleteValueObject && ( m_pValueObject != NULL ) )
	{
		delete m_pValueObject;
	}

	m_Enabled = false;
	m_PPQIndex = -1;
	m_ObjectType = IO_INVALID_OBJECT;
	m_IOId.clear();
	m_DeleteValueObject = true;
	m_pValueObject = NULL;
  m_pValueParent = NULL;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIOEntryHostStruct.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:09:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2011 10:24:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
