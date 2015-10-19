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

#pragma once

#pragma region Includes
#include <map>
#include "SVAcquisitionConstructParams.h"
#include "SVTriggerDeviceParams.h"
#include "SVIMTypeInfoStruct.h"
#include "SVConfigurationLibrary/SVObserverEnums.h"
#pragma endregion Includes

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
	static void GetSupportedSVIMList( Insertor insertor )
	{
		for (SVIMTypeMap::const_iterator l_Iter = m_SVIMTypeMap.begin(); l_Iter != m_SVIMTypeMap.end(); ++l_Iter)
		{
			const SVIMTypeInfoStruct& l_SVIMInfo = l_Iter->second;

			if ( l_SVIMInfo.m_Supported )
			{
				insertor = l_Iter->first;
			}
		}
	}

	//************************************
	//! This is a static method to generate the software trigger name
	//! \param iDig <in> the index the digitizer
	//! \returns the string with the name
	//************************************
	static SVString BuildSoftwareTriggerDeviceName(int iDig);

	//************************************
	//! This is a static method to generate the acquisition trigger name
	//! \param iDig <in> the index the digitizer
	//! \returns the string with the name
	//************************************
	static SVString BuildAcquisitionTriggerDeviceName(int iDig);

	//************************************
	//! This is a static method to generate the IO board trigger name
	//! \param iDig <in> the index the digitizer
	//! \returns the string with the name
	//************************************
	static SVString BuildIOBoardTriggerDeviceName(int iDig);

	//************************************
	//! This is a static method to check if two product types are compatible
	//! \param ConfigType <in> the configuration product type
	//! \param ProductType <in> the SVIM product type
	//! \returns true if the ConfigType is compatible to the ProductType
	//************************************
	static bool IsCompatible( SVIMProductEnum ConfigType, SVIMProductEnum ProductType );

	//************************************
	//! This is a static method to check if the product type is color
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a color type
	//************************************
	static bool IsColorSystem(SVIMProductEnum ProductType);

	//************************************
	//! This is a static method to check if the product type is mixed (mono and color camera)
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a mixed type
	//************************************
	static bool IsMixedSystem(SVIMProductEnum ProductType);

	//************************************
	//! This is a static method to check if the product type is a GigE system
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a GigE type
	//************************************
	static bool IsMatroxGige(SVIMProductEnum ProductType);

	//************************************
	//! This is a static method to check if the product type is digital
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a digital type
	//************************************
	static bool IsDigitalSVIM(SVIMProductEnum ProductType);

	//************************************
	//! This is a static method to check if the product type is RAID
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a RAID type
	//************************************
	static bool IsProductTypeRAID(SVIMProductEnum ProductType);

	//************************************
	//! This is a static method to check if the product type is non IO 
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a non IO type
	//************************************
	static bool IsNonIOSVIM(SVIMProductEnum ProductType);

protected:
	SVHardwareManifest();

	void Startup();
	void Shutdown();

private:
	/*
	Do not implement the following to methods.
	These definitions prevent copy and assignment operations.
	*/
	SVHardwareManifest( const SVHardwareManifest& p_rObject );
	const SVHardwareManifest& operator=( const SVHardwareManifest& p_rObject );

	SVTriggerDeviceParamsVector m_TriggerDeviceParams;
	SVAcquisitionConstructParamsVector m_AcquisitionDeviceParams;

	typedef std::map< SVIMProductEnum, SVIMTypeInfoStruct > SVIMTypeMap;
	static const SVIMTypeMap m_SVIMTypeMap;
};

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
