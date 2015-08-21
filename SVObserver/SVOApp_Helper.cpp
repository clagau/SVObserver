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
#pragma endregion Includes

#pragma region implementation of methods from ISVOApp_Helper
bool SvOi::isUnrestrictedFileAccess()
{
	return TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
}
#pragma endregion implementation of methods from ISVOApp_Helper