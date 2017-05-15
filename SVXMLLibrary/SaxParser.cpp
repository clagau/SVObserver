///*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
// class SAXParser see 
/// (http://www.codeproject.com/Articles/11085/Parsing-XML-using-a-C-wrapper-for-SAX)
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comutil.h>
#include "SaxParser.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
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
	SaxParser::SaxParser():
	  m_ParseTime(0),
	  m_pReader(nullptr),
	  m_pContentHandler(nullptr),
	  m_pErrorHandler(nullptr),
	  m_pSchemaCache(nullptr),
	  m_pAttachedElementHandler(nullptr),
	  m_ComInit(false)
	{

		// Initialize COM.
		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if((S_OK != hr) && (S_FALSE != hr) && (RPC_E_CHANGED_MODE != hr))
		{
			ASSERT(false);
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_Sax_ConInitializeExFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16073_COINITIALIZE_ );
		}
		else if(S_FALSE == hr)
		{
			//COM already initialize
			CoUninitialize( );
		}
		else if(S_OK == hr)
		{
			//COM has been initialized
			m_ComInit = true;
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
		if ( nullptr != m_pReader )
		{
			m_pReader->Release();
			
			m_pReader = nullptr;
		}

		// Release the schema cache object.
		if ( nullptr != m_pSchemaCache )
		{
			m_pSchemaCache->Release();
			m_pSchemaCache = nullptr;
		}

		if( nullptr != m_pErrorHandler)
		{
			delete m_pErrorHandler;
			m_pErrorHandler = nullptr;
		}

		if( nullptr != m_pContentHandler)
		{
			delete m_pContentHandler;
			m_pContentHandler = nullptr;
		}

		// Shutdown COM.
		if(m_ComInit)
		{
			CoUninitialize();
		}
	}

	bool SaxParser::IsReady() const 
	{
		return (nullptr != m_pReader);
	}

	void  SaxParser::CreateSaxReader()
	{
		HRESULT hr = CoCreateInstance(
			__uuidof(SAXXMLReader60), 
			nullptr, 
			CLSCTX_ALL, 
			__uuidof(ISAXXMLReader), 
			(void **)&m_pReader);

		if ( SUCCEEDED(hr) )
		{        
			// Set the content handler.
			m_pContentHandler = new SaxContentHandler;
			hr = m_pReader->putContentHandler(m_pContentHandler);
			if ( FAILED(hr) )
			{
				delete m_pContentHandler;
				m_pContentHandler = nullptr;
			}

			// Set the error handler.
			m_pErrorHandler = new SaxErrorHandler;
			hr = m_pReader->putErrorHandler(m_pErrorHandler);
			if ( FAILED(hr) )
			{
				delete m_pErrorHandler;
				m_pErrorHandler = nullptr;
			}
		}
		else
		{
			delete m_pErrorHandler;
			m_pErrorHandler = nullptr;

			delete m_pContentHandler;
			m_pContentHandler = nullptr;

			m_pReader = nullptr;
		}

	}

	void SaxParser::CreateSchemaCache()
	{
		if ( nullptr == m_pReader )
		{
			return;
		}

		HRESULT hr = CoCreateInstance(
			__uuidof(XMLSchemaCache60), 
			nullptr, 
			CLSCTX_ALL, 
			__uuidof(IXMLDOMSchemaCollection2), 
			(void **)&m_pSchemaCache);

		if ( SUCCEEDED(hr) )
		{
			// Set the "schemas" property in the reader in order
			// to associate the schema cache with the reader.
			hr = m_pReader->putProperty(L"schemas", _variant_t(m_pSchemaCache));
			if ( FAILED(hr) )
			{
				OutputDebugString(_T("CXmlParserImpl::CreateSchemaCache(): putProperty(L\"schemas\",...) failed\n"));
			}
		}
	}

	void  SaxParser::AttachElementHandler(ISaxElementHandler* pElementHandler)
	{
		m_pAttachedElementHandler = pElementHandler;

		if( nullptr != m_pContentHandler )
		{
			m_pContentHandler->AttachElementHandler(pElementHandler);
		}

		if( nullptr != m_pErrorHandler )
		{
			m_pErrorHandler->AttachElementHandler(pElementHandler);
		}
	}
	
	void  SaxParser::DetachElementHandler()
	{
		if( nullptr != m_pErrorHandler )
		{
			m_pErrorHandler->AttachElementHandler(nullptr);
		}

		if( nullptr != m_pContentHandler )
		{
			m_pContentHandler->AttachElementHandler(nullptr);
		}

		m_pAttachedElementHandler = nullptr;
	}

	// Set parser feature options.
	HRESULT  SaxParser::SetParserFeature(const std::wstring& featureName, bool value)
	{
		if ( featureName.empty() )
		{
			return E_INVALIDARG;
		}
		if ( !IsReady() )
		{
			return E_FAIL;
		}

		VARIANT_BOOL vfValue = (value ? VARIANT_TRUE : VARIANT_FALSE);
		_bstr_t bstrFeature = featureName.c_str();

		HRESULT hr = m_pReader->putFeature(bstrFeature, vfValue);
		return hr;
	}

	HRESULT   SaxParser::GetParserFeature(const std::wstring& featureName, bool& value) const
	{
		if ( featureName.empty() )
		{
			return E_INVALIDARG;
		}
		if ( !IsReady() )
		{
			return E_FAIL;
		}

		VARIANT_BOOL vfValue = VARIANT_FALSE;
		_bstr_t bstrFeature = featureName.c_str();

		HRESULT hr = m_pReader->getFeature(bstrFeature, &vfValue);
		
		value = (vfValue == VARIANT_TRUE ? true : false);

		return hr;
	}

	HRESULT  SaxParser::AddValidationSchema(const std::wstring& namespaceURI, const std::wstring& xsdPath)
	{
		if ( !IsReady() )
		{
			return E_FAIL;
		}
		if ( nullptr == m_pSchemaCache )
		{
			return E_FAIL;
		}

		// Check for existing schema associated with this namespace URI.
		ISchema* pExistingSchema = nullptr;
		_bstr_t bstrNamespace = namespaceURI.c_str();
		HRESULT hr = m_pSchemaCache->getSchema(bstrNamespace, &pExistingSchema);
		if ( SUCCEEDED(hr) )
		{
			// Remove the existing schema.
			hr = m_pSchemaCache->remove(bstrNamespace);
			
		}

		// Add the new schema.
			if ( SUCCEEDED(hr) )
			{
				hr = m_pSchemaCache->add(bstrNamespace, _variant_t(xsdPath.c_str()));
			}
		return hr;
	}
	
	HRESULT   SaxParser::RemoveValidationSchema(const std::wstring& namespaceURI)
	{
		if ( !IsReady() )
		{
			return E_FAIL;
		}
		if ( nullptr == m_pSchemaCache )
		{
			return E_FAIL;
		}

		// Check for existing schema associated with this namespace URI.
		ISchema* pExistingSchema( nullptr );
		_bstr_t bstrNamespace = namespaceURI.c_str();
		HRESULT hr = m_pSchemaCache->getSchema(bstrNamespace, &pExistingSchema);
		if ( SUCCEEDED(hr) )
		{
			// Remove the existing schema.
			hr = m_pSchemaCache->remove(bstrNamespace);
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
		HRESULT hr = m_pReader-> parse(rVar);
		m_ParseTime = ::GetTickCount() - Tick;

		return hr;
	}
	
	HRESULT   SaxParser::ParseFile(LPCTSTR FileName)
	{
		if ( !IsReady() )
		{
			return E_FAIL;
		}
		_bstr_t FileNameToParse( FileName );
		DWORD Tick = ::GetTickCount();
		HRESULT hr = m_pReader->parseURL( FileNameToParse );
		m_ParseTime = ::GetTickCount() -Tick;

		return hr;
	}
} //namespace SvXml

