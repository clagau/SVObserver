//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityGlobals
//* .File Name       : $Workfile:   SVUtilityGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:26  $
//******************************************************************************

#ifndef SVUTILITYGLOBALS_H
#define SVUTILITYGLOBALS_H

#include <vector>
#include <comdef.h>

class SVString;

typedef unsigned long SVHANDLE;

typedef std::vector< unsigned char > SVByteVector;

extern const GUID SV_GUID_NULL;

inline bool operator < ( const GUID& guid1, const GUID& guid2 )
{
	RPC_STATUS l_Status;
	return ::UuidCompare( const_cast< GUID* >( &guid1 ), const_cast< GUID* >( &guid2 ), &l_Status ) < 0;
}

struct SVGetNextGUID
{
	static void Next( GUID& p_rId )
	{
		switch( ::UuidCreate( &p_rId ) )
		{
			case RPC_S_OK:
			case RPC_S_UUID_LOCAL_ONLY:
			{
				break;
			}
			default:
			{
				p_rId = SV_GUID_NULL;

				break;
			}
		}
	}
};

HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv);

void KeepPrevError( HRESULT& p_rhrPrev, HRESULT p_hrNew );
HRESULT KeepPrevErrorReturnPrev( HRESULT& p_rhrPrev, HRESULT p_hrNew );
HRESULT KeepPrevErrorReturnNew( HRESULT& p_rhrPrev, HRESULT p_hrNew );


#endif // #ifndef SVUTILITYGLOBALS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVUtilityGlobals.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:39:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Jul 2012 14:42:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed operator <(const GUID&, const GUID&) to use UuidCompare instead of memcmp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Sep 2011 10:59:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  737
 * SCR Title:  Fix serialization bug associated with control characters in strings
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed unused signature SVRemoveEscapeCharacters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Oct 2010 15:16:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include a new byte block storage object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 09:41:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:33:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

