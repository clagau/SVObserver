//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDatabase
//* .File Name       : $Workfile:   SVGigeCameraDatabase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:48  $
//******************************************************************************
#ifndef SVGIGECAMERADATABASE_H
#define SVGIGECAMERADATABASE_H

#include <map>
#include <deque>
#include <comdef.h>
#include "SVUtilityLibrary/SVString.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"

typedef std::deque<_bstr_t> SVGigeEnumNameList;

class SVGigeCameraDatabase
{
	// Enum to String mapping
	typedef std::map<SVGigeParameterEnum, _bstr_t> SVGigeEnumToString;

private:
	static const SVGigeEnumToString m_enumToString;

	SVGigeCameraDatabase();
	~SVGigeCameraDatabase();

public:
	static long GetFeatureCount();
	static HRESULT GetFeatureNames(SVGigeEnumNameList& list);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVGigeCameraDatabase.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:36:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2010 08:01:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:17:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

