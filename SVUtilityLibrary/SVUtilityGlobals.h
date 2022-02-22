//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityLibrary
//* .File Name       : $Workfile:   SVUtilityGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:26  $
//******************************************************************************

#pragma once

#define SV_SEVERITY( p_Status ) \
	( ( ( ( unsigned long ) p_Status ) >> 30 ) & 0x00000003 )

void KeepPrevError( HRESULT& p_rhrPrev, HRESULT p_hrNew );

