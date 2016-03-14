//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAXContentHandlerImpl
//* .File Name       : $Workfile:   SVSAXContentHandlerImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:53:00  $
//******************************************************************************
#pragma once

namespace MSXML2
{
	struct ISAXContentHandler;
	struct ISAXLocator;
	struct ISAXAttributes;
};

class SVSAXContentHandlerImpl : public MSXML2::ISAXContentHandler
{
public:
    SVSAXContentHandlerImpl();
    virtual ~SVSAXContentHandlerImpl();

	// This must be correctly implemented, if your handler must be a COM Object
	long __stdcall QueryInterface(const struct _GUID &,void **);
	unsigned long __stdcall AddRef();
	unsigned long __stdcall Release();

	virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
		/* [in] */ MSXML2::ISAXLocator* pLocator);

	virtual HRESULT STDMETHODCALLTYPE startDocument();

	virtual HRESULT STDMETHODCALLTYPE endDocument();

	virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
		/* [in] */ unsigned short* pwchPrefix,
		/* [in] */ int cchPrefix,
		/* [in] */ unsigned short* pwchUri,
		/* [in] */ int cchUri);
        
	virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
		/* [in] */ unsigned short* pwchPrefix,
		/* [in] */ int cchPrefix);

	virtual HRESULT STDMETHODCALLTYPE startElement( 
		/* [in] */ unsigned short* pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ unsigned short* pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ unsigned short* pwchRawName,
		/* [in] */ int cchRawName,
		/* [in] */ MSXML2::ISAXAttributes* pAttributes);

	virtual HRESULT STDMETHODCALLTYPE endElement( 
		/* [in] */ unsigned short* pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ unsigned short* pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ unsigned short* pwchRawName,
		/* [in] */ int cchRawName);

    virtual HRESULT STDMETHODCALLTYPE characters( 
        /* [in] */ unsigned short* pwchChars,
        /* [in] */ int cchChars);
    
    virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
        /* [in] */ unsigned short* pwchChars,
        /* [in] */ int cchChars);
    
    virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
        /* [in] */ unsigned short* pwchTarget,
        /* [in] */ int cchTarget,
        /* [in] */ unsigned short* pwchData,
        /* [in] */ int cchData);
    
    virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
        /* [in] */ unsigned short* pwchName,
        /* [in] */ int cchName);
};

