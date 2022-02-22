//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
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

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::putDocumentLocator(MSXML2::ISAXLocator* )
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

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::startPrefixMapping(unsigned short* , int , unsigned short* , int )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::endPrefixMapping(unsigned short* , int )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::startElement(unsigned short* , int , unsigned short* , int , unsigned short* , int , MSXML2::ISAXAttributes* )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::endElement(unsigned short* , int , unsigned short* , int , unsigned short* , int )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::characters(unsigned short* , int )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::ignorableWhitespace(unsigned short* , int )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::processingInstruction(unsigned short* , int , unsigned short* , int )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE SVSAXContentHandlerImpl::skippedEntity(unsigned short* , int )
{
    return S_OK;
}

long __stdcall SVSAXContentHandlerImpl::QueryInterface(const struct _GUID&, void** )
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

