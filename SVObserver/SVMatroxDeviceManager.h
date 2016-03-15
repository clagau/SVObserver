// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDeviceManager
// * .File Name       : $Workfile:   SVMatroxDeviceManager.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:32:54  $
// ******************************************************************************

#pragma once
#ifndef __MATROXDEVICEMANAGER_H__
#define __MATROXDEVICEMANAGER_H__

#include "SVObserverEnums.h"
#include "SVMatroxAppManager.h"
#include "SVMatroxDeviceParamCollection.h"

#include <mil.h>
#include <map>

#include "mildetect.h"
#include <static_check.h>

// if you get an error on this line, you do not have the correct mil.h installed
FILE_SCOPE_STATIC_CHECK( SV_CURRENT_MIL_1394_PATCH >= 22, Need_to_compile_with_1394_patch_22_mil_dot_h_or_greater );


class SVMatroxDeviceManager
{
public:
	SVMatroxDeviceManager();
	~SVMatroxDeviceManager();

	HRESULT Create();
	HRESULT Destroy();
	HRESULT Reset();
	HRESULT ReallocDigitizer( SVMatroxDeviceParamCollection& device );

	HRESULT GetAllDevices( SVMatroxDeviceParamCollectionArray& aDevices );
	HRESULT GetDevice( const CString& sDeviceName, SVMatroxDeviceParamCollection& device );

	bool IsValidDigitizerSubsystem();
	int GetMaxNumDigitizers();

	static HRESULT WriteCameraRegister( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulValue );
	static HRESULT ReadCameraRegister( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long& rulValue );
	static HRESULT WriteCameraRegisterBlock( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulCount, unsigned long* paulValue );
	static HRESULT ReadCameraRegisterBlock( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulCount, unsigned long* paulValue );

	static long SwitchByteOrder( long& l );
	static unsigned long SwitchByteOrder( unsigned long& l );

protected:

	HRESULT AllocDigitizer( SVMatroxDeviceParamCollection& device );
	HRESULT FreeDigitizer( SVMatroxDeviceParamCollection& device );

	HRESULT AddDigitizer( SVMatroxDeviceParamCollection& device );

	HRESULT RemoveDigitizer( const CString& sDigName );


	MIL_ID 	m_MilSystem;
	long m_lNumDigitizers;
	long m_lMaxNumDigitizers;
	bool m_bHubSystem;

	typedef std::map<CString, SVMatroxDeviceParamCollection> DigitizerParamMapType;
	typedef std::map<MIL_ID, long> DigIdToDigNumMapType;	// used by read/write camera register fns
	DigitizerParamMapType m_mapDigitizerParams;
	static DigIdToDigNumMapType m_mapDigitizerIds;	// yuk! don't want to make it static, but it will only be used by read/write camera register fns

	SVMatroxAppManager m_AppManager;	// just declaring this will initialize the Mil App

	static long MFTYPE HandleError(long lHookType, MIL_ID EventId, void MPTYPE* pUserData);
};

#endif	//#ifndef __MATROXDEVICEMANAGER_H__

