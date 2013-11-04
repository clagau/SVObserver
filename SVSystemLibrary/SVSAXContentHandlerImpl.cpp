//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAXContentHandlerImpl
//* .File Name       : $Workfile:   SVSAXContentHandlerImpl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:52:44  $
//******************************************************************************

#include "StdAfx.h"
#include "SVSAXContentHandlerImpl.h"

SVSAXContentHandlerImpl::SVSAXContentHandlerImpl()
{
}

SVSAXContentHandlerImpl::~SVSAXContentHandlerImpl()
{
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::putDocumentLocator(MSXML2::ISAXLocator* pLocator)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::startDocument()
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::endDocument()
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::startPrefixMapping(unsigned short* pwchPrefix, int cchPrefix, unsigned short* pwchUri, int cchUri)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::endPrefixMapping(unsigned short* pwchPrefix, int cchPrefix)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::startElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName, MSXML2::ISAXAttributes* pAttributes)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::endElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::characters(unsigned short* pwchChars, int cchChars)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::ignorableWhitespace(unsigned short* pwchChars, int cchChars)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::processingInstruction(unsigned short* pwchTarget, int cchTarget, unsigned short* pwchData, int cchData)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::skippedEntity(unsigned short* pwchVal, int cchVal)
{
    return S_OK;
}

long __stdcall SVSAXContentHandlerImpl::QueryInterface(const struct _GUID &riid, void** ppvObject)
{
    // hack-hack-hack!
    return 0;
}

unsigned long __stdcall SVSAXContentHandlerImpl::AddRef()
{
    // hack-hack-hack!
    return 0;
}

unsigned long __stdcall SVSAXContentHandlerImpl::Release()
{
    // hack-hack-hack!
    return 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVSAXContentHandlerImpl.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:52:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2010 08:47:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include issues with SVSystemLibrary.
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
