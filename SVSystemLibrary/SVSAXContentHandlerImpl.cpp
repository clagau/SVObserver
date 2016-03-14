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

