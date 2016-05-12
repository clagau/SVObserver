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
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <iterator>
#include "SVRemoteOutputDataController.h"

#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVXMLLibrary/SVNavigateTree.h"
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
	
	return l_Status;
}

// Destroy iterates through the remote group parameters map and destroys all remote group parameters 
// Then it clears the map.
void SVRemoteOutputDataController::Destroy()
{
	// Destroys Remote Group Parameters.
	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		delete( l_it->second);
	}
	m_RemoteGroupParameters.clear();
}

// AddItem creates a new RemoteOutputObject and adds it to the remote group parameters
HRESULT SVRemoteOutputDataController::AddItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ )
{
	HRESULT l_hr = E_FAIL;
	SVRemoteOutputObject* l_pEntry = new SVRemoteOutputObject;
	if( l_pEntry )
	{
		l_pEntry->SetInputObjectId( p_InputObjectID );
		l_pEntry->SetGroupID( SVString(p_strRemoteGroupId) );
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
			l_par->SetPPQName( SVString(p_strPPQ) );
			m_RemoteGroupParameters[p_strRemoteGroupId] = l_par;
		}
		else
		{
			// Add new output to Remote Output Control Parameters map.
			m_RemoteGroupParameters[ p_strRemoteGroupId ]->AddOutput( l_pEntry );
			m_RemoteGroupParameters[ p_strRemoteGroupId ]->SetPPQName( SVString(p_strPPQ) );
		}
		p_pNewOutput = l_pEntry;
		l_hr = S_OK;
	}
	return l_hr;
}

// This gives us a way to get the data associated with a Remote Group Id.
SVRemoteOutputGroup* SVRemoteOutputDataController::GetControlPar( const CString& p_strGroupID )
{
	SVRemoteOutputGroup* l_pPars = nullptr;
	if( m_RemoteGroupParameters.find( p_strGroupID ) != m_RemoteGroupParameters.end() )
	{
		l_pPars = m_RemoteGroupParameters[p_strGroupID];
	}
	return l_pPars;
}

// Get the RemoteOutputGroupPar associated with the Remote Group Id.
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

// Get the element Count from the SVRemoteOutputGroup for the given Remote Group Id
size_t SVRemoteOutputDataController::GetItemCount( const CString& p_strRemoteGroup )
{
	size_t l_lSize = 0;
	if( m_RemoteGroupParameters.find( p_strRemoteGroup ) != m_RemoteGroupParameters.end() )
	{
		l_lSize = m_RemoteGroupParameters[p_strRemoteGroup]->RemoteOutputListSize();
	}
	return l_lSize;
}

// Get a RemoteOutputObject based on index from the given Remote Group Id
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
bool SVRemoteOutputDataController::GetParameters( SVObjectXMLWriter& rWriter )
{
	_variant_t svVariant;

	ClearUnUsedData();	// clears unused remote data

	if( m_RemoteGroupParameters.size() > 0 )
	{
		rWriter.StartElement( CTAG_REMOTE_OUTPUT_PARAMETERS );
		
		svVariant = SVGUID( m_outObjectInfo.UniqueObjectID ).ToVARIANT();
		rWriter.WriteAttribute(  CTAG_UNIQUE_REFERENCE_ID, svVariant );

		// Remote Output Parameters
		SVRemoteOutputGroupMap::iterator l_it;

		long lIndex = 0;
		for( l_it = m_RemoteGroupParameters.begin(); l_it != m_RemoteGroupParameters.end() ; ++l_it )
		{
			CString l_strBranch;

			lIndex++;
			l_strBranch.Format( "%s_%d",CTAG_REMOTE_GROUP_ID, lIndex );
			rWriter.StartElement( l_strBranch );
			l_it->second->GetParameters( rWriter );
			rWriter.EndElement();
		}
		rWriter.EndElement();
	}

	return true;
}

// Tree >> Parameters ( Restore from archive )
BOOL SVRemoteOutputDataController::SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent )
{
	BOOL bOk = TRUE;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiIORemoteOutput = nullptr;

	BOOL l_bTmp = SVNavigateTree::GetItemBranch( p_rTree, CTAG_REMOTE_OUTPUT_PARAMETERS,p_htiParent, htiIORemoteOutput ) ;

	if( l_bTmp )
	{
		bOk = SVNavigateTree::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiIORemoteOutput, svVariant );
		if ( bOk )
		{
			SVOutputStreamManager::Instance().EraseOutputController();

			SVGUID ObjectID( svVariant );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			m_outObjectInfo.UniqueObjectID = ObjectID;

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
				SVTreeType::SVBranchHandle htiBranch = nullptr;
				CString l_strEntry;
				CString l_strGroupID;

				l_strEntry.Format( "%s_%d", CTAG_REMOTE_GROUP_ID, ++l_lEntryNum );
				l_bTmp = SVNavigateTree::GetItemBranch( p_rTree, l_strEntry, htiIORemoteOutput, htiBranch );

				if ( l_bTmp )
				{
					SVRemoteOutputGroup* l_ControlParameter = new SVRemoteOutputGroup;
					l_bTmp = l_ControlParameter->SetParameters( p_rTree, htiBranch );
					SVRemoteOutputObject* l_pRemOut = l_ControlParameter->GetFirstObject();
					if( l_pRemOut )
					{
						l_strGroupID = l_pRemOut->GetGroupID().c_str();
					}
					if( l_bTmp && nullptr != l_pRemOut )
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

// Write Outputs with the supplied PPQ name.
HRESULT SVRemoteOutputDataController::WriteOutputs( const CString& p_strRemoteGroupID, SVProductInfoStruct *pProduct)
{
	HRESULT hr = S_FALSE;

	if( pProduct && (m_RemoteGroupParameters.find(p_strRemoteGroupID) != m_RemoteGroupParameters.end()) )
	{
		long l_lOutputIndex=-1;
		l_lOutputIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();
		std::vector<CString> l_strAddresses;  // place to collect address strings
		std::vector<CString> l_strStringValues;

		// parameters for a Remote Output Group
		SVRemoteOutputGroup* l_RemoteGroupPar = m_RemoteGroupParameters[p_strRemoteGroupID];

		//  Implement Write Function....
		OutputDebugString(_T("Write Outputs Function\n"));
	}
	return hr;

} // end WriteOutputs(...

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
HRESULT SVRemoteOutputDataController::GetPPQs( std::vector<CString>& p_astrPPQs, SVConfigurationObject* pConfig )
{
	HRESULT l_hr = S_OK;

	//Only ASSERT not to change runtime
	ASSERT( nullptr != pConfig );

	p_astrPPQs.clear();

	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		long l_lPPQCount = pConfig->GetPPQCount( );
		CString l_strRemoteOutputID = l_it->first;
		for( int i = 0 ; i < l_lPPQCount ; i++ )
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if( nullptr != pPPQ )
			{
				if( SVString( pPPQ->GetName() ) == l_it->second->GetPPQName() )
				{
					p_astrPPQs.push_back( pPPQ->GetName() );
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


// This function is used to tell if the Remote Outputs screen should be hidden
bool SVRemoteOutputDataController::IsEmpty()
{
	return m_RemoteGroupParameters.empty() ;
}

// This function deletes a remote output and removes it from the Remote Group control parameters class.
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
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if( nullptr != pConfig )
	{
		long l_lCount = pConfig->GetPPQCount();
		typedef std::pair<SVString, SVGUID> PPQInfo;
		typedef std::deque<PPQInfo> PPQInfoList;
		PPQInfoList l_PPQInfos;
		for( long l_lIndex = 0 ; l_lIndex < l_lCount ; l_lIndex++ )
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(l_lIndex);
			if( nullptr != pPPQ )
			{
				l_PPQInfos.push_back( std::make_pair(pPPQ->GetName(), pPPQ->GetUniqueObjectID()) );
			}
		}
	
		// Find dead PPQs...
		for( SVRemoteOutputGroupMap::iterator l_it = m_RemoteGroupParameters.begin(); l_it != m_RemoteGroupParameters.end(); )
		{
			// match PPQ name and GUID, because it could have been deleted and re-added...
			PPQInfo ppqInfo(std::make_pair(l_it->second->GetPPQName(), l_it->second->GetPPQObjectId()));
			PPQInfoList::const_iterator l_ppqIt = std::find_if( l_PPQInfos.begin(), l_PPQInfos.end(), [&ppqInfo](const PPQInfo& rInfo)->bool
			{
				bool bRetVal = (ppqInfo.first == rInfo.first); 
				bRetVal = bRetVal && (ppqInfo.second == rInfo.second);
				return bRetVal;
			} );
			if (l_ppqIt == l_PPQInfos.end())
			{
				CString l_strTmp;
				l_strTmp.Format(_T("Deleting Remote Output Group associated with PPQ %s\n"), ppqInfo.first.c_str() );
				OutputDebugString(l_strTmp);
				l_it = m_RemoteGroupParameters.erase(l_it);
			}
			else
			{
				++l_it;
			}
		}
		if( m_RemoteGroupParameters.empty() )
		{
			// Hide the Remote Output Tab if no outputs exist.
			TheSVObserverApp.HideRemoteOutputTab( );
		}
		TheSVObserverApp.OnUpdateAllIOViews(); // updates the view after clearing unused.
	}
	return S_OK;
}

// Adds the trigger count if the parameter map is empty
HRESULT SVRemoteOutputDataController::AddDefaultOutputs(CString p_strRemoteGroupID, SVPPQObject* pPPQ )
{
	HRESULT l_hr = S_FALSE;
	ASSERT( nullptr != pPPQ );
	if( nullptr == pPPQ)
	{
		return l_hr;
	}

	if( !p_strRemoteGroupID.IsEmpty() )
	{
		if( m_RemoteGroupParameters.find( p_strRemoteGroupID ) == m_RemoteGroupParameters.end() )
		{
			SVRemoteOutputObject* l_pNewOutput = nullptr;
			
			AddItem( p_strRemoteGroupID, l_pNewOutput, pPPQ->m_voTriggerCount.GetUniqueObjectID(), pPPQ->GetName() );
		}
		l_hr = S_OK;
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
				if( nullptr == l_pInputVO )
				{
					l_pInputVO = SVObjectManagerClass::Instance().GetObjectCompleteName(l_pOutput->GetInputValueObjectName().c_str());
					if( nullptr == l_pInputVO )
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

void SVRemoteOutputDataController::SetupRemoteOutput(SVConfigurationObject* pConfig)
{
	HRESULT l_hr = S_OK;

	//Only ASSERT not to change runtime
	ASSERT( nullptr != pConfig );

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

	if( S_OK == l_hr )
	{
		SetupRemoteOutputGroup(pConfig, l_OriginalList );

		if( S_OK == l_hr )
		{
			TheSVObserverApp.GetIODoc()->SetModifiedFlag();
			if( IsEmpty() )
			{
				TheSVObserverApp.HideIOTab( SVRemoteOutputsViewID );
			}
			else
			{
				TheSVObserverApp.ShowIOTab( SVRemoteOutputsViewID );
				// Set Active IO Tabbed view to the Remote Outputs Tab
				TheSVObserverApp.SetActiveIOTabView( SVRemoteOutputsViewID );
			}
			TheSVObserverApp.OnUpdateAllIOViews();
		}
	}
}

void SVRemoteOutputDataController::SetupRemoteOutputGroup(SVConfigurationObject* pConfig, SVGroupDefVect& p_rOriginalList )
{
	//If pConfig is nullptr then no need to go further
	ASSERT( nullptr != pConfig );
	if( nullptr == pConfig )
	{
		return;
	}

	SVSVIMStateClass::AddState(SV_STATE_EDITING);
	// these containers hold the list of ppq names that will be used for Remote Groups.
	CStringVec l_AvailablePPQs;
	// Initialize PPQ - Remote Groups by selecting from dialog.
	long l_lPPQSize = pConfig->GetPPQCount( );
	for( long l = 0 ; l < l_lPPQSize ; l++ )
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(l);
		if( nullptr != pPPQ )
		{
			l_AvailablePPQs.push_back( pPPQ->GetName() );
		}
	}


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
					SVPPQObject* pPPQ( nullptr );
					BOOL l_bTmp = pConfig->GetChildObjectByName(l_dlg.m_SetupGroup[j].m_strPPQ, &pPPQ );
					if( l_bTmp )
					{
						AddDefaultOutputs(l_astrNewItems[i], pPPQ );
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
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

bool SVRemoteOutputDataController::RenameGroup( CString oldName, CString newName )
{
	bool l_bRet = false;
	SVRemoteOutputGroupMap::iterator l_it = m_RemoteGroupParameters.find( oldName );
	if( l_it != m_RemoteGroupParameters.end() )
	{
		m_RemoteGroupParameters[newName] = l_it->second;

		m_RemoteGroupParameters[newName]->SetGroupName( SVString(newName) );
		m_RemoteGroupParameters.erase( l_it );
		l_bRet = true;
	}
	return l_bRet;
}


