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


namespace Seidenader
{
	namespace  SVXMLLibrary
	{
		/// \brief SaxContentHandler implements ISAXContentHandler
		/// Receive content events during parsing and delegate
		// them to the attached element handler.
		///naming convention from msxml6.h is used 
		class SaxContentHandler : public ISAXContentHandler
		{
		public:

#pragma region Constructor
			SaxContentHandler(void);
			virtual ~SaxContentHandler(void);
#pragma endregion Constructor

#pragma region Public Methods
			virtual long __stdcall          QueryInterface(const struct _GUID &, void **);
			virtual unsigned long STDMETHODCALLTYPE AddRef(void);
			virtual unsigned long STDMETHODCALLTYPE Release(void);
			
			virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
				/* [in] */ ISAXLocator *pLocator);
			
			virtual HRESULT STDMETHODCALLTYPE startDocument( void);
			
			virtual HRESULT STDMETHODCALLTYPE endDocument( void);
			
			virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
				/* [in] */ const wchar_t *pwchPrefix,
				/* [in] */ int cchPrefix,
				/* [in] */ const wchar_t *pwchUri,
				/* [in] */ int cchUri);

			virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
				/* [in] */ const wchar_t *pwchPrefix,
				/* [in] */ int cchPrefix) ;

			virtual HRESULT STDMETHODCALLTYPE startElement( 
				/* [in] */ const wchar_t *pwchNamespaceUri,
				/* [in] */ int cchNamespaceUri,
				/* [in] */ const wchar_t *pwchLocalName,
				/* [in] */ int cchLocalName,
				/* [in] */ const wchar_t *pwchQName,
				/* [in] */ int cchQName,
				/* [in] */ ISAXAttributes *pAttributes);

			virtual HRESULT STDMETHODCALLTYPE endElement( 
				/* [in] */ const wchar_t *pwchNamespaceUri,
				/* [in] */ int cchNamespaceUri,
				/* [in] */ const wchar_t *pwchLocalName,
				/* [in] */ int cchLocalName,
				/* [in] */ const wchar_t *pwchQName,
				/* [in] */ int cchQName);

		virtual HRESULT STDMETHODCALLTYPE characters( 
				/* [in] */ const wchar_t *pwchChars,
				/* [in] */ int cchChars);

			virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
				/* [in] */ const wchar_t *pwchChars,
				/* [in] */ int cchChars);

			virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
				/* [in] */ const wchar_t *pwchTarget,
				/* [in] */ int cchTarget,
				/* [in] */ const wchar_t *pwchData,
				/* [in] */ int cchData);

			virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
				/* [in] */ const wchar_t *pwchName,
				/* [in] */ int cchName);

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
		private:
			int m_depth;
			ISaxElementHandler* m_ElementHandler; 
		};
	}
}
namespace SvXml = Seidenader::SVXMLLibrary;