//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAXErrorHandlerImpl
//* .File Name       : $Workfile:   SVSAXErrorHandlerImpl.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:01:34  $
//******************************************************************************

#include "SVSAXErrorHandlerImpl.h"

template<typename ErrorStrategy>
SVSAXErrorHandlerImpl<ErrorStrategy>::SVSAXErrorHandlerImpl(ErrorStrategy& errorStrategy)
: m_rStrategy(errorStrategy)
{
}

template<typename ErrorStrategy>
SVSAXErrorHandlerImpl<ErrorStrategy>::~SVSAXErrorHandlerImpl()
{
}

template<typename ErrorStrategy>
HRESULT STDMETHODCALLTYPE SVSAXErrorHandlerImpl<ErrorStrategy>::error(MSXML2::ISAXLocator* pLocator, unsigned short* pwchErrorMessage, HRESULT errCode)
{
	m_rStrategy.ReportError(reinterpret_cast<wchar_t *>(pwchErrorMessage), errCode);
	return S_OK;
}

template<typename ErrorStrategy>
HRESULT STDMETHODCALLTYPE SVSAXErrorHandlerImpl<ErrorStrategy>::fatalError(MSXML2::ISAXLocator* pLocator, unsigned short* pwchErrorMessage, HRESULT errCode)
{
	m_rStrategy.ReportError(reinterpret_cast<wchar_t *>(pwchErrorMessage), errCode);
	return S_OK;
}

template<typename ErrorStrategy>
HRESULT STDMETHODCALLTYPE SVSAXErrorHandlerImpl<ErrorStrategy>::ignorableWarning(MSXML2::ISAXLocator* pLocator, unsigned short* pwchErrorMessage, HRESULT errCode)
{
	return S_OK;
}

template<typename ErrorStrategy>
long __stdcall SVSAXErrorHandlerImpl<ErrorStrategy>::QueryInterface(const struct _GUID &,void **)
{
	// hack-hack-hack!
	return 0;
}

template<typename ErrorStrategy>
unsigned long __stdcall SVSAXErrorHandlerImpl<ErrorStrategy>::AddRef()
{
	// hack-hack-hack!
	return 0;
}

template<typename ErrorStrategy>
unsigned long __stdcall SVSAXErrorHandlerImpl<ErrorStrategy>::Release()
{
	// hack-hack-hack!
	return 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVSAXErrorHandlerImpl.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:01:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2010 07:48:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
