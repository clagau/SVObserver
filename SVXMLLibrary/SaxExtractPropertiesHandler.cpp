///*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "SaxExtractPropertiesHandler.h"
#include "Definitions/StringTypeDef.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


namespace SvXml
{
	SaxExtractPropertiesHandler::SaxExtractPropertiesHandler(): 
	m_depth(0),
	m_status(PARSING_NONE),
	
	m_StartNodeAttName(L"Base"),
	m_StartNodeAttType (L"SV_BASENODE"),
	m_pCurrentNodeInDataTree(nullptr),
	m_pCurrentNodeInRevisionTree(nullptr),
	m_pCurrentNodeInEncryptionTree(nullptr),
	m_BreakIfComplete(false),
	m_Complete(false)
{
	m_SaxParser.AttachElementHandler(this); 
}



	void SaxExtractPropertiesHandler::Clear()
	{
		m_depth  =0 ;
		m_status = PARSING_NONE; 
		m_pCurrentNodeInDataTree = nullptr;
		m_pCurrentNodeInRevisionTree = nullptr;
		m_pCurrentNodeInEncryptionTree = nullptr;
		m_DataTree.clear();
		m_EncryptionTree.clear();
		m_RevisionTree.clear();
		//m_BreakIfComplete = false;
		//m_Properties.clear();
	}

	SaxExtractPropertiesHandler::~SaxExtractPropertiesHandler(void)
	{
		Clear();
	}

	
	
	HRESULT  SaxExtractPropertiesHandler::ExtractProperties(LPCTSTR FileName, unsigned long& rVersionNumber)
	{
		SaxProperty sPaxProp;
		sPaxProp.NodeName = L"Environment";
		sPaxProp.DataName =  L"VersionNumber";
		sPaxProp.Type = VT_UI4;
		m_Properties[L"VersionNumber"] = sPaxProp;

		m_Complete = false; 
		m_BreakIfComplete = true;
		HRESULT hres =  BuildFromXMLFile(FileName);
		
		rVersionNumber = 0;
		if(m_Properties.find(L"VersionNumber") != m_Properties.end() )
		{
			rVersionNumber = _wtoi (m_Properties[L"VersionNumber"].Content.c_str()); 
		}

		m_Complete = false;
		m_BreakIfComplete = false;
		m_Properties.clear();
		return hres;	
	}
	
	
	HRESULT  SaxExtractPropertiesHandler::ExtractProperties(LPCTSTR FileName, long& rNewDisableMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber)
	{

		SaxProperty sPaxProp;
		
		sPaxProp.NodeName = L"Inspection Process";
		sPaxProp.DataName =  L"NewDisableMethod";
		sPaxProp.Type = VT_I4;
		m_Properties[ L"NewDisableMethod"] =  sPaxProp;
		
		sPaxProp.NodeName = L"Inspection Process";
		sPaxProp.DataName =  L"EnableAuxiliaryExtent";
		sPaxProp.Type = VT_I4;
		m_Properties[L"EnableAuxiliaryExtent"] =  (sPaxProp);
		
		sPaxProp.NodeName = L"Environment";
		sPaxProp.DataName =  L"VersionNumber";
		sPaxProp.Type = VT_UI4;
		m_Properties[L"VersionNumber"] = sPaxProp;
		
		m_Complete = false; 
		m_BreakIfComplete = true;
		HRESULT hres =  BuildFromXMLFile(FileName);
		rNewDisableMethod = _wtoi (m_Properties[L"NewDisableMethod"].Content.c_str());
		rEnableAuxExtents = _wtoi (m_Properties[ L"EnableAuxiliaryExtent"].Content.c_str()); 
		rVersionNumber =0;
		
		if(m_Properties.find(L"VersionNumber") != m_Properties.end() )
		{
			rVersionNumber = _wtoi (m_Properties[L"VersionNumber"].Content.c_str()); 
		}
		m_Complete = false;
		m_BreakIfComplete = false;
		m_Properties.clear();
		return hres;	
	}

	HRESULT  SaxExtractPropertiesHandler::BuildFromXMLFile(LPCTSTR FileName)
	{

		if( !m_SaxParser.IsReady() )
		{
			return E_FAIL;
		}
		Clear();

		HRESULT hr(E_FAIL);
		try
		{
			hr =   m_SaxParser.ParseFile(FileName);
		}
		catch ( const SvStl::MessageContainer&  )
		{
			;
			// if a exception occurs E_FAil is returned.
			// exception are only thrown and logged in in VariantHelper.;
		}
		if(hr == E_ABORT && m_Complete == true)
		{
			hr = S_OK;
		}
		return hr;
	}

	HRESULT  SaxExtractPropertiesHandler::OnStartElement(const wchar_t  *, int , const wchar_t *pwchName,  int cchName,ISAXAttributes *pAttributes,int  ) 
	{
		SpTreeElement spNewElement = std::make_shared<SaxTreeElement>(pwchName, cchName);
		spNewElement->SetAttributes(pAttributes);

		if(m_status == PARSING_NONE)
		{
			if(spNewElement->IsRevisionBaseNode())
			{
				m_status = PARSING_REVISION;
				m_depth =0;
			}
			else if(spNewElement->IsDataBaseNode(m_StartNodeAttType.c_str(), m_StartNodeAttName.c_str()) )
			{
				m_status = PARSING_DATATREE;
				m_depth =0;
			}
			else if (spNewElement->IsEncryptionBaseNode())
			{
				m_status =  PARSING_ENCRYPTION;
				m_depth =0;
			}

		}

		if(m_status == PARSING_NONE)
		{
			return S_OK;
		}

		if(m_status == PARSING_REVISION)
		{
			if(nullptr != m_pCurrentNodeInRevisionTree )
			{
				m_pCurrentNodeInRevisionTree =	m_pCurrentNodeInRevisionTree->insert( spNewElement).node(); 
			}
			else // insert first Element 
			{
				assert(m_RevisionTree.is_root()); 
				*(m_RevisionTree.get()) = spNewElement;
				m_pCurrentNodeInRevisionTree = &m_RevisionTree;
			}
		}
		else if(m_status == PARSING_ENCRYPTION)
		{
			if(nullptr != m_pCurrentNodeInEncryptionTree )
			{
				m_pCurrentNodeInEncryptionTree =	m_pCurrentNodeInEncryptionTree->insert( spNewElement).node(); 
			}
			else // insert first Element 
			{
				assert(m_EncryptionTree.is_root()); 
				*(m_EncryptionTree.get()) = spNewElement;
				m_pCurrentNodeInEncryptionTree = &m_EncryptionTree;
			}
		}
		else if (m_status == PARSING_DATATREE )
		{
			if(nullptr != m_pCurrentNodeInDataTree )
			{
				m_pCurrentNodeInDataTree =	m_pCurrentNodeInDataTree->insert( spNewElement).node(); 
			}
			else // insert first Element 
			{
				assert(m_DataTree.is_root()); 
				*(m_DataTree.get()) = spNewElement;
				m_pCurrentNodeInDataTree = &m_DataTree;
			}
		}
		m_depth++;
		return S_OK;
	}


	SaxTreeElement* SaxExtractPropertiesHandler::GetCurrentSaxTreeElement()
	{

		SaxTreeElement*  pElement(nullptr);
		if(m_status == PARSING_DATATREE)
		{
			pElement = (m_pCurrentNodeInDataTree->get())->get();
		}
		else if (m_status == PARSING_REVISION)
		{
			pElement = (m_pCurrentNodeInRevisionTree->get())->get();
		}
		else if (m_status == PARSING_ENCRYPTION)
		{
			pElement = (m_pCurrentNodeInEncryptionTree->get()->get());
		}
		return pElement;
	}

	HRESULT  SaxExtractPropertiesHandler::OnElementData( const wchar_t *pwchData,  int cchData, int ) 
	{
		if(m_status != PARSING_NONE)
		{
			SaxTreeElement* pElement  = GetCurrentSaxTreeElement(); 
			if(pElement)
			{
				pElement->AppendToContent(pwchData,cchData);
			}
			else
			{
				SvStl::MessageContainer Exception;
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_UnexpectedOnElementData, SvStl::SourceFileParams(StdMessageParams),SvStl::Err_16068_OnElementData  );
				throw Exception;
			}

		}
		return S_OK;
	}

	
	HRESULT  SaxExtractPropertiesHandler::OnEndElement(const wchar_t  *, int ,const wchar_t*,  int , int  )
	{
		if(m_status == PARSING_NONE)
		{
			return S_OK;
		}

		m_depth--;

		SaxTreeElement* pElement  = GetCurrentSaxTreeElement(); 
		if(pElement)
		{
			pElement->ClearEmptyContent();
		}


		if (m_status == PARSING_REVISION)
		{

			if(m_pCurrentNodeInRevisionTree)
			{
				m_pCurrentNodeInRevisionTree = m_pCurrentNodeInRevisionTree->parent();
			}
			else
			{
				assert(m_pCurrentNodeInRevisionTree);
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidPointer, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16069_INVALIDPOINTER );

			}
			if(nullptr == m_pCurrentNodeInRevisionTree)
			{
				m_status = PARSING_NONE;
				assert(m_depth == 0 );
			}
		}
		else if (m_status == PARSING_ENCRYPTION)
		{

			if(m_pCurrentNodeInEncryptionTree)
			{
				m_pCurrentNodeInEncryptionTree = m_pCurrentNodeInEncryptionTree->parent();
			}
			else
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidPointer, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16070_INVALIDPOINTER );
				assert(m_pCurrentNodeInEncryptionTree);
			}

			if(nullptr == m_pCurrentNodeInEncryptionTree)
			{
				//SetDecryption();
				m_status = PARSING_NONE;
				assert(m_depth == 0 );
			}
		}

		else if (m_status == PARSING_DATATREE)
		{

			if(m_pCurrentNodeInDataTree)
			{
				m_pCurrentNodeInDataTree = m_pCurrentNodeInDataTree->parent();
			}
			else
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidPointer, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16070_INVALIDPOINTER );
				assert(m_pCurrentNodeInDataTree);
			}

			if(nullptr == m_pCurrentNodeInDataTree)
			{
			
				m_status = PARSING_NONE;
				assert(m_depth == 0 );
			}
		
		}

		if(nullptr != pElement && pElement->GetElementType() == SaxTreeElement::DATA_Element  && nullptr != m_pCurrentNodeInDataTree )
		{
			std::wstring stName = pElement->GetNameAtt();
			std::wstring stNodeName = m_pCurrentNodeInDataTree->get()->get()->GetNameAtt();
			
			std::map<std::wstring, SaxProperty>::iterator it =   m_Properties.begin();
			int notfound = 0;
			for(; it != m_Properties.end(); ++it)
			{
				if(it->second.IsFound == false )
				{
					if(it->second.DataName == stName && it->second.NodeName ==  stNodeName)
					{
						it->second.IsFound = true;
						it->second.Content = pElement->GetContent();
					}
					else
					{
						notfound++;
					}

				}
				
			}
			if(notfound == 0 && m_BreakIfComplete == true)
			{
				m_Complete = true;
				return E_ABORT;//E_FAIL;
			}
			
		}
		return S_OK;
	}


	HRESULT   SaxExtractPropertiesHandler::OnXMLError(int line, int column, const wchar_t *pwchErrorText, unsigned long errorCode, bool )
	{
		if(m_Complete && errorCode == E_ABORT)
		{
			return S_OK;
		}
		SvDef::StringVector messageList;
		messageList.push_back(SvUl::Format(_T("%i"), line));
		messageList.push_back(SvUl::Format(_T("%i"), column));
		messageList.push_back(SvUl::Format(_T("%i"), errorCode));
		messageList.push_back(SvUl::createStdString(pwchErrorText));
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_83_SAX_PARSER_ERROR, SvStl::Tid_XML_Error, messageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16063_SAXPARSER );
		assert(false);
		return S_OK;
	}
} //namespace SvXml
