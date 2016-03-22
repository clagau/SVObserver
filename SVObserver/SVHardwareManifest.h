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
//Moved to precompiled header: #include <map>
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

