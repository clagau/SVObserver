// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDCamFactoryRegistrar.cpp
// * .File Name       : $Workfile:   SVDCamFactoryRegistrar.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:36:38  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDCamFactoryRegistrar.h"
#include "SVDCamDragonFlyDriver.h"
#include "SVDCamFleaDriver.h"
#include "SVDCamFlea2Driver.h"
#include "SVDCamBaslerDriver.h"
#include "SVDCamISGDriver.h"
#include "SVOMFCLibrary/SVTemplate.h"

void SVDCamFactoryRegistrar::Register()
{
	bool bRegister = TheDCamDriverFactory::Instance().Register(CString(_T("Standard")), SVDCamStandardDriver::CreateNew );
	bool bDefault  = TheDCamDriverFactory::Instance().SetDefault(CString(_T("Standard")));

	bool bRegisterDragonfly = TheDCamDriverFactory::Instance().Register(CString(_T("Dragonfly")), SVDCamDragonflyDriver::CreateNew );
	bool bRegisterFlea = TheDCamDriverFactory::Instance().Register(CString(_T("Flea")), SVDCamFleaDriver::CreateNew );
	bool bRegisterFlea2 = TheDCamDriverFactory::Instance().Register(CString(_T("Flea2")), SVDCamFlea2Driver::CreateNew );

	bool bRegisterBasler = TheDCamDriverFactory::Instance().Register(CString(_T("Basler")), SVDCamBaslerDriver::CreateNew );

	bool bRegisterMHPAS = TheDCamDriverFactory::Instance().Register(CString(_T("ISG_MHPAS")), SVDCamISGMHPASDriver::CreateNew );
	bool bRegisterLightWise = TheDCamDriverFactory::Instance().Register(CString(_T("ISG_LightWise")), SVDCamISGLightWiseDriver::CreateNew );
}

