//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHardwareManifest
//* .File Name       : $Workfile:   SVHardwareManifest.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   11 Jun 2013 15:26:12  $
//******************************************************************************

#ifndef SVHARDWAREMANIFEST_H
#define SVHARDWAREMANIFEST_H

#include <map>
#include "SVAcquisitionConstructParams.h"
#include "SVTriggerDeviceParams.h"
#include "SVIMTypeInfoStruct.h"
#include "SVConfigurationLibrary/SVObserverEnums.h"

class SVHardwareManifest
{
public:
	friend class SVObserverApp;

	static SVHardwareManifest& Instance();

	virtual ~SVHardwareManifest();

	const SVTriggerDeviceParamsVector& GetTriggerDeviceParams() const;
	const SVAcquisitionConstructParamsVector& GetAcquisitionDeviceParams() const;

	static SVIMTypeInfoStruct GetSVIMTypeInfo( SVIMProductEnum p_ID  );

	template <typename Insertor>
	static void GetSupportedSVIMList(bool bColor, Insertor insertor)
	{
		for (SVIMTypeMap::const_iterator l_Iter = m_SVIMTypeMap.begin(); l_Iter != m_SVIMTypeMap.end(); ++l_Iter)
		{
			const SVIMTypeInfoStruct& l_SVIMInfo = l_Iter->second;

			if ( l_SVIMInfo.m_ColorSystem == bColor && l_SVIMInfo.m_Supported == true )
			{
				insertor = l_Iter->first;
			}
		}
	}

	static SVString BuildSoftwareTriggerDeviceName(int iDig);
	static SVString BuildAcquisitionTriggerDeviceName(int iDig);
	static SVString BuildIOBoardTriggerDeviceName(int iDig);

	static bool IsCompatible( SVIMProductEnum p_CurrentConfigType, SVIMProductEnum p_NewConfigType );
	static bool IsColorSystem(SVIMProductEnum p_ProductType);
	static bool IsMatroxGige(SVIMProductEnum p_ProductType);
	static bool IsDigitalSVIM(SVIMProductEnum p_ProductType);
	static bool IsProductTypeRAID(SVIMProductEnum p_ProductType);
	static bool IsNonIOSVIM(SVIMProductEnum p_ProductType);

protected:
	SVHardwareManifest();

	void Startup();
	void Shutdown();

private:
	/*
	Do not implement the following to methods.
	These definitions prevent copy and assignement operations.
	*/
	SVHardwareManifest( const SVHardwareManifest& p_rObject );
	const SVHardwareManifest& operator=( const SVHardwareManifest& p_rObject );

	SVTriggerDeviceParamsVector m_TriggerDeviceParams;
	SVAcquisitionConstructParamsVector m_AcquisitionDeviceParams;

	typedef std::map< SVIMProductEnum, SVIMTypeInfoStruct > SVIMTypeMap;
	static const SVIMTypeMap m_SVIMTypeMap;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVHardwareManifest.h_v  $
 * 
 *    Rev 1.1   11 Jun 2013 15:26:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 May 2013 10:00:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums.h to SCConfigurationLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:45:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jan 2013 10:46:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added BuildAcquisitionTriggerDeviceName method
 * Added  IsNonIOSVIM method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jan 2013 16:04:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Moved hardware tables and look-up fucntionality to the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2010 08:05:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2010 14:13:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
