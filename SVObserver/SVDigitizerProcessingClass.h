//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVUtilityLibrary/SVString.h"
#include "SVAcquisitionClass.h"
#pragma endregion Includes

class SVDigitizerLoadLibraryClass;

class SVDigitizerProcessingClass
{
public:
	static SVDigitizerProcessingClass& Instance();

	virtual ~SVDigitizerProcessingClass();

	void clear();

	void ClearDevices();

	HRESULT UpdateDigitizerSubsystem( SVDigitizerLoadLibraryClass* pDigitizerSubsystem );

	HRESULT GetAcquisitionDeviceList( CStringArray& rList ) const;

	bool IsValidDigitizerSubsystem( LPCTSTR digitizerName ) const;
	SVDigitizerLoadLibraryClass* GetDigitizerSubsystem( LPCTSTR digitizerName ) const;

	SVAcquisitionClassPtr GetDigitizer( LPCTSTR szName ) const;
	SVAcquisitionClassPtr GetAcquisitionDevice( LPCTSTR szName ) const;

	HRESULT DestroyBuffers();

	HRESULT ConnectDevices();
	HRESULT DisconnectDevices();

	HRESULT ScanForCameras();

	HRESULT StoreLastCameraImage();
	HRESULT RestoreLastCameraImage();

	//************************************
	//! The method is used to set the digitizer color settings
	//! \param DigitizerName <in> The name of the digitizer
	//! \returns S_OK on success
	//************************************
	HRESULT SetDigitizerColor( LPCTSTR DigitizerName, bool isColor );

	SVString GetReOrderedCamera( int CameraIndex ) const;

	// These two (2) methods, Startup, Shutdown, are only meant to be called by the main application class and no other
	// They used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call these methods anywhere else, and described via this comment
	void Startup();		// This method is only meant to be called by the main application class
	void Shutdown();	// This method is only meant to be called by the main application class

private:
	SVDigitizerProcessingClass();

	typedef std::set< SVDigitizerLoadLibraryClass* > SVDigitizerSubsystemSet;
	typedef std::map< SVString, SVAcquisitionClassPtr > SVNameDigitizerMap;
	typedef std::map< SVString, SVDigitizerLoadLibraryClass* > SVNameDigitizerSubsystemMap;

	HRESULT AddDigitizer( LPCTSTR Name, LPCTSTR AcquisitionName, SVDigitizerLoadLibraryClass* pDigitizerSubsystem, unsigned long p_Handle );

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

