// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCISAPERA
// * .File Name       : $Workfile:   SVCISapera.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:06  $
// ******************************************************************************

#pragma once

#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVCIAcqStruct.h"
#include "SVCIBufferStruct.h"
#include "SVImageLibrary/SVLut.h"
#include "SVImageLibrary/SVLightReference.h"

HRESULT g_CICreate();
HRESULT g_CIDestroy( bool p_bClose = false );

HRESULT g_CIBufferCreate( unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, unsigned long p_ulType, unsigned long *p_pulIndex );
HRESULT g_CIBufferCreate( BSTR p_bstrFileName, unsigned long *p_pulIndex );
HRESULT g_CIBufferDestroy( unsigned long p_ulIndex );

HRESULT g_CIAcqCreate( unsigned long p_ulServerIndex, unsigned long p_ulAcqIndex, unsigned long *p_pulIndex );
HRESULT g_CIAcqLoadCameraFiles( unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames );
HRESULT g_CIAcqBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount );
HRESULT g_CIAcqTriggerRegister( unsigned long p_ulIndex, SVCallbackStruct &p_rCallback );
HRESULT g_CIAcqRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface );
HRESULT g_CIAcqStart( unsigned long p_ulIndex );
HRESULT g_CIAcqStop( unsigned long p_ulIndex );
HRESULT g_CIAcqUnregisterBufferInterface( unsigned long p_ulIndex );
HRESULT g_CIAcqTriggerUnregister( unsigned long p_ulIndex, SVCallbackStruct &p_rCallback );
HRESULT g_CIAcqTriggerUnregisterAll( unsigned long p_ulIndex );

HRESULT g_CIAcqTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
HRESULT g_CIAcqTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT g_CIAcqTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT g_CIAcqTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

HRESULT g_CIAcqBufferDestroyAll( unsigned long p_ulIndex );
HRESULT g_CIAcqUnloadCameraFiles( unsigned long p_ulIndex );
HRESULT g_CIAcqDestroy( unsigned long p_ulIndex );

HRESULT g_CIAcqGetLightReference( unsigned long p_ulIndex, unsigned long p_ulType, long &p_rlValue );
HRESULT g_CIAcqSetLightReference( unsigned long p_ulIndex, unsigned long p_ulType, long p_lValue );
HRESULT g_CIAcqGetLightReference( unsigned long p_ulIndex, SVLightReference& lr );
HRESULT g_CIAcqSetLightReference( unsigned long p_ulIndex, const SVLightReference& lr );

HRESULT g_CIAcqGetLut( unsigned long p_ulIndex, VARIANT &p_rvarValue );
HRESULT g_CIAcqSetLut( unsigned long p_ulIndex, VARIANT &p_rvarValue );
HRESULT g_CIAcqGetLut( unsigned long p_ulIndex, SVLut& lut );
HRESULT g_CIAcqSetLut( unsigned long p_ulIndex, const SVLut& lut );
HRESULT g_CIAcqNeutralLutToRawLut(const SVLut& lut, CByteArray& aRawBytes);
HRESULT g_CIAcqRawLutToNeutralLut(const CByteArray& aRawBytes, SVLut& lut);

HRESULT g_CIAcqEnableSoftwareTrigger( unsigned long p_ulIndex );
HRESULT g_CIAcqSoftwareTrigger( unsigned long p_ulIndex );

extern SVCIAcqStructVector g_caAcqs;
extern SVCIBufferStructCArray g_caBuffers;

