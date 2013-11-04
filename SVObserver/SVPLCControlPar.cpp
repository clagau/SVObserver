//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCControlPar
//* .File Name       : $Workfile:   SVPLCControlPar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:23:44  $
//******************************************************************************

#include "stdafx.h"
#include <comdef.h>
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVPLCControlPar.h"
#include "SVObserver.h"

SVPLCControlPar::SVPLCControlPar(void)
:m_lQueueSize(2)
,m_lBlockCount(0)
,m_lBlockSize(1)
,m_bCollapseView(false)
,m_lStartingAddressCounter(0)
{
}

SVPLCControlPar::~SVPLCControlPar(void)
{
}

void SVPLCControlPar::Destroy()
{
	SVPLCOutputObjectList::iterator l_it;
	for( l_it = m_PLCOutputs.begin() ; l_it != m_PLCOutputs.end(); ++l_it )
	{
		delete (*l_it);
	}
	m_PLCOutputs.clear();
	m_lBlockSize = 0;
}


// Parameters >> Tree ( Save )
BOOL SVPLCControlPar::GetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle htiParent ) const
{
	BOOL bOk = TRUE;

	_variant_t svVariant;

	svVariant = m_lBlockCount;
	SVNavigateTreeClass::AddItem( p_rTree, htiParent, CTAG_PLC_BLOCK_SIZE, svVariant );
	svVariant.Clear();

	svVariant = m_lQueueSize;
	SVNavigateTreeClass::AddItem( p_rTree, htiParent, CTAG_PLC_QUEUE_SIZE, svVariant );
	svVariant.Clear();

	// PLC elements 
	for( size_t i = 0 ; i < m_PLCOutputs.size(); i++ )
	{
		SVTreeType::SVBranchHandle htiPLCEntry = NULL;
		CString l_strBranch;
		l_strBranch.Format( "%s_%d",CTAG_PLC_ENTRY, i + 1 );

		if ( SVNavigateTreeClass::SetBranch( p_rTree, htiParent, l_strBranch, &htiPLCEntry ) )
		{
			m_PLCOutputs[i]->GetParameters( p_rTree, htiPLCEntry );
		}
	}

	// Save the Array of starting Addresses for use in Block writing.
	ASSERT( m_lBlockCount == m_strBlockAddresses.size() );
	for( size_t i = 0 ; i < m_strBlockAddresses.size(); i++ )
	{
		CString l_strBlockId;
		l_strBlockId.Format( "%s_%d",CTAG_PLC_BLOCK_START, i + 1 );
		svVariant = m_strBlockAddresses[i];
		SVNavigateTreeClass::AddItem( p_rTree, htiParent, l_strBlockId, svVariant );
		svVariant.Clear();
	}

	return bOk;
}

// Sets the SVMaterialsTree with data from this class.
HRESULT SVPLCControlPar::GetMaterials( SVMaterialsTreeAdapter& p_rMaterialsTree, SVMaterialsTreeAdapter::SVTreeContainer* p_pParent )
{
	_variant_t l_vVariant;

	SVMaterials l_Materials;

	l_vVariant = m_lBlockCount ;
	HRESULT l_hr = l_Materials.UpdateMaterial( CTAG_PLC_BLOCK_SIZE, l_vVariant );
	l_vVariant.Clear();

	l_vVariant = m_lQueueSize ;
	KeepPrevError( l_hr, l_Materials.UpdateMaterial( CTAG_PLC_QUEUE_SIZE, l_vVariant ));
	l_vVariant.Clear();

	// PLC elements 
	for( size_t i = 0 ; i < m_PLCOutputs.size(); i++ )
	{
		SVString l_strBranch;
		l_strBranch.Format( "%s_%d",CTAG_PLC_ENTRY, i + 1 );

		SVMaterialsTreeAdapter::SVTreeContainer* l_pBranch;
		// This code snippet is to create a branch.
		SVMaterialsTreeAdapter l_Parent( *p_pParent );
		SVMaterialsTreeAdapter::SVTreeElement l_Element( l_strBranch, SVMaterials() );
		SVMaterialsTreeAdapter::iterator l_Iter;
		l_Iter = l_Parent.insert( l_Element, l_Parent.end() );
		l_pBranch = l_Iter.GetChildTree();
		// ***********************************************
		if( l_pBranch )
		{
			KeepPrevError( l_hr, m_PLCOutputs[i]->GetMaterials( p_rMaterialsTree, l_pBranch ));
		}
	}

	// Save the Array of starting Addresses for use in Block writing.
	ASSERT( m_lBlockCount == m_strBlockAddresses.size() );
	for( size_t i = 0 ; i < m_strBlockAddresses.size(); i++ )
	{
		SVString l_strBlockId;
		l_strBlockId.Format( "%s_%d",CTAG_PLC_BLOCK_START, i + 1 );

		l_vVariant = m_strBlockAddresses[i] ;
		KeepPrevError( l_hr, l_Materials.GetMaterial( l_strBlockId, l_vVariant ));
	}

	SVMaterialsTreeAdapter::SVTreeElement l_Element( CTAG_PLC_PARAMETERS, l_Materials );
	p_pParent->insert( p_pParent->end(), l_Element );

	return l_hr;
}

// Sets the data of this class with the MaterialsTree.
HRESULT SVPLCControlPar::Update( SVMaterialsTreeAdapter& p_rMaterialsTree )
{
	HRESULT l_hr = S_OK;

	SVMaterialsTreeAdapter::SVTreeElement* l_pElement = p_rMaterialsTree.get();

	SVMaterials l_Materials = l_pElement->second;

	_variant_t l_vVariant;
	l_hr = l_Materials.GetMaterial( CTAG_PLC_BLOCK_SIZE, l_vVariant );

	if( l_hr == S_OK )
	{
		// PLC Block Size
		m_lBlockCount = l_vVariant;
		if( m_lBlockCount < 1 )
		{
			m_lBlockCount = 1;
		}
		if( m_lBlockCount > 30 )
		{
			m_lBlockCount = 30;
		}
	}

	l_hr = l_Materials.GetMaterial( CTAG_PLC_QUEUE_SIZE, l_vVariant );
	if ( l_hr == S_OK )
	{
		// PLC Queue Size
		m_lQueueSize = l_vVariant;
		if( m_lQueueSize < 1 )
		{
			m_lQueueSize = 1;
		}
		if( m_lQueueSize > 30 )
		{
			m_lQueueSize = 30;
		}
	}


	// PLC Block starting address list
	HRESULT l_hrTmp = S_OK;
	long l_lEntryNum = 0;
	while( l_hrTmp == S_OK )
	{
		SVString l_strEntry;
		l_strEntry.Format( "%s_%d", CTAG_PLC_BLOCK_START, ++l_lEntryNum );
		l_hrTmp = l_Materials.GetMaterial( l_strEntry, l_vVariant );

		if( l_hrTmp == S_OK )
		{
			CString l_strTmp = l_vVariant;
			m_strBlockAddresses.push_back( l_strTmp );
		}
	}


	// PLC Output List
	l_hrTmp = S_OK;
	SVPLCOutputObject* l_TmpOutput = NULL;
	l_lEntryNum = 0;
	while( l_hrTmp == S_OK )
	{
		SVString l_strEntry;
		l_strEntry.Format( "%s_%d", CTAG_PLC_ENTRY, ++l_lEntryNum );
		SVMaterialsTreeAdapter::iterator l_it;
		l_it = p_rMaterialsTree.find( l_strEntry );
		if ( l_it != p_rMaterialsTree.end() )
		{
			// A new tree branch....
			SVMaterialsTreeAdapter::SVTreeContainer* l_pTreeContainer = l_it.GetChildTree( );
			if( l_pTreeContainer != NULL )
			{
				SVMaterialsTreeAdapter l_rmtaBranch( *l_pTreeContainer );

				l_TmpOutput = new SVPLCOutputObject;
				l_hrTmp = l_TmpOutput->Update( l_rmtaBranch );
				if( l_hrTmp == S_OK )
				{
					m_PLCOutputs.push_back( l_TmpOutput );
				}
				else
				{
					// delete the tmp object because something failed now we are done with it.
					delete l_TmpOutput;
				}
			}
		}
		else
		{
			l_hrTmp = S_FALSE;
		}
	}

	return l_hr;
}


// Sets parameters from Tree Control ( Restore )
BOOL SVPLCControlPar::SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = TRUE;
	_variant_t svVariant;

	if ( bOk )
	{
		// PLC Block Size
		bOk = SVNavigateTreeClass::GetItem( p_rTree, CTAG_PLC_BLOCK_SIZE, htiParent, svVariant );
		if ( bOk )
		{
			m_lBlockCount = svVariant;

			if( m_lBlockCount < 1 )
			{
				m_lBlockCount = 1;
			}
		}
	}

	if ( bOk )
	{
		// PLC Queue Size
		bOk = SVNavigateTreeClass::GetItem( p_rTree, CTAG_PLC_QUEUE_SIZE, htiParent, svVariant );
		if ( bOk )
		{
			m_lQueueSize = svVariant;
		}
	}


	// PLC Output List
	if ( bOk )
	{
		BOOL l_bTmp = TRUE;
		SVPLCOutputObject* l_TmpOutput = NULL;
		long l_lEntryNum = 0;
		while( l_bTmp )
		{
			SVTreeType::SVBranchHandle htiBranch=NULL;
			CString l_strEntry;
			l_strEntry.Format( "%s_%d", CTAG_PLC_ENTRY, ++l_lEntryNum );
			l_bTmp = SVNavigateTreeClass::GetItemBranch( p_rTree, l_strEntry, htiParent, htiBranch );
			if ( l_bTmp )
			{
				l_TmpOutput = new SVPLCOutputObject;
				l_bTmp = l_TmpOutput->SetParameters(p_rTree, htiBranch);
				if( l_bTmp )
				{
					m_PLCOutputs.push_back( l_TmpOutput );
				}
				else
				{
					// delete the tmp object because something failed now we are done with it.
					delete l_TmpOutput;
				}
			}
		}
	}

	// PLC Block starting address list
	if ( bOk )
	{
		BOOL l_bTmp = TRUE;
		SVPLCOutputObject* l_TmpOutput = NULL;
		long l_lEntryNum = 0;
		while( l_bTmp )
		{
			CString l_strEntry;
			l_strEntry.Format( "%s_%d", CTAG_PLC_BLOCK_START, ++l_lEntryNum );
			l_bTmp = SVNavigateTreeClass::GetItem( p_rTree, l_strEntry, htiParent, svVariant );
			if( l_bTmp )
			{
				CString l_strTmp = svVariant;

				m_strBlockAddresses.push_back( l_strTmp );
			}
		}
	}

	SortPLCList();
	CalculateBlockSize();

	return bOk;
}

// Gets the queue size parameter
long SVPLCControlPar::GetQueueSize( )
{
	return m_lQueueSize;
}

// Sets the Queue size parameter.
long SVPLCControlPar::SetQueueSize( long p_lQueueSize )
{
	m_lQueueSize = p_lQueueSize;
	return m_lQueueSize;
}

// Gets the block count parameter.
long SVPLCControlPar::GetBlockCount()
{
	return m_lBlockCount;
}


long SVPLCControlPar::SetBlockCount( long p_lBlockCount )
{
	m_lBlockCount = p_lBlockCount;
	if( m_lBlockCount < 1 )		// Block Count is never allowed to be zero.
		m_lBlockCount = 1;
	return m_lBlockCount;
}

// PLC Identifier and new output object will be managed by SVPLCControlPar
HRESULT SVPLCControlPar::AddOutput( SVPLCOutputObject* p_pOutputEntry )
{
	HRESULT l_hr = S_OK;
	m_PLCOutputs.push_back( p_pOutputEntry );
	l_hr = SortPLCList();
	l_hr = CalculateBlockSize();
	return l_hr;
}


HRESULT SVPLCControlPar::RemoveOutput( GUID p_pOutputEntry )
{
	HRESULT l_hr = -3128;	// The item does not exist

	SVPLCOutputObjectList::iterator l_it;
	for( l_it = m_PLCOutputs.begin() ; l_it != m_PLCOutputs.end() ; ++l_it )
	{
		if( (*l_it)->GetClassID() == p_pOutputEntry )
		{
			delete *l_it;
			m_PLCOutputs.erase( l_it );
			l_hr = CalculateBlockSize();
			break;
		}
	}
	return l_hr;
}


HRESULT SVPLCControlPar::Delete( SVPLCOutputObject* pPLCOutput)
{
	HRESULT l_hr = -3128 ;

	SVPLCOutputObjectList::iterator l_it;
	l_it = std::find( m_PLCOutputs.begin(), m_PLCOutputs.end(), pPLCOutput );
	if( l_it != m_PLCOutputs.end() )
	{
		m_PLCOutputs.erase( l_it );
		delete pPLCOutput;
		l_hr =	CalculateBlockSize();
	}
	return l_hr;
}


size_t SVPLCControlPar::PLCListSize()
{
	return m_PLCOutputs.size();
}

HRESULT SVPLCControlPar::GetItem( long l_lIndex, SVPLCOutputObject*& p_rItem )
{
	HRESULT l_hr = -3129 ;
	if( static_cast<long>(m_PLCOutputs.size()) > l_lIndex  )
	{
		p_rItem = m_PLCOutputs[l_lIndex];
		l_hr = S_OK;
	}
	return l_hr;	
}



class EntrySortLess
{
public:
	bool operator()(  SVPLCOutputObject* q,  SVPLCOutputObject* p ) const
	{
		bool l_bGreater= false;
		// Figure which PPQ this is...
		CString l_PLCName1 = q->GetPLCName();
		CString l_PLCName2 = p->GetPLCName();

		if( l_PLCName1 > l_PLCName2 )
		{
			l_bGreater = true;
		}
		else
		if( l_PLCName1 == l_PLCName2 )
		{
			long l_l1 = p->GetElement();
			long l_l2 = q->GetElement();
			if( l_l1 > l_l2 )
			{
				l_bGreater = true;
			}
			else
			{
				// Compare channels
			}
		}
		return l_bGreater;
	}
};

HRESULT SVPLCControlPar::SortPLCList()
{

	std::sort( m_PLCOutputs.begin(), m_PLCOutputs.end(), EntrySortLess());

	return S_OK;
}

SVPLCOutputObject* SVPLCControlPar::GetItem( long l_lIndex )
{
	if( m_PLCOutputs.size() > (size_t)l_lIndex )
	{
		return m_PLCOutputs[l_lIndex];
	}
	else
	{
		return NULL;
	}
}

SVPLCOutputObject* SVPLCControlPar::GetLastObject( )
{
	if( m_PLCOutputs.size() > 0 )
	{
		return *m_PLCOutputs.rbegin();
	}
	else
	{
		return NULL;
	}
}
SVPLCOutputObject* SVPLCControlPar::GetFirstObject( )
{
	if( m_PLCOutputs.size() > 0 )
	{
		return *m_PLCOutputs.begin();
	}
	else
	{
		return NULL;
	}
}

// Calculate Total Block size based on the last element + last element size
// this assumes that they are in order.
HRESULT SVPLCControlPar::CalculateBlockSize()
{
	m_lBlockSize = 0;
	SVPLCOutputObject* l_pOutput = GetLastObject();
	if( l_pOutput )
	{
		if( l_pOutput->SizeInBits() )
		{
			long l_lElementSize = GetElementBitSize();
			m_lBlockSize = l_pOutput->GetElement() + l_pOutput->GetSize() / l_lElementSize ;
		}
		else
		{
			m_lBlockSize = l_pOutput->GetElement() + l_pOutput->GetSize() ;
		}
	}
	return S_OK;
}

long SVPLCControlPar::GetBlockSize()
{
	return m_lBlockSize;
}

HRESULT SVPLCControlPar::GetBlockAddresses( VecStringArray& p_Addresses )
{
	HRESULT l_hr = S_FALSE;
	p_Addresses = m_strBlockAddresses;	// Copy of the addresses.
	return l_hr;
}


HRESULT SVPLCControlPar::SetBlockAddresses(  VecStringArray p_Addresses )
{
	HRESULT l_hr = S_OK;
	long l_lFirstType = 0;
	long l_lSize = 0;
	CString l_strAddress;
	for( size_t i = 0 ; i < p_Addresses.size() ; ++i )
	{
		l_strAddress = p_Addresses[i];
		if( i == 0  )
		{
			TheSVObserverApp.m_PLCManager.GetDataType( &l_lSize, &l_lFirstType, l_strAddress );
		}
		else
		{
			long l_lType = 0;
			TheSVObserverApp.m_PLCManager.GetDataType( &l_lSize, &l_lType, l_strAddress );
			if( l_lType != l_lFirstType )
			{
				l_hr = S_FALSE;
				break;
			}
		}
	}
	m_lBlockCount = p_Addresses.size();
	if( l_hr == S_OK )
	{
		m_strBlockAddresses = p_Addresses;
	}
	return l_hr;
}


CString SVPLCControlPar::GetBlockAddress( int p_lIndex )
{
	CString l_strTmp;
	if( m_strBlockAddresses.size() > (size_t)p_lIndex )
	{
		l_strTmp = m_strBlockAddresses[p_lIndex];
	}
	return l_strTmp;
}

// This class creates the materials tree.
HRESULT SVPLCControlPar::GetData( SVMaterials& p_rMaterials )
{
	HRESULT l_hr = S_OK;


	//*********** Block Count *************
	// put data in variant.
	_variant_t l_vVariant( m_lBlockCount ); 
	// put variant into materials.
	p_rMaterials.UpdateMaterial( CTAG_PLC_BLOCK_SIZE, l_vVariant ); //
	l_vVariant.Clear();

	// ************* Queue Size **************
	// put data in variant.
	l_vVariant = m_lQueueSize; 
	// put variant into materials.
	p_rMaterials.UpdateMaterial( CTAG_PLC_QUEUE_SIZE, l_vVariant ); //
	l_vVariant.Clear();


	// ************* PLC elements ***************

	for( size_t i = 0 ; i < m_strBlockAddresses.size(); i++ )
	{
		SVString l_strBranch;
		l_strBranch.Format( "%s_%d",CTAG_PLC_BLOCK_START, i + 1 );
		_variant_t l_vTmp(m_strBlockAddresses[i]);
		p_rMaterials.UpdateMaterial( l_strBranch, l_vTmp );
	}



	return l_hr;
}

// The materials tree sets this data class.
HRESULT SVPLCControlPar::SetData( SVMaterials& p_rMaterials )
{
	HRESULT l_hr = S_OK;

	SVMaterials l_ParameterMaterials;

	//*********** Block Count *************
	// put data in variant.
	_variant_t l_vVariant; 

	l_hr = p_rMaterials.GetMaterial(CTAG_PLC_BLOCK_SIZE, l_vVariant );
	m_lBlockCount = l_vVariant;

	// ************* Queue Size **************
	l_hr = p_rMaterials.GetMaterial(CTAG_PLC_QUEUE_SIZE, l_vVariant );
	m_lQueueSize = l_vVariant ; 

	// ************* PLC elements ***************



	// load the Array of starting Addresses for use in Block writing.
	long l_lIndex = 0;
	HRESULT l_hrTmp = S_OK;
	m_strBlockAddresses.clear();
	for( l_lIndex = 0 ; l_lIndex < m_lBlockCount; l_lIndex++ )
	{
		SVString l_strBranch;
		l_strBranch.Format( "%s_%d",CTAG_PLC_BLOCK_START, l_lIndex + 1 );

		l_hrTmp = p_rMaterials.GetMaterial( l_strBranch, l_vVariant );

		if( l_hrTmp == S_OK )
		{
			CString l_strValue = l_vVariant;
			m_strBlockAddresses.push_back( l_strValue );
		}
	}

	return l_hr;
}

// This function finds the objects index from the Object list.
HRESULT SVPLCControlPar::Find( SVPLCOutputObject* p_pObject, long& p_rlIndex )
{
	HRESULT l_hr = S_FALSE;
	for( size_t x = 0 ; x < m_PLCOutputs.size() ; x++ )
	{
		if( m_PLCOutputs[x] == p_pObject )
		{
			p_rlIndex = x;
			l_hr = S_OK;
			break;
		}
	}
	return l_hr;
}

// Element size is extracted from the first element in the starting block addresses.
long SVPLCControlPar::GetElementBitSize()
{
	long l_lSize = 16; // Default size
	long l_ltype;
	long l_lBytes;
	if( m_strBlockAddresses.size() > 0 )
	{
		if( TheSVObserverApp.m_PLCManager.GetDataType( &l_lBytes, &l_ltype, m_strBlockAddresses[0]) == S_OK)
		{
			l_lSize = l_lBytes * 8;
		}
	}
	return l_lSize;
}

// Used to expand or collapse the View 
bool SVPLCControlPar::GetCollapse()
{
	return m_bCollapseView;
}

// Used to expand or collapse the View 
void SVPLCControlPar::SetCollapse( bool p_bCollapse )
{
	m_bCollapseView = p_bCollapse;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCControlPar.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:23:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   11 Oct 2012 10:59:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   04 Oct 2012 10:49:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   25 Sep 2012 15:24:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Sep 2012 18:29:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Sep 2012 15:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Jul 2012 14:19:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   13 Dec 2010 11:28:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Nov 2010 11:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 Oct 2010 14:08:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Dec 2009 12:25:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jan 2009 13:06:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed defaults for Queue size and Block size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jan 2009 08:47:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved current Address Counter variable from the SVPLCDataController to the SVPLCControlPar class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Dec 2008 15:31:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in SetData( Materials )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2008 13:18:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed unused functions and fixed initialization for block count.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:08:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

