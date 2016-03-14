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

#if !defined __XML_MACROS
#define __XML_MACROS

#define CHECKHR(x){hr = x; m_ExceptionSet = FALSE;m_SVException.clearMessage();if (FAILED(hr)){m_ExceptionSet = TRUE;goto CleanUp;}}
#define SAFERELEASE(p) {if (p) {(p)->Release(); p = NULL;}}
#define RETURN return (hr == S_OK) ? TRUE : FALSE
#define CHECKXMLDOC	 if(m_pDoc == NULL)return FALSE

#endif

