//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// class SVSaxXMLHandler
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SaxXMLHandler.h"
#include "SVNavigateTree.h"
#include "VariantHelper.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvXml
{
SaxXMLHandler::SaxXMLHandler(void): 
m_depth(0),
	m_status(PARSING_NONE),
	m_WriteTime(0),
	m_StartNodeAttName(L"Base"),
	m_StartNodeAttType (L"SV_BASENODE"),
	m_ParseArray(false),
	m_pData_Tree(nullptr),
	m_CurrentBranchHandle(nullptr),
	m_pCurrentNodeInRevisionTree(nullptr),
	m_pCurrentNodeInEncryptionTree(nullptr),
	m_pSaxEncryptionHandler(nullptr)
{
	m_SaxParser.AttachElementHandler(this); 
}

void SaxXMLHandler::Clear()
{
	m_depth  =0 ;
	m_status = PARSING_NONE; 
	m_WriteTime =0 ;
	m_ParseArray = false;
	m_CurrentBranchHandle = nullptr;
	m_pCurrentNodeInRevisionTree = nullptr;
	m_pCurrentNodeInEncryptionTree = nullptr;
	m_EncryptionTree.clear();
	m_RevisionTree.clear();
}

SaxXMLHandler::~SaxXMLHandler(void)
{
	Clear();
}

void SaxXMLHandler::SetSaxEncryptionHandler(SaxEncryptionHandler *pSaxEncryptionHandler)
{
	m_pSaxEncryptionHandler = pSaxEncryptionHandler;
}

HRESULT SaxXMLHandler::BuildFromXMLString(SVXMLMaterialsTree* pDataTree , const _variant_t &var)
{
	m_pData_Tree = pDataTree;
	if( !m_SaxParser.IsReady() )
	{
		return E_FAIL;
	}
	Clear();
	HRESULT hr(E_FAIL);
	try
	{
		hr = m_SaxParser.ParseXml(var);
	}
	catch ( const SvStl::MessageContainer&  )
	{
		// if a exception occurs E_FAil is returned.
		// exception are only thrown and logged in in VariantHelper.
		;
	}
	return   hr;
}

HRESULT  SaxXMLHandler::BuildFromXMLFile(SVXMLMaterialsTree* pDataTree , LPCTSTR FileName)
{
	m_pData_Tree = pDataTree;
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
		// if a exception occurs E_FAil is returned.
		// exception are only thrown and logged in in VariantHelper.;
	}


	return hr;
}

DWORD  SaxXMLHandler::GetParseTime(void ) const
{
	return m_SaxParser.GetParseTime();
}

HRESULT  SaxXMLHandler::OnStartElement(const wchar_t  *, int , const wchar_t *pwchName,  int cchName,ISAXAttributes *pAttributes,int  ) 
{
	m_spNewElement.reset();
	m_spNewElement = SpTreeElement( new SaxTreeElement(pwchName, cchName));	
	m_spNewElement->SetAttributes(pAttributes);
	bool ParseArray = false;

	if(IsEncrypted())
	{
		m_pSaxEncryptionHandler->DecryptAttribute(m_spNewElement.get());
	}

	if(m_status == PARSING_NONE)
	{
		if(m_spNewElement->IsRevisionBaseNode())
		{
			m_status = PARSING_REVISION;
			m_depth =0;
		}
		else if(m_spNewElement->IsDataBaseNode(m_StartNodeAttType.c_str(), m_StartNodeAttName.c_str()) && nullptr != m_pData_Tree )
		{
			m_status = PARSING_DATATREE;
			m_depth =0;
		}
		else if (m_spNewElement->IsEncryptionBaseNode())
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
			m_pCurrentNodeInRevisionTree =	m_pCurrentNodeInRevisionTree->insert( m_spNewElement).node(); 
		}
		else // insert first Element 
		{
			Log_Assert(m_RevisionTree.is_root()); 
			*(m_RevisionTree.get()) = m_spNewElement;
			m_pCurrentNodeInRevisionTree = &m_RevisionTree;
		}
	}
	else if(m_status == PARSING_ENCRYPTION)
	{
		if(nullptr != m_pCurrentNodeInEncryptionTree )
		{
			m_pCurrentNodeInEncryptionTree =	m_pCurrentNodeInEncryptionTree->insert( m_spNewElement).node(); 
		}
		else // insert first Element 
		{
			Log_Assert(m_EncryptionTree.is_root()); 
			*(m_EncryptionTree.get()) = m_spNewElement;
			m_pCurrentNodeInEncryptionTree = &m_EncryptionTree;
		}
	}
	else if (m_status == PARSING_DATATREE )
	{
		if(m_depth ==0 )
		{
			m_CurrentBranchHandle  = m_pData_Tree->getRoot(); 
			//first Branch is not included do nothing else 
		}
		else 
		{
			switch(m_spNewElement->GetElementType())
			{
			case SaxTreeElement::NODE_Element:
				{
					if (m_spNewElement->IsVtArrayNode())
					{
						///Waiting for arrayelements 
						VectorClear(m_spNewElement->GetNameAtt());
						ParseArray = true;
					}
					else
					{
						///new branch 
						SVXMLMaterialsTree::SVBranchHandle  newBranchHandle;
						std::string name {_bstr_t(m_spNewElement->GetNameAtt())};
						SVNavigateTree::AddBranch( *m_pData_Tree , m_CurrentBranchHandle, name.c_str(), &newBranchHandle);
						m_CurrentBranchHandle = newBranchHandle;
					}


					break;
				}
			case SaxTreeElement::DATA_Element:
				{
					ParseArray = m_ParseArray;
					///we are waiting for Data and Insert this at EndElemnt

					break;
				}
			default:
				{
					SvStl::MessageContainer Exception;
					Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_UnexpectedType, SvStl::SourceFileParams(StdMessageParams));
					throw Exception;
					break;
				}

			}
		}
	}

	m_ParseArray = ParseArray ;
	m_depth++;

	return S_OK;
}

SaxTreeElement* SaxXMLHandler::GetCurrentSaxTreeElement()
{

	SaxTreeElement*  pElement(nullptr);
	if(m_status == PARSING_DATATREE)
		pElement =  m_spNewElement.get();
	else if (m_status == PARSING_REVISION)
		pElement = (m_pCurrentNodeInRevisionTree->get())->get();
	else if (m_status == PARSING_ENCRYPTION)
		pElement = (m_pCurrentNodeInEncryptionTree->get()->get());
	return pElement;

}

HRESULT  SaxXMLHandler::OnElementData( const wchar_t *pwchData,  int cchData, int ) 
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
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_UnexpectedOnElementData, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}

	}
	return S_OK;
}

HRESULT  SaxXMLHandler::OnEndElement(const wchar_t  *, int ,const wchar_t *pwchName,  int cchName, int  )
{
	if(m_status == PARSING_NONE)
	{
		return S_OK;
	}

	m_depth--;
	SpTreeElement	spNewElement = SpTreeElement( new SaxTreeElement(pwchName, cchName));	
	SaxTreeElement::EType Elementtype =  spNewElement->GetElementType();


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
			Log_Assert(m_pCurrentNodeInRevisionTree);
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidPointer, SvStl::SourceFileParams(StdMessageParams));

		}
		if(nullptr == m_pCurrentNodeInRevisionTree)
		{
			m_status = PARSING_NONE;
			Log_Assert(m_depth == 0 );
		}
		return S_OK;
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
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidPointer, SvStl::SourceFileParams(StdMessageParams));
			Log_Assert(m_pCurrentNodeInEncryptionTree);
		}

		if(nullptr == m_pCurrentNodeInEncryptionTree)
		{
			SetDecryption();
			m_status = PARSING_NONE;
			Log_Assert(m_depth == 0 );
		}
		return S_OK;
	}
	else if(PARSING_DATATREE == m_status   && SaxTreeElement::DATA_Element == Elementtype )
	{

		SaxTreeElement* pSaxTreeElement = GetCurrentSaxTreeElement();
		if(IsEncrypted())
		{
			m_pSaxEncryptionHandler->DecryptContent(pSaxTreeElement);
		}

		if(nullptr == pSaxTreeElement ||  Elementtype !=  pSaxTreeElement->GetElementType() )
		{
			Log_Assert(false);
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidPointer, SvStl::SourceFileParams(StdMessageParams));
		}

		if(m_ParseArray && pSaxTreeElement)
		{
			///insert last data to the array
			switch (pSaxTreeElement->GetVarTypeAtt())
			{
			case VT_I4:
				{
					if(nullptr == m_spIntVector.get())
					{
						m_spIntVector = std::unique_ptr<std::vector<int>> (new(std::vector<int>));
					}
					int val = _wtoi(pSaxTreeElement->GetContent());
					m_spIntVector->push_back(val); 
					break;
				}
			case VT_UI4:
				{
					if(nullptr == m_spUINTVector.get())
					{
						m_spUINTVector = std::unique_ptr<std::vector<UINT>> (new(std::vector<UINT>));
					}
					int val = _wtoi(pSaxTreeElement->GetContent());
					m_spUINTVector->push_back(val); 
					break;
				}
			case VT_R8:
			{
				if (nullptr == m_spDoubleVector.get())
				{
					m_spDoubleVector = std::unique_ptr<std::vector<double>>(new(std::vector<double>));
				}
				double val = _wtof(pSaxTreeElement->GetContent());
				m_spDoubleVector->push_back(val);
				break;
			}
			case VT_BSTR:
			default:
				{
					Log_Assert(false);
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidArrayElement, SvStl::SourceFileParams(StdMessageParams));
					break;
				}
			}

		}
		else
		{
			/// insert data as new leaf
			_variant_t var;
			VariantHelper::ToVariant(m_spNewElement->GetVarTypeAtt(),m_spNewElement->GetContent(),&var);
			std::string name {_bstr_t(m_spNewElement->GetNameAtt())};
			SVNavigateTree::AddItem( *m_pData_Tree , m_CurrentBranchHandle, name.c_str(), var );
		}

	}
	else if(PARSING_DATATREE == m_status  && SaxTreeElement::NODE_Element == Elementtype)
	{	
		if(m_ParseArray)
		{

			_variant_t variant = GetVariantArray();
			if( VT_I4 == (variant.vt & VT_TYPEMASK)   || VT_UI4 == (variant.vt & VT_TYPEMASK) || VT_R8 == (variant.vt & VT_TYPEMASK) || VT_EMPTY == (variant.vt & VT_TYPEMASK) )
			{
				std::string name {_bstr_t(m_ArrayName.c_str())};
				SVNavigateTree::AddItem( *m_pData_Tree , m_CurrentBranchHandle, name.c_str(), variant );
			}
			else
			{
				Log_Assert(false);
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_XML_InvalidArrayElement, SvStl::SourceFileParams(StdMessageParams));
			}

			m_ParseArray = false;
		}
		else
		{
			m_CurrentBranchHandle = m_pData_Tree->getParentBranch(m_CurrentBranchHandle);
		}
	}

	return S_OK;
}

HRESULT  SaxXMLHandler::OnXMLError(int line, int column, const wchar_t *pwchErrorText, unsigned long errorCode, bool )
{
	SvDef::StringVector messageList;
	messageList.push_back(std::format(_T("{}"), line));
	messageList.push_back(std::format(_T("{}"), column));
	messageList.push_back(std::format(_T("{}"), errorCode));
	messageList.push_back(SvUl::createStdString(pwchErrorText));
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_SVO_83_SAX_PARSER_ERROR, SvStl::Tid_XML_Error, messageList, SvStl::SourceFileParams(StdMessageParams));
	Log_Assert(false);
	return S_OK;
}

DWORD  SaxXMLHandler::GetWriteTime(void ) const
{
	return 0;
}

void SaxXMLHandler::PrintConfigTree(std::wstringstream &myStream, const tcl::sequential_tree<SpTreeElement>& rtree)
{
	SpTreeElement  spElement;

	spElement =  *(rtree.get());
	if(rtree.empty())
	{
		if(spElement.get())
		{
			spElement->PrintElement(myStream);
		}
	}
	else 
	{
		if(spElement.get())
		{
			spElement->PrintElementStart(myStream);
		}
		tcl::sequential_tree<SpTreeElement>::const_node_iterator it = rtree.node_begin();
		for( ; it !=rtree.node_end(); it++ )
		{
			PrintConfigTree(myStream, *it);
		}
		if(spElement.get())
		{
			spElement->PrintElementEnd(myStream);
		}
	}
}

bool SaxXMLHandler::WriteToXML(const wchar_t* path, EPARSING what )
{
	DWORD tick = ::GetTickCount();
	std::wofstream XMLFile;
	std::wstringstream mystream;
	switch(what)
	{
	case PARSING_ENCRYPTION:
		PrintConfigTree(mystream, m_EncryptionTree);
		break;
	case PARSING_REVISION:
		PrintConfigTree(mystream, m_RevisionTree);
		break;
	case PARSING_DATATREE:
		///TODO;

		break;
	}

	m_WriteTime = ::GetTickCount() - tick;

	XMLFile.open( path );
	XMLFile.write(mystream.str().c_str(), mystream.str().size());
	XMLFile.close();

	return true;
}

void SaxXMLHandler::VectorClear(const wchar_t *pName)
{
	m_ArrayName = pName;
	m_spIntVector.reset();
	m_spUINTVector.reset();
	m_spDoubleVector.reset();
	m_spWstringVector.reset();
}

_variant_t  SaxXMLHandler::GetVariantArray()
{
	_variant_t result;

	if(m_spIntVector.get())
	{
		SAFEARRAY* pSafeArray = ::SafeArrayCreateVector(VT_I4, 0, static_cast<ULONG> (m_spIntVector->size()));
		for(long i =0 ; i < static_cast<long>(m_spIntVector->size());i++ )
		{
			if (S_OK != ::SafeArrayPutElement(pSafeArray, &i, static_cast<void*> (&m_spIntVector->at(i))))
			{
				::SafeArrayDestroy(pSafeArray);
				return result;
			}
		}
		result.vt = VT_I4 | VT_ARRAY;
		result.parray = pSafeArray;
	}
	else if(m_spUINTVector.get())
	{
		SAFEARRAY* pSafeArray = ::SafeArrayCreateVector(VT_I4, 0, static_cast<ULONG> (m_spUINTVector->size()));
		for (long i = 0; i < static_cast<long>(m_spUINTVector->size()); i++)
		{
			if (S_OK != ::SafeArrayPutElement(pSafeArray, &i, static_cast<void*> (&m_spUINTVector->at(i))))
			{
				::SafeArrayDestroy(pSafeArray);
				return result;
			}
		}
		result.vt = VT_I4 | VT_ARRAY;
		result.parray = pSafeArray;
	} 
	else if (m_spDoubleVector.get())
	{
		SAFEARRAY* pSafeArray = ::SafeArrayCreateVector(VT_R8, 0, static_cast<ULONG> (m_spDoubleVector->size()));
		for (long i = 0; i < static_cast<long>(m_spDoubleVector->size()); i++)
		{
			if (S_OK != ::SafeArrayPutElement(pSafeArray, &i, static_cast<void*> (&m_spDoubleVector->at(i))))
			{
				::SafeArrayDestroy(pSafeArray);
				return result;
			}
		}
		result.vt = VT_R8 | VT_ARRAY;
		result.parray = pSafeArray;
	}
	return result;
}

bool SaxXMLHandler::IsEncrypted()
{
	return (nullptr != m_pSaxEncryptionHandler && m_pSaxEncryptionHandler->SaxIsEncrypted());
}

void SaxXMLHandler::SetDecryption()
{
	///First Element should be 
	///<Encryption xmlns="x-schema:#SVR00001" IsActive="TRUE">

	if(nullptr == m_pSaxEncryptionHandler)
	{
		return;
	}
	m_pSaxEncryptionHandler->DisableEncryption();

	SpTreeElement  spElement;
	spElement =  *(m_EncryptionTree.get());

	if(nullptr == spElement.get() || !spElement->IsEncryptionBaseNode())
	{
		return;
	}
	if( wcscmp( spElement->GetSpecialAtt(L"xmlns"), L"x-schema:#SVR00001") != 0)
	{
		return;
	}
	if( wcscmp( spElement->GetSpecialAtt(L"IsActive"), L"TRUE") != 0)
	{
		return;
	}
	if(m_EncryptionTree.empty())
	{
		return;
	}
	tcl::sequential_tree<SpTreeElement>::const_node_iterator it = m_EncryptionTree.node_begin();
	for( ; it !=m_EncryptionTree.node_end(); it++ )
	{
		spElement =  *(it->get());
	}
	if(nullptr == spElement.get() ||  spElement->GetElementType()  !=  SaxTreeElement::DATA_Element )
	{
		return;
	}
	m_pSaxEncryptionHandler->SaxSetEncryption(spElement->GetNameAtt(), spElement->GetContent());

	return;
}

void convertXmlToTree(const std::string& rXmlData, SvXml::SVXMLMaterialsTree& rTree)
{
	SvXml::SaxXMLHandler  SaxHandler;
	HRESULT	Result = SaxHandler.BuildFromXMLString(&rTree, _variant_t(rXmlData.c_str()));
	if (false == SUCCEEDED(Result))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_XML_To_Tree_Failed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
}
} //namespace SvXml
