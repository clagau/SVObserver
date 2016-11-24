//*****************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This file implemented the function for ISVOApp_Helper 
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\ISVOApp_Helper.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#pragma region implementation of methods from ISVOApp_Helper
bool SvOi::isUnrestrictedFileAccess()
{
	return TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
}

bool SvOi::isOkToEdit()
{
	return TheSVObserverApp.OkToEdit();
}

long SvOi::GetSourceImageDepth()
{
	return TheSVObserverApp.GetSourceImageDepth();
}
#pragma endregion implementation of methods from ISVOApp_Helper