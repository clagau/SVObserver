//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClsids
//* .File Name       : $Workfile:   SVClsids.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 14:05:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVClsids.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

////////////////////////////////////////////////////////////////////////////////
// NOTE: Add new GUIDs at the bottom.
////////////////////////////////////////////////////////////////////////////////
const GUID RootUidGuid = { 0xa5c94500, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID GlobalUidGuid = { 0xa5c94501, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentUidGuid = { 0xa5c94502, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModelNumberUidGuid = { 0xa5c94503, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentWinKeyUidGuid = { 0xa5c94504, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentImageUpdateUidGuid = { 0xa5c94505, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentResultUpdateUidGuid = { 0xa5c94506, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModeIsRunUidGuid = { 0xa5c94507, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModeIsStopUidGuid = { 0xa5c94508, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModeIsRegressionTestUidGuid = { 0xa5c94509, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModeIsTestUidGuid = { 0xa5c9450a, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModeIsEditUidGuid = { 0xa5c9450b, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModeValueUidGuid = { 0xa5c9450c, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentModeUidGuid = { 0xa5c9450d, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentAutoSaveUidGuid = { 0xa5c9450e, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentDiskProtectionUidGuid = { 0xa5c9450f, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };
const GUID EnvironmentStartLastConfigUidGuid = { 0xa5c94510, 0xf204, 0x469d,{ 0x8f, 0xff, 0x48, 0x26, 0xef, 0x33, 0x18, 0x51 } };


// Leave 0x100 GUID's for Camera variables free after base to increment in the program
const GUID CameraBaseSerialNumberUidGuid = {0x80c6a000, 0x6ae8, 0x4733, { 0x99, 0x68, 0x1d, 0x1, 0xc7, 0x16, 0xc4, 0x60 }};
const GUID CameraBaseGainUidGuid = {0x80c6a100, 0x6ae8, 0x4733, { 0x99, 0x68, 0x1d, 0x1, 0xc7, 0x16, 0xc4, 0x60 }};
const GUID CameraBaseShutterUidGuid = {0x80c6a200, 0x6ae8, 0x4733, { 0x99, 0x68, 0x1d, 0x1, 0xc7, 0x16, 0xc4, 0x60 }};
const GUID CameraBaseRegPathUidGuid = {0x80c6a300, 0x6ae8, 0x4733, {0x99, 0x68, 0x1d, 0x1, 0xc7, 0x16, 0xc4, 0x60}};
const GUID CameraBaseRegFileUidGuid = {0x80c6a400, 0x6ae8, 0x4733, {0x99, 0x68, 0x1d, 0x1, 0xc7, 0x16, 0xc4, 0x60}};
// Leave 0x100 GUID's for PPQ variables free after base to increment in the program
const GUID PpqBaseLengthUidGuid = {0x6ce19000, 0xce64, 0x4b64, { 0x90, 0xf1, 0x2a, 0xc7, 0x58, 0xc0, 0x4c, 0x4c }};
// NOTICE: + 0x100 sequence is at DigitalInputUidGuid
const GUID PpqBaseTriggerCountUidGuid = {0xed697000, 0x546f, 0x4832, { 0xb6, 0x73, 0xf0, 0xe0, 0xd5, 0x37, 0xec, 0x00 }};

// Leave 0x100 GUID's  for IO-Entries variables free after base to increment in the program
extern const GUID DigitalInputUidGuid = {0x6ce19100, 0xce64, 0x4b64,{ 0x90, 0xf1, 0x2a, 0xc7, 0x58, 0xc0, 0x4c, 0x4c }};
extern const GUID DigitalOutputUidGuid = {0x6ce19200, 0xce64, 0x4b64,{ 0x90, 0xf1, 0x2a, 0xc7, 0x58, 0xc0, 0x4c, 0x4c }};
extern const GUID RemoteInputUidGuid = {0x6ce19300, 0xce64, 0x4b64,{ 0x90, 0xf1, 0x2a, 0xc7, 0x58, 0xc0, 0x4c, 0x4c }};
