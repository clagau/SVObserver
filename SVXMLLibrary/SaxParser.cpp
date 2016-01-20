///*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
// class SAXParser see 
/// (http://www.codeproject.com/Articles/11085/Parsing-XML-using-a-C-wrapper-for-SAX)
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "SaxParser.h"
#include <comutil.h>
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader
{
	namespace  SVXMLLibrary
	{
		SaxParser::SaxParser():
	m_ParsTime(0),
		m_reader(nullptr),
		m_contentHandler(nullptr),
		m_errorHandler(nullptr),
		m_schemaCache(nullptr),
		m_attachElementHandler(nullptr),
		m_bComInit(false)
	{

		// Initialize COM.
		HRESULT hr = CoInitializeEx(NULL,COINIT_MULTITHREADED);
		if((hr != S_OK) && (hr != S_FALSE) && (hr != RPC_E_CHANGED_MODE ))
		{
			ASSERT(false);
			std::string msg = "CoInitializeEx Failed";
			SvStl::MessageMgrNoDisplay Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, msg.c_str(), StdMessageParams, SvOi::Err_16073_COINITIALIZE_ );
		}
		else if (hr == S_FALSE)
		{
			//COM already initialize
			CoUninitialize( );
		}
		else if(hr == S_OK)
		{
			//COM has been initialized
			m_bComInit = true;
		}


		// Create the SAX XML reader (COM class).
		CreateSaxReader();

		// Create the schema cache (COM class) for XSD validation.
		CreateSchemaCache();
	}


	SaxParser::~SaxParser() 
	{
		DetachElementHandler();

		// Release the SAX XML reader object.
		if ( m_reader != NULL )
		{
			m_reader->Release();
			
			m_reader = NULL;
		}

		// Release the schema cache object.
		if ( m_schemaCache != NULL )
		{
			m_schemaCache->Release();
			m_schemaCache = NULL;
		}

		delete m_errorHandler;
		m_errorHandler = nullptr;

		delete m_contentHandler;
		m_contentHandler = nullptr;

		// Shutdown COM.
		if(m_bComInit)
		{
			CoUninitialize();
		}



	}


	
	bool SaxParser::IsReady() const 
	{
		return (m_reader != NULL);

	}

	void  SaxParser::CreateSaxReader()
	{
		HRESULT hr = CoCreateInstance(
			__uuidof(SAXXMLReader60), 
			NULL, 
			CLSCTX_ALL, 
			__uuidof(ISAXXMLReader), 
			(void **)&m_reader);

		if ( SUCCEEDED(hr) )
		{        
			// Set the content handler.
			m_contentHandler = new SaxContentHandler;
			hr = m_reader->putContentHandler(m_contentHandler);
			if ( FAILED(hr) )
			{
				delete m_contentHandler;
				m_contentHandler = nullptr;
			}

			// Set the error handler.
			m_errorHandler = new SaxErrorHandler;
			hr = m_reader->putErrorHandler(m_errorHandler);
			if ( FAILED(hr) )
			{
				delete m_errorHandler;
				m_errorHandler = nullptr;
			}
		}
		else
		{
			delete m_errorHandler;
			m_errorHandler = nullptr;

			delete m_contentHandler;
			m_contentHandler = nullptr;

			m_reader = nullptr;
		}

	}
	void SaxParser::CreateSchemaCache()
	{
		if ( m_reader == NULL )
			return;

		HRESULT hr = CoCreateInstance(
			__uuidof(XMLSchemaCache60), 
			NULL, 
			CLSCTX_ALL, 
			__uuidof(IXMLDOMSchemaCollection2), 
			(void **)&m_schemaCache);

		if ( SUCCEEDED(hr) )
		{
			// Set the "schemas" property in the reader in order
			// to associate the schema cache with the reader.
			hr = m_reader->putProperty(L"schemas", _variant_t(m_schemaCache));
			if ( FAILED(hr) )
			{
				OutputDebugString(_T("CXmlParserImpl::CreateSchemaCache(): putProperty(L\"schemas\",...) failed\n"));
			}
		}
	}

	void  SaxParser::AttachElementHandler(ISaxElementHandler* pElementHandler)
	{
		m_attachElementHandler = pElementHandler;

		if ( m_contentHandler != NULL )
			m_contentHandler->AttachElementHandler(pElementHandler);

		if ( m_errorHandler != NULL )
			m_errorHandler->AttachElementHandler(pElementHandler);
	}
	
	void  SaxParser::DetachElementHandler()
	{
		if ( m_errorHandler != NULL )
			m_errorHandler->AttachElementHandler(nullptr);

		if ( m_contentHandler != NULL )
			m_contentHandler->AttachElementHandler(nullptr);

		m_attachElementHandler  = NULL;

	}

	// Set parser feature options.
	HRESULT  SaxParser::SetParserFeature(const std::wstring& featureName, bool value)
	{
		if ( featureName.empty() )
			return E_INVALIDARG;
		if ( !IsReady() )
			return E_FAIL;

		VARIANT_BOOL vfValue = (value ? VARIANT_TRUE : VARIANT_FALSE);
		_bstr_t bstrFeature = featureName.c_str();

		HRESULT hr = m_reader->putFeature(bstrFeature, vfValue);
		return hr;
	}
	HRESULT   SaxParser::GetParserFeature(const std::wstring& featureName, bool& value) const
	{
		if ( featureName.empty() )
			return E_INVALIDARG;
		if ( !IsReady() )
			return E_FAIL;

		VARIANT_BOOL vfValue = VARIANT_FALSE;
		_bstr_t bstrFeature = featureName.c_str();

		HRESULT hr = m_reader->getFeature(bstrFeature, &vfValue);
		
		value = (vfValue == VARIANT_TRUE ? true : false);

		return hr;
	}

	


	HRESULT  SaxParser::AddValidationSchema(const std::wstring& namespaceURI, const std::wstring& xsdPath)
	{
		if ( !IsReady() )
			return E_FAIL;
		if ( m_schemaCache == NULL )
			return E_FAIL;

		// Check for existing schema associated with this namespace URI.
		ISchema* pExistingSchema = NULL;
		_bstr_t bstrNamespace = namespaceURI.c_str();
		HRESULT hr = m_schemaCache->getSchema(bstrNamespace, &pExistingSchema);
		if ( SUCCEEDED(hr) )
		{
			// Remove the existing schema.
			hr = m_schemaCache->remove(bstrNamespace);
			
		}

		// Add the new schema.
			if ( SUCCEEDED(hr) )
			{
				hr = m_schemaCache->add(bstrNamespace, _variant_t(xsdPath.c_str()));
			}
		return hr;
	}
	
	HRESULT   SaxParser::RemoveValidationSchema(const std::wstring& namespaceURI)
	{
		if ( !IsReady() )
			return E_FAIL;
		if ( m_schemaCache == NULL )
			return E_FAIL;

		// Check for existing schema associated with this namespace URI.
		ISchema* pExistingSchema = NULL;
		_bstr_t bstrNamespace = namespaceURI.c_str();
		HRESULT hr = m_schemaCache->getSchema(bstrNamespace, &pExistingSchema);
		if ( SUCCEEDED(hr) )
		{
			// Remove the existing schema.
			hr = m_schemaCache->remove(bstrNamespace);

		}
		return hr;
	}


	HRESULT  SaxParser::ParseXml(const _variant_t& rVar )
	{
		if ( !IsReady() )
		{
			return E_FAIL;
		}
		DWORD Tick = ::GetTickCount();
		

		HRESULT hr = m_reader-> parse(rVar);
		m_ParsTime = ::GetTickCount() -Tick;
		return hr;
	}

	
	HRESULT   SaxParser::ParseFile(const wchar_t * pwstrPath)
	{
		if ( !IsReady() )
		{
			return E_FAIL;
		}
		DWORD Tick = ::GetTickCount();
		HRESULT hr = m_reader->parseURL(pwstrPath);
		m_ParsTime = ::GetTickCount() -Tick;
		return hr;
	}

	}
}


