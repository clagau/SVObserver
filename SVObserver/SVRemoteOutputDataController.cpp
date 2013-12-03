//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputDataController
//* .File Name       : $Workfile:   SVRemoteOutputDataController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:58:46  $
//******************************************************************************

#include "stdafx.h"
#include <algorithm>
#include <comdef.h>
#include <iterator>
#include "SVRemoteOutputDataController.h"

#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVObserver.h"
#include "SVOutputStreamManager.h"
#include "SVSVIMStateClass.h"

SV_IMPLEMENT_CLASS( SVRemoteOutputDataController, SVRemoteOutputDataControllerGUID );

SVRemoteOutputDataController::SVRemoteOutputDataController( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
{
	SVOutputStreamManager::Instance().InsertOutputController( GetUniqueObjectID() );
}

SVRemoteOutputDataController::SVRemoteOutputDataController( SVObjectClass *pOwner, int StringResourceID )
: SVObjectClass( pOwner, StringResourceID )
{
	SVOutputStreamManager::Instance().InsertOutputController( GetUniqueObjectID() );
}

SVRemoteOutputDataController::~SVRemoteOutputDataController()
{
	SVOutputStreamManager::Instance().EraseOutputController();

	Destroy();
}

//SEJ - What is this for???
HRESULT SVRemoteOutputDataController::ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr )
{
	HRESULT l_Status = E_FAIL;

	/*
	Json::Reader l_Reader;
	Json::Value l_JsonValues;
	SVString l_Command;

	if( l_Reader.parse( p_rDataPtr->m_JsonCommand, l_JsonValues, false ) )
	{
		if( l_JsonValues.isObject() )
		{
			Json::Value l_JsonCommand = l_JsonValues[ _T( "Command" ) ];

			if( l_JsonCommand.isString() )
			{
				l_Command = l_JsonCommand.asString();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( l_Status == S_OK )
	{
		if( l_Command == _T( "QueryStreamNames" ) )
		{
			Json::FastWriter l_Writer;
			Json::Value l_Object(Json::objectValue);
			Json::Value l_Array(Json::arrayValue);

			if( !( m_InputStreams.empty() ) )
			{
				SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

				l_AutoLock.Assign( &m_Lock );

				SVRemoteOutputGroupMap::iterator l_Iter = m_RemoteGroupParameters.begin();

				while( l_Iter != m_RemoteGroupParameters.end() )
				{
					l_Array.append( static_cast< LPCTSTR >( l_Iter->first ) );

					++l_Iter;
				}
			}

			l_Object[ _T( "StreamNames" ) ] = l_Array;

			p_rJsonResults = l_Writer.write( l_Object ).c_str();
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	p_rDataPtr->NotifyRequestComplete();
	*/

	return l_Status;
}

// Destroy iterates through the PLC control parameter map and destroys all instances of the PLC.
// Then it clears the map.
void SVRemoteOutputDataController::Destroy()
{
	// Destroys PLC Control Parameters.
	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		delete( l_it->second);
	}
	m_RemoteGroupParameters.clear();
}

// AddItem creates a new RemoteOutputObject and adds it to the PLC SVRemoteOutputGroup
HRESULT SVRemoteOutputDataController::AddItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ )
{
	HRESULT l_hr = E_FAIL;
	SVRemoteOutputObject* l_pEntry = new SVRemoteOutputObject;
	if( l_pEntry )
	{
		l_pEntry->SetInputObjectId( p_InputObjectID );
		l_pEntry->SetGroupID( p_strRemoteGroupId );
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectA( p_InputObjectID );
		if( l_pObject )
		{
			SVValueObjectClass* l_pValueObject = dynamic_cast<SVValueObjectClass*>( l_pObject );
			if( l_pValueObject )
			{
				if( l_pValueObject->GetObjectType() == SVStringValueObjectType )
				{
					// l_bConvert = false;
				}
			}
		}
		else
		{
			OutputDebugString(_T("Added an Remote Output that had an invalid input!\n"));
			CString strTmp;
			strTmp.Format(_T("Added an Remote Output that had an invalid input!\n"));
			strTmp += l_pObject->GetCompleteObjectName();
			OutputDebugString( strTmp );
		}
		if( m_RemoteGroupParameters.find( p_strRemoteGroupId) == m_RemoteGroupParameters.end() )
		{
			SVRemoteOutputGroup* l_par = new SVRemoteOutputGroup(p_strRemoteGroupId);

			l_par->AddOutput( l_pEntry );
			l_par->SetPPQName( p_strPPQ );
			m_RemoteGroupParameters[p_strRemoteGroupId] = l_par;
		}
		else
		{
			// Add new output to Remote Output Control Parameters map.
			m_RemoteGroupParameters[ p_strRemoteGroupId ]->AddOutput( l_pEntry );
			m_RemoteGroupParameters[ p_strRemoteGroupId ]->SetPPQName( p_strPPQ );
		}
		p_pNewOutput = l_pEntry;
		l_hr = S_OK;
	}
	return l_hr;
}

// This gives us a way to get the data associated with a Remote Group Id.
SVRemoteOutputGroup* SVRemoteOutputDataController::GetControlPar( const CString& p_strGroupID )
{
	SVRemoteOutputGroup* l_pPars = NULL;
	if( m_RemoteGroupParameters.find( p_strGroupID ) != m_RemoteGroupParameters.end() )
	{
		l_pPars = m_RemoteGroupParameters[p_strGroupID];
	}
	return l_pPars;
}

// Gets the data associated with a plc into a SVMaterials.
HRESULT SVRemoteOutputDataController::GetRemoteOutputControlData( SVMaterials& p_rMaterials, const CString& p_strGroupID )
{
	HRESULT l_hr = S_FALSE;
	l_hr = m_RemoteGroupParameters[p_strGroupID]->GetData( p_rMaterials );
	return l_hr;
}

// Sets the PLCControlPar with a SVmaterials data.
HRESULT SVRemoteOutputDataController::SetRemoteOutputControlData( SVMaterials& p_rMaterials, const CString& p_strRemoteGroup )
{
	return m_RemoteGroupParameters[p_strRemoteGroup]->SetData( p_rMaterials );
}

// Get the PLCControlPar associated with the PLCID.
HRESULT SVRemoteOutputDataController::GetControlPar( const CString& p_strRemoteGroup, SVRemoteOutputGroup*& p_pControl )
{
	HRESULT l_hr = S_FALSE;
	if( m_RemoteGroupParameters.find( p_strRemoteGroup ) != m_RemoteGroupParameters.end() )
	{
		p_pControl = m_RemoteGroupParameters[p_strRemoteGroup];
		l_hr = S_OK;
	}
	return l_hr;
}

// Set the Control Parameters Map with the SVRemoteOutputGroup at the supplied plcID.
HRESULT SVRemoteOutputDataController::SetControlPar( const CString& p_strRemoteGroup, SVRemoteOutputGroup* p_PLCControl )
{
	HRESULT l_hr = S_FALSE;
	m_RemoteGroupParameters[p_strRemoteGroup] = p_PLCControl;
	l_hr = S_OK;
	return l_hr;
}

// Get the element Count from the SVRemoteOutputGroup for the given PLCID
size_t SVRemoteOutputDataController::GetItemCount( const CString& p_strRemoteGroup )
{
	size_t l_lSize = 0;
	if( m_RemoteGroupParameters.find( p_strRemoteGroup ) != m_RemoteGroupParameters.end() )
	{
		l_lSize = m_RemoteGroupParameters[p_strRemoteGroup]->RemoteOutputListSize();
	}
	return l_lSize;
}

// Get a RemoteOutputObject based on index from the given PLCId.
HRESULT SVRemoteOutputDataController::GetItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem )
{
	HRESULT l_hr = -3130;
	if( m_RemoteGroupParameters.find( p_strRemoteGroupId ) != m_RemoteGroupParameters.end() )
	{
		l_hr = m_RemoteGroupParameters[p_strRemoteGroupId]->GetItem( l_lIndex, p_rItem );
	}
	return l_hr;
}

// Parameters >> Tree ( Save )
BOOL SVRemoteOutputDataController::GetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent )
{
	BOOL bOk = FALSE;
	_variant_t svVariant;

	ClearUnUsedData();	// clears unused plc data

	SVTreeType::SVBranchHandle htiIORemoteGroup = NULL;
	if( m_RemoteGroupParameters.size() > 0 )
	{
		if ( SVNavigateTreeClass::SetBranch( p_rTree, p_htiParent, CTAG_REMOTE_OUTPUT_PARAMETERS, &htiIORemoteGroup ) )
		{
			svVariant = SVGUID( outObjectInfo.UniqueObjectID ).ToVARIANT();
			SVNavigateTreeClass::AddItem( p_rTree, htiIORemoteGroup, CTAG_UNIQUE_REFERENCE_ID, svVariant );

			// Remote Output Parameters
			SVRemoteOutputGroupMap::iterator l_it;

			long lIndex = 0;
			for( l_it = m_RemoteGroupParameters.begin(); l_it != m_RemoteGroupParameters.end() ; ++l_it )
			{
				SVTreeType::SVBranchHandle htiRemoteOutputEntry = NULL;
				CString l_strBranch;

				lIndex++;
				l_strBranch.Format( "%s_%d",CTAG_REMOTE_GROUP_ID, lIndex );
				if ( SVNavigateTreeClass::SetBranch( p_rTree, htiIORemoteGroup, l_strBranch, &htiRemoteOutputEntry ) )
				{
					l_it->second->GetParameters( p_rTree, htiRemoteOutputEntry );
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
BOOL SVRemoteOutputDataController::SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent )
{
	BOOL bOk = TRUE;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiIORemoteOutput = NULL;

	BOOL l_bTmp = SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_REMOTE_OUTPUT_PARAMETERS,p_htiParent, htiIORemoteOutput ) ;

	if( l_bTmp )
	{
		bOk = SVNavigateTreeClass::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiIORemoteOutput, svVariant );
		if ( bOk )
		{
			SVOutputStreamManager::Instance().EraseOutputController();

			SVGUID ObjectID( svVariant );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			outObjectInfo.UniqueObjectID = ObjectID;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( this );

			SVOutputStreamManager::Instance().InsertOutputController( GetUniqueObjectID() );
		}

		// Remote Output Parameters
		if( bOk )
		{
			BOOL l_bTmp = TRUE;
			long l_lEntryNum = 0;
			while( l_bTmp )
			{
				SVTreeType::SVBranchHandle htiBranch=NULL;
				CString l_strEntry;
				CString l_strGroupID;

				l_strEntry.Format( "%s_%d", CTAG_REMOTE_GROUP_ID, ++l_lEntryNum );
				l_bTmp = SVNavigateTreeClass::GetItemBranch( p_rTree, l_strEntry, htiIORemoteOutput, htiBranch );

				if ( l_bTmp )
				{
					SVRemoteOutputGroup* l_ControlParameter = new SVRemoteOutputGroup;
					l_bTmp = l_ControlParameter->SetParameters( p_rTree, htiBranch );
					SVRemoteOutputObject* l_pRemOut = l_ControlParameter->GetFirstObject();
					if( l_pRemOut )
					{
						l_strGroupID = l_pRemOut->GetGroupID().c_str();
					}
					if( l_bTmp && l_pRemOut != NULL )
					{
						m_RemoteGroupParameters[ l_strGroupID ] = l_ControlParameter;
					}
					l_ControlParameter->AttachStreamManager();
				}
			}
		}
	}

	return bOk;
}

// Add Branch is used to build a new branch into a MaterialsTreeAdapter.
HRESULT SVRemoteOutputDataController::AddBranch(SVMaterialsTreeAdapter& p_rTree, 
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
HRESULT SVRemoteOutputDataController::GetMaterials( SVMaterialsTreeAdapter& p_rMaterialsTree, 
										  SVMaterialsTreeAdapter::SVTreeContainer* p_pParent )
{
	HRESULT l_hr = S_OK;
	_variant_t l_vVariant;
	
	ClearUnUsedData();	// clears unused data

	SVMaterialsTreeAdapter::SVTreeContainer* l_pBranch = NULL;

	SVMaterials l_Materials;


	if ( AddBranch( p_rMaterialsTree, CTAG_REMOTE_OUTPUT_PARAMETERS, p_pParent, l_pBranch ) == S_OK )
	{
		SVMaterialsTreeAdapter::SVTreeElement l_Element( CTAG_REMOTE_OUTPUT_PARAMETERS, l_Materials );
		l_pBranch->insert(l_pBranch->end(), l_Element );

		// Remote Output Parameters
		SVRemoteOutputGroupMap::iterator l_it;

		long lIndex = 0;
		for( l_it = m_RemoteGroupParameters.begin(); l_it != m_RemoteGroupParameters.end() ; ++l_it )
		{
			CString l_strBranch;
			lIndex++;
			l_strBranch.Format( "%s_%d",CTAG_REMOTE_OUTPUT_PAR_NUM, lIndex );
			SVMaterialsTreeAdapter::SVTreeContainer* l_pSubBranch;

			if ( AddBranch( p_rMaterialsTree, l_strBranch, l_pBranch, l_pSubBranch ) == S_OK )
			{
				l_it->second->GetMaterials( p_rMaterialsTree,  l_pSubBranch);
			}
		}

	}
	return l_hr;
}

// Update updates this class from a materials tree.
HRESULT SVRemoteOutputDataController::Update( SVMaterialsTreeAdapter& p_rMaterialsTree )
{
	_variant_t l_vVariant;

	HRESULT l_hr = S_OK;
	// RemoteOutputDataController Object ID
	
	SVMaterialsTreeAdapter::iterator l_it = p_rMaterialsTree.find( CTAG_REMOTE_OUTPUT_PARAMETERS);
	if( l_it != p_rMaterialsTree.end() )
	{
		// Get the materials
		SVMaterials& l_rMaterials = l_it->second;

		// Unique GUID for this object.
		l_hr = l_rMaterials.GetMaterial( CTAG_UNIQUE_REFERENCE_ID, l_vVariant );
		SVString l_strTmp = l_vVariant;
		
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

				// Remote Group Parameters
				l_strEntry.Format( "%s_%d", CTAG_REMOTE_OUTPUT_PAR_NUM, ++l_lEntryNum );
				l_it = l_rmtaBranch.find( l_strEntry );
				if( l_it != l_rmtaBranch.end() )
				{
					l_pTreeContainer = l_it.GetChildTree();
					SVMaterialsTreeAdapter l_rmtaChannelBranch( *l_pTreeContainer );

					SVRemoteOutputGroup* l_ControlParameter = new SVRemoteOutputGroup;
					KeepPrevError( l_hr, l_ControlParameter->Update( l_rmtaChannelBranch ));
					if( l_hr == S_OK )
					{
						CString l_strGroup = l_ControlParameter->GetGroupName().c_str();
						m_RemoteGroupParameters[ l_strGroup ] = l_ControlParameter;
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




// Write Outputs with the supplied PPQ name.
HRESULT SVRemoteOutputDataController::WriteOutputs( const CString& p_strRemoteGroupID, SVProductInfoStruct *pProduct)
{
	HRESULT hr = S_FALSE;

	if( pProduct && (m_RemoteGroupParameters.find(p_strRemoteGroupID) != m_RemoteGroupParameters.end()) )
	{
		// BRW - PLC has been deprecated.
		//long l_lMaxDTSize = TheSVObserverApp.m_PLCManager.GetMaxDTSize();

		// BRW - l_pAddressStrings and l_pStringValues were never used again.
		/*std::vector<LPCTSTR> l_pAddressStrings;			// place to collect Address pointers
		std::vector<LPCTSTR> l_pStringValues;
		l_pAddressStrings.resize(l_lMaxDTSize);
		l_pStringValues.resize(l_lMaxDTSize);*/
		long l_lOutputIndex=-1;
		l_lOutputIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();
		std::vector<CString> l_strAddresses;  // place to collect address strings
		std::vector<CString> l_strStringValues;

//		char l_pTmpBuf[256];

		// BRW - l_pvData was never used again.
		/*std::vector<VARIANT> l_pvData;
		l_pvData.resize(l_lMaxDTSize);*/

		// parameters for a Remote Output Group
		SVRemoteOutputGroup* l_RemoteGroupPar = m_RemoteGroupParameters[p_strRemoteGroupID];

		//  Implement Write Function....
		OutputDebugString(_T("Write Outputs Function\n"));
	}
	return hr;

} // end WriteOutputs(...



// This function will get the value from the given value object and convert it to a string.
// Supply the value object, and product index
// Supply a SVString.
HRESULT SVRemoteOutputDataController::GetValueObjectDataConvert2String( CString& p_rStringOut, SVValueObjectClass* p_pValueObject, long p_lProductIndex )
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



// Gets the last remote output object with in this group.
SVRemoteOutputObject* SVRemoteOutputDataController::GetLastObject( const CString& p_strRemoteOutputName )
{
	return m_RemoteGroupParameters[p_strRemoteOutputName]->GetLastObject();
}

// Gets the last output object with in this group.
SVRemoteOutputObject* SVRemoteOutputDataController::GetFirstObject( const CString& p_strGroupId )
{
	return m_RemoteGroupParameters[p_strGroupId]->GetFirstObject();
}

// This function fills a list with group ids that are setup in the output data controller.
HRESULT SVRemoteOutputDataController::GetRemoteOutputGroups( std::vector<CString>& p_astrRemoteOutputGroupIds )
{
	HRESULT l_hr = S_OK;

	p_astrRemoteOutputGroupIds.clear();

	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		p_astrRemoteOutputGroupIds.push_back( l_it->first );
	}

	return l_hr;
}

// This function fills a list with the PPQ names that are associated with Groups that are setup.
HRESULT SVRemoteOutputDataController::GetPPQs( std::vector<CString>& p_astrPPQs, SVConfigurationObject* p_pConfig )
{
	HRESULT l_hr = S_OK;

	p_astrPPQs.clear();

	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		long l_lPPQCount;
		p_pConfig->GetPPQCount( l_lPPQCount );
		CString l_strRemoteOutputID = l_it->first;
		for( int i = 0 ; i < l_lPPQCount ; i++ )
		{
			SVPPQObject* l_pPPQ = NULL;
			p_pConfig->GetPPQ(i, &l_pPPQ );
			if( l_pPPQ )
			{
				if( l_pPPQ->GetName() == l_it->second->GetPPQName() )
				{
					p_astrPPQs.push_back( l_pPPQ->GetName() );
					break;
				}
			}
		}
	}
	return l_hr;
}

HRESULT SVRemoteOutputDataController::GetGroupNames( std::vector<CString>& p_astrPPQs )
{
	HRESULT l_hr = S_OK;
	p_astrPPQs.clear();

	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		p_astrPPQs.push_back( l_it->first );
	}
	return l_hr;
}


// This function is used to tell if the PLC Outputs screen should be hidden
bool SVRemoteOutputDataController::IsEmpty()
{
	return m_RemoteGroupParameters.empty() ;
}

// This function delets a PLC output and removes it from the plc control parameters class.
HRESULT SVRemoteOutputDataController::DeleteRemoteOutputEntry( const CString& p_strRemoteGroupId, SVRemoteOutputObject* p_pOutputObject )
{
	HRESULT l_hr;
	l_hr = m_RemoteGroupParameters[p_strRemoteGroupId]->Delete( p_pOutputObject );
	return l_hr;
}

// DeleteRemoteOutput deletes the entire RemoteOutput control parameters class.
HRESULT SVRemoteOutputDataController::DeleteRemoteOutput( const CString& p_strGroupId )
{
	HRESULT l_hr = S_FALSE;
	SVRemoteOutputGroupMap::iterator l_it;
	l_it = m_RemoteGroupParameters.find( p_strGroupId );
	if( l_it != m_RemoteGroupParameters.end() )
	{
		delete l_it->second;
		m_RemoteGroupParameters.erase( l_it );
		l_hr = S_OK;
	}

	if( m_RemoteGroupParameters.empty() )
	{
		// Hide the Remote Output Tab if no outputs exist.
		TheSVObserverApp.HideRemoteOutputTab( );
	}
	return l_hr;
}

// Returns the count of Remote Output Groups that are defined.
size_t SVRemoteOutputDataController::GetRemoteOutputGroupCount()
{
	return m_RemoteGroupParameters.size();
}



// After a ppq is deleted this function cleans up data from the map.
HRESULT SVRemoteOutputDataController::ClearUnUsedData( )
{
	long l_lCount =0; //GetRemoteOutputGroupCount( );
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	pConfig->GetPPQCount(l_lCount);
	std::vector<CString > l_strPPQs;
	for( long l_lIndex = 0 ; l_lIndex < l_lCount ; l_lIndex++ )
	{
		SVPPQObject* l_pPPQ = NULL;
		pConfig->GetPPQ(l_lIndex, &l_pPPQ );
		if( l_pPPQ != NULL )
		{
			l_strPPQs.push_back( l_pPPQ->GetName() );
		}
	}


	SVRemoteOutputGroupMap::iterator l_EraseIt = m_RemoteGroupParameters.end();

	for( SVRemoteOutputGroupMap::iterator l_it = m_RemoteGroupParameters.begin(); l_it != m_RemoteGroupParameters.end() ; ++l_it)
	{
		if( l_EraseIt != m_RemoteGroupParameters.end() )
		{
			m_RemoteGroupParameters.erase( l_EraseIt );
			l_EraseIt = m_RemoteGroupParameters.end();
		}
		if( std::find( l_strPPQs.begin(), l_strPPQs.end(), l_it->second->GetPPQName()  ) == l_strPPQs.end() )
		{
			CString l_strTmp;
			l_strTmp.Format(_T("Deleting Control Parameter associated with PPQ %s\n"), l_it->second->GetPPQName() );
			OutputDebugString(l_strTmp);
			l_EraseIt = l_it;
		}
	}

	if( l_EraseIt != m_RemoteGroupParameters.end() )
	{
		m_RemoteGroupParameters.erase( l_EraseIt );
	}
	TheSVObserverApp.OnUpdateAllIOViews(); // updates the view after clearing unused.
	
	return S_OK;
}


// Adds the trigger count if the plc parameter map is empty
HRESULT SVRemoteOutputDataController::AddDefaultOutputs(CString p_strRemoteGroupID, SVPPQObject* p_pPPQ )
{
	HRESULT l_hr = S_FALSE;
	if( !p_strRemoteGroupID.IsEmpty() )
	{
		if( m_RemoteGroupParameters.find( p_strRemoteGroupID ) == m_RemoteGroupParameters.end() )
		{
			SVRemoteOutputObject* l_pNewOutput = NULL;
			
			AddItem( p_strRemoteGroupID, l_pNewOutput, p_pPPQ->m_voTriggerCount.GetUniqueObjectID(), p_pPPQ->GetName() );

			//SVRemoteOutputGroup l_Pars = GetControlPar( p_strRemoteGroupID );

			//SetControlPar( p_strRemoteGroupID, l_Pars );
		}
		l_hr = S_OK;
	}
	return l_hr;
}

// Get List index returns the index for a specific PLC Output.
HRESULT SVRemoteOutputDataController::GetListIndex( SVRemoteOutputObject* p_pOutput, const CString& p_strRemoteOutputGroupName, long& p_rlIndex )
{
	HRESULT l_hr = S_FALSE;
	SVRemoteOutputGroupMap::iterator l_it;
	l_it = m_RemoteGroupParameters.find( p_strRemoteOutputGroupName );
	if( l_it != m_RemoteGroupParameters.end() )
	{
		l_hr = l_it->second->Find( p_pOutput, p_rlIndex );
	}
	else
	{
		p_rlIndex = -1;
	}
	return l_hr;
}

// This function walks the input list and attempts to validate each input GUID 
// by getting a pointer from the Object Manager for each GUID.
// If an input fails then it is deleted.
HRESULT SVRemoteOutputDataController::ValidateInputs()
{
	HRESULT l_hr = S_OK;
	SVRemoteOutputGroupMap::iterator l_it = m_RemoteGroupParameters.begin();
	for( ; l_it!= m_RemoteGroupParameters.end() ; ++l_it )
	{
		SVRemoteOutputGroup* l_pGroup = l_it->second;
		for( int i = static_cast<int>(l_pGroup->RemoteOutputListSize()-1) ; i > -1  ; i-- )
		{
			SVRemoteOutputObject* l_pOutput = l_pGroup->GetItem( i );
			if( l_pOutput )
			{
				GUID l_GUID;
				::KeepPrevError( l_hr, l_pOutput->GetInputValueObjectGUID( l_GUID ));
				SVObjectClass* l_pInputVO = SVObjectManagerClass::Instance().GetObject( l_GUID );
				if( l_pInputVO == NULL )
				{
					l_pInputVO = SVObjectManagerClass::Instance().GetObjectCompleteName(l_pOutput->GetInputValueObjectName().c_str());
					if( l_pInputVO == NULL  )
					{
						OutputDebugString(_T("Input Deleted \n"));
						KeepPrevError( l_hr, l_pGroup->Delete( l_pOutput ));
					}
				}
			}
			else
			{
				KeepPrevError( l_hr, l_pGroup->Delete( l_pOutput ));
			}
		}
	}
	return S_OK;
}

void SVRemoteOutputDataController::SetupRemoteOutput(SVConfigurationObject* p_pConfig)
{
	HRESULT l_hr = S_OK;

	SVGroupDefVect l_OriginalList;
	std::vector<CString> l_GroupOutputNames;
	l_hr = GetRemoteOutputGroups( l_GroupOutputNames );

	for( std::vector<CString>::iterator l_it = l_GroupOutputNames.begin() ; l_it != l_GroupOutputNames.end() ; ++l_it)
	{
		SVGroupDef l_grp;
		l_grp.m_strName = *l_it;
		SVRemoteOutputGroup* l_pControlPar = GetControlPar(*l_it);
		l_grp.m_strPPQ = l_pControlPar->GetPPQName().c_str();
		l_OriginalList.push_back( l_grp );
	}

	if( l_hr == S_OK )
	{
		SetupRemoteOutputGroup(p_pConfig, l_OriginalList );

		if( l_hr == S_OK )
		{
			TheSVObserverApp.GetIODoc()->SetModifiedFlag();
			if( IsEmpty() )
			{
				TheSVObserverApp.HideIOTab( SVRemoteOutputsViewID );
			}
			else
			{
				TheSVObserverApp.ShowIOTab( SVRemoteOutputsViewID );
				// Set Active IO Tabbed view to the PLC Outputs Tab
				TheSVObserverApp.SetActiveIOTabView( SVRemoteOutputsViewID );
			}
			TheSVObserverApp.OnUpdateAllIOViews();
		}
	}
}

void SVRemoteOutputDataController::SetupRemoteOutputGroup(SVConfigurationObject* p_pConfig, SVGroupDefVect& p_rOriginalList )
{
	// these containers hold the list of ppq names that will be used for plcs.
	CStringVec l_AvailablePPQs;
	// Initialize PPQ - PLCs by selecting from dialog.
	long l_lPPQSize;
	p_pConfig->GetPPQCount( l_lPPQSize );
	for( long l = 0 ; l < l_lPPQSize ; l++ )
	{
		SVPPQObject* l_pPPQ = NULL;
		BOOL l_bTmp = p_pConfig->GetPPQ(l, &l_pPPQ );
		if( l_bTmp )
		{
			l_AvailablePPQs.push_back( l_pPPQ->GetName() );
		}
	}
	// Setup PPQs to have Renmote Output Groups
	CStringVec l_astrCurrentPLCs;

	SVRemoteOutputGroupAddRemoveDlg l_dlg;
	l_dlg.m_astrAvailablePPQs = l_AvailablePPQs;
	l_dlg.m_SetupGroup = p_rOriginalList;
	if( l_dlg.DoModal() == IDOK )
	{
		CStringVec l_astrNewItems;
		CStringVec l_astrRemovedItems;
		CStringPairVect l_aRenamedItems;

		// Get a list of what was removed.
		l_dlg.GetNewItems( l_astrNewItems );
		l_dlg.GetRemoved( l_astrRemovedItems );
		l_dlg.GetRenamed( l_aRenamedItems );

		for( CStringPairVect::iterator l_it = l_aRenamedItems.begin() ; l_it != l_aRenamedItems.end() ; ++l_it )
		{
			RenameGroup( l_it->first, l_it->second );
			SVOutputStreamManager::Instance().Rename( l_it->first, l_it->second );
		}

		// Add New Output Groups
		for( size_t i = 0; i < l_astrNewItems.size() ; i++ )
		{
			for( size_t j = 0 ; j < l_dlg.m_SetupGroup.size() ; j++ )
			{
				if( l_dlg.m_SetupGroup[j].m_strName == l_astrNewItems[i] )
				{
					SVPPQObject* l_pPPQ = NULL;
					BOOL l_bTmp = p_pConfig->GetChildObjectByName(l_dlg.m_SetupGroup[j].m_strPPQ, &l_pPPQ );
					if( l_bTmp )
					{
						AddDefaultOutputs(l_astrNewItems[i], l_pPPQ );
					}
				}
			}
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}

		// Remove Items
		for( size_t i = 0; i < l_astrRemovedItems.size() ; i++ )
		{
			DeleteRemoteOutput( l_astrRemovedItems[i] );

			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}
	}
}

bool SVRemoteOutputDataController::RenameGroup( CString oldName, CString newName )
{
	bool l_bRet = false;
	SVRemoteOutputGroupMap::iterator l_it = m_RemoteGroupParameters.find( oldName );
	if( l_it != m_RemoteGroupParameters.end() )
	{
		m_RemoteGroupParameters[newName] = l_it->second;

		m_RemoteGroupParameters[newName]->SetGroupName( newName);
		m_RemoteGroupParameters.erase( l_it );
		l_bRet = true;
	}
	return l_bRet;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteOutputDataController.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 07:58:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:49:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   11 Oct 2012 10:59:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Oct 2012 11:19:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove initialization of leaf object to NULL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   25 Sep 2012 15:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Sep 2012 18:32:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Sep 2012 15:37:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   14 Aug 2012 15:23:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added missing delete to DeleteRemoteOutput.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Aug 2012 11:04:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix update issues with remote outputs view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Jul 2012 14:15:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jun 2012 14:18:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Apr 2012 11:37:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to call the rename function in the SVOutputStreamManager from the SetupRemoteOutputGroup function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2012 13:45:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added AttachStreamManager() to the SetParameters function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 10:16:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote output stream and communications to that remote output manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jan 2012 11:54:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

