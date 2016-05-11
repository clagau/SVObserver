//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputDataController
//* .File Name       : $Workfile:   SVRemoteOutputDataController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:46:32  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>

#include "svobjectlibrary\svobjectclass.h"
#include "SVObjectLibrary/SVObjectNotifyTemplate.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

#include "SVObjectCommandDataJson.h"
#include "SVRemoteOutputGroup.h"
#include "SVRemoteOutputGroupAddRemoveDlg.h"
#include "SVRemoteOutputObject.h"

struct SVProductInfoStruct;
class SVConfigurationObject;

class SVRemoteOutputDataController : 
	public SVObjectClass,
	public SVObjectNotifyTemplate< SVObjectCommandDataJsonPtr >
{
	SV_DECLARE_CLASS( SVRemoteOutputDataController );

public:
	SVRemoteOutputDataController( LPCSTR ObjectName );
	SVRemoteOutputDataController( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEOUTPUTCONTROLLER );

	virtual ~SVRemoteOutputDataController();

	virtual HRESULT ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr );

	HRESULT AddItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ );

	size_t GetItemCount( const CString& p_strRemoteGroupId );
	HRESULT GetItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem );

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool GetParameters( SVObjectXMLWriter& rWriter );

	BOOL SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent );

	void Destroy();

	// Writes Outputs for a RemoteOutput name.
	HRESULT WriteOutputs( const CString& p_strRemoteGroupID, SVProductInfoStruct *pProduct);

	// Gets the last RemoteOutput object in the list
	SVRemoteOutputObject* GetLastObject( const CString& p_strRemoteGroupId );
	// Gets the first RemoteOutput object in the list
	SVRemoteOutputObject* GetFirstObject( const CString& p_strRemoteGroupId );

	// Gets a list of RemoteOutput names 
	HRESULT GetRemoteOutputGroups( std::vector<CString>& p_astrRemoteOutputIds );

	HRESULT GetPPQs( std::vector<CString>& p_astrPPQs,  SVConfigurationObject* p_pConfig );
	HRESULT GetGroupNames( std::vector<CString>& p_astrPPQs );

	// Deletes a single remote Output.
	HRESULT DeleteRemoteOutputEntry( const CString& p_strRemoteGroupId, SVRemoteOutputObject* p_pOutputObject);

	// Deletes an entire logical RemoteOutput associated with a PPQ
	HRESULT DeleteRemoteOutput( const CString& p_strRemoteGroupId );
	
	LPCSTR GetConnectString( );	

	HRESULT SetConnectString( LPCSTR p_strConnection );

	// This function is used to tell if the RemoteOutput Outputs screen should be hidden
	bool IsEmpty();

	// Returns the number of Remote Output Groups.
	size_t GetRemoteOutputGroupCount();

	// Returns a copy of the SVRemoteOutputGroup class based on the Remote Group ID.
	SVRemoteOutputGroup* GetControlPar( const CString& p_strRemoteGroupID );

	// returns a pointer to a SVRemoteOutputGroup class based on the Remote Group ID.
	HRESULT GetControlPar( const CString& p_strRemoteGroupID, SVRemoteOutputGroup*& p_pControl );

	// Adds the defaults to a SVRemoteOutputGroup object
	HRESULT AddDefaultOutputs( CString p_strRemoteGroupID, SVPPQObject* p_pPPQ );

	void SetupRemoteOutput(SVConfigurationObject* p_pConfig );

	void SetupRemoteOutputGroup(SVConfigurationObject* p_pConfig, SVGroupDefVect& p_rOriginalList );

	// Returns the RemoteOutput size of the first element in the starting block address array.
	long GetElementBitSize( const CString& p_strRemoteOutputId );

	// This function walks the input list and attempts to validate each input GUID 
	// by getting a pointer from the Object Manager for each GUID.
	// If an input fails then it is deleted.
	HRESULT ValidateInputs();

	HRESULT ClearUnUsedData( );

private:
	HRESULT GetValueObjectData( void* l_mem, long p_lNumBytes, SVValueObjectClass* p_ValueObject, long p_lProductIndex );

	bool RenameGroup( CString oldName, CString newName );
	SVRemoteOutputGroupMap m_RemoteGroupParameters;	// Map of RemoteOutput parameters based on RemoteOutput ID
};


