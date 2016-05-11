//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityStorage.cpp
//* .File Name       : $Workfile:   SVSecurityStorage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:00:48  $
//******************************************************************************

#include "stdafx.h"
#import <msxml6.dll> // MSXML
#include "SVSecurityStorage.h"

#include "SVMessage/SVMessage.h"
#include "SVXMLLibrary/SVXMLClass.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVSecurity.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVXMLLibrary/SaxEncryptionHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Jims NOT REALY TRUE OR FALSE return values for HRESULT.
const long SV_IT_DOES = S_OK;
const long SV_IT_DOES_NOT = S_FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVSecurityStorage::SVSecurityStorage()
{
	m_lCurrentIndex = 0;
	m_lCurrentData = 0;
	m_bAutoEdit = false;
	m_bUseLogon = false;
	m_lTimeout = 0;
	m_strCurrentPW = _T("");
	m_strCurrentUser = _T("");
}

SVSecurityStorage::~SVSecurityStorage()
{

}

HRESULT SVSecurityStorage::Add( long lID, LPCTSTR sName, LPCTSTR sNTGroup, bool bForcePrompt )
{
	try
	{
		m_aNodes.push_back( SVAccessPointNode( lID, sName, sNTGroup, bForcePrompt ) );
	}
	catch(...)
	{
		return -3013;
	}
	return S_OK;
}

HRESULT SVSecurityStorage::Add( long lID, LPCTSTR sName)
{
	try
	{
		m_aNodes.push_back( SVAccessPointNode( lID, sName));
	}
	catch(...)
	{
		return -3015;
	}
	return S_OK;
}


// Find by Id starts at the root of the tree.
SVAccessPointNode* SVSecurityStorage::FindByID( long lId )
{
	size_t iSize = m_aNodes.size();
	for ( size_t i = 0 ; i < iSize ; i++ )
	{
		if( m_aNodes[ i ].m_lID == lId)
			return &(m_aNodes[ i ] );
	}
	return nullptr;
}

void SVSecurityStorage::SetUser( LPCTSTR strUser )
{
	m_strCurrentUser = strUser;
}

void SVSecurityStorage::ClearUser( )
{
	m_strCurrentUser.Empty();
}

void SVSecurityStorage::SetPW( LPCTSTR strPW )
{
	m_strCurrentPW = strPW;
}

void SVSecurityStorage::ClearPW( )
{
	m_strCurrentPW.Empty();
}

CString SVSecurityStorage::GetCurrentUser() const
{
	return m_strCurrentUser;
}

CString SVSecurityStorage::GetCurrentPassword() const
{
	return m_strCurrentPW;
}

HRESULT SVSecurityStorage::Save( LPCTSTR pFileName )
{
USES_CONVERSION;

	HRESULT	hr;

	BSTR l_bstrFormat = nullptr;
	BSTR l_bstrFileName = nullptr;

	SvXml::SVXMLMaterialsTree XMLTree ;
	SVXMLClass svX;

	hr = S_OK;

	while (1)
	{
		hr = GetMaterialsTree( XMLTree.getTree() );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = svX.Initialize(0,  // do not use checksums
							0,  // do not use program version/data revision info
							1); // use encryption
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		l_bstrFormat = ::SysAllocString(L"Security File");
		if( nullptr == l_bstrFormat )
		{
			hr = E_FAIL;
			break;
		}

		l_bstrFileName = ::SysAllocString(T2W(pFileName));
		if( nullptr == l_bstrFileName )
		{
			hr = E_FAIL;
			break;
		}

		hr = svX.CopyTreeToXMLFile(XMLTree, 1, l_bstrFormat, nullptr, l_bstrFileName);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		break;
	}

	SysFreeString( l_bstrFileName );
	SysFreeString( l_bstrFormat );

	return hr;
}


HRESULT SVSecurityStorage::SetUseLogon( bool bUse )
{
	m_bUseLogon = bUse;
	return S_OK;
}

HRESULT SVSecurityStorage::SetUserTimeout(long lTime)
{
	m_lTimeout = lTime >= 0 ? lTime : 0 ;
	return S_OK;
}

long SVSecurityStorage::GetUserTimeout() const
{
	return m_lTimeout;
}

bool SVSecurityStorage::GetUseLogon() const
{
	return m_bUseLogon;
}

bool SVSecurityStorage::GetAutoEdit() const
{
	return m_bAutoEdit;
}

HRESULT SVSecurityStorage::SetUseAutoEdit( bool p_bAutoEdit )
{
	m_bAutoEdit = p_bAutoEdit;
	return S_OK;
}

HRESULT SVSecurityStorage::Load( LPCTSTR pFileName )
{
	
	SvXml::SaxEncryptionHandler EncyptionHandler;
	SvXml::SVXMLMaterialsTree TestTree;
	SvXml::SVXMLMaterialsTree XMLTree;
	SvXml::SaxXMLHandler<SvXml::SVXMLMaterialsTree>  SaxHandler;
	SaxHandler.SetSaxEncryptionHandler(&EncyptionHandler);

	CA2W  pwXmlFilename(pFileName); 
	HRESULT hr  = SaxHandler.BuildFromXMLFile(&XMLTree,  pwXmlFilename );	
	
	if( S_OK == hr )
	{
		SVAccessPointNodeVectorArray l_aNodes;

		hr = ProcessMaterialsTree( l_aNodes, XMLTree.getTree() );

		if( S_OK == hr )
		{
			SVAccessPointNodeVectorArray::iterator iterInitNodes;
			for( iterInitNodes = m_aNodes.begin() ; iterInitNodes != m_aNodes.end() ; ++iterInitNodes )
			{
				SVAccessPointNodeVectorArray::iterator iterFileNodes;
				for( iterFileNodes = l_aNodes.begin() ; iterFileNodes != l_aNodes.end() ; ++iterFileNodes )
				{
					// If the IDs are the same then copy the data unless it is the securityManager which cannot be changed.
					if( (*iterInitNodes).m_lID == (*iterFileNodes).m_lID  &&
						(*iterInitNodes).m_lID != SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER )
					{
						(*iterInitNodes).CopyData( *iterFileNodes ); 
					}
				}
			}

			hr = ProcessRoot( XMLTree.getTree() );
		}
	}

	return hr;
}

HRESULT SVSecurityStorage::GetMaterialsTree( SVMaterialsTree::SVTreeContainer& rTree ) const
{
	HRESULT l_Status( S_OK );

	rTree.clear();

	if( m_aNodes.size() != 0 )
	{
		if( S_OK == l_Status )
		{
			rTree.set( SVMaterialsTree::SVTreeElement( _T( "Root" ), SVMaterialDataPtr( nullptr ) ) );

			rTree.insert( SVMaterialsTree::SVTreeElement( SVString( _T("Use Logon") ), new SVMaterialData( _variant_t( GetUseLogon() ) ) ) );
			rTree.insert( SVMaterialsTree::SVTreeElement( SVString( _T("Logon Timeout") ), new SVMaterialData( _variant_t( GetUserTimeout() ) ) ) );
			rTree.insert( SVMaterialsTree::SVTreeElement( SVString( _T("Current User") ), new SVMaterialData( _variant_t( GetCurrentUser() ) ) ) );
			rTree.insert( SVMaterialsTree::SVTreeElement( SVString( _T("Current PW") ), new SVMaterialData( _variant_t( GetCurrentPassword() ) ) ) );
			rTree.insert( SVMaterialsTree::SVTreeElement( SVString( _T("Use Auto Edit") ), new SVMaterialData( _variant_t( GetAutoEdit() ) ) ) );
		}

		if( S_OK == l_Status )
		{
			SVAccessPointNodeVectorArray::const_iterator l_NodeIter( m_aNodes.begin() );

			while( S_OK == l_Status && l_NodeIter != m_aNodes.end() )
			{
				if( !l_NodeIter->m_bHasData )
				{
					SVString l_Name( l_NodeIter->m_strName );
					SVMaterialsTree::SVTreeElement Element( l_Name, SVMaterialDataPtr(nullptr) );
					SVMaterialsTree::iterator l_Iter( rTree.insert( Element ) );

					if( l_Iter != rTree.end() && nullptr != l_Iter.node() )
					{
						l_Status = GetChildMaterialsTree( *l_Iter.node(), ++l_NodeIter );
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVSecurityStorage::GetChildMaterialsTree( SVMaterialsTree::SVTreeContainer& rTree, SVAccessPointNodeVectorArray::const_iterator& p_rNodeIter ) const
{
	HRESULT l_Status( S_OK );

	while( S_OK == l_Status && p_rNodeIter != m_aNodes.end() && p_rNodeIter->m_bHasData )
	{
		SVMaterialsTree::SVTreeContainer* pNode( nullptr );
		if( S_OK == l_Status )
		{
			SVString l_Name( p_rNodeIter->m_strName );
			SVMaterialsTree::SVTreeElement Element( l_Name, SVMaterialDataPtr(nullptr) );
			SVMaterialsTree::iterator Iter( rTree.insert( Element ) );

			if( rTree.end() != Iter )
			{
				pNode = Iter.node();
			}
			else
			{
				l_Status = E_FAIL;
			}

		}
		if( nullptr != pNode )
		{
			pNode->insert( SVMaterialsTree::SVTreeElement( SVString( _T("Force Prompt") ), new SVMaterialData( _variant_t( p_rNodeIter->m_bForcePrompt ) ) ) );
			pNode->insert( SVMaterialsTree::SVTreeElement( SVString( _T("NT Group") ), new SVMaterialData( _variant_t( p_rNodeIter->m_strNTGroup ) ) ) );
			pNode->insert( SVMaterialsTree::SVTreeElement( SVString( _T("ID") ), new SVMaterialData( _variant_t( p_rNodeIter->m_lID ) ) ) );
		}
		
		++p_rNodeIter;
	}

	return l_Status;
}

HRESULT SVSecurityStorage::ProcessMaterialsTree( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTree::SVTreeContainer& rTree )
{
	HRESULT l_Status( S_OK );

	p_rNewArray.clear();

	SVMaterialsTree::const_iterator Iter( rTree.begin() );

	while( S_OK == l_Status && Iter != rTree.end() && nullptr != Iter.node() )
	{
		//Process only nodes not leaves
		if( Iter->second.empty() )
		{
			l_Status = ProcessChild( p_rNewArray, *Iter.node() );
		}

		++Iter;
	}

	return l_Status;
}

HRESULT SVSecurityStorage::ProcessChild( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTree::SVTreeContainer& rTree )
{
	HRESULT l_Status( S_OK );

	if( nullptr != rTree.get() )
	{
		SVString l_Name( rTree.get()->first );

		SVMaterialsTree::ElementData Value;
		_variant_t l_ID;
		_variant_t l_ForcePrompt;
		_variant_t l_NTGroup;
		if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("ID") ), Value ) )
		{
			l_ID = Value;
		}
		Value.clear();
		if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("Force Prompt") ), Value ) )
		{
			l_ForcePrompt = Value;
		}
		Value.clear();
		if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("NT Group") ), Value ) )
		{
			l_NTGroup = Value;
		}
		if( VT_EMPTY != l_ID.vt && VT_EMPTY != l_ForcePrompt.vt && VT_EMPTY != l_NTGroup.vt )
		{
			SVAccessPointNode l_Node( l_ID, l_Name.c_str(), static_cast< _bstr_t >( l_NTGroup ), l_ForcePrompt );

			p_rNewArray.push_back( l_Node );
		}
		else
		{
			SVAccessPointNode l_Node;

			l_Node.m_strName = l_Name.c_str();

			p_rNewArray.push_back( l_Node );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	SVMaterialsTree::const_iterator l_Iter( rTree.begin() );

	while( S_OK == l_Status && l_Iter != rTree.end() )
	{
		//Process only nodes not leaves
		if( nullptr != l_Iter.node() && l_Iter->second.empty() )
		{
			l_Status = ProcessChild( p_rNewArray, *l_Iter.node() );
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVSecurityStorage::ProcessRoot( const SVMaterialsTree::SVTreeContainer& rTree )
{
	HRESULT l_Status( S_OK );

	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;

	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("Use Logon") ), MaterialData ) )
	{
		Value = MaterialData;
		l_Status = SetUseLogon( Value );
	}
		
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("Logon Timeout") ), MaterialData ) )
	{
		Value = MaterialData;
		l_Status = SetUserTimeout( Value );
	}

	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("Current User") ), MaterialData ) )
	{
		Value = MaterialData;
		m_strCurrentUser = Value;
	}

	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("Current PW") ), MaterialData ) )
	{
		Value = MaterialData;
		m_strCurrentPW = Value;
	}

	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rTree, SVString( _T("Use Auto Edit") ), MaterialData ) )
	{
		Value = MaterialData;
		l_Status = SetUseAutoEdit( Value );
	}

	return l_Status;
}

