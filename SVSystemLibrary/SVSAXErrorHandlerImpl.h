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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVSAXErrorHandlerImpl.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:53:14   bWalter
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
