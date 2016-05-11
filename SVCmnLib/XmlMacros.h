//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : XmlMacros
//* .File Name       : $Workfile:   XmlMacros.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:10:14  $
//******************************************************************************

#pragma once

#define CHECKHR(x){hr = x; m_ExceptionSet = false;m_SVException.clearMessage();if (FAILED(hr)){m_ExceptionSet = true;goto CleanUp;}}
#define SAFERELEASE(p) {if (p) {(p)->Release(); p = nullptr;}}
#define RETURN return (S_OK == hr) ? true : false		// this is dangerous! why is there no terminating semicolon here?
#define CHECKXMLDOC	 if(nullptr == m_pDoc)return false	// this is dangerous! why is there no terminating semicolon here?
