//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataManagerEnums
//* .File Name       : $Workfile:   SVDataManagerEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:46  $
//******************************************************************************

#pragma once

enum SVDataManagerLockTypeEnum
{
   SV_INVALID_TYPE = -1,
   SV_ACQUISITION = 0,
   SV_INSPECTION,
   SV_PPQ,
   SV_ARCHIVE,
   SV_DCOM,
   SV_DISPLAY,
   SV_LAST_INSPECTED,
   SV_OTHER       // Code assumes that SV_OTHER will always be last.
};

enum SVLogType
{
	SVLogDMErrors = 1,
	SVLogDMAcquisition = 2,
	SVLogDMInspection = 4,
	SVLogDMPPQ = 8,
	SVLogDMArchive = 16,
	SVLogDMDCom = 32,
	SVLogDMDisplay = 64,
	SVLogDMLastInspection = 128,
	SVLogDMOther = 256,
};


