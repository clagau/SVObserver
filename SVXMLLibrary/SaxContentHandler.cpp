///*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
////  class SVSaxContentHandler
//*****************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SaxContentHandler.h"
#include "ISaxElementHandler.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


namespace SvXml
{
	SaxContentHandler::SaxContentHandler(void)
	{
		m_depth = 0;
		m_pElementHandler  = nullptr;
	}

	SaxContentHandler::~SaxContentHandler(void)
	{
	}
		
	long STDMETHODCALLTYPE SaxContentHandler::QueryInterface(const struct _GUID &riid,void ** ppvObject)
	{
		// Not implemented as this class is not a COM object.
		return 0;
	}

	unsigned long STDMETHODCALLTYPE SaxContentHandler::AddRef()
	{
		// Not implemented as this class is not a COM object.
		return 0;
	}

	unsigned long STDMETHODCALLTYPE SaxContentHandler::Release()
	{
		// Not implemented as this class is not a COM object.
		return 0;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::putDocumentLocator( 
		/* [in] */ ISAXLocator  *pLocator)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::startDocument()
	{
		m_depth = 0;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::endDocument( void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::startPrefixMapping( 
		/* [in] */ const  wchar_t *pwchPrefix,
		/* [in] */ int cchPrefix,
		/* [in] */ const  wchar_t *pwchUri,
		/* [in] */ int cchUri)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::endPrefixMapping( 
		/* [in] */ const wchar_t  *pwchPrefix,
		/* [in] */ int cchPrefix)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::startElement( 
		/* [in] */ const wchar_t  *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t  *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t  *pwchRawName,
		/* [in] */ int cchRawName,
		/* [in] */ ISAXAttributes  *pAttributes)
	{
			
		++m_depth;
		if( nullptr != m_pElementHandler )
		{
			return m_pElementHandler->OnStartElement(pwchNamespaceUri, cchNamespaceUri , pwchLocalName, cchLocalName, pAttributes, m_depth  );
		}
		return S_OK;
			
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::endElement( 
		/* [in] */ const wchar_t  *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t  *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t  *pwchRawName,
		/* [in] */ int cchRawName)
	{
		--m_depth;
		if( nullptr != m_pElementHandler )
		{
			return m_pElementHandler->OnEndElement(pwchNamespaceUri, cchNamespaceUri ,pwchLocalName, cchLocalName, m_depth  );
		}
		return S_OK;
	}

			
	HRESULT STDMETHODCALLTYPE SaxContentHandler::characters( 
		/* [in] */ const wchar_t  *pwchChars,
		/* [in] */ int cchChars)
	{
		if( nullptr != m_pElementHandler )
		{
			return m_pElementHandler->OnElementData(pwchChars, cchChars,m_depth );
		}
		return S_OK;

			
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::ignorableWhitespace( 
		/* [in] */ const wchar_t *pwchChars,
		/* [in] */ int cchChars)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::processingInstruction( 
		/* [in] */ const wchar_t *pwchTarget,
		/* [in] */ int cchTarget,
		/* [in] */ const wchar_t  *pwchData,
		/* [in] */ int cchData)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::skippedEntity( 
		/* [in] */ const wchar_t __RPC_FAR *pwchVal,
		/* [in] */ int cchVal)
	{
		return S_OK;
	}

	void SaxContentHandler::AttachElementHandler(ISaxElementHandler* pElementHandler)
	{
		m_pElementHandler  = pElementHandler;
	}
} //namespace SvXml
