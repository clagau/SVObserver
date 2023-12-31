//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObserverEnums.h"
#pragma endregion Includes

namespace SvDef
{
	enum class TriggerType;
}

class SVHardwareManifest
{
public:
	static SVHardwareManifest& Instance();

	virtual ~SVHardwareManifest();

	const SVTriggerDeviceParamsVector& GetTriggerDeviceParams() const;
	const SVAcquisitionConstructParamsVector& GetAcquisitionDeviceParams() const;

	static SVIMTypeInfoStruct GetSVIMTypeInfo( SVIMProductEnum p_ID  );

	template <typename Inserter>
	static void GetSupportedSVIMList( Inserter inserter )
	{
		for (SVIMTypeMap::const_iterator l_Iter = m_SVIMTypeMap.begin(); l_Iter != m_SVIMTypeMap.end(); ++l_Iter)
		{
			const SVIMTypeInfoStruct& l_SVIMInfo = l_Iter->second;

			if ( l_SVIMInfo.m_Supported )
			{
				// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
				inserter = l_Iter->first;
			}
		}
	}

	static std::string BuildTriggerDeviceName(SVIMProductEnum productType, int iDig, SvDef::TriggerType triggerType);

	//************************************
	//! This is a static method to generate the software trigger name
	//! \param iDig <in> the index the digitizer
	//! \returns the string with the name
	//************************************
	static std::string BuildSoftwareTriggerDeviceName(int iDig);

	//************************************
	//! This is a static method to generate the acquisition trigger name
	//! \param iDig <in> the index the digitizer
	//! \returns the string with the name
	//************************************
	static std::string BuildAcquisitionTriggerDeviceName(int iDig);

	//************************************
	//! This is a static method to generate the IO board trigger name
	//! \param iDig <in> the index the digitizer
	//! \returns the string with the name
	//************************************
	static std::string BuildIOBoardTriggerDeviceName(int iDig);

	//************************************
	//! This is a static method to generate the hardware trigger name
	//! \param iDig <in> the index the digitizer
	//! \returns the string with the name
	//************************************
	static std::string BuildHardwareTriggerDeviceName(int iDig);
	
	//************************************
	//! This is a static method to check if the product type loaded from a configuration is valid
	//! \param ProductType <in> the SVIM product type
	//! \returns true if the ProductType is valid
	//************************************
	static bool IsValidProductType( SVIMProductEnum ProductType );

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
	//! This is a static method to check if the product type is a discrete IO system
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a discrete system
	//************************************
	static bool isDiscreteIOSystem(SVIMProductEnum ProductType);

	//************************************
	//! This is a static method to check if the product type is a PLC system
	//! \param ProductType <in> the SVIM product type
	//! \returns true if it is a discrete system
	//************************************
	static bool isPlcSystem(SVIMProductEnum ProductType);

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

	// These two (2) methods, Startup, Shutdown are only meant to be called by the main application class and no other
	// They used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call these methods anywhere else, as described via this comment
	void Startup();	 // This method is only meant to be called by the main application class
	void Shutdown(); // This method is only meant to be called by the main application class

private:
	SVHardwareManifest();
	
	/*
	Do not implement the following two methods.
	These definitions prevent copy and assignment operations.
	*/
	SVHardwareManifest( const SVHardwareManifest& p_rObject );
	const SVHardwareManifest& operator=( const SVHardwareManifest& p_rObject );

	SVTriggerDeviceParamsVector m_TriggerDeviceParams;
	SVAcquisitionConstructParamsVector m_AcquisitionDeviceParams;

	typedef std::map< SVIMProductEnum, SVIMTypeInfoStruct > SVIMTypeMap;
	static const SVIMTypeMap m_SVIMTypeMap;
};
