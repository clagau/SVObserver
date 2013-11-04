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

#define CHECKHR(x){hr = x; m_ExceptionSet = FALSE;m_SVException.ResetException();if (FAILED(hr)){m_ExceptionSet = TRUE;SETEXCEPTION0(m_SVException, 0x00)goto CleanUp;}}
#define SAFERELEASE(p) {if (p) {(p)->Release(); p = NULL;}}
#define RETURN return (hr == S_OK) ? TRUE : FALSE
#define CHECKXMLDOC	 if(m_pDoc == NULL)return FALSE

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\XmlMacros.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/