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
	constexpr uint32_t RootId = 1;
	constexpr uint32_t GlobalId = 2;
	constexpr uint32_t EnvironmentId = 3;
	constexpr uint32_t ViewId = 4;
	constexpr uint32_t EnvironmentModelNumberId = 5;
	constexpr uint32_t EnvironmentWinId = 6;
	constexpr uint32_t EnvironmentWinKeyId = 7;
	constexpr uint32_t EnvironmentImageUpdateId = 8;
	constexpr uint32_t EnvironmentResultUpdateId = 9;
	constexpr uint32_t EnvironmentModeIsRunId = 10;
	constexpr uint32_t EnvironmentModeIsStopId = 11;
	constexpr uint32_t EnvironmentModeIsRegressionTestId = 12;
	constexpr uint32_t EnvironmentModeIsTestId = 13;
	constexpr uint32_t EnvironmentModeIsEditId = 14;
	constexpr uint32_t EnvironmentModeValueId = 15;
	constexpr uint32_t EnvironmentModeId = 16;
	constexpr uint32_t EnvironmentAutoSaveId = 17;
	constexpr uint32_t EnvironmentDiskProtectionId = 18;
	constexpr uint32_t EnvironmentStartLastConfigId = 19;
	constexpr uint32_t EnvironmentConfigurationNameId = 20;
	constexpr uint32_t EnvironmentConfigurationFileNameId = 21;
	constexpr uint32_t EnvironmentCurrentDateId = 22;
	constexpr uint32_t EnvironmentCurrentTimeId = 23;
	constexpr uint32_t EnvironmentSerialNumberId = 24;
	constexpr uint32_t IOControllerId = 25;
	constexpr uint32_t InputObjectListId = 26;
	constexpr uint32_t OutputObjectListId = 27;
	constexpr uint32_t ModuleReadyId = 28;
	constexpr uint32_t RaidErrorBitId = 29;
	constexpr uint32_t ConfigObjectId = 30;
	constexpr uint32_t RemoteDataControllerObjectId = 31;


	// Unique base ID for following variables always have 0x100 ids free
	constexpr uint32_t CameraBaseSerialNumberId = 0x100;
	constexpr uint32_t CameraBaseGainId = 0x200;
	constexpr uint32_t CameraBaseShutterId = 0x300;
	constexpr uint32_t CameraBaseRegPathId = 0x400;
	constexpr uint32_t CameraBaseRegFileId = 0x500;
	constexpr uint32_t PpqBaseLengthId = 0x600;
	constexpr uint32_t PpqBaseTriggerCountId = 0x700;
	constexpr uint32_t DigitalInputId = 0x800;
	constexpr uint32_t DigitalOutputId = 0x900;
	constexpr uint32_t RemoteInputId = 0x1000;
	constexpr uint32_t AcquisitionId = 0x1100;
	constexpr uint32_t PlcOutputId = 0x1200;

	constexpr uint32_t FirstPossibleObjectId = 1000000; //First possible object id for automatic generated ids.
}

////////////////////////////////////////////////////////////////////////////////
// Insert new Ids immediately before the previous line.
////////////////////////////////////////////////////////////////////////////////

