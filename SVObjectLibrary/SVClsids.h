//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClsids
//* .File Name       : $Workfile:   SVClsids.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 14:05:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// SVObserver Object IDs 
////////////////////////////////////////////////////////////////////////////////
namespace ObjectIdEnum
{
	constexpr uint32_t RootUidId = 1;
	constexpr uint32_t GlobalUidId = 2;
	constexpr uint32_t EnvironmentUidId = 3;
	constexpr uint32_t ViewId = 4;
	constexpr uint32_t EnvironmentModelNumberUidId = 5;
	constexpr uint32_t EnvironmentWin = 6;
	constexpr uint32_t EnvironmentWinKeyUidId = 7;
	constexpr uint32_t EnvironmentImageUpdateUidId = 8;
	constexpr uint32_t EnvironmentResultUpdateUidId = 9;
	constexpr uint32_t EnvironmentModeIsRunUidId = 10;
	constexpr uint32_t EnvironmentModeIsStopUidId = 11;
	constexpr uint32_t EnvironmentModeIsRegressionTestUidId = 12;
	constexpr uint32_t EnvironmentModeIsTestUidId = 13;
	constexpr uint32_t EnvironmentModeIsEditUidId = 14;
	constexpr uint32_t EnvironmentModeValueUidId = 15;
	constexpr uint32_t EnvironmentModeUidId = 16;
	constexpr uint32_t EnvironmentAutoSaveUidId = 17;
	constexpr uint32_t EnvironmentDiskProtectionUidId = 18;
	constexpr uint32_t EnvironmentStartLastConfigUidId = 19;
	constexpr uint32_t EnvironmentConfigurationName = 20;
	constexpr uint32_t EnvironmentConfigurationFileName = 21;
	constexpr uint32_t EnvironmentCurrentDate = 22;
	constexpr uint32_t EnvironmentCurrentTime = 23;
	constexpr uint32_t EnvironmentSerialNumber = 24;

	// Unique base ID for following variables always have 0x100 ids free
	constexpr uint32_t CameraBaseSerialNumberUidId = 0x100;
	constexpr uint32_t CameraBaseGainUidId = 0x200;
	constexpr uint32_t CameraBaseShutterUidId = 0x300;
	constexpr uint32_t CameraBaseRegPathUidId = 0x400;
	constexpr uint32_t CameraBaseRegFileUidId = 0x500;
	constexpr uint32_t PpqBaseLengthUidId = 0x600;
	constexpr uint32_t PpqBaseTriggerCountUidId = 0x700;
	constexpr uint32_t DigitalInputUidId = 0x800;
	constexpr uint32_t DigitalOutputUidId = 0x900;
	constexpr uint32_t RemoteInputUidId = 0x1000;
	constexpr uint32_t AcquisitionId = 0x1100;

	constexpr uint32_t FirstPossibleObjectId = 1000000; //First possible object id for automatic generated ids.
}

////////////////////////////////////////////////////////////////////////////////
// Insert new Ids immediately before the previous line.
////////////////////////////////////////////////////////////////////////////////

