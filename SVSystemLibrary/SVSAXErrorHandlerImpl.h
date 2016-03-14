//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAXErrorHandlerImpl
//* .File Name       : $Workfile:   SVSAXErrorHandlerImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:53:14  $
//******************************************************************************
#pragma once

namespace MSXML2
{
	struct ISAXErrorHandler;
	struct ISAXLocator;
};

template<typename ErrorStrategy>
class SVSAXErrorHandlerImpl : public MSXML2::ISAXErrorHandler
{
private:
	ErrorStrategy& m_rStrategy;

public:
	SVSAXErrorHandlerImpl(ErrorStrategy& strategy);
	virtual ~SVSAXErrorHandlerImpl();

	// This must be correctly implemented, if your handler must be a COM Object
	long __stdcall QueryInterface(const struct _GUID &,void **);
	unsigned long __stdcall AddRef();
	unsigned long __stdcall Release();

	virtual HRESULT STDMETHODCALLTYPE error( 
		/* [in] */ MSXML2::ISAXLocator* pLocator,
		/* [in] */ unsigned short* pwchErrorMessage,
		/* [in] */ HRESULT errCode);

	virtual HRESULT STDMETHODCALLTYPE fatalError( 
		/* [in] */ MSXML2::ISAXLocator* pLocator,
		/* [in] */ unsigned short* pwchErrorMessage,
		/* [in] */ HRESULT errCode);

	virtual HRESULT STDMETHODCALLTYPE ignorableWarning( 
		/* [in] */ MSXML2::ISAXLocator* pLocator,
		/* [in] */ unsigned short* pwchErrorMessage,
		/* [in] */ HRESULT errCode);
};

#include "SVSAXErrorHandlerImpl.inl"

