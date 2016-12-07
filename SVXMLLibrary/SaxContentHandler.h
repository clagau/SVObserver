//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// class  class SaxContentHandler implements ISAXContentHandler
//******************************************************************************
#pragma once
#pragma region Includes
#include <msxml6.h>
#include "ISaxElementHandler.h"
#pragma endregion Includes


namespace Seidenader { namespace  SVXMLLibrary
{
	/// \brief SaxContentHandler implements ISAXContentHandler
	/// Receive content events during parsing and delegate
	// them to the attached element handler.
	///naming convention from msxml6.h is used 
	class SaxContentHandler : public ISAXContentHandler
	{
	#pragma region Constructor
	public:
		SaxContentHandler(void);
		virtual ~SaxContentHandler(void);
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		virtual long __stdcall QueryInterface(const struct _GUID &, void **) override;
		virtual unsigned long STDMETHODCALLTYPE AddRef(void) override;
		virtual unsigned long STDMETHODCALLTYPE Release(void) override;
			
		virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
			/* [in] */ ISAXLocator *pLocator) override;
			
		virtual HRESULT STDMETHODCALLTYPE startDocument( void) override;
			
		virtual HRESULT STDMETHODCALLTYPE endDocument( void) override;
			
		virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
			/* [in] */ const wchar_t *pwchPrefix,
			/* [in] */ int cchPrefix,
			/* [in] */ const wchar_t *pwchUri,
			/* [in] */ int cchUri) override;

		virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
			/* [in] */ const wchar_t *pwchPrefix,
			/* [in] */ int cchPrefix) override;

		virtual HRESULT STDMETHODCALLTYPE startElement( 
			/* [in] */ const wchar_t *pwchNamespaceUri,
			/* [in] */ int cchNamespaceUri,
			/* [in] */ const wchar_t *pwchLocalName,
			/* [in] */ int cchLocalName,
			/* [in] */ const wchar_t *pwchQName,
			/* [in] */ int cchQName,
			/* [in] */ ISAXAttributes *pAttributes) override;

		virtual HRESULT STDMETHODCALLTYPE endElement( 
			/* [in] */ const wchar_t *pwchNamespaceUri,
			/* [in] */ int cchNamespaceUri,
			/* [in] */ const wchar_t *pwchLocalName,
			/* [in] */ int cchLocalName,
			/* [in] */ const wchar_t *pwchQName,
			/* [in] */ int cchQName) override;

		virtual HRESULT STDMETHODCALLTYPE characters( 
			/* [in] */ const wchar_t *pwchChars,
			/* [in] */ int cchChars) override;

		virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
			/* [in] */ const wchar_t *pwchChars,
			/* [in] */ int cchChars) override;

		virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
			/* [in] */ const wchar_t *pwchTarget,
			/* [in] */ int cchTarget,
			/* [in] */ const wchar_t *pwchData,
			/* [in] */ int cchData) override;

		virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
			/* [in] */ const wchar_t *pwchName,
			/* [in] */ int cchName) override;

		//************************************
		//! Set the ElementHandler 
		//! \param pElementHandler [in]
		//! \returns void
		//************************************
		void AttachElementHandler(ISaxElementHandler*  pElementHandler);

		//************************************
		//! Set the element handler to null 
		//! \returns void
		//************************************
		void DetachElementHandler();
	#pragma endregion Public Methods
	
	#pragma region Member variables
	private:
		int m_depth;
		ISaxElementHandler* m_pElementHandler; 
	#pragma endregion Member variables
	};
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */

namespace SvXml = Seidenader::SVXMLLibrary;