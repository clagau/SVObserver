//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitizerProcessingClass
//* .File Name       : $Workfile:   SVDigitizerProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Aug 2014 07:24:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <utility>
#include "SVAcquisitionClass.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvTrig
{
	class SVDigitizerLoadLibraryClass;
}//namespace SvTrig

namespace SvIe
{

class SVDigitizerProcessingClass
{
public:
	static SVDigitizerProcessingClass& Instance();

	virtual ~SVDigitizerProcessingClass();

	void clear();

	void ClearDevices();

	HRESULT UpdateDigitizerSubsystem( SvTrig::SVDigitizerLoadLibraryClass* pDigitizerSubsystem );

	HRESULT GetAcquisitionDeviceList( SvDef::StringVector& rList ) const;

	bool IsValidDigitizerSubsystem( LPCTSTR digitizerName ) const;
	SvTrig::SVDigitizerLoadLibraryClass* GetDigitizerSubsystem( LPCTSTR digitizerName ) const;

	SVAcquisitionClassPtr GetDigitizer( LPCTSTR szName ) const;
	SVAcquisitionClassPtr GetAcquisitionDevice( LPCTSTR szName ) const;

	HRESULT DestroyBuffers();

	HRESULT ConnectDevices();
	HRESULT DisconnectDevices();

	HRESULT ScanForCameras();

	//************************************
	//! The method is used to set the digitizer color settings
	//! \param DigitizerName <in> The name of the digitizer
	//! \returns S_OK on success
	//************************************
	HRESULT SetDigitizerColor( LPCTSTR DigitizerName, bool isColor );

	//************************************
	//! The method is used to get the acquisition device name from the camera index
	//! \param CameraIndex <in> camera index
	//! \returns the acquisition name as a string
	//************************************
	std::string GetReOrderedCamera( int CameraIndex ) const;

	//************************************
	//! The method is used to get the acquisition device name from the IP Address
	//! \param CameraIPAddress <in> the camera IP address
	//! \returns the acquisition name as a string
	//************************************
	std::string GetReOrderedCamera( LPCTSTR CameraIPAddress ) const;

	//************************************
	//! The method is used to get the digitizer ID name from the camera index
	//! \param CameraIndex <in> camera index
	//! \returns the digitizer ID
	//************************************
	int getDigitizerID( const int CameraID ) const;

	// These two (2) methods, Startup, Shutdown, are only meant to be called by the main application class and no other
	// They used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call these methods anywhere else, and described via this comment
	void Startup();		// This method is only meant to be called by the main application class
	void Shutdown();	// This method is only meant to be called by the main application class

private:
	SVDigitizerProcessingClass();

	typedef std::set<SvTrig::SVDigitizerLoadLibraryClass*> SVDigitizerSubsystemSet;
	typedef std::map<std::string, SVAcquisitionClassPtr> SVNameDigitizerMap;
	typedef std::unordered_map<std::string, SvTrig::SVDigitizerLoadLibraryClass*> SVNameDigitizerSubsystemMap;

	HRESULT AddDigitizer( LPCTSTR Name, LPCTSTR AcquisitionName, SvTrig::SVDigitizerLoadLibraryClass* pDigitizerSubsystem, unsigned long p_Handle );

	HRESULT UpdateMatroxDevices();

	//Do not implement this method. This definition prevent copy and assignement operations.
	SVDigitizerProcessingClass( const SVDigitizerProcessingClass& p_rObject );

	//Do not implement this method. This definition prevent copy and assignement operations.
	const SVDigitizerProcessingClass& operator=( const SVDigitizerProcessingClass& p_rObject );

	SVDigitizerSubsystemSet m_Subsystems;

	SVNameDigitizerMap m_Digitizers;
	SVNameDigitizerMap m_AcquisitionDevices;

	SVNameDigitizerSubsystemMap m_DigitizerSubsystems;
};

} //namespace SvIe
