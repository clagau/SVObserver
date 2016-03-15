// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDCamFactoryRegistrar.h
// * .File Name       : $Workfile:   SVDCamFactoryRegistrar.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:36:40  $
// ******************************************************************************

#ifndef INCL_SVDCAMFACTORYREGISTRAR_H
#define INCL_SVDCAMFACTORYREGISTRAR_H

#include "SVDCamDriver.h"

typedef TFactorySingleton<CString, SVDCamDriver>::factory TheDCamDriverFactory;

class SVDCamFactoryRegistrar
{
public:
	static void Register();
};
#endif

