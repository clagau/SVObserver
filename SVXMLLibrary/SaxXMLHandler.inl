//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// class SVSaxXMLHandler
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "SaxXMLHandler.h"
#include "SaxEncryptionHandler.h"
#include "SVXMLLibrary\SVNavigateTree.h"
#include "SVXMLLibrary\VariantHelper.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include <sequential_tree.h>
#pragma endregion Includes 

namespace Seidenader { namespace  SVXMLLibrary
{
	template <typename TreeType>
	SaxXMLHandler<TreeType>::SaxXMLHandler(void): 
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

	template<typename TreeType>
	void SaxXMLHandler<TreeType>::Clear()
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

	template<typename TreeType>
	SaxXMLHandler<TreeType>::~SaxXMLHandler(void)
	{
		Clear();
	}

	template<typename TreeType>
	void SaxXMLHandler<TreeType>::SetSaxEncryptionHandler(SaxEncryptionHandler *pSaxEncryptionHandler)
	{
		m_pSaxEncryptionHandler = pSaxEncryptionHandler;
	}

	template<typename TreeType>
	HRESULT SaxXMLHandler<TreeType>::BuildFromXMLString(TreeType*  dataTree , const _variant_t &var)
	{
		m_pData_Tree = dataTree;
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

	template<typename TreeType>
	HRESULT  SaxXMLHandler<TreeType>::BuildFromXMLFile(TreeType*  dataTree , const wchar_t * pwstrPath)
	{
		m_pData_Tree = dataTree;
		if( !m_SaxParser.IsReady() )
		{
			return E_FAIL;
		}
		Clear();

		HRESULT hr(E_FAIL);
		try
		{
			hr =   m_SaxParser.ParseFile(pwstrPath);
		}
		catch ( const SvStl::MessageContainer&  )
		{
			// if a exception occurs E_FAil is returned.
			// exception are only thrown and logged in in VariantHelper.;
		}


		return hr;
	}

	template<typename TreeType>
	DWORD  SaxXMLHandler<TreeType>::GetParseTime(void ) const
	{
		return m_SaxParser.GetParseTime();
	}

	template<typename TreeType>
	HRESULT  SaxXMLHandler<TreeType>::OnStartElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchName,  int cchName,ISAXAttributes *pAttributes,int depth ) 
	{
		m_spNewElement.reset();
		m_spNewElement = SpTreeElement( new SaxTreeElement(pwchName, cchName));	
		int NoOfAt  = m_spNewElement->SetAttributes(pAttributes);
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
				ASSERT(m_RevisionTree.is_root()); 
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
				ASSERT(m_EncryptionTree.is_root()); 
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
							typename TreeType::SVBranchHandle  newBranchHandle;
							SVNavigateTree::AddBranch( *m_pData_Tree , m_CurrentBranchHandle, CW2CT(m_spNewElement->GetNameAtt()), &newBranchHandle);
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
						std::stringstream Errorstream;
						Errorstream << L"Unexpected TreeElementType in OnStartElement";
						SvStl::MessageContainer Exception;
						Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR,Errorstream.str().c_str(), StdMessageParams,SvOi::Err_16067_OnStartElement  );
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

	template<typename TreeType>
	SaxTreeElement* SaxXMLHandler<TreeType>::GetCurrentSaxTreeElement()
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

	template<typename TreeType>
	HRESULT  SaxXMLHandler<TreeType>::OnElementData( const wchar_t *pwchData,  int cchData, int depth) 
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
				std::stringstream Errorstream;
				Errorstream << L"Unexpected OnElementData";
				SvStl::MessageContainer Exception;
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR,Errorstream.str().c_str(), StdMessageParams,SvOi::Err_16068_OnElementData  );
				throw Exception;
			}

		}
		return S_OK;
	}

	template<typename TreeType>
	HRESULT  SaxXMLHandler<TreeType>::OnEndElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri,const wchar_t *pwchName,  int cchName, int depth )
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
				ASSERT(m_pCurrentNodeInRevisionTree);
				std::string msg = "Invalid Pointer";
				SvStl::MessageMgrNoDisplay Exception(SvStl::LogOnly);
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, msg.c_str(), StdMessageParams, SvOi::Err_16069_INVALIDPOINTER );

			}
			if(nullptr == m_pCurrentNodeInRevisionTree)
			{
				m_status = PARSING_NONE;
				ASSERT(m_depth == 0 );
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
				std::string msg = "Invalid Pointer";
				SvStl::MessageMgrNoDisplay Exception(SvStl::LogOnly);
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, msg.c_str(), StdMessageParams, SvOi::Err_16070_INVALIDPOINTER );
				ASSERT(m_pCurrentNodeInEncryptionTree);
			}

			if(nullptr == m_pCurrentNodeInEncryptionTree)
			{
				SetDecryption();
				m_status = PARSING_NONE;
				ASSERT(m_depth == 0 );
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
				ASSERT(false);
				std::string msg = "Invalid Pointer";
				SvStl::MessageMgrNoDisplay Exception(SvStl::LogOnly);
				Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, msg.c_str(), StdMessageParams, SvOi::Err_16071_INVALIDPOINTER );
			}

			if(m_ParseArray && pSaxTreeElement)
			{
				///insert last data to the array
				switch (pSaxTreeElement->GetVarTypeAtt())
				{
				case VT_I4:
					{
						if(m_spIntVector.get() == nullptr)
						{
							m_spIntVector = std::unique_ptr<std::vector<int>> (new(std::vector<int>));
						}
						int val = _wtoi(pSaxTreeElement->GetContent());
						m_spIntVector->push_back(val); 
						break;
					}
				case VT_UI4:
					{
						if(m_spUINTVector.get() == nullptr)
						{
							m_spUINTVector = std::unique_ptr<std::vector<UINT>> (new(std::vector<UINT>));
						}
						int val = _wtoi(pSaxTreeElement->GetContent());
						m_spUINTVector->push_back(val); 
						break;
					}
				case VT_BSTR:
				default:
					{
						ASSERT(false);
						std::string msg = "Invalid ArrayElement in OnEndElement";
						SvStl::MessageMgrNoDisplay Exception(SvStl::LogOnly);
						Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, msg.c_str(), StdMessageParams, SvOi::Err_16074_InvalidArrayElement );

						break;
					}
				}

			}
			else
			{
				/// insert data as new leaf
				_variant_t var;
				VariantHelper::ToVariant(m_spNewElement->GetVarTypeAtt(),m_spNewElement->GetContent(),&var);
				SVNavigateTree::AddItem( *m_pData_Tree , m_CurrentBranchHandle, CW2CT(m_spNewElement->GetNameAtt()), var );
			}

		}
		else if(PARSING_DATATREE == m_status  && SaxTreeElement::NODE_Element == Elementtype)
		{	
			if(m_ParseArray)
			{

				_variant_t variant = GetVariantArray();
				if( VT_I4 == (variant.vt & VT_TYPEMASK)   || VT_UI4 == (variant.vt & VT_TYPEMASK)   || VT_EMPTY == (variant.vt & VT_TYPEMASK) )
				{

					SVNavigateTree::AddItem( *m_pData_Tree , m_CurrentBranchHandle, CW2CT(m_ArrayName.c_str()), variant );
				}
				else
				{
					ASSERT(false);
					std::string msg = "Invalid ArrayElement in OnEndElement";
					SvStl::MessageMgrNoDisplay Exception(SvStl::LogOnly);
					Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, msg.c_str(), StdMessageParams, SvOi::Err_16072_INVALID_ARRAYELEMENT );
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




	template<typename TreeType>
	HRESULT  SaxXMLHandler<TreeType>::OnXMLError(int line, int column, const wchar_t *pwchErrorText, unsigned long errorCode, bool fatal )
	{
		CString csError(pwchErrorText);
		CString  csMessage;
		csMessage.Format("Error in Line, Column, Errornumber  : %i , %i , %i  : %s ",line, column,errorCode, csError.GetString()   );
		SvStl::MessageMgrNoDisplay Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_SVO_83_SAX_PARSER_ERROR, csMessage, StdMessageParams, SvOi::Err_16063_SAXPARSER );
		ASSERT(false);
		return S_OK;

	}

	template<typename TreeType>
	DWORD  SaxXMLHandler<TreeType>::GetWriteTime(void ) const
	{
		return 0;
	}

	template<typename TreeType>
	void SaxXMLHandler<TreeType>::PrintConfigTree(std::wstringstream &myStream, const tcl::sequential_tree<SpTreeElement>& rtree)
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

	template<typename TreeType>
	bool SaxXMLHandler<TreeType>::WriteToXML(const wchar_t* path, EPARSING what )
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
		XMLFile << mystream.str();
		XMLFile.close();

		return true;
	}

	template<typename TreeType>
	void SaxXMLHandler<TreeType>::VectorClear(const wchar_t *pName)
	{
		m_ArrayName = pName;
		m_spIntVector.release();
		m_spUINTVector.release();
		m_spWstringVector.release();
	}

	template<typename TreeType>
	_variant_t  SaxXMLHandler<TreeType>::GetVariantArray()
	{
		COleSafeArray saRet;
		if(m_spIntVector.get())
		{
			saRet.CreateOneDim(VT_I4,static_cast<DWORD>(m_spIntVector->size()));

			for(long i =0 ; i < static_cast<long>(m_spIntVector->size());i++ )
			{
				saRet.PutElement(&i,&(m_spIntVector->at(i)));
			}
		}
		else if(m_spUINTVector.get())
		{
			saRet.CreateOneDim(VT_I4,static_cast<DWORD>(m_spUINTVector->size()));

			for(long i =0 ; i < static_cast<long>(m_spUINTVector->size());i++ )
			{
				saRet.PutElement(&i,&(m_spUINTVector->at(i)));
			}
		} 
		return saRet.Detach();
	}


	template<typename TreeType>
	bool SaxXMLHandler<TreeType>::IsEncrypted()
	{
		return (nullptr != m_pSaxEncryptionHandler && m_pSaxEncryptionHandler->SaxIsEncrypted());
	}

	template<typename TreeType>
	void SaxXMLHandler<TreeType>::SetDecryption()
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

} /* namespace SVXMLLibrary */ } /* namespace Seidenader */
