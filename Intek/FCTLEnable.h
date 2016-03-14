//******************************************************************************
//* .Module Name     : Enable Driver
//* .File Name       : $Workfile:   FCTLEnable.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   29 Apr 2013 10:07:54  $
//******************************************************************************

//* This function overrides the licence file validation and enables the driver 
//* to function with any camera.

/*******************************************************************************
  Global enable function for FireStack.
  intek, c. kuehnel, 8.12.2005
*******************************************************************************/

#ifndef FCTLENABLE_H
#define FCTLENABLE_H

#ifdef __cplusplus
extern "C" {
#endif

UINT32 __cdecl EnableDriver(CARDHANDLE hCard);

#ifdef __cplusplus
}
#endif

#endif

