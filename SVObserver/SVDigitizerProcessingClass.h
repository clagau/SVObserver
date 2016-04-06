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
	friend class SVObserverApp;

	static SVDigitizerProcessingClass& Instance();

	virtual ~SVDigitizerProcessingClass();

	void clear();

	void ClearDevices();

	HRESULT UpdateDigitizerSubsystem( SVDigitizerLoadLibraryClass* p_pDigitizerSubsystem );

	HRESULT GetDigitizerList( CStringArray& rList ) const;
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
	//! The method is used to select the corresponding digitizer
	//! \param AcquisitionName <in> The name of the acquisition device
	//! \returns S_OK on success
	//************************************
	HRESULT SelectDigitizer( LPCTSTR AcquisitionName );

	SVString GetReOrderedCamera( LPCTSTR Name ) const;

protected:
	SVDigitizerProcessingClass();

	void Startup();
	void Shutdown();

private:
	typedef std::set< SVDigitizerLoadLibraryClass* > SVDigitizerSubsystemSet;
	typedef std::map< SVString, SVAcquisitionClassPtr > SVNameDigitizerMap;
	typedef std::map< SVString, SVDigitizerLoadLibraryClass* > SVNameDigitizerSubsystemMap;

	HRESULT AddDigitizer( LPCTSTR p_szName, SVDigitizerLoadLibraryClass* p_pDigitizerSubsystem, unsigned long p_Handle );

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

