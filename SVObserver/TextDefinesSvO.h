//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the SVObserver project.
// Items in this file should be organized alphabetically, since that's how
// they will be displayed by IntelliSense.
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVObserver
	{
		const TCHAR* const c_textErrorDetachingInput = _T( "Error detaching input." );
		const TCHAR* const c_textErrorFillingInputs = _T( "Error filling inputs." );
		const TCHAR* const c_textErrorFindingPPQEntries = _T( "Error finding PPQ entries." );
		const TCHAR* const c_textErrorGettingInputObjectList = _T( "Error getting input object list." );
		const TCHAR* const c_textErrorGettingInputs = _T( "Error getting inputs." );
		const TCHAR* const c_textErrorGettingOutputs = _T( "Error getting outputs." );
		const TCHAR* const c_textErrorGettingPLCData = _T( "Error getting PLC data." );
		const TCHAR* const c_textErrorGettingPPQ = _T( "Error getting PPQ." );
		const TCHAR* const c_textErrorGettingPPQCount = _T( "Error getting PPQ count." );
		const TCHAR* const c_textErrorGettingTimeStamp = _T( "Error getting time stamp." );
		const TCHAR* const c_textErrorLockingInputImageRequests = _T( "Error locking input image requests." );
		const TCHAR* const c_textErrorLockingInputRequests = _T( "Error locking input requests." );
		const TCHAR* const c_textErrorRemovingAllInputRequests = _T( "Error removing all input requests." );
		const TCHAR* const c_textErrorRemovingAllInputImageRequests = _T( "Error removing all input image requests." );
		const TCHAR* const c_textErrorUnlockingInputImageRequests = _T( "Error unlocking input image requests." );
		const TCHAR* const c_textErrorUnlockingInputRequests = _T( "Error unlocking input requests." );
		const TCHAR* const c_textLoadingResourceDllFailed = _T( "Could not load resource dll.\nThe program will terminated." );
	}
}
namespace SvO = Seidenader::SVObserver;