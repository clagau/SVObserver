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

