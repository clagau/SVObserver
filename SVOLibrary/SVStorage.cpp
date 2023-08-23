//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorage
//* .File Name       : $Workfile:   SVStorage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVStorage.h"

SVStorage::SVStorage()
: m_StorageType( SVVisionProcessor::SVStorageUnknown ), m_Variant()
{
	#if defined (TRACE_THEM_ALL) || defined (TRACE_SVSTORAGE)
	TRACE0("+++++++SVStorage() Constructor+++++++++++\n");
	#endif
}

SVStorage::SVStorage( SVVisionProcessor::SVStorageType StorageType, const _variant_t& rVariant )
: m_StorageType( StorageType ), m_Variant( rVariant )
{
	#if defined (TRACE_THEM_ALL) || defined (TRACE_SVSTORAGE)
	TRACE0("+++++++SVStorage(Parameter) Constructor+++++++++++\n");
	#endif
}

