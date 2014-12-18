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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVThreadManager\SVThreadAttribute.h_v  $
 * 
 *    Rev 1.0   17 Dec 2014 07:36:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Enum attribute in seperate file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
