//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionConstructParams
//* .File Name       : $Workfile:   SVAcquisitionConstructParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:50  $
//******************************************************************************

#ifndef SVACQUISITIONCONSTRUCTPARAMS_H
#define SVACQUISITIONCONSTRUCTPARAMS_H

#include <vector>
#include "SVImageLibrary/SVLut.h"
#include "SVUtilityLibrary/SVString.h"

struct SVAcquisitionConstructParams
{
	SVAcquisitionConstructParams();
	SVAcquisitionConstructParams( const SVAcquisitionConstructParams& p_rObject );
	SVAcquisitionConstructParams( LPCTSTR p_szDeviceName, LPCTSTR p_szDigitizerName,
		int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize,
		const SVLutInfo& p_rLUTInfo, int p_LRBandSize, int p_LRConstrast, int p_LRBrightness );
	SVAcquisitionConstructParams( LPCTSTR p_szDeviceName, LPCTSTR p_szDigitizerName,
		int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize );

	virtual ~SVAcquisitionConstructParams();

	const SVAcquisitionConstructParams& operator=( const SVAcquisitionConstructParams& p_rObject );

	SVString m_DeviceName;
	SVString m_DigitizerName;

	int m_Channel;
	int m_DigNumber;
	int m_BandSize;
	int m_BandMaxSize;

	bool m_LUTAndLRSet;

	SVLutInfo m_LUTInfo;

	int m_LRBandSize;
	int m_LRConstrast;
	int m_LRBrightness;

};

typedef std::vector< SVAcquisitionConstructParams > SVAcquisitionConstructParamsVector;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAcquisitionConstructParams.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:23:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Feb 2010 14:17:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use new construction functionality via the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2010 12:15:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of single line construction functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
