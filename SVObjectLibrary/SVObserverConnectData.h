//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserverConnectData
//* .File Name       : $Workfile:   SVObserverConnectData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:20:02  $
//******************************************************************************

#ifndef SVOBSERVERCONNECTDATA_H
#define SVOBSERVERCONNECTDATA_H

#include "SVMaterialsLibrary/SVMaterials.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"

class SVObserverConnectData
{
public:
	SVObserverConnectData( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectId )
		: m_SubjectDataName( p_rSubjectDataName ), m_SubjectId( p_rSubjectId ), m_Attributes() {}

	SVObserverConnectData( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectId, const SVMaterials& p_rAttributes )
		: m_SubjectDataName( p_rSubjectDataName ), m_SubjectId( p_rSubjectId ), m_Attributes( p_rAttributes ) {}

	virtual ~SVObserverConnectData() {}

	const SVString& GetSubjectDataName() const { return m_SubjectDataName; }
	const SVGUID& GetSubjectId() const { return m_SubjectId; }
	const SVMaterials& GetAttributes() const { return m_Attributes; }

protected:
	SVString m_SubjectDataName;
	SVGUID m_SubjectId;
	SVMaterials m_Attributes;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObserverConnectData.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:20:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:19:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update object manager to fix issues with subject/observer pattern functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/