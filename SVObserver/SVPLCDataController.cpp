//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCDataController
//* .File Name       : $Workfile:   SVPLCDataController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Oct 2013 11:00:30  $
//******************************************************************************

#include "stdafx.h"
#ifndef _WIN64
#include <comdef.h>
#include "SVUtilityLibrary\SVGUID.h"
#include "SVP_SRC\SVPLCInterfaceClass.h"
#include "SVPLCDataController.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVConfigurationObject.h"
#include <algorithm>
#include <iterator>
#include "SVPLCAddRemoveDlg.h"
#include "SVIODoc.h"

SV_IMPLEMENT_CLASS( SVPLCDataController, SVPLCDataControllerGUID );

//#define SWAP_ENDIAN

SVPLCDataController::SVPLCDataController(void)
:m_lHeartBeatTime(15)
{
	m_szPLCConnectionString[0] = 0;
	SVObjectClass::SVObjectClass();
}

SVPLCDataController::~SVPLCDataController(void)
{
	Destroy();
}

// Destroy itterates through the PLC control parameter map and destroys all instances of the PLC.
// Then it clears the map.
void SVPLCDataController::Destroy()
{
	
	// Destroys PLC Control Parameters.
	SVPLCControlParMap::iterator l_it;
	for( l_it = m_PLCParameters.begin() ; l_it != m_PLCParameters.end() ; ++l_it )
	{
		l_it->second.Destroy();
	}
	m_PLCParameters.clear();
}

// AddItem creates a new PLCOutputObject and adds it to the PLC SVPLCControlPar
HRESULT SVPLCDataController::AddItem( const CString& p_strPLCId, SVPLCOutputObject*& p_pNewOutput, long p_lElement, long p_lSize, GUID p_InputObjectID, long p_lBitNum, bool p_bUseBitSize )
{
	HRESULT l_hr = E_FAIL;
	SVPLCOutputObject* l_pPLCEntry= new SVPLCOutputObject;
	if( l_pPLCEntry )
	{
		l_pPLCEntry->SetElement( p_lElement );
		l_pPLCEntry->SetInputObjectId( p_InputObjectID );
		l_pPLCEntry->SetPLCName( p_strPLCId );
		l_pPLCEntry->SetSize( p_lSize );
		bool l_bConvert = true;
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectA( p_InputObjectID );
		if( l_pObject )
		{
			SVValueObjectClass* l_pValueObject = dynamic_cast<SVValueObjectClass*>( l_pObject );
			if( l_pValueObject )
			{
				if( l_pValueObject->GetObjectType() == SVStringValueObjectType )
				{
					l_bConvert = false;
				}
			}
		}
		l_pPLCEntry->SetConvert( l_bConvert );
		l_pPLCEntry->SetSizeInBits( p_bUseBitSize );
		l_pPLCEntry->SetBit( p_lBitNum );
		// Add new output to PLC Parameters map.
		m_PLCParameters[ p_strPLCId ].AddOutput( l_pPLCEntry );
		// Moved to plc parameters
		// m_PLCOutputs.push_back( l_pPLCEntry );
		p_pNewOutput = l_pPLCEntry;
		l_hr = S_OK;
	}
	return l_hr;
}

// This gives us a way to get the data associated with a PLC Id.
SVPLCControlPar SVPLCDataController::GetControlPar( const CString& p_strPLC )
{
	SVPLCControlPar l_Pars;
	if( m_PLCParameters.find( p_strPLC ) != m_PLCParameters.end() )
	{
		l_Pars = m_PLCParameters[p_strPLC];
	}
	return l_Pars;
}

// Gets the data associated with a plc into a SVMaterials.
HRESULT SVPLCDataController::GetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC )
{
	HRESULT l_hr = S_FALSE;
	l_hr = m_PLCParameters[p_strPLC].GetData( p_rMaterials );
	return l_hr;
}

// Sets the PLCControlPar with a SVmaterials data.
HRESULT SVPLCDataController::SetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC )
{
	return m_PLCParameters[p_strPLC].SetData( p_rMaterials );
}

// Get the PLCControlPar associated with the PLCID.
HRESULT SVPLCDataController::GetControlPar( const CString& p_strPLC, SVPLCControlPar** p_pControl )
{
	HRESULT l_hr = S_FALSE;
	if( m_PLCParameters.find( p_strPLC ) != m_PLCParameters.end() )
	{
		*p_pControl = &m_PLCParameters[p_strPLC];
		l_hr = S_OK;
	}
	return l_hr;
}

// Set the Control Parameters Map with the SVPLCControlPar at the supplied plcID.
HRESULT SVPLCDataController::SetControlPar( const CString& p_strPLC, SVPLCControlPar p_PLCControl )
{
	HRESULT l_hr = S_FALSE;
	m_PLCParameters[p_strPLC] = p_PLCControl;
	l_hr = S_OK;
	return l_hr;
}

// Remove the PLCID by its GUID
HRESULT SVPLCDataController::RemoveItem( const CString& p_strPLCId, GUID p_PLCEntry )
{
	HRESULT l_hr = -3130  ; // List is empty
	// Move to SVPLCControlPar
	if( m_PLCParameters.find( p_strPLCId ) != m_PLCParameters.end() )
	{
		l_hr = m_PLCParameters[p_strPLCId].RemoveOutput( p_PLCEntry );
	}
	return l_hr;
}

// Get the element Count from the SVPLCControlPar for the given PLCID
size_t SVPLCDataController::GetItemCount( const CString& p_strPLCId )
{
	size_t l_lSize = 0;
	if( m_PLCParameters.find( p_strPLCId ) != m_PLCParameters.end() )
	{
		l_lSize = m_PLCParameters[p_strPLCId].PLCListSize();
	}
	return l_lSize;
}

// Get a PLCOutputObject based on index from the given PLCId.
HRESULT SVPLCDataController::GetItem( const CString& p_strPLCId, long l_lIndex, SVPLCOutputObject*& p_rItem )
{
	HRESULT l_hr = -3130;
	if( m_PLCParameters.find( p_strPLCId ) != m_PLCParameters.end() )
	{
		l_hr = m_PLCParameters[p_strPLCId].GetItem( l_lIndex, p_rItem );
	}
	return l_hr;
}

// Parameters >> Tree ( Save )
BOOL SVPLCDataController::GetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent )
{
	BOOL bOk = FALSE;
	_variant_t svVariant;

	ClearUnUsedData();	// clears unused plc data

	SVTreeType::SVBranchHandle htiIOPLC = NULL;
	if( m_PLCParameters.size() > 0 )
	{
		if ( SVNavigateTreeClass::SetBranch( p_rTree, p_htiParent, CTAG_PLC_PARAMETERS, &htiIOPLC ) )
		{
			svVariant = SVGUID( outObjectInfo.UniqueObjectID ).ToVARIANT();
			SVNavigateTreeClass::AddItem( p_rTree, htiIOPLC, CTAG_UNIQUE_REFERENCE_ID, svVariant );
			svVariant.Clear();

			svVariant = m_szPLCConnectionString;
			SVNavigateTreeClass::AddItem( p_rTree, htiIOPLC, CTAG_PLC_CONNECTION_STRING, svVariant );
			svVariant.Clear();

			svVariant = m_strHeartBeatAddress;
			SVNavigateTreeClass::AddItem( p_rTree, htiIOPLC, CTAG_PLC_HEARTBEAT_ADDR, svVariant );
			svVariant.Clear();

			svVariant = m_lHeartBeatTime;
			SVNavigateTreeClass::AddItem( p_rTree, htiIOPLC, CTAG_PLC_HEARTBEAT_TIME, svVariant );
			svVariant.Clear();

			// PLC Parameters
			SVPLCControlParMap::iterator l_it;

			long lIndex = 0;
			for( l_it = m_PLCParameters.begin(); l_it != m_PLCParameters.end() ; ++l_it )
			{
				SVTreeType::SVBranchHandle htiPLCEntry = NULL;
				CString l_strBranch;

				lIndex++;
				l_strBranch.Format( "%s_%d",CTAG_PLC_CHANNEL_PAR, lIndex );
				if ( SVNavigateTreeClass::SetBranch( p_rTree, htiIOPLC, l_strBranch, &htiPLCEntry ) )
				{
					l_it->second.GetParameters( p_rTree, htiPLCEntry );
				}
			}

			bOk = TRUE;
		}
	}
	else
	{
		bOk = TRUE;
	}
	return bOk;
}

// Tree >> Parameters ( Restore from archive )
BOOL SVPLCDataController::SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent )
{
	BOOL bOk = TRUE;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiIOPLC = NULL;

	// PLCDataController Object ID
	BOOL l_bTmp = SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_PLC_PARAMETERS,p_htiParent, htiIOPLC ) ;
	if( l_bTmp )
	{
		bOk = SVNavigateTreeClass::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiIOPLC, svVariant );

		if ( bOk )
		{
			SVGUID ObjectID = svVariant;

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			outObjectInfo.UniqueObjectID = ObjectID;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}

		// PLC Connection String
		if( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( p_rTree, CTAG_PLC_CONNECTION_STRING, htiIOPLC, svVariant );
			if( bOk )
			{
				_bstr_t l_Temp = svVariant;

				strcpy( m_szPLCConnectionString, l_Temp );
			}
		}

		if ( bOk )
		{
			// PLC Heart Beat Address
			BOOL bOk = SVNavigateTreeClass::GetItem( p_rTree, CTAG_PLC_HEARTBEAT_ADDR, htiIOPLC, svVariant );
			if ( bOk )
			{
				m_strHeartBeatAddress = svVariant;
			}
		}

		if ( bOk )
		{
			// PLC Heart Beat Time
			BOOL bOk = SVNavigateTreeClass::GetItem( p_rTree, CTAG_PLC_HEARTBEAT_TIME, htiIOPLC, svVariant );
			if ( bOk )
			{
				m_lHeartBeatTime =  svVariant;
			}
		}

		// PLC Parameters
		if( bOk )
		{
			BOOL l_bTmp = TRUE;
			long l_lEntryNum = 0;
			while( l_bTmp )
			{
				SVTreeType::SVBranchHandle htiBranch=NULL;
				CString l_strEntry;
				CString l_strPLCID;

				l_strEntry.Format( "%s_%d", CTAG_PLC_CHANNEL_PAR, ++l_lEntryNum );
				l_strPLCID.Format( "PLC_%d", l_lEntryNum );
				l_bTmp = SVNavigateTreeClass::GetItemBranch( p_rTree, l_strEntry, htiIOPLC, htiBranch );

				if ( l_bTmp )
				{
					SVPLCControlPar l_ControlParameter;
					l_bTmp = l_ControlParameter.SetParameters( p_rTree, htiBranch );
					SVPLCOutputObject* l_pPLCOut = l_ControlParameter.GetFirstObject();
					if( l_pPLCOut )
					{
						HRESULT l_hr = l_pPLCOut->GetPLCName( l_strPLCID );
						ASSERT( l_hr == S_OK );
					}
					if( l_bTmp && l_pPLCOut != NULL )
					{
						m_PLCParameters[ l_strPLCID ] = l_ControlParameter;
					}
				}
			}
		}
	}

	return bOk;
}

// Add Branch is used to build a new branch into a MaterialsTreeAdapter.
HRESULT SVPLCDataController::AddBranch(SVMaterialsTreeAdapter& p_rTree, 
																LPCTSTR lpszName, 
																SVMaterialsTreeAdapter::SVTreeContainer* p_pParent, 
																SVMaterialsTreeAdapter::SVTreeContainer*& p_rpBranch)
{
	HRESULT hrOk = S_OK;

	if( p_pParent != NULL )
	{
		SVMaterialsTreeAdapter l_Parent( *p_pParent );
		SVMaterialsTreeAdapter::SVTreeElement l_Element( lpszName, SVMaterials() );
		SVMaterialsTreeAdapter::iterator l_Iter;

		l_Iter = l_Parent.insert( l_Element, l_Parent.end() );

		if( l_Iter != l_Parent.end() )
		{
			p_rpBranch = l_Iter.GetChildTree();
		}
		else
		{
			p_rpBranch = NULL;

			hrOk = S_FALSE;
		}
	}
	else
	{
		p_rpBranch = NULL;

		hrOk = E_FAIL;
	}

	return hrOk;
}
// GetMaterials Gets saves data from this class to the SVTreeContainer parent.
HRESULT SVPLCDataController::GetMaterials( SVMaterialsTreeAdapter& p_rMaterialsTree, 
										  SVMaterialsTreeAdapter::SVTreeContainer* p_pParent )
{
	HRESULT l_hr = S_OK;
	_variant_t l_vVariant;
	
	ClearUnUsedData();	// clears unused plc data

	SVMaterialsTreeAdapter::SVTreeContainer* l_pBranch = NULL;

	SVMaterials l_Materials;


	if ( AddBranch( p_rMaterialsTree, CTAG_PLC_PARAMETERS, p_pParent, l_pBranch ) == S_OK )
	{
		SVGUID l_GUID( outObjectInfo.UniqueObjectID );

		l_vVariant = l_GUID.ToVARIANT();
		l_hr = l_Materials.UpdateMaterial( CTAG_UNIQUE_REFERENCE_ID, l_vVariant );
		l_vVariant.Clear();

		l_vVariant = m_szPLCConnectionString ;
		l_hr = l_Materials.UpdateMaterial( CTAG_PLC_CONNECTION_STRING, l_vVariant);
		l_vVariant.Clear();

		l_vVariant = m_strHeartBeatAddress;
		l_hr = l_Materials.UpdateMaterial( CTAG_PLC_HEARTBEAT_ADDR, l_vVariant );
		l_vVariant.Clear();

		l_vVariant = m_lHeartBeatTime;
		l_hr = l_Materials.UpdateMaterial( CTAG_PLC_HEARTBEAT_TIME, l_vVariant );

		SVMaterialsTreeAdapter::SVTreeElement l_Element( CTAG_PLC_PARAMETERS, l_Materials );
		l_pBranch->insert(l_pBranch->end(), l_Element );

		// PLC Parameters
		SVPLCControlParMap::iterator l_it;

		long lIndex = 0;
		for( l_it = m_PLCParameters.begin(); l_it != m_PLCParameters.end() ; ++l_it )
		{
			CString l_strBranch;
			lIndex++;
			l_strBranch.Format( "%s_%d",CTAG_PLC_CHANNEL_PAR, lIndex );
			SVMaterialsTreeAdapter::SVTreeContainer* l_pSubBranch;

			if ( AddBranch( p_rMaterialsTree, l_strBranch, l_pBranch, l_pSubBranch ) == S_OK )
			{
				l_it->second.GetMaterials( p_rMaterialsTree,  l_pSubBranch);
			}
		}

	}
	return l_hr;
}

// Update updates this class from a materials tree.
HRESULT SVPLCDataController::Update( SVMaterialsTreeAdapter& p_rMaterialsTree )
{
	_variant_t l_vVariant;

	HRESULT l_hr = S_OK;
	// PLCDataController Object ID
	
	SVMaterialsTreeAdapter::iterator l_it = p_rMaterialsTree.find( CTAG_PLC_PARAMETERS);
	if( l_it != p_rMaterialsTree.end() )
	{
		// Get the materials
		SVMaterials& l_rMaterials = l_it->second;

		// Unique GUID for this object.
		l_hr = l_rMaterials.GetMaterial( CTAG_UNIQUE_REFERENCE_ID, l_vVariant );
		SVString l_strTmp = l_vVariant;
		
		if ( l_hr == S_OK )
		{
			SVGUID ObjectID( l_vVariant );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			outObjectInfo.UniqueObjectID = ObjectID;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}

		// PLC Connection String
		KeepPrevError( l_hr, l_rMaterials.GetMaterial( CTAG_PLC_CONNECTION_STRING, l_vVariant ));
		if( l_hr == S_OK )
		{
			SVString l_strTmp = l_vVariant;
			strcpy_s( m_szPLCConnectionString, 255, l_strTmp.ToString() );
		}

		KeepPrevError( l_hr, l_rMaterials.GetMaterial( CTAG_PLC_HEARTBEAT_ADDR, l_vVariant ));
		if( l_hr == S_OK )
		{
			// PLC Heart Beat Address
			m_strHeartBeatAddress =  l_vVariant;
		}

		KeepPrevError( l_hr, l_rMaterials.GetMaterial( CTAG_PLC_HEARTBEAT_TIME, l_vVariant ));
		if( l_hr == S_OK )
		{
			// PLC Heart Beat Time
			m_lHeartBeatTime =  l_vVariant;
		}

		// A new tree branch....
		SVMaterialsTreeAdapter::SVTreeContainer* l_pTreeContainer = l_it.GetChildTree( );
		if( l_pTreeContainer != NULL )
		{
			SVMaterialsTreeAdapter l_rmtaBranch( *l_pTreeContainer );

			long l_lEntryNum = 0;
			bool l_bDone = false;
			while( l_bDone == false )
			{
				SVString l_strEntry;

				// PLC Parameters
				l_strEntry.Format( "%s_%d", CTAG_PLC_CHANNEL_PAR, ++l_lEntryNum );
				l_it = l_rmtaBranch.find( l_strEntry );
				if( l_it != l_rmtaBranch.end() )
				{
					l_pTreeContainer = l_it.GetChildTree();
					SVMaterialsTreeAdapter l_rmtaChannelBranch( *l_pTreeContainer );

					SVPLCControlPar l_ControlParameter;
					KeepPrevError( l_hr, l_ControlParameter.Update( l_rmtaChannelBranch ));
					if( l_hr == S_OK )
					{
						// This is a temperary way to get the plc name....
						CString l_strPLCID ;
						l_strPLCID.Format( "PLC_%d", l_lEntryNum );

						m_PLCParameters[ l_strPLCID ] = l_ControlParameter;
					}
				}
				else
				{
					l_bDone = true;
				}
			}
		}			
	}

	return l_hr;
}



// Sorts the PLC lists by PLC element.
HRESULT SVPLCDataController::SortPLCLists()
{
	HRESULT l_hr = S_OK;
	SVPLCControlParMap::iterator l_it;
	for( l_it =	m_PLCParameters.begin() ; l_it != m_PLCParameters.end() ; ++l_it )
	{
		l_hr = l_it->second.SortPLCList();
	}
	return l_hr;
}

// Validates an element against the PLC output list to make
// sure there are no conflicts.
HRESULT SVPLCDataController::ValidateElement( const CString& p_strPLC, GUID p_InputObjectGuid, long p_lElement, long p_lSize, long p_lListIndex, long p_lBit )
{
	HRESULT l_hr = S_OK;
	SVPLCControlPar l_PLCControl = GetControlPar( p_strPLC );

	long l_lElementBitSize = l_PLCControl.GetElementBitSize();
	long l_lSize = p_lSize;
	// check if it extends beyond a total length of 256 bytes ( get info from plc )
	if( p_lElement + p_lSize > 256 )
	{
		l_hr = TOO_LARGE;
	}

	for( size_t i = 0 ; i < l_PLCControl.PLCListSize() ; i++ )
	{
		if( i == p_lListIndex )
			continue;
		SVPLCOutputObject* l_pOutput = l_PLCControl.GetItem( i );
		long l_lExistingElement = l_pOutput->GetElement();
		long l_lExistingSize = l_pOutput->GetSize();
		// Check if the input object is the same as an existing.
		GUID l_tmpGuid;
		l_pOutput->GetInputValueObjectGUID(l_tmpGuid);
		if( p_InputObjectGuid == l_tmpGuid )
		{
			l_hr = INPUT_ALREADY_USED;
			break;
		}

		// Check if there is already an element.
		if(  l_lExistingElement == p_lElement )
		{
			// if this is a bit output then check if any bits conflict.
			if( l_pOutput->SizeInBits() && p_lBit > -1 )
			{
				// Build existing bit mask.
				__int64 l_lExistingMask = ((1 << (l_pOutput->GetBit() + l_pOutput->GetSize())) -1) 
										& ~((1 << l_pOutput->GetBit()) - 1);
				
				// Build new bit mask.
				__int64 l_lNewMask = ((1 << (p_lBit + p_lSize)) -1) 
										& ~((1 << p_lBit) - 1);
				if( l_lNewMask & l_lExistingMask )
				{
					l_hr = BITS_OVERLAP;
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				l_hr = ALREADY_USED;
				break;
			}
		}

		// Calculate port size from bit size
		if( p_lBit > -1 )
		{
			l_lSize = (p_lBit + p_lSize + l_lElementBitSize-1)/l_lElementBitSize;
		}
		else
		{
			l_lSize = p_lSize;
		}

		// check if its length makes it overlap
		if( l_lExistingElement >= p_lElement && l_lExistingElement < (p_lElement + l_lSize) )
		{
			l_hr = OVERLAPPING;
			break;
		}
	}

	return l_hr;
}

// Validate Block Addresses validates 
// 1. That the addresses are all the same type.
// 2. That the PLC Address List Size + a block address does not cause addresses to fall outside the MaxBlockSize.
// 3. That the PLC Address List size does not exceed a block spacing size.
HRESULT SVPLCDataController::ValidateBlockAddresses( const CString& p_strPLCName, SVMaterials p_BlockAddresses )
{
	// Get block count
	HRESULT l_hr = S_OK;
	_variant_t l_vTemp;
	long l_lTmpBlockCount = 1;
	l_hr = p_BlockAddresses.GetMaterial( CTAG_PLC_BLOCK_SIZE, l_vTemp );
	if( l_hr == S_OK )
	{
		l_lTmpBlockCount = l_vTemp;

		long l_lDataType=0;
		long l_lTmpType=0;
		SVPLCControlPar& l_PLCPar = m_PLCParameters[p_strPLCName];
		SVPLCManager& l_PLCMgr = TheSVObserverApp.m_PLCManager;
		long l_lMaxDTSize = l_PLCMgr.GetMaxDTSize();
		long l_lMinDataTableSpacing = l_lMaxDTSize;
		long l_lLastFile = 0;
		long l_lLastElement = 0;
		long l_lDataSize = 1;

		for( long l_lIndex = 0; l_lIndex < l_lTmpBlockCount ; l_lIndex++ )
		{
			SVString l_strTmp;
			l_strTmp.Format( "%s_%d", CTAG_PLC_BLOCK_START, l_lIndex + 1 );
			l_hr = p_BlockAddresses.GetMaterial( l_strTmp, l_vTemp );
			CString l_strAddress = l_vTemp;
			if( l_hr == S_OK )
			{
				// 1. Check if Data Type is different
				if( l_lIndex == 0 )
				{
					l_hr = TheSVObserverApp.m_PLCManager.GetDataType( &l_lDataSize, &l_lDataType, l_strAddress );
					if( l_hr != S_OK )
						break;
				}
				else
				{
					l_hr = TheSVObserverApp.m_PLCManager.GetDataType( &l_lDataSize, &l_lTmpType, l_strAddress );
					if( l_lTmpType != l_lDataType || l_hr != S_OK )
					{
						// Error Data types are different.
						l_hr = DIFFERENT_TYPES;
						break;
					}
				}

				// Calculate Minimum spacing
				long l_lCurrentFile;
				long l_lCurrentElement;
				long l_lBit;
				l_hr = GetInfoFromAddress( l_lCurrentFile, l_lCurrentElement, l_lBit, l_strAddress );
				if( l_lIndex != 0 )
				{
					if( l_lCurrentFile == l_lLastFile )
					{
						if( l_lCurrentElement - l_lLastElement < l_lMinDataTableSpacing )
						l_lMinDataTableSpacing = l_lCurrentElement - l_lLastElement;
					}
				}
				l_lLastFile = l_lCurrentFile;
				l_lLastElement = l_lCurrentElement;
				// 2. PLC Address List Size + a block address MaxBlockSize.
				if( (l_lCurrentElement + l_PLCPar.GetBlockSize()) * l_lDataSize >= l_lMaxDTSize )
				{
					// Data is over the end of the data table.
					l_hr = PAST_THE_END;
					break;
				}
			}
		}

		// Check minimum spacing against blockSize
		if( l_PLCPar.GetBlockSize() > l_lMinDataTableSpacing )
		{
			// Error data is overlapping.
			l_hr = OVERLAPPING_DATA;
		}
	}
	return l_hr;
}

// This function takes the plc type and returns a compatible variant type.
VARTYPE SVPLCDataController::VariantTypeFromPLCType( long p_lFirstDataType )
{
	VARTYPE l_lRet = VT_I2;
	switch( p_lFirstDataType )
	{
		case SV_PLC_INT16:
		{
			l_lRet = VT_I2;
			break;
		}
		case SV_PLC_INT32:
		{
			l_lRet = VT_I4;
			break;
		}
		case SV_PLC_SPFLOAT:
		{
			l_lRet = VT_R4;
			break;
		}
		case SV_PLC_DPFLOAT:
		{
			l_lRet = VT_R8;
			break;
		}
		case SV_PLC_BIT:
		{
			l_lRet = VT_I2;
			break;
		}
	}
	return l_lRet;
}

// Write Outputs with the supplied PPQ name.
HRESULT SVPLCDataController::WriteOutputs( const CString& p_strPLCName, SVProductInfoStruct *pProduct)
{
	HRESULT hr = S_FALSE;

	if( pProduct && (m_PLCParameters.find(p_strPLCName) != m_PLCParameters.end()) )
	{
		long l_lMaxDTSize = TheSVObserverApp.m_PLCManager.GetMaxDTSize();
		std::vector<LPCTSTR> l_pAddressStrings;			// place to collect Address pointers
		std::vector<LPCTSTR> l_pStringValues;
		l_pAddressStrings.resize(l_lMaxDTSize);
		l_pStringValues.resize(l_lMaxDTSize);
		long l_lOutputIndex=-1;
		l_lOutputIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();
		std::vector<CString> l_strAddresses;  // place to collect address strings
		std::vector<CString> l_strStringValues;

		char l_pTmpBuf[256];

		std::vector<VARIANT> l_pvData;
		l_pvData.resize(l_lMaxDTSize);

		// parameters for a logical PLC / PPQ setup
		SVPLCControlPar& l_PLCPar = m_PLCParameters[p_strPLCName];

		long l_lBlockCount = l_PLCPar.GetBlockCount();
		if( l_lBlockCount > 0 )
		{

			long l_lBlockSize = l_PLCPar.GetBlockSize();
			long l_lCurrentBlockOffset = (l_PLCPar.m_lStartingAddressCounter % l_lBlockCount) ;
			long l_lFirstElement = 0;
			long l_lFirstBit = -1;
			long l_lFileNum = 0;
			SVPLCOutputObject* l_pFirstOutput = NULL;
			SVPLCDataTypeEnum l_FirstDataType = SV_PLC_UNKNOWN;


			// Get the current block starting address
			CString l_strCurrentBlockAddress = l_PLCPar.GetBlockAddress( l_lCurrentBlockOffset );

			// First Address information from the starting addresses array.
			GetInfoFromAddress( l_lFileNum, l_lFirstElement, l_lFirstBit, l_strCurrentBlockAddress );
			l_FirstDataType = GetPLCDataType( l_strCurrentBlockAddress );
			long l_lPreviousElement = -1;
			long l_lCurrentPLCDataIndex=-1;
			size_t l_lElementCount = l_PLCPar.PLCListSize();
			long l_lElementBitSize = l_PLCPar.GetElementBitSize();

			// Get the last object so we can calculate how much data is needed.
			long l_lDataSize = l_PLCPar.GetBlockSize();

			// initialize all variant data.
			l_pvData[0].llVal = 0;
			l_pvData[0].vt = VariantTypeFromPLCType( l_FirstDataType );
			for( int index = 1 ; index < l_lDataSize ; index++ )
			{	
				l_pvData[index] = l_pvData[0];
			}

			// itterate through the plc outputs
			for( size_t i = 0 ; i < l_lElementCount ; i++ )
			{

				SVPLCOutputObject* l_pOutput = l_PLCPar.GetItem( i ) ;

				CString l_strPLCName;
				l_pOutput->GetPLCName( l_strPLCName );
				// verify the plc name is correct.
				if( l_strPLCName == p_strPLCName )
				{
					// Guid Input to value object.
					GUID l_InputGUID;
					l_pOutput->GetInputValueObjectGUID( l_InputGUID );
					SVValueObjectClass* l_pValueObject = dynamic_cast<SVValueObjectClass*>(SVObjectManagerClass::Instance().GetObjectA( l_InputGUID ));

					if( l_pValueObject == NULL )
					{
						ASSERT(0);
						continue;
					}

					// collect address...
					long l_lCurrentElement = l_pOutput->GetElement();
					long l_lNumElements = l_pOutput->GetSize();

					// if we are using bits, size is divided by ElementBitSize but rounded up
					// bits data is sent using a word array. 
					long l_lCalculatedElementSize = l_pOutput->SizeInBits() ? (l_lNumElements + l_pOutput->GetBit() + l_lElementBitSize-1) / l_lElementBitSize : l_lNumElements;


					if( l_lFirstElement + l_lCurrentElement + l_lCalculatedElementSize >= l_lMaxDTSize )
					{
						ASSERT(0);
						continue;
					}

					////////////// Address Collection //////////////////////
					// Start with the starting block address.
					CString l_strCurrentAddress = l_strCurrentBlockAddress;

					// If we are using bit addresses then the element may be the same.
					if( l_lPreviousElement != l_lCurrentElement  )
					{
						// Keep the data indexes caught up to address indexes.
						while( l_lCurrentPLCDataIndex < l_lCurrentElement )
						{
							l_lCurrentPLCDataIndex++ ;
						}

						// This is where we make a new address if needed.
						for( int j = l_lPreviousElement ; j < l_lCurrentElement ; j++ )
						{
							// Add the element number to this starting address.
							l_strCurrentAddress = l_strCurrentBlockAddress;
							AddOffsetToAddress( l_strCurrentAddress, j + 1 );
							l_strAddresses.push_back( l_strCurrentAddress );
						}
						l_lPreviousElement = l_lCurrentElement;

						// We have to fill in the address array with extra values
						// based on the size of elements if this is the last element.
						if( i == l_lElementCount-1 )
						{
							for( int count = 1 ; count < l_lCalculatedElementSize ; count++ )
							{
								AddOffsetToAddress( l_strCurrentAddress, 1 );
								l_strAddresses.push_back( l_strCurrentAddress );				
							}
						}
					}
					else
					{
						// make the same address again.
						AddOffsetToAddress( l_strCurrentAddress , l_lCurrentElement );
					}


					//////////////////// Data Section  ////////////////////
					// If size is in bits then handle the data through words.
					if( l_pOutput->SizeInBits() )
					{
						if( l_pOutput->GetConvert() )
						{
							long l_lStartingPort = l_pOutput->GetElement(); 
							long l_lStartingChannel = l_pOutput->GetBit();
							double l_dValue = 0;
							hr = l_pValueObject->GetValue( l_lOutputIndex, l_dValue );
							long l_lValue = static_cast<long>(l_dValue);
							long l_lHi = l_lValue << l_lStartingChannel;	// Shift the data to the starting bit
							long l_lLow = l_lValue >> (l_lElementBitSize - l_lStartingChannel);
							l_lValue = l_lHi | l_lLow;
							if( hr == S_OK )
							{
								for( int l_iIndex = 0 ; l_iIndex < l_lNumElements ; l_iIndex++ )
								{
									long l_lChannel = l_lStartingChannel + l_iIndex;
									long l_lPort = l_lCurrentPLCDataIndex + l_lChannel / l_lElementBitSize;
									long l_lValue1 = l_lChannel < l_lElementBitSize ? l_lValue : l_lValue >> l_lElementBitSize;
									l_lChannel %= l_lElementBitSize;
									l_pvData[l_lPort].vt = l_pvData[0].vt;
									unsigned long l_lMask = 1 << l_lChannel;
									if( ( l_lValue1 & l_lMask  ) > 0 )
									{
										l_pvData[l_lPort].lVal |= l_lMask;
									}
									else
									{
										l_pvData[l_lPort].lVal &= ~l_lMask;
									}
								}
							}
						}
						else
						{
							hr = GetValueObjectData( &l_pvData[l_lCurrentPLCDataIndex ].iVal, l_lNumElements, l_pValueObject, l_lOutputIndex );
							l_lCurrentPLCDataIndex++ ;
						}
					}
					else
					{
						switch( l_FirstDataType )  // PLC data type from the first plc address.
						{
							case SV_PLC_STRING:
							{
								double l_dValue = 0;
								hr = l_pValueObject->GetValue( l_dValue );
								if( hr == S_OK )
								{
									CString l_strTmp;
									hr = GetValueObjectDataConvert2String( l_strTmp, l_pValueObject, l_lOutputIndex );
									l_strStringValues.push_back(l_strTmp);
									l_pStringValues[i] = static_cast<LPCTSTR>(l_strStringValues[i]); // need a temperary pointer to the strings.
								}
								break;
							}

							case SV_PLC_BIT:
							case SV_PLC_INT16: 
							{
								l_pvData[l_lCurrentPLCDataIndex ].vt = VT_I2;
								if( l_pOutput->GetConvert() ) // convert to The target type
								{
									double l_dValue = 0;
									hr = l_pValueObject->GetValue( l_lOutputIndex, l_dValue );
									if( hr == S_OK )
									{
										long l_lValue = static_cast<long>( l_dValue );
										l_pvData[l_lCurrentPLCDataIndex ].iVal = static_cast<short>( l_lValue);
										l_lCurrentPLCDataIndex++ ;
										for( int i = 1 ; i < l_lNumElements ; i++ )
										{
											l_pvData[l_lCurrentPLCDataIndex ].vt = VT_I2;
											l_pvData[l_lCurrentPLCDataIndex].iVal = static_cast<short>( l_lValue >> (16 * i ));
											l_lCurrentPLCDataIndex++ ;
										}
									}
								}
								else
								{
									hr = GetValueObjectData( l_pTmpBuf, l_lNumElements * 2, l_pValueObject, l_lOutputIndex );
									short* l_pwData = (short*) l_pTmpBuf;
									for( int k = 0 ; k < l_lNumElements ; k++ )
									{
										l_pvData[l_lCurrentPLCDataIndex ].vt = VT_I2;
										l_pvData[l_lCurrentPLCDataIndex ].iVal = *(l_pwData + k);
										l_lCurrentPLCDataIndex++;
									}
								}
								break;
							}
							case SV_PLC_INT32:
							{
								l_pvData[l_lCurrentPLCDataIndex ].vt = VT_I4;
								if( l_pOutput->GetConvert() )
								{
									double l_dValue = 0;
									hr = l_pValueObject->GetValue( l_lOutputIndex, l_dValue );
									if( hr == S_OK )
									{
										long l_lValue = static_cast<long>( l_dValue );
										l_pvData[l_lCurrentPLCDataIndex ].lVal = l_lValue;
										l_lCurrentPLCDataIndex++ ;
										for( int i = 1 ; i < l_lNumElements ; i++ )
										{
											l_pvData[l_lCurrentPLCDataIndex ].vt = VT_I4;
											l_pvData[l_lCurrentPLCDataIndex ].lVal = l_lValue >> (32 * i );
											l_lCurrentPLCDataIndex++ ;
										}
									}
								}
								else
								{
									hr = GetValueObjectData( l_pTmpBuf, l_lNumElements * 4, l_pValueObject, l_lOutputIndex );
									long* l_plData = (long*) l_pTmpBuf;
									for( int k = 0 ; k < l_lNumElements ; k++ )
									{
										l_pvData[l_lCurrentPLCDataIndex ].vt = VT_I4;
										l_pvData[l_lCurrentPLCDataIndex ].lVal = *(l_plData + k);
										l_lCurrentPLCDataIndex++ ;
									}
								}
								break;
							}
							
							case SV_PLC_SPFLOAT:
							{
								l_pvData[l_lCurrentPLCDataIndex ].vt = VT_R4;
								if( l_pOutput->GetConvert() )
								{
									double l_dValue = 0;
									hr = l_pValueObject->GetValue( l_lOutputIndex, l_dValue );
									if( hr == S_OK )
									{
										l_pvData[l_lCurrentPLCDataIndex ].fltVal = static_cast<float>(l_dValue);
										l_lCurrentPLCDataIndex++ ;
										for( int i = 1 ; i < l_lNumElements ; i++ )
										{
											l_pvData[l_lCurrentPLCDataIndex ].vt = VT_R4;
											l_pvData[l_lCurrentPLCDataIndex].fltVal = 0.0;
											l_lCurrentPLCDataIndex++ ;
										}
									}
								}
								else
								{
									hr = GetValueObjectData( l_pTmpBuf, l_lNumElements * 4, l_pValueObject, l_lOutputIndex );
									float* l_pfltData = (float*) l_pTmpBuf;
									for( int k = 0 ; k < l_lNumElements ; k++ )
									{
										l_pvData[l_lCurrentPLCDataIndex ].vt = VT_R4;
										l_pvData[l_lCurrentPLCDataIndex ].fltVal = *(l_pfltData + k);
										l_lCurrentPLCDataIndex++ ;
									}
								}
								break;
							}
							case SV_PLC_DPFLOAT:
							{
								l_pvData[ l_lCurrentPLCDataIndex ].vt = VT_R8;
								if( l_pOutput->GetConvert() )
								{
									double l_dValue = 0;
									hr = l_pValueObject->GetValue( l_lOutputIndex, l_dValue );
									if( hr == S_OK )
									{
										l_pvData[l_lCurrentPLCDataIndex ].dblVal = l_dValue;
										l_lCurrentPLCDataIndex++ ;
										for( int i = 1 ; i < l_lNumElements ; i++ )
										{
											l_pvData[l_lCurrentPLCDataIndex ].vt = VT_R8;
											l_pvData[l_lCurrentPLCDataIndex ].dblVal = 0.0;
											l_lCurrentPLCDataIndex++ ;
										}
									}
								}
								else
								{
									hr = GetValueObjectData( l_pTmpBuf, l_lNumElements * 8, l_pValueObject, l_lOutputIndex );
									double* l_pdData = (double*) l_pTmpBuf;
									for( int k = 0 ; k < l_lNumElements ; k++ )
									{
										l_pvData[l_lCurrentPLCDataIndex ].vt = VT_R8;
										l_pvData[l_lCurrentPLCDataIndex ].dblVal = *(l_pdData + k);
										l_lCurrentPLCDataIndex++;
									}
								}
								break;
							}
						}
					}
				}
			}

			// Get address pointers
			for( size_t i = 0 ; i < l_strAddresses.size() ; i++ )
			{
				l_pAddressStrings[i] = static_cast<LPCTSTR>(l_strAddresses[i]);
			}


			switch( l_FirstDataType )
			{
				case SV_PLC_STRING:
				{
					hr = TheSVObserverApp.m_PLCManager.SendStringBlock( p_strPLCName, 
						(LPCSTR*)&l_pAddressStrings[0], 
						(LPCSTR*)&l_pStringValues[0], 
						l_strAddresses.size() );
					break;
				}

				case SV_PLC_BIT:
				case SV_PLC_INT16:
				{
					hr = TheSVObserverApp.m_PLCManager.SendIntBlock( p_strPLCName, 
						(LPCSTR*)&l_pAddressStrings[0], 
						&l_pvData[0], 
						l_strAddresses.size());
					break;
				}
				case SV_PLC_INT32:
				{
					hr = TheSVObserverApp.m_PLCManager.SendIntBlock( p_strPLCName, 
						(LPCSTR*)&l_pAddressStrings[0], 
						&l_pvData[0], 
						l_strAddresses.size());
					break;
				}
				
				case SV_PLC_SPFLOAT:
				case SV_PLC_DPFLOAT:
				{
					hr = TheSVObserverApp.m_PLCManager.SendIntBlock( p_strPLCName, 
						(LPCSTR*)&l_pAddressStrings[0], 
						&l_pvData[0], 
						l_strAddresses.size());
					break;
				}
			}
		}
		if( hr == S_OK )
		{
			l_PLCPar.m_lStartingAddressCounter++;
		}
		else
		{
	#ifdef _DEBUG
			CString strTmp;
			strTmp.Format(_T("Return Code from PLCDataController Send function %d"), hr );
			OutputDebugString(strTmp);
	#endif
		}
	}
	return hr;

} // end WriteOutputs(...


// This function will get value in raw data from the given value object.
// Supply the value object
// Supply storage and allowable max size.
HRESULT SVPLCDataController::GetValueObjectData( void* p_pMem, long p_lNumBytes, SVValueObjectClass* p_pValueObject, long p_lProductIndex )
{
	HRESULT hr = S_OK;

	SVObjectTypeEnum l_eObjectType = p_pValueObject->GetObjectType();

	memset( p_pMem, 0, p_lNumBytes ); // Clear the plc raw data..

	switch( l_eObjectType )
	{
		case SVDWordValueObjectType: // 4 bytes
		{
			SVDWordValueObjectClass* l_vo = dynamic_cast<SVDWordValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				DWORD l_dwValue;
				l_vo->GetValue( p_lProductIndex, l_dwValue );
				size_t l_size = p_lNumBytes >= sizeof( DWORD ) ? sizeof(DWORD) : p_lNumBytes;
				memcpy(p_pMem, &l_dwValue, l_size );
			}
			break;
		}
		case SVLongValueObjectType:  // 4 bytes
		{
			SVLongValueObjectClass* l_vo = dynamic_cast<SVLongValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				long l_lValue;
				l_vo->GetValue( p_lProductIndex, l_lValue );
				size_t l_size = p_lNumBytes >= sizeof( long ) ? sizeof(long) : p_lNumBytes;
				memcpy(p_pMem, &l_lValue, l_size );
			}
			break;
		}

		case SVDoubleValueObjectType:	// float = 4 bytes
		{								// Convert the double to a float for AB
			SVDoubleValueObjectClass* l_vo = dynamic_cast<SVDoubleValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				double l_dValue;
				l_vo->GetValue( p_lProductIndex, l_dValue );
				float l_fValue = static_cast<float>(l_dValue);
				size_t l_size = p_lNumBytes >= sizeof( float ) ? sizeof(float) : p_lNumBytes;
				memcpy(p_pMem, &l_fValue, l_size );
			}
			break;
		}
		case SVBoolValueObjectType:	 // 1 byte
		{
			SVBoolValueObjectClass* l_vo = dynamic_cast<SVBoolValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				bool l_bValue;
				l_vo->GetValue( p_lProductIndex, l_bValue );
				size_t l_size = p_lNumBytes >= sizeof( bool ) ? sizeof( bool ) : p_lNumBytes;
				memcpy(p_pMem, &l_bValue, l_size );
			}
			break;
		}
		case SVPointValueObjectType: // 4 Words
		{
			SVPointValueObjectClass* l_vo = dynamic_cast<SVPointValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				POINT l_ptValue;
				l_vo->GetValue( p_lProductIndex, l_ptValue );
				size_t l_size = p_lNumBytes >= sizeof( POINT ) ? sizeof( POINT ) : p_lNumBytes;
				memcpy(p_pMem, &l_ptValue, l_size );
			}
			break;
		}
		case SVByteValueObjectType:  // 1 Byte
		{
			SVByteValueObjectClass* l_vo = dynamic_cast<SVByteValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				BYTE l_ucValue;
				l_vo->GetValue( p_lProductIndex, l_ucValue );
				size_t l_size = p_lNumBytes >= sizeof( BYTE ) ? sizeof( BYTE ) : p_lNumBytes;
				memcpy(p_pMem, &l_ucValue, l_size );
			}
			break;
		}
		case SVStringValueObjectType: // Up to 42 Words
		{
			SVStringValueObjectClass* l_vo = dynamic_cast<SVStringValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				CString l_strValue;
				l_vo->GetValue( p_lProductIndex, l_strValue );
				size_t l_size = p_lNumBytes >= l_strValue.GetLength() ? l_strValue.GetLength() : p_lNumBytes;
				l_size = (l_size + 1) & 0xfe; // Round up to the next even number.
				LPCTSTR l_pChar = static_cast<LPCTSTR>(l_strValue);
				memcpy(p_pMem, l_pChar, l_size);
			}
			break;
		}
		case SVEnumValueObjectType: // 2 Words
		{
			SVEnumerateValueObjectClass* l_vo = dynamic_cast<SVEnumerateValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				long l_lValue;
				l_vo->GetValue( p_lProductIndex, l_lValue );
				size_t l_size = p_lNumBytes >= sizeof( long ) ? sizeof( long ) : p_lNumBytes;
				memcpy(p_pMem, &l_lValue, l_size );
			}
			break;
		}
		case SVDPointValueObjectType: // 8 Words
		{
			SVDPointValueObjectClass* l_vo = dynamic_cast<SVDPointValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				SVDPointClass l_dptValue;
				POINT l_ptValue;
				l_vo->GetValue( p_lProductIndex, l_dptValue );
				l_dptValue.GetValue( l_ptValue );
				size_t l_size = p_lNumBytes >= sizeof( POINT ) ? sizeof( POINT ) : p_lNumBytes;
				memcpy(p_pMem, &l_ptValue, l_size );
			}
			break;
		}
		case SVInt64ValueObjectType: // 8 Words
		{
			SVInt64ValueObjectClass* l_vo = dynamic_cast<SVInt64ValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				__int64 l_i64Value;
				l_vo->GetValue( p_lProductIndex, l_i64Value );
				size_t l_size = p_lNumBytes >= sizeof( l_i64Value ) ? sizeof( l_i64Value ) : p_lNumBytes;
				memcpy(p_pMem, &l_i64Value, l_size );
			}
			break;
		}
		case SVCharValueObjectType: // 1 Byte
		{
			SVCharValueObjectClass* l_vo = dynamic_cast<SVCharValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				char l_cValue;
				l_vo->GetValue( p_lProductIndex, l_cValue );
				size_t l_size = p_lNumBytes >= sizeof( char ) ? sizeof( char ) : p_lNumBytes;
				memcpy(p_pMem, &l_cValue, l_size );
			}
			break;
		}
		case SVVariantValueObjectType: // ???
		{
			SVVariantValueObjectClass* l_vo = dynamic_cast<SVVariantValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				VARIANT l_vValue;
				l_vo->GetValue( p_lProductIndex, l_vValue );
				// Check the type of the variant....
				switch( l_vValue.vt )
				{
					case VT_I4:
					{
						size_t l_size = p_lNumBytes >= sizeof( long ) ? sizeof( long ) : p_lNumBytes;
						memcpy(p_pMem, &l_vValue.lVal, l_size );
						break;
					}
					case VT_I8:
					{
						size_t l_size = p_lNumBytes >= sizeof( __int64 ) ? sizeof( __int64 ) : p_lNumBytes;
						memcpy(p_pMem, &l_vValue.llVal, l_size );
						break;
					}
					case VT_R4:
					{
						size_t l_size = p_lNumBytes >= sizeof( float ) ? sizeof( float ) : p_lNumBytes;
						memcpy(p_pMem, &l_vValue.fltVal, l_size );
						break;
					}
					case VT_R8:
					{
						size_t l_size = p_lNumBytes >= sizeof( double ) ? sizeof( double ) : p_lNumBytes;
						memcpy(p_pMem, &l_vValue.dblVal, l_size );
						break;
					}
					case VT_BSTR:
					{
						_bstr_t l_bstTmp(l_vValue.bstrVal);
						const char* l_buf = l_bstTmp;
						size_t l_size = static_cast<size_t>(p_lNumBytes) >= strlen( l_buf ) ? strlen( l_buf ) : static_cast<size_t>(p_lNumBytes);
						memcpy( p_pMem, &l_buf, l_size );
						break;
					}
					default:
					{
						hr = S_FALSE;
						OutputDebugString(_T("Variant Type conversion Not Implemented"));
						break;
					}
				}
			}
			break;
		}
		default:
		{
			hr = S_FALSE;
			OutputDebugString(_T("Object Type Conversion Not Implemented"));
			break;
		}
	}
	return hr;
}


// This function will get the value from the given value object and convert it to a string.
// Supply the value object, and product index
// Supply a SVString.
HRESULT SVPLCDataController::GetValueObjectDataConvert2String( CString& p_rStringOut, SVValueObjectClass* p_pValueObject, long p_lProductIndex )
{
	HRESULT hr = S_OK;

	SVObjectTypeEnum l_eObjectType = p_pValueObject->GetObjectType();

	switch( l_eObjectType )
	{
		case SVDWordValueObjectType:
		{
			SVDWordValueObjectClass* l_vo = dynamic_cast<SVDWordValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				DWORD l_dwValue;
				l_vo->GetValue( p_lProductIndex, l_dwValue );
				p_rStringOut.Format(_T("%ld"), l_dwValue);
			}
			break;
		}
		case SVLongValueObjectType:
		{
			SVLongValueObjectClass* l_vo = dynamic_cast<SVLongValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				long l_lValue;
				l_vo->GetValue( p_lProductIndex, l_lValue );
				p_rStringOut.Format(_T("%ld"), l_lValue);
			}
			break;
		}

		case SVDoubleValueObjectType:
		{
			SVDoubleValueObjectClass* l_vo = dynamic_cast<SVDoubleValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				double l_dValue;
				l_vo->GetValue( p_lProductIndex, l_dValue );
				p_rStringOut.Format(_T("%f"), l_dValue);
			}
			break;
		}
		case SVBoolValueObjectType:
		{
			SVBoolValueObjectClass* l_vo = dynamic_cast<SVBoolValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				bool l_bValue;
				l_vo->GetValue( p_lProductIndex, l_bValue );
				p_rStringOut.Format(_T("%s"), l_bValue ? _T("True") : _T("False") );
			}
			break;
		}
		case SVPointValueObjectType:
		{
			SVPointValueObjectClass* l_vo = dynamic_cast<SVPointValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				l_vo->GetValue( p_lProductIndex, p_rStringOut );
			}
			break;
		}
		case SVByteValueObjectType:  
		{
			SVByteValueObjectClass* l_vo = dynamic_cast<SVByteValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				BYTE l_ucValue;
				l_vo->GetValue( p_lProductIndex, l_ucValue );
				p_rStringOut.Format(_T("%d"), l_ucValue );
			}
			break;
		}
		case SVStringValueObjectType: 
		{
			SVStringValueObjectClass* l_vo = dynamic_cast<SVStringValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				l_vo->GetValue( p_lProductIndex, p_rStringOut );
			}
			break;
		}
		case SVEnumValueObjectType:
		{
			SVEnumerateValueObjectClass* l_vo = dynamic_cast<SVEnumerateValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				l_vo->GetValue( p_lProductIndex, p_rStringOut );
			}
			break;
		}
		case SVDPointValueObjectType:
		{
			SVDPointValueObjectClass* l_vo = dynamic_cast<SVDPointValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				l_vo->GetValue( p_lProductIndex, p_rStringOut );
			}
			break;
		}
		case SVInt64ValueObjectType:
		{
			SVInt64ValueObjectClass* l_vo = dynamic_cast<SVInt64ValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				l_vo->GetValue( p_lProductIndex, p_rStringOut );
			}
			break;
		}
		case SVCharValueObjectType:
		{
			SVCharValueObjectClass* l_vo = dynamic_cast<SVCharValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				l_vo->GetValue( p_lProductIndex, p_rStringOut );
			}
			break;
		}
		case SVVariantValueObjectType:
		{
			SVVariantValueObjectClass* l_vo = dynamic_cast<SVVariantValueObjectClass*>(p_pValueObject);
			if( l_vo )
			{
				l_vo->GetValue( p_lProductIndex, p_rStringOut );
			}
			break;
		}
		default:
		{
			hr = S_FALSE;
			OutputDebugString(_T("Object Type Conversion Not Implemented"));
			break;
		}
	}
	return hr;
}



enum CheckStateEnum
{
	CSFileType = 0,
	CSFileNum,
	CSSeperator,
	CSElement,
	CSValidAddress,
	CSBadAddress,
};

// Gets the last PLCObject with this PPQ.
SVPLCOutputObject* SVPLCDataController::GetLastObject( const CString& p_strPLCId )
{
	return m_PLCParameters[p_strPLCId].GetLastObject();

}

// Gets the last PLCObject with this PPQ.
SVPLCOutputObject* SVPLCDataController::GetFirstObject( const CString& p_strPLCId )
{
	return m_PLCParameters[p_strPLCId].GetFirstObject();
}


// Temperary way to connect PPQ_1 to PLC_1 ect...
CString SVPLCDataController::AssociatePPQToPLC( const CString& p_strPPQ )
{
	// Temperary Connection of PPQ to PLC
	CString strPLCName;
	long lSize = p_strPPQ.GetLength();
	if( lSize > 4 )
	{
		long lNum = atol( p_strPPQ.Mid(4) );
		strPLCName.Format( "PLC_%d", lNum );
	}
	else
	{
		ASSERT(0);
		strPLCName = "PLC_1";
	}
	return strPLCName;
}

// This function fills a list with PLC Ids that are setup in the plc data controller.
HRESULT SVPLCDataController::GetPLCs( std::vector<CString>& p_astrPLCIds )
{
	HRESULT l_hr = S_OK;

	p_astrPLCIds.clear();

	SVPLCControlParMap::iterator l_it;
	for( l_it = m_PLCParameters.begin() ; l_it != m_PLCParameters.end() ; ++l_it )
	{
		p_astrPLCIds.push_back( l_it->first );
	}

	return l_hr;
}

// This function fills a list with the PPQ names that are associated with plcs that are setup.
HRESULT SVPLCDataController::GetPPQs( std::vector<CString>& p_astrPLCIds, SVConfigurationObject* pConfig )
{
	HRESULT l_hr = S_OK;

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		l_hr = S_FALSE;
		return l_hr;
	}

	p_astrPLCIds.clear();

	SVPLCControlParMap::iterator l_it;
	for( l_it = m_PLCParameters.begin() ; l_it != m_PLCParameters.end() ; ++l_it )
	{
		long l_lPPQCount;
		pConfig->GetPPQCount( l_lPPQCount );
		CString l_strPLCID = l_it->first;
		for( int i = 0 ; i < l_lPPQCount ; i++ )
		{
			SVPPQObject* pPPQ( nullptr );
			pConfig->GetPPQ(i, &pPPQ );
			if( nullptr != pPPQ )
			{
				if( pPPQ->GetPLCName() == l_strPLCID )
				{
					p_astrPLCIds.push_back( pPPQ->GetName() );
					break;
				}
			}
		}
	}
	return l_hr;
}

// This function is used to tell if the PLC Outputs screen should be hidden
bool SVPLCDataController::IsEmpty()
{
	return m_PLCParameters.empty() ;
}

// Get Block Count gets the number of PLC blocks for the given PLCID.
long SVPLCDataController::GetBlockCount( const CString& p_strPLC )
{
	long lBlockCount=-1;
	if( m_PLCParameters.end() != m_PLCParameters.find( p_strPLC ) )
	{
		lBlockCount =  m_PLCParameters[p_strPLC].GetBlockCount();
	}
	return lBlockCount;
}

// Sets the PLC Block count also known as "PLC Transfer block count"
HRESULT SVPLCDataController::SetBlockCount( const CString& p_strPLC, long p_plNewCount )
{
	HRESULT l_hr = S_FALSE;
	if( m_PLCParameters.end() != m_PLCParameters.find( p_strPLC ) )
	{
		m_PLCParameters[p_strPLC].SetBlockCount( p_plNewCount ) ;
		l_hr = S_OK;
	}
	return l_hr;
}

// Gets the queue size used to limit the max amount of APC queuing
// This is a setting that gets sent to the PLC dll.
long SVPLCDataController::GetQueueSize( const CString& p_strPLC )
{
	long lQueueSize=-1;
	if( m_PLCParameters.end() != m_PLCParameters.find( p_strPLC ) )
	{
		lQueueSize =  m_PLCParameters[p_strPLC].GetQueueSize();
	}
	return lQueueSize;
}

// Sets the Queue size.
HRESULT SVPLCDataController::SetQueueSize( const CString& p_strPLC, long p_plNewSize )
{
	HRESULT l_hr = S_FALSE;
	if( m_PLCParameters.end() != m_PLCParameters.find( p_strPLC ) )
	{
		m_PLCParameters[p_strPLC].SetQueueSize( p_plNewSize ) ;
		l_hr = S_OK;
	}
	return l_hr;
}

// This function delets a PLC output and removes it from the plc control parameters class.
HRESULT SVPLCDataController::DeletePLCEntry( const CString& p_strPLCId, SVPLCOutputObject* p_pOutputObject )
{
	HRESULT l_hr;
	l_hr = m_PLCParameters[p_strPLCId].Delete( p_pOutputObject );
	return l_hr;
}

// Delete PLC deletes the entire PLC control parameters class.
HRESULT SVPLCDataController::DeletePLC( const CString& p_strPLCId )
{
	HRESULT l_hr = S_FALSE;
	SVPLCControlParMap::iterator l_it;
	l_it = m_PLCParameters.find( p_strPLCId );
	if( l_it != m_PLCParameters.end() )
	{
		l_it->second.Destroy();
		m_PLCParameters.erase( l_it );
		l_hr = S_OK;
	}

	TheSVObserverApp.m_PLCManager.DestroyPLC( p_strPLCId );
	if( m_PLCParameters.empty() )
	{
		TheSVObserverApp.m_PLCManager.ClosePLC();
	}
	return l_hr;
}

// Returns the count of plcs that are defined.
size_t SVPLCDataController::GetPLCCount() const
{
	return m_PLCParameters.size();
}

// Gets the heartbeat address that is used for the plc dll.
HRESULT SVPLCDataController::GetHeartBeatAddress( CString& p_strHeartBeatAddress )
{
	HRESULT l_hr = S_OK ;
	p_strHeartBeatAddress = m_strHeartBeatAddress;
	return l_hr;
}

// Sets the heartbeat address parameter.
HRESULT SVPLCDataController::SetHeartBeatAddress( CString p_strHeartBeatAddress )
{
	HRESULT l_hr = S_OK ;
	m_strHeartBeatAddress = p_strHeartBeatAddress;
	return l_hr;
}

// Gets the heartbeat time parameter.
HRESULT SVPLCDataController::GetHeartBeatTime( long& p_lTime )
{
	p_lTime = m_lHeartBeatTime;
	return S_OK;
}

// Sets the heartbeat time parameter.
HRESULT SVPLCDataController::SetHeartBeatTime( long p_lTime )
{
	HRESULT l_hr = -3130 ;
	if( p_lTime > 0 && p_lTime < 120 )
	{
		m_lHeartBeatTime = p_lTime;
		l_hr = S_OK;
	}
	return S_OK;
}

// Gets the connection string.
LPCSTR SVPLCDataController::GetConnectString( )
{
	return m_szPLCConnectionString;
}

// Sets the connection String. Fixed size of 256 characters.
HRESULT SVPLCDataController::SetConnectString( LPCSTR p_strConnection )
{
	strcpy_s( m_szPLCConnectionString, 256, p_strConnection);
	return S_OK;
}


// Gets a suggested next element based on the last Element and the size.
bool SVPLCDataController::GetNextValidElement( long& p_lNewElement, SVPLCOutputObject* p_pPLCOutput )
{
	bool l_bRet = false;
	if( p_pPLCOutput != NULL )
	{
		long l_lElement = p_pPLCOutput->GetElement() ;
		if( p_pPLCOutput->SizeInBits() )
		{
			p_lNewElement = l_lElement + ( p_pPLCOutput->GetBit() + p_pPLCOutput->GetSize() + 15 )/16;
		}
		else
		{
			p_lNewElement = l_lElement + p_pPLCOutput->GetSize();
		}
		l_bRet = true;
	}
	return l_bRet;
}


// After a ppq is deleted this function cleans up data from the map.
HRESULT SVPLCDataController::ClearUnUsedData( )
{
	long l_lCount = 0;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	//If the pointer is a nullptr then the count is 0
	if( nullptr != pConfig ) {pConfig->GetPPQCount( l_lCount ); }
	
	std::vector<CString > l_strPLCs;
	for( long l_lIndex = 0 ; l_lIndex < l_lCount ; l_lIndex++ )
	{
		SVPPQObject* pPPQ( nullptr );
		pConfig->GetPPQ(l_lIndex, &pPPQ );
		if( nullptr != pPPQ )
		{
			l_strPLCs.push_back( pPPQ->GetPLCName() );
		}
	}

	SVPLCControlParMap::iterator l_it = m_PLCParameters.begin();
	while( l_it != m_PLCParameters.end() )
	{
		if( std::find( l_strPLCs.begin(),l_strPLCs.end(), l_it->first ) == l_strPLCs.end() )
		{
			l_it = m_PLCParameters.erase( l_it );
		}
		else
		{
			++l_it;
		}
	}

	return S_OK;
}

// This function modifies the address by adding the offset to the string address. "PLC Specific"
HRESULT SVPLCDataController::AddOffsetToAddress(  CString& p_strAddress, long p_lOffset, long p_lBit )
{
	char buf[128];
	strcpy_s(buf, 128, static_cast<LPCTSTR>(p_strAddress) );

	HRESULT l_hr = TheSVObserverApp.m_PLCManager.AddOffsetToAddress( buf, p_lOffset, p_lBit );

	if( l_hr == S_OK )
	{
		p_strAddress = buf;
	}
	return l_hr;
}

// Gets data type from an address.  "PLC Specific"
SVPLCDataTypeEnum SVPLCDataController::GetPLCDataType( const CString& l_strAddress )
{
	long l_lType = 0;
	long l_lSize = 0;
	TheSVObserverApp.m_PLCManager.GetDataType( &l_lSize, &l_lType, l_strAddress );
	return static_cast<SVPLCDataTypeEnum> (l_lType);
}

// Gets port and bit info from an address.  "PLC Specific"
HRESULT SVPLCDataController::GetInfoFromAddress( long& p_rlFileNum, long& p_rlElement, long& p_rlBitNum, const CString& p_rStrAddress )
{
	HRESULT l_hr = TheSVObserverApp.m_PLCManager.GetInfoFromAddress( &p_rlFileNum, &p_rlElement, &p_rlBitNum, p_rStrAddress );
	return S_OK;
}

// Adds the trigger count if the plc parameter map is empty
HRESULT SVPLCDataController::AddDefaultOutputs(SVPPQObject* pPPQ )
{
	HRESULT l_hr = S_FALSE;
	if( nullptr != pPPQ )
	{
		SVPLCOutputObject* l_pNewOutput = NULL;
		CString l_strPLCName = pPPQ->GetPLCName();
		if( m_PLCParameters.find( l_strPLCName ) == m_PLCParameters.end() )
		{
			CString l_strAddress;
			long l_lSize;
			long l_lType;
			TheSVObserverApp.m_PLCManager.GetDefaultPLCAddress( l_strAddress );
			TheSVObserverApp.m_PLCManager.GetDataType( &l_lSize, &l_lType, l_strAddress );
			l_lSize = 4 / l_lSize;
			if( l_lSize < 1)
			{
				l_lSize = 1;
			}
			AddItem( l_strPLCName, l_pNewOutput, 0, l_lSize , pPPQ->m_voTriggerCount.GetUniqueObjectID(), 0, false );
			// Add default block address
			// Get Default plc address from plc dll.
			SVPLCControlPar l_Pars = GetControlPar( l_strPLCName );
			SVPLCControlPar::VecStringArray l_astrAddresses;
			l_Pars.GetBlockAddresses( l_astrAddresses );
			l_astrAddresses.push_back( l_strAddress );
			l_Pars.SetBlockAddresses( l_astrAddresses );
			SetControlPar( l_strPLCName, l_Pars );
		}
		l_hr = S_OK;
	}
	return l_hr;
}

// Get List index returns the index for a specific PLC Output.
HRESULT SVPLCDataController::GetListIndex( SVPLCOutputObject* p_pOutput, const CString& p_strPLCName, long& p_rlIndex )
{
	HRESULT l_hr = S_FALSE;
	SVPLCControlParMap::iterator l_it;
	l_it = m_PLCParameters.find( p_strPLCName );
	if( l_it != m_PLCParameters.end() )
	{
		l_hr = l_it->second.Find( p_pOutput, p_rlIndex );
	}
	else
	{
		p_rlIndex = -1;
	}
	return l_hr;
}

// This is the element size in bits.
long SVPLCDataController::GetElementBitSize( const CString& p_strPLC )
{
	long lElementBitSize=16;
	if( m_PLCParameters.end() != m_PLCParameters.find( p_strPLC ) )
	{
		lElementBitSize =  m_PLCParameters[p_strPLC].GetElementBitSize();
	}
	return lElementBitSize;
}

// This function walks the input list and attempts to validate each input GUID 
// by getting a pointer from the Object Manager for each GUID.
// If an input fails then it is deleted.
HRESULT SVPLCDataController::ValidateInputs()
{
	HRESULT l_hr = S_OK;
	SVPLCControlParMap::iterator l_it = m_PLCParameters.begin();
	for( ; l_it!= m_PLCParameters.end() ; ++l_it )
	{
		SVPLCControlPar& l_rCntlPars = l_it->second;
		for( int i = l_rCntlPars.GetElementBitSize()-1 ; i > -1  ; i-- )
		{
			SVPLCOutputObject* l_pOutput = l_rCntlPars.GetItem( i );
			if( l_pOutput )
			{
				GUID l_GUID;
				::KeepPrevError( l_hr, l_pOutput->GetInputValueObjectGUID( l_GUID ));
				SVObjectClass* l_pInputVO = SVObjectManagerClass::Instance().GetObject( l_GUID );
				if( l_pInputVO == NULL )
				{
					OutputDebugString(_T("Input Deleted \n"));
					KeepPrevError( l_hr, l_rCntlPars.Delete( l_pOutput ));
				}
			}
			else
			{
				KeepPrevError( l_hr, l_rCntlPars.Delete( l_pOutput ));
			}
		}
	}
	return S_OK;
}

// Returns the flag that remembers the state of the GUI PLCOutput List.
// This flag is not saved with the configuration at this time.
bool SVPLCDataController::GetCollapse( const CString& p_strPLCId )
{
	bool l_bCollapse=false;
	if( m_PLCParameters.end() != m_PLCParameters.find( p_strPLCId ) )
	{
		l_bCollapse =  m_PLCParameters[p_strPLCId].GetCollapse();
	}
	return l_bCollapse;
}

// Sets the flag for expanding / collapsing the PLC Outputs list
void SVPLCDataController::SetCollapse( const CString& p_strPLCId, bool p_bCollapse )
{
	if( m_PLCParameters.end() != m_PLCParameters.find( p_strPLCId ) )
	{
		m_PLCParameters[p_strPLCId].SetCollapse(p_bCollapse);
	}
}

// Calculates new element Size based on the plc element size and
// the SVObserver data type.
// The default element size will be 1.
long SVPLCDataController::CalcNewElementSize( const CString& l_strPLCID, SVObjectTypeEnum p_eType )
{
	long l_Size = GetElementBitSize( l_strPLCID ) /8 ;

	if( l_Size <= 0 )
	{	// Safty...
		l_Size = 1;
	}

	long l_ElementSize = 1;
	switch( p_eType )
	{
		case SVDWordValueObjectType:
		case SVLongValueObjectType:
		{
			l_ElementSize = 4 / l_Size;
			break;
		}
		case SVDoubleValueObjectType :
		{
			// FR00100.0112 first converts to a 32bit long
			// Therefore the size starts at 4 bytes
			l_ElementSize = 4 / l_Size;
			break;
		}
		case SVBoolValueObjectType :
		{
			l_ElementSize = 1;
			break;
		}
		case SVPointValueObjectType :
		{
			l_ElementSize = 8 / l_Size;
			break;
		}
		case SVByteValueObjectType :
		{
			l_ElementSize = 1;
			break;
		}
		case SVStringValueObjectType :
		{
			l_ElementSize = 1;
			break;
		}
		case SVEnumValueObjectType :
		{
			l_ElementSize = 1;
			break;
		}
		case SVDPointValueObjectType :
		{
			// double points are converted to longs then 
			// sent.
			l_ElementSize = 8 / l_Size;
			break;
		}
		case SVInt64ValueObjectType :
		{
			l_ElementSize = 8 / l_Size;
			break;
		}
		case SVCharValueObjectType :
		{
			l_ElementSize = 1;
			break;
		}
	}
	if( l_ElementSize < 1 )
	{
		l_ElementSize = 1;
	}
	return l_ElementSize;
}


void SVPLCDataController::SetupPLC(SVConfigurationObject* pConfig)
{
	HRESULT l_hr = S_OK;

	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return;
	}
	// Add PLC Wizzard
	// Setup Connection
	char szConnection[257];
	SVPLCManager& l_rPLCManager = TheSVObserverApp.m_PLCManager;

	CStringVec l_OriginalList;
	GetPPQs( l_OriginalList, pConfig );

	if( !l_rPLCManager.IsOpen() )
	{
		CString l_strTmp;
		l_hr = l_rPLCManager.OpenPLC(TheSVObserverApp.GetPLCDLL());	// Default PLC...
		l_rPLCManager.Startup( pConfig );
	}

	if( l_hr == S_OK )
	{
		VARIANT l_vValue;
		CString l_strValue;
		// Get the data from the storage and put it in the dll
		// HeartBeat Address
		l_hr = GetHeartBeatAddress( l_strValue );
		if( l_hr == S_OK )
		{
			_bstr_t l_bstValue( l_strValue );
			l_vValue.bstrVal = l_bstValue.Detach();
			l_vValue.vt = VT_BSTR;
			l_rPLCManager.SetParameterValue( "", 1, l_vValue );
		}
		// HeartBeat Time.
		long l_lTime;
		KeepPrevError( l_hr, GetHeartBeatTime( l_lTime ));
		if( l_hr == S_OK )
		{
			l_vValue.lVal = l_lTime;
			l_vValue.vt = VT_I4;
			l_hr = l_rPLCManager.SetParameterValue( "", 2, l_vValue );
		}

		strcpy_s( szConnection, 256, GetConnectString() );
		KeepPrevError( l_hr, l_rPLCManager.SetupConnection( szConnection, 256 ));
		if( l_hr == S_OK )
		{
			if( l_hr == S_OK )
			{
				// HeartBeat Address >> Data Controller.
				::VariantInit( &l_vValue );
				HRESULT l_hr = l_rPLCManager.GetParameterValue( "",1, &l_vValue);
				ASSERT( l_hr == S_OK );
				if( l_hr == S_OK )
				{
					if( l_vValue.vt == VT_BSTR )
					{
						_bstr_t l_bstVal;
						l_bstVal.Attach( l_vValue.bstrVal );
						l_strValue = static_cast<const char * >(l_bstVal);
						SetHeartBeatAddress( l_strValue );
					}

				}
				// HeartBeat Time >> Data Controller
				::VariantInit( &l_vValue );
				KeepPrevError( l_hr, l_rPLCManager.GetParameterValue( "", 2, &l_vValue));
				ASSERT( l_hr == S_OK );
				if( l_hr == S_OK )
				{
					if( l_vValue.vt == VT_I4 )
					{
						SetHeartBeatTime( l_vValue.lVal );
					}
				}

				// If the plc parameters change then we must destroy all logical plcs.
				bool l_bPLCParametersChanged = false;
				if( strcmp( szConnection, GetConnectString() ) != 0 )
				{
					l_bPLCParametersChanged = true;

					// Destroy all PLCs
					l_OriginalList.clear();
				}

				// PLC Setup String
				KeepPrevError( l_hr, SetConnectString( szConnection ));

				if( l_hr == S_OK )
				{
					SetupPLCPPQs( pConfig, l_OriginalList );

					if( l_bPLCParametersChanged )
					{
						// Destroy all PLCs
						l_rPLCManager.Shutdown( );
					}
				}

				l_hr = l_rPLCManager.Startup( pConfig );

				if( l_hr == S_OK )
				{
					TheSVObserverApp.GetIODoc()->SetModifiedFlag();
					if( IsEmpty() )
					{
						TheSVObserverApp.HidePLCTab();
					}
					else
					{
						TheSVObserverApp.ShowIOTab( SVIOPLCOutputsViewID );
						// Set Active IO Tabbed view to the PLC Outputs Tab
						TheSVObserverApp.SetActiveIOTabView( SVIOPLCOutputsViewID );
					}
					TheSVObserverApp.OnUpdateAllIOViews();
				}
			}
		}
	}
}

void SVPLCDataController::SetupPLCPPQs(SVConfigurationObject* pConfig, CStringVec& p_rOriginalList )
{
	//Without valid config pointer don't need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return;
	}
	// these containers hold the list of ppq names that will be used for plcs.
	CStringVec l_AvailablePPQs;
	// Initialize PPQ - PLCs by selecting from dialog.
	long l_lPPQSize;
	pConfig->GetPPQCount( l_lPPQSize );
	for( long l = 0 ; l < l_lPPQSize ; l++ )
	{
		SVPPQObject* pPPQ( nullptr );
		BOOL l_bTmp = pConfig->GetPPQ(l, &pPPQ );
		if( l_bTmp )
		{
			l_AvailablePPQs.push_back( pPPQ->GetName() );
		}
	}
	// Setup PPQs to have PLCs
	CStringVec l_astrCurrentPLCs;

	SVPLCAddRemoveDlg l_dlg;
	l_dlg.m_astrAvailablePPQs = l_AvailablePPQs;
	l_dlg.m_astrSetupPPQs = p_rOriginalList;
	if( l_dlg.DoModal() == IDOK )
	{
		CStringVec l_astrNewItems;
		CStringVec l_astrRemovedItems;
		// Get a list of what was removed.
		l_dlg.GetNewItems( l_astrNewItems );
		l_dlg.GetRemoved( l_astrRemovedItems );


		// Add New PLCs associated with PPQs
		for( size_t i = 0; i < l_astrNewItems.size() ; i++ )
		{
			for( long l = 0 ; l < l_lPPQSize ; l++ )
			{
				SVPPQObject* pPPQ( nullptr );
				BOOL l_bTmp = pConfig->GetPPQ(l, &pPPQ );
				if( l_bTmp )
				{
					if( pPPQ->GetName() == l_astrNewItems[i] )
					{
						AddDefaultOutputs( pPPQ );
					}
				}
			}
		}

		// Remove Items
		for( size_t i = 0; i < l_astrRemovedItems.size() ; i++ )
		{
			for( long l = 0 ; l < l_lPPQSize ; l++ )
			{
				SVPPQObject* pPPQ( nullptr );
				BOOL l_bTmp = pConfig->GetPPQ(l, &pPPQ );
				if( l_bTmp )
				{
					if( pPPQ->GetName() == l_astrRemovedItems[i] )
					{
						DeletePLC( pPPQ->GetPLCName() );
					}
				}
			}
		}

	}

}
#endif


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPLCDataController.cpp_v  $
 * 
 *    Rev 1.1   30 Oct 2013 11:00:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to remove deprecated code from the 64bit solution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:24:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   11 Oct 2012 10:59:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 Sep 2012 15:24:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   18 Sep 2012 18:29:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   04 Sep 2012 15:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   19 Jun 2012 13:57:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Jan 2012 11:47:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   13 Dec 2010 11:28:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Dec 2010 13:25:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   05 Nov 2010 11:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   30 Aug 2010 13:51:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 12:25:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jul 2009 12:23:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Feb 2009 13:59:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SetParameters so it will not create a plc if there are no outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jan 2009 11:51:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made plc ID parameters const reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Jan 2009 16:11:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved Edit / Add PLC Wizard logic to data controller class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jan 2009 08:47:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved current Address Counter variable from the SVPLCDataController to the SVPLCControlPar class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Dec 2008 13:05:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in WriteOutputs function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Dec 2008 12:59:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added comments and fixed some error handling mistakes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Dec 2008 09:40:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change function name in SVPLCDataController to make its function more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:08:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

