//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityStorage.cpp
//* .File Name       : $Workfile:   SVSecurityStorage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:04:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVSecurityStorage.h"

#include "SVMessage/SVMessage.h"
#include "SVXMLLibrary/SVXMLClass.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

#include "SVSecurity.h"

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
	int iSize = m_aNodes.size();
	for ( int i = 0 ; i < iSize ; i++ )
	{
		if( m_aNodes[ i ].m_lID == lId)
			return &(m_aNodes[ i ] );
	}
	return NULL;
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

	BSTR l_bstrFormat = NULL;
	BSTR l_bstrFileName = NULL;

	SVMaterialsTree l_Tree;
	SVXMLMaterialsTree l_XMLTree( l_Tree );
	SVXMLClass svX;

	hr = S_OK;

	while (1)
	{
		hr = GetMaterialsTree( l_Tree );
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = svX.Initialize(0,  // do not use checksums
							0,  // do not use program version/data revision info
							1); // use encryption
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		l_bstrFormat = ::SysAllocString(L"Security File");
		if( l_bstrFormat == NULL )
		{
			hr = E_FAIL;
			break;
		}

		l_bstrFileName = ::SysAllocString(T2W(pFileName));
		if( l_bstrFileName == NULL )
		{
			hr = E_FAIL;
			break;
		}

		hr = svX.CopyTreeToXMLFile(l_XMLTree, 1, l_bstrFormat, NULL, l_bstrFileName);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
	SVMaterialsTree l_Tree;
	SVXMLMaterialsTree l_XMLTree( l_Tree );
	SVXMLClass svX;

	HRESULT hr = svX.Initialize();
	
	CString sFormat;
	CString sFileName;
	sFormat.Format(_T("Security File"));
	sFileName.Format(pFileName);
	BSTR bstRevHist = NULL;
	BSTR bstChangeHist = NULL;

	BSTR l_bstName = sFileName.AllocSysString();
	hr = svX.CopyXMLFileToTree(l_XMLTree, 1, l_bstName, &bstRevHist , &bstChangeHist );

	SysFreeString( l_bstName );
	SysFreeString( bstRevHist );
	SysFreeString( bstChangeHist );	

	if( hr == S_OK )
	{
		SVAccessPointNodeVectorArray l_aNodes;

		hr = ProcessMaterialsTree( l_aNodes, l_Tree );

		if( hr == S_OK )
		{
			long l_initsize = m_aNodes.size();
			long l_filesize = l_aNodes.size();

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

			hr = ProcessRoot( l_Tree );
		}
	}

	return hr;
}

HRESULT SVSecurityStorage::GetMaterialsTree( SVMaterialsTree& p_rMaterialsTree ) const
{
	HRESULT l_Status( S_OK );

	p_rMaterialsTree.Clear();

	if( m_aNodes.size() != 0 )
	{
		SVMaterialsTree::iterator l_RootIter( p_rMaterialsTree.end() );

		if( l_Status == S_OK )
		{
			SVMaterials l_Materials;

			l_Status = l_Materials.UpdateMaterial( _T("Use Logon"), GetUseLogon() );

			if( l_Status == S_OK )
			{
				l_Status = l_Materials.UpdateMaterial( _T("Logon Timeout"), GetUserTimeout() );
			}

			if( l_Status == S_OK )
			{
				l_Status = l_Materials.UpdateMaterial( _T("Current User"), _variant_t( GetCurrentUser() ) );
			}

			if( l_Status == S_OK )
			{
				l_Status = l_Materials.UpdateMaterial( _T("Current PW"), _variant_t( GetCurrentPassword() ) );
			}

			if( l_Status == S_OK )
			{
				l_Status = l_Materials.UpdateMaterial( _T("Use Auto Edit"), GetAutoEdit() );
			}

			if( l_Status == S_OK )
			{
				p_rMaterialsTree.set( SVMaterialsTree::SVTreeElement( _T( "Root" ), l_Materials ) );
			}
		}

		if( l_Status == S_OK )
		{
			SVAccessPointNodeVectorArray::const_iterator l_NodeIter( m_aNodes.begin() );

			while( l_Status == S_OK && l_NodeIter != m_aNodes.end() )
			{
				if( !l_NodeIter->m_bHasData )
				{
					SVMaterials l_NodeMaterials;
					SVString l_Name( l_NodeIter->m_strName );
					SVMaterialsTreeAdapter::SVMaterialsElement l_Element( l_Name, l_NodeMaterials );
					SVMaterialsTreeAdapter::iterator l_Iter( p_rMaterialsTree.insert( l_Element, p_rMaterialsTree.end() ) );

					if( l_Iter != p_rMaterialsTree.end() && l_Iter.GetChildTree() != NULL )
					{
						SVMaterialsTreeAdapter l_ChildTree( *( l_Iter.GetChildTree() ) );

						l_Status = GetChildMaterialsTree( l_ChildTree, ++l_NodeIter );
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

HRESULT SVSecurityStorage::GetChildMaterialsTree( SVMaterialsTreeAdapter& p_rMaterialsTree, SVAccessPointNodeVectorArray::const_iterator& p_rNodeIter ) const
{
	HRESULT l_Status( S_OK );

	while( l_Status == S_OK && p_rNodeIter != m_aNodes.end() && p_rNodeIter->m_bHasData )
	{
		SVMaterials l_NodeMaterials;

		l_Status = l_NodeMaterials.UpdateMaterial( _T( "Force Prompt" ), p_rNodeIter->m_bForcePrompt );

		if( l_Status == S_OK )
		{
			l_Status = l_NodeMaterials.UpdateMaterial( _T( "NT Group" ), _variant_t( p_rNodeIter->m_strNTGroup ) );
		}

		if( l_Status == S_OK )
		{
			l_Status = l_NodeMaterials.UpdateMaterial( _T( "ID" ), p_rNodeIter->m_lID );
		}

		if( l_Status == S_OK )
		{
			SVString l_Name( p_rNodeIter->m_strName );
			SVMaterialsTreeAdapter::SVMaterialsElement l_Element( l_Name, l_NodeMaterials );
			SVMaterialsTreeAdapter::iterator l_Iter( p_rMaterialsTree.insert( l_Element, p_rMaterialsTree.end() ) );

			if( l_Iter != p_rMaterialsTree.end() )
			{
				++p_rNodeIter;
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	return l_Status;
}

HRESULT SVSecurityStorage::ProcessMaterialsTree( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTree& p_rMaterialsTree )
{
	HRESULT l_Status( S_OK );

	p_rNewArray.clear();

	SVMaterialsTree& l_Tree( const_cast< SVMaterialsTree& >( p_rMaterialsTree ) );

	SVMaterialsTree::iterator l_Iter( l_Tree.begin() );

	while( l_Status == S_OK && l_Iter != l_Tree.end() && l_Iter.GetChildTree() != NULL )
	{
		SVMaterialsTreeAdapter l_ChildTree( *( l_Iter.GetChildTree() ) );

		l_Status = ProcessChild( p_rNewArray, l_ChildTree );

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVSecurityStorage::ProcessChild( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTreeAdapter& p_rMaterialsTree )
{
	HRESULT l_Status( S_OK );

	SVMaterialsTreeAdapter& l_Tree( const_cast< SVMaterialsTreeAdapter& >( p_rMaterialsTree ) );

	if( l_Tree.get() != NULL )
	{
		SVString l_Name( l_Tree.get()->first );
		const SVMaterials& l_rMaterials( l_Tree.get()->second );

		_variant_t l_ID;
		_variant_t l_ForcePrompt;
		_variant_t l_NTGroup;

		if( l_rMaterials.GetMaterial( _T( "ID" ), l_ID ) == S_OK &&
			l_rMaterials.GetMaterial( _T( "Force Prompt" ), l_ForcePrompt ) == S_OK &&
			l_rMaterials.GetMaterial( _T( "NT Group" ), l_NTGroup ) == S_OK )
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

	SVMaterialsTreeAdapter::iterator l_Iter( l_Tree.begin() );

	while( l_Status == S_OK && l_Iter != l_Tree.end() )
	{
		if( l_Iter.GetChildTree() != NULL )
		{
			SVMaterialsTreeAdapter l_ChildTree( *( l_Iter.GetChildTree() ) );

			l_Status = ProcessChild( p_rNewArray, l_ChildTree );
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVSecurityStorage::ProcessRoot( const SVMaterialsTree& p_rMaterialsTree )
{
	HRESULT l_Status( S_OK );

	SVMaterialsTree& l_Tree( const_cast< SVMaterialsTree& >( p_rMaterialsTree ) );

	if( l_Tree.get() != NULL )
	{
		_variant_t l_Data;

		const SVMaterials& l_rMaterials( l_Tree.get()->second );

		l_Status = l_rMaterials.GetMaterial( _T( "Use Logon" ), l_Data );

		if( l_Status == S_OK )
		{
			l_Status = SetUseLogon( l_Data );
		}

		if( l_Status == S_OK )
		{
			l_Status = l_rMaterials.GetMaterial( _T( "Logon Timeout" ), l_Data );

			if( l_Status == S_OK )
			{
				l_Status = SetUserTimeout( l_Data );
			}
		}

		if( l_Status == S_OK )
		{
			l_Status = l_rMaterials.GetMaterial( _T( "Current User" ), l_Data );

			if( l_Status == S_OK )
			{
				m_strCurrentUser = l_Data;
			}
		}

		if( l_Status == S_OK )
		{
			l_Status = l_rMaterials.GetMaterial( _T( "Current PW" ), l_Data );

			if( l_Status == S_OK )
			{
				m_strCurrentPW = l_Data;
			}
		}

		if( l_Status == S_OK )
		{
			l_Status = l_rMaterials.GetMaterial( _T( "Use Auto Edit" ), l_Data );

			if( l_Status == S_OK )
			{
				l_Status = SetUseAutoEdit( l_Data );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecurityStorage.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:04:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   11 Oct 2012 10:08:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to reflect fixing issues with XML functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   04 Oct 2012 08:43:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   25 Sep 2012 13:12:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated materials calls to use the new update method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Sep 2012 18:43:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   15 Dec 2009 10:55:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   30 Oct 2007 14:39:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Load and Save to free BSTRs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Jun 2007 09:25:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   23 Mar 2006 08:50:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  558
 * SCR Title:  Expand Code in SVSecurity DLL
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ClearPW and ClearUser.
 * Added static_cast<long>(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   20 Sep 2005 10:56:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified security save function to use encryption.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Aug 2005 11:58:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Load Bug by copying only data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Aug 2005 08:29:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Load so the security access points load based on ID not Name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   06 Jul 2005 13:49:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jun 2005 15:19:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Global settings assignment in load( function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jun 2005 14:13:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   General Cleanup commented code ect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2005 09:21:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSecurityStorageClass to use stl instead of the Microsoft CArray.  Fixed Load problem with SecurityStorage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:10:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
