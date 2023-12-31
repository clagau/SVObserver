///*****************************************************************************
/// \copyright (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
		
	long STDMETHODCALLTYPE SaxContentHandler::QueryInterface(const struct _GUID &,void ** )
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
		/* [in] */ ISAXLocator  *)
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
		/* [in] */ const  wchar_t *,
		/* [in] */ int ,
		/* [in] */ const  wchar_t *,
		/* [in] */ int )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::endPrefixMapping( 
		/* [in] */ const wchar_t  *,
		/* [in] */ int )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::startElement( 
		/* [in] */ const wchar_t  *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t  *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t  *,
		/* [in] */ int ,
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
		/* [in] */ const wchar_t  *,
		/* [in] */ int )
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
		/* [in] */ const wchar_t *,
		/* [in] */ int )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::processingInstruction( 
		/* [in] */ const wchar_t *,
		/* [in] */ int ,
		/* [in] */ const wchar_t  *,
		/* [in] */ int )
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SaxContentHandler::skippedEntity( 
		/* [in] */ const wchar_t __RPC_FAR*,
		/* [in] */ int )
	{
		return S_OK;
	}

	void SaxContentHandler::AttachElementHandler(ISaxElementHandler* pElementHandler)
	{
		m_pElementHandler  = pElementHandler;
	}
} //namespace SvXml
