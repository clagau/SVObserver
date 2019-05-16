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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <iterator>
#include "SVRemoteOutputDataController.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVRemoteOutputDataController, SVRemoteOutputDataControllerGUID );

SVRemoteOutputDataController::SVRemoteOutputDataController( LPCTSTR ObjectName )
: SVObjectClass( ObjectName )
{
}

SVRemoteOutputDataController::SVRemoteOutputDataController( SVObjectClass *pOwner, int StringResourceID )
: SVObjectClass( pOwner, StringResourceID )
{
}

SVRemoteOutputDataController::~SVRemoteOutputDataController()
{
	Destroy();
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
HRESULT SVRemoteOutputDataController::AddItem( const std::string& rRemoteGroupId, SVRemoteOutputObject*& pNewOutput, GUID InputObjectID, const std::string& rPPQ )
{
	HRESULT l_hr = E_FAIL;
	SVRemoteOutputObject* l_pEntry = new SVRemoteOutputObject;
	if( l_pEntry )
	{
		l_pEntry->SetInputObjectId( InputObjectID );
		l_pEntry->SetGroupID( rRemoteGroupId );
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( InputObjectID );
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
		if( nullptr == pValueObject )
		{
			std::string Temp( _T("Added an Remote Output that had an invalid input!\n") );
			Temp += pObject->GetCompleteName();
			OutputDebugString( Temp.c_str() );
		}
		if( m_RemoteGroupParameters.end() == m_RemoteGroupParameters.find( rRemoteGroupId ) )
		{
			SVRemoteOutputGroup* l_par = new SVRemoteOutputGroup( rRemoteGroupId.c_str() );

			l_par->AddOutput( l_pEntry );
			l_par->SetPPQName( rPPQ );
			m_RemoteGroupParameters[rRemoteGroupId] = l_par;
		}
		else
		{
			// Add new output to Remote Output Control Parameters map.
			m_RemoteGroupParameters[ rRemoteGroupId ]->AddOutput( l_pEntry );
			m_RemoteGroupParameters[ rRemoteGroupId ]->SetPPQName( rPPQ );
		}
		pNewOutput = l_pEntry;
		l_hr = S_OK;
	}
	return l_hr;
}

// This gives us a way to get the data associated with a Remote Group Id.
SVRemoteOutputGroup* SVRemoteOutputDataController::GetControlPar( const std::string& rRemoteGroupId )
{
	SVRemoteOutputGroup* l_pPars = nullptr;
	if( m_RemoteGroupParameters.end() != m_RemoteGroupParameters.find( rRemoteGroupId ) )
	{
		l_pPars = m_RemoteGroupParameters[rRemoteGroupId];
	}
	return l_pPars;
}

// Get the RemoteOutputGroupPar associated with the Remote Group Id.
HRESULT SVRemoteOutputDataController::GetControlPar( const std::string& rRemoteGroupId, SVRemoteOutputGroup*& p_pControl )
{
	HRESULT l_hr = S_FALSE;
	if( m_RemoteGroupParameters.end() != m_RemoteGroupParameters.find( rRemoteGroupId ) )
	{
		p_pControl = m_RemoteGroupParameters[rRemoteGroupId];
		l_hr = S_OK;
	}
	return l_hr;
}

// Get the element Count from the SVRemoteOutputGroup for the given Remote Group Id
size_t SVRemoteOutputDataController::GetItemCount( const std::string& rRemoteGroupId )
{
	size_t l_lSize = 0;
	if( m_RemoteGroupParameters.end() != m_RemoteGroupParameters.find( rRemoteGroupId ) )
	{
		l_lSize = m_RemoteGroupParameters[rRemoteGroupId]->RemoteOutputListSize();
	}
	return l_lSize;
}

// Get a RemoteOutputObject based on index from the given Remote Group Id
HRESULT SVRemoteOutputDataController::GetItem( const std::string& rRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem )
{
	HRESULT l_hr = -3130;
	if( m_RemoteGroupParameters.end() != m_RemoteGroupParameters.find( rRemoteGroupId ) )
	{
		l_hr = m_RemoteGroupParameters[rRemoteGroupId]->GetItem( l_lIndex, p_rItem );
	}
	return l_hr;
}

// Parameters >> Tree ( Save )
bool SVRemoteOutputDataController::GetParameters(SvXml::SVObjectXMLWriter& rWriter )
{
	_variant_t svVariant;

	ClearUnUsedData();	// clears unused remote data

	if( m_RemoteGroupParameters.size() > 0 )
	{
		rWriter.StartElement( SvXml::CTAG_REMOTE_OUTPUT_PARAMETERS );
		
		svVariant = m_outObjectInfo.getUniqueObjectID().ToVARIANT();
		rWriter.WriteAttribute(  SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant );

		// Remote Output Parameters
		SVRemoteOutputGroupMap::iterator l_it;

		long lIndex = 0;
		for( l_it = m_RemoteGroupParameters.begin(); l_it != m_RemoteGroupParameters.end() ; ++l_it )
		{
			lIndex++;
			std::string Branch = SvUl::Format( "%s_%d", SvXml::CTAG_REMOTE_GROUP_ID, lIndex );
			rWriter.StartElement( Branch.c_str() );
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

	BOOL l_bTmp = SvXml::SVNavigateTree::GetItemBranch( p_rTree, SvXml::CTAG_REMOTE_OUTPUT_PARAMETERS,p_htiParent, htiIORemoteOutput ) ;

	if( l_bTmp )
	{
		bOk = SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiIORemoteOutput, svVariant );
		if ( bOk )
		{

			SVGUID UniqueID( svVariant );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			m_outObjectInfo.GetObjectReference().setGuid(UniqueID);

			SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}

		// Remote Output Parameters
		if( bOk )
		{
			BOOL l_bTmp = TRUE;
			long l_lEntryNum = 0;
			while( l_bTmp )
			{
				SVTreeType::SVBranchHandle htiBranch = nullptr;
				std::string GroupID;

				std::string Entry = SvUl::Format( _T("%s_%d"), SvXml::CTAG_REMOTE_GROUP_ID, ++l_lEntryNum );
				l_bTmp = SvXml::SVNavigateTree::GetItemBranch( p_rTree, Entry.c_str(), htiIORemoteOutput, htiBranch );

				if ( l_bTmp )
				{
					SVRemoteOutputGroup* l_ControlParameter = new SVRemoteOutputGroup;
					l_bTmp = l_ControlParameter->SetParameters( p_rTree, htiBranch );
					SVRemoteOutputObject* pRemoteOutput = l_ControlParameter->GetFirstObject();
					if( nullptr != pRemoteOutput )
					{
						GroupID = pRemoteOutput->GetGroupID();
					}
					if( l_bTmp && nullptr != pRemoteOutput )
					{
						m_RemoteGroupParameters[ GroupID ] = l_ControlParameter;
					}
				}
			}
		}
	}

	return bOk;
}

// Gets the last remote output object with in this group.
SVRemoteOutputObject* SVRemoteOutputDataController::GetLastObject( const std::string& rRemoteGroupId )
{
	if( m_RemoteGroupParameters.end() != m_RemoteGroupParameters.find( rRemoteGroupId ) )
	{
		return m_RemoteGroupParameters[rRemoteGroupId]->GetLastObject();
	}

	return nullptr;
}

// Gets the last output object with in this group.
SVRemoteOutputObject* SVRemoteOutputDataController::GetFirstObject( const std::string& rRemoteGroupId )
{
	if( m_RemoteGroupParameters.end() != m_RemoteGroupParameters.find( rRemoteGroupId ) )
	{
		return m_RemoteGroupParameters[rRemoteGroupId]->GetFirstObject();
	}

	return nullptr;
}

// This function fills a list with group ids that are setup in the output data controller.
HRESULT SVRemoteOutputDataController::GetRemoteOutputGroups( SvDef::StringVector& rRemoteOutputGroupIds )
{
	HRESULT l_hr = S_OK;

	rRemoteOutputGroupIds.clear();

	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		rRemoteOutputGroupIds.push_back( l_it->first );
	}

	return l_hr;
}

// This function fills a list with the PPQ names that are associated with Groups that are setup.
HRESULT SVRemoteOutputDataController::GetPPQs( SvDef::StringVector& rPPQs, SVConfigurationObject* pConfig )
{
	HRESULT l_hr = S_OK;

	//Only ASSERT not to change runtime
	ASSERT( nullptr != pConfig );

	rPPQs.clear();

	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		long l_lPPQCount = pConfig->GetPPQCount( );
		for( int i = 0 ; i < l_lPPQCount ; i++ )
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if( nullptr != pPPQ )
			{
				if( pPPQ->GetName() == l_it->second->GetPPQName() )
				{
					rPPQs.push_back( pPPQ->GetName() );
					break;
				}
			}
		}
	}
	return l_hr;
}

HRESULT SVRemoteOutputDataController::GetGroupNames( SvDef::StringVector& rPPQs )
{
	HRESULT l_hr = S_OK;
	rPPQs.clear();

	SVRemoteOutputGroupMap::iterator l_it;
	for( l_it = m_RemoteGroupParameters.begin() ; l_it != m_RemoteGroupParameters.end() ; ++l_it )
	{
		rPPQs.push_back( l_it->first );
	}
	return l_hr;
}


// This function is used to tell if the Remote Outputs screen should be hidden
bool SVRemoteOutputDataController::IsEmpty()
{
	return m_RemoteGroupParameters.empty() ;
}

// This function deletes a remote output and removes it from the Remote Group control parameters class.
HRESULT SVRemoteOutputDataController::DeleteRemoteOutputEntry( const std::string& rRemoteGroupId, SVRemoteOutputObject* p_pOutputObject )
{
	HRESULT l_hr;
	l_hr = m_RemoteGroupParameters[rRemoteGroupId]->Delete( p_pOutputObject );
	return l_hr;
}

// DeleteRemoteOutput deletes the entire RemoteOutput control parameters class.
HRESULT SVRemoteOutputDataController::DeleteRemoteOutput( const std::string& rRemoteGroupId )
{
	HRESULT l_hr = S_FALSE;
	SVRemoteOutputGroupMap::iterator l_it;
	l_it = m_RemoteGroupParameters.find( rRemoteGroupId );
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
		typedef std::pair<std::string, SVGUID> PPQInfo;
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
				std::string Text = SvUl::Format(_T("Deleting Remote Output Group associated with PPQ %s\n"), ppqInfo.first.c_str() );
				OutputDebugString( Text.c_str() );
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
HRESULT SVRemoteOutputDataController::AddDefaultOutputs( const std::string& rRemoteGroupID, SVPPQObject* pPPQ )
{
	HRESULT l_hr = S_FALSE;
	ASSERT( nullptr != pPPQ );
	if( nullptr == pPPQ)
	{
		return l_hr;
	}

	if( !rRemoteGroupID.empty() )
	{
		if( m_RemoteGroupParameters.find( rRemoteGroupID ) == m_RemoteGroupParameters.end() )
		{
			SVRemoteOutputObject* pNewOutput = nullptr;
			SvVol::BasicValueObjectPtr pPpqTriggerCount =  pPPQ->getPpqVaraible(SvDef::FqnPpqTriggerCount);
			if(nullptr != pPpqTriggerCount)
			{
				AddItem( rRemoteGroupID, pNewOutput, pPpqTriggerCount->GetUniqueObjectID(), pPPQ->GetName() );
			}
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
	SvDef::StringVector GroupOutputNames;
	l_hr = GetRemoteOutputGroups( GroupOutputNames );
	SvDef::StringVector::const_iterator Iter;
	for( Iter = GroupOutputNames.begin() ; Iter != GroupOutputNames.end() ; ++Iter)
	{
		SVGroupDef GroupDevice;
		GroupDevice.m_Name = *Iter;
		SVRemoteOutputGroup* l_pControlPar = GetControlPar(*Iter);
		GroupDevice.m_PPQName = l_pControlPar->GetPPQName().c_str();
		l_OriginalList.push_back( GroupDevice );
	}

	if( S_OK == l_hr )
	{
		SetupRemoteOutputGroup(pConfig, l_OriginalList );

		if( S_OK == l_hr )
		{
			SVIODoc* pIODoc = TheSVObserverApp.GetIODoc();
			if(nullptr != pIODoc)
			{
				pIODoc->SetModifiedFlag();
			}
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
	SvDef::StringVector AvailablePPQs;
	// Initialize PPQ - Remote Groups by selecting from dialog.
	long l_lPPQSize = pConfig->GetPPQCount( );
	for( long l = 0 ; l < l_lPPQSize ; l++ )
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(l);
		if( nullptr != pPPQ )
		{
			AvailablePPQs.push_back( pPPQ->GetName() );
		}
	}


	SVRemoteOutputGroupAddRemoveDlg l_dlg;
	l_dlg.m_AvailablePPQs = AvailablePPQs;
	l_dlg.m_SetupGroup = p_rOriginalList;
	if( l_dlg.DoModal() == IDOK )
	{
		SvDef::StringVector NewItems;
		SvDef::StringVector RemovedItems;
		SvDef::StringPairVector RenamedItems;

		// Get a list of what was removed.
		l_dlg.GetNewItems( NewItems );
		l_dlg.GetRemoved( RemovedItems );
		l_dlg.GetRenamed( RenamedItems );

		for(SvDef::StringPairVector::iterator l_it = RenamedItems.begin() ; l_it != RenamedItems.end() ; ++l_it )
		{
			RenameGroup( l_it->first, l_it->second );
		}

		// Add New Output Groups
		for( size_t i = 0; i < NewItems.size() ; i++ )
		{
			for( size_t j = 0 ; j < l_dlg.m_SetupGroup.size() ; j++ )
			{
				if( l_dlg.m_SetupGroup[j].m_Name == NewItems[i] )
				{
					SVPPQObject* pPPQ( nullptr );
					bool l_bTmp = pConfig->GetChildObjectByName(l_dlg.m_SetupGroup[j].m_PPQName.c_str(), &pPPQ );
					if( l_bTmp )
					{
						AddDefaultOutputs(NewItems[i], pPPQ );
					}
				}
			}
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}

		// Remove Items
		for( size_t i = 0; i < RemovedItems.size() ; i++ )
		{
			DeleteRemoteOutput( RemovedItems[i] );

			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

bool SVRemoteOutputDataController::RenameGroup( const std::string& rOldName, const std::string& rNewName )
{
	bool l_bRet = false;
	SVRemoteOutputGroupMap::iterator l_it = m_RemoteGroupParameters.find( rOldName );
	if( l_it != m_RemoteGroupParameters.end() )
	{
		m_RemoteGroupParameters[rNewName] = l_it->second;

		m_RemoteGroupParameters[rNewName]->SetGroupName( rNewName );
		m_RemoteGroupParameters.erase( l_it );
		l_bRet = true;
	}
	return l_bRet;
}


