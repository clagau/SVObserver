//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultItemDefinition
//* .File Name       : $Workfile:   SVIPResultItemDefinition.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:45:54  $
//******************************************************************************

#ifndef SVIPRESULTITEMDEFINITION_H
#define SVIPRESULTITEMDEFINITION_H

#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"

class SVIPResultItemDefinition
{
public:
	SVIPResultItemDefinition();
	SVIPResultItemDefinition( const SVGUID& p_rObjectID );
	SVIPResultItemDefinition( const SVGUID& p_rObjectID, int Index );
	SVIPResultItemDefinition( const SVIPResultItemDefinition& p_rObject );

	bool operator<( const SVIPResultItemDefinition& p_rObject ) const;
	bool operator==( const SVIPResultItemDefinition& p_rObject ) const;

	const SVGUID& GetObjectID() const;
	bool GetIndexPresent() const;
	
	int GetIndex() const;

protected:
	SVGUID m_ObjectID;

	bool m_IndexPresent;
	int m_Index;
	

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPResultItemDefinition.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:45:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:58:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/