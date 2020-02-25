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
//Moved to precompiled header: #include <guiddef.h>
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// SVObserver Class IDs (Guids)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// NOTE: Add new GUIDs at the bottom.
////////////////////////////////////////////////////////////////////////////////
extern const GUID RootUidGuid;
extern const GUID GlobalUidGuid;
extern const GUID EnvironmentUidGuid;
extern const GUID EnvironmentModelNumberUidGuid;
extern const GUID EnvironmentWinKeyUidGuid;
extern const GUID EnvironmentImageUpdateUidGuid;
extern const GUID EnvironmentResultUpdateUidGuid;
extern const GUID EnvironmentModeIsRunUidGuid;
extern const GUID EnvironmentModeIsStopUidGuid;
extern const GUID EnvironmentModeIsRegressionTestUidGuid;
extern const GUID EnvironmentModeIsTestUidGuid;
extern const GUID EnvironmentModeIsEditUidGuid;
extern const GUID EnvironmentModeValueUidGuid;
extern const GUID EnvironmentModeUidGuid;
extern const GUID EnvironmentAutoSaveUidGuid;
extern const GUID EnvironmentDiskProtectionUidGuid;
extern const GUID EnvironmentStartLastConfigUidGuid;

// Unique base ID GUID for Camera variables always have 0x100 GUIDS free after the base GUID
extern const GUID CameraBaseSerialNumberUidGuid;
extern const GUID CameraBaseGainUidGuid;
extern const GUID CameraBaseShutterUidGuid;
extern const GUID CameraBaseRegPathUidGuid;
extern const GUID CameraBaseRegFileUidGuid;
// Unique base ID GUID for PPQ variables always have 0x100 GUIDS free after the base GUID
extern const GUID PpqBaseLengthUidGuid;
extern const GUID PpqBaseTriggerCountUidGuid;

// Unique base ID GUID for IO-Entries variables always have 0x100 GUIDS free after the base GUID
extern const GUID DigitalInputUidGuid;
extern const GUID DigitalOutputUidGuid;
extern const GUID RemoteInputUidGuid;

////////////////////////////////////////////////////////////////////////////////
// Insert new GUIDs immediately before the previous line.
////////////////////////////////////////////////////////////////////////////////

