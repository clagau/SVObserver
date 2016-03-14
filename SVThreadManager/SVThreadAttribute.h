//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadAttribute
//* .File Name       : $Workfile:   SVThreadAttribute.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Dec 2014 07:36:12  $
//******************************************************************************

#pragma once


enum SVThreadAttribute
{
	SVNone = 0,
	SVAffinityBasic = 1,
	SVAffinityPPQ = 2,
	SVAffinityAcq = 4,
	SVAffinityTrigger = 8,
	SVAffinityEditAllowed = 0x8000,
	SVAffinityBaseType = 0x7FFF,
	SVAffinityUser = SVAffinityBasic | SVAffinityEditAllowed,
	SVAffinityDummy = 0x10,
};

