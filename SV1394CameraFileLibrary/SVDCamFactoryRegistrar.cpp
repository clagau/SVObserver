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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamFactoryRegistrar.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:36:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 11:13:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Oct 2009 11:16:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated source code to include new functionality for the Flea2 camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2009 19:19:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Split Dragonfly and Flea classes apart and fixed issues with functionality and const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:59:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
