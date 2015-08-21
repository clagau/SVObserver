//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGUID
//* .File Name       : $Workfile:   SVGUID.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:26  $
//******************************************************************************

#ifndef SVGUID_H
#define SVGUID_H

#include <comdef.h>

#include "SVString.h"
#include "SVUtilityGlobals.h"

extern const GUID  SVInvalidGUID;

/**
@SVObjectName Global Unique Identifier (GUID)

@SVObjectOverview

@SVObjectOperations

*/
class SVGUID
{
public:
	SVGUID();
	SVGUID( const SVGUID& p_rObject );
	SVGUID( const GUID& p_rGuid );
	SVGUID( const BSTR& p_rString );
	SVGUID( const VARIANT& p_rVariant );

	virtual ~SVGUID();

	bool empty() const;

	void clear();

	operator const GUID& () const;
	operator GUID& ();

	GUID* operator&();
	
	const GUID& ToGUID() const;
	GUID& ToGUID();

	_variant_t ToVARIANT() const;
	_bstr_t ToBSTR() const;
	SVString ToString() const;

	const SVGUID& operator=( const SVGUID& p_rObject );
	const SVGUID& operator=( const GUID& p_rGuid );
	const SVGUID& operator=( const BSTR& p_rString );
	const SVGUID& operator=( const VARIANT& p_rVariant );

	bool operator==( const SVGUID& p_rObject ) const;
	bool operator==( const GUID& p_rGuid ) const;

	bool operator<( const SVGUID& p_rObject ) const;
	bool operator<( const GUID& p_rGuid ) const;

	bool operator>( const SVGUID& p_rObject ) const;
	bool operator>( const GUID& p_rGuid ) const;

protected:
	GUID m_Guid;

};

inline bool operator==( const GUID& p_rLeft, const SVGUID& p_rRight )
{
	return ( p_rRight == p_rLeft );
}

inline bool operator<( const GUID& p_rLeft, const SVGUID& p_rRight )
{
	return ( p_rRight > p_rLeft );
}

inline bool operator>( const GUID& p_rLeft, const SVGUID& p_rRight )
{
	return ( p_rRight < p_rLeft );
}

#endif // #ifndef SVGUID_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVGUID.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:21:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jun 2012 17:42:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 16:11:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 09:40:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:28:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

