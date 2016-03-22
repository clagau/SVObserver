//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// class  class SaxParser
/// Wrapper class for SAX2 Parsing
/// see  (http://www.codeproject.com/Articles/11085/Parsing-XML-using-a-C-wrapper-for-SAX)
/// from nschan
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comutil.h>
#include <msxml6.h>
#include "ISaxElementHandler.h"
#include "SaxContentHandler.h"
#include "SaxErrorHandler.h"
#pragma endregion Includes

namespace Seidenader { namespace  SVXMLLibrary
{
	/// \brief Wrapper class for SAX2 parsing
	class SaxParser
	{
	#pragma region Constructor
	public:
		SaxParser();
		virtual ~SaxParser();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! Is the parser available (e.g. was the COM class created properly?).
		//! \returns bool
		//************************************
		bool IsReady() const;

		//************************************
		//!  Attach XML events handler.
		//! \param pElementHandler [in]
		//! \returns void
		//************************************
		void AttachElementHandler(ISaxElementHandler* pElementHandler);
			
		//************************************
		//! Detach XML events Handlers 
		//! \returns void
		//************************************
		void DetachElementHandler();

		//************************************
		//! Set parser feature options see docu for ISAXXMLReader
		//! \param featureName [in]
		//! \param value [in]
		//! \returns bool
		//************************************
		HRESULT  SetParserFeature(const std::wstring& featureName, bool value);
			
		//************************************
		//! Get Parser feature see docu for ISAXXMLReader
		//! \param featureName [in]
		//! \param value [in]
		//! \returns HRESULT
		//************************************
		HRESULT  GetParserFeature(const std::wstring& featureName, bool& value) const;

			
		//************************************
		//! Add XSD schemas for validation.  see docu for XMLSchemaCache60
		//! \param namespaceURI [in] The namespaceURI can be an empty string.
		//! \param xsdPath [in]
		//! \returns HRESULT
		//************************************
		HRESULT  AddValidationSchema(const std::wstring& namespaceURI, const std::wstring& xsdPath);
			
		//************************************
		//! Remove XSD schemas for validation.  see docu for XMLSchemaCache60
		//! \param namespaceURI [in]
		//! \returns HRESULT
		//************************************
		HRESULT  RemoveValidationSchema(const std::wstring& namespaceURI);
			
		//************************************
		//! Parse XML string 
		//! \param rVar [in] //variant with xml string
		//! \returns HRESULT
		//************************************
		HRESULT   ParseXml(const _variant_t& rVar );
		
			
		//************************************
		//! Parse a local file path, or a HTTP URL path.
		//! \param pwstrPath [in]
		//! \returns HRESULT
		//************************************
		HRESULT  ParseFile(const wchar_t * pwstrPath);
			
		//************************************
		//!Time needed for parsing in ms
		//! \returns DWORD time in ms 
		//************************************
		DWORD GetParseTime() const {return m_ParseTime;};
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		//************************************
		//! Create the SAX XML reader (COM class).
		//! \returns void
		//************************************
		void CreateSaxReader();
			
		//************************************
		//!  Create the schema cache (COM class) for XSD validation.
		//! \returns void
		//************************************
		void CreateSchemaCache();
	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		DWORD m_ParseTime; 

		// SAX reader.
		ISAXXMLReader* m_pReader;

		// Content handlers to be used with the SAX reader.
		SaxContentHandler* m_pContentHandler;
			
		///Error handlers to be used with the SAX reader.
		SaxErrorHandler*   m_pErrorHandler;

		// Schema cache for XSD validation.
		IXMLDOMSchemaCollection2* m_pSchemaCache;

		// Our own XML events handler.
		ISaxElementHandler*  m_pAttachedElementHandler;
			
		/// true if CoInitializeEx was successful
		bool m_ComInit;
	#pragma endregion Member variables
	};
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */

namespace SvXml = Seidenader::SVXMLLibrary;