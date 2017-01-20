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

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "svobjectlibrary\svobjectclass.h"
#include "SVObjectLibrary/SVObjectNotifyTemplate.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVObjectCommandDataJson.h"
#include "SVRemoteOutputGroup.h"
#include "SVRemoteOutputGroupAddRemoveDlg.h"
#include "SVRemoteOutputObject.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

struct SVProductInfoStruct;
class SVConfigurationObject;

class SVRemoteOutputDataController : 
	public SVObjectClass,
	public SVObjectNotifyTemplate< SVObjectCommandDataJsonPtr >
{
	SV_DECLARE_CLASS( SVRemoteOutputDataController );

public:
	SVRemoteOutputDataController( LPCTSTR ObjectName );
	SVRemoteOutputDataController( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEOUTPUTCONTROLLER );

	virtual ~SVRemoteOutputDataController();

	virtual HRESULT ProcessNotifyData( SVObjectCommandDataJsonPtr& rDataPtr ) override;

	HRESULT AddItem( const SVString& rRemoteGroupId, SVRemoteOutputObject*& pNewOutput, GUID InputObjectID, const SVString& rPPQ );

	size_t GetItemCount( const SVString& rRemoteGroupId );
	HRESULT GetItem( const SVString& rRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem );

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool GetParameters( SVObjectXMLWriter& rWriter );

	BOOL SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent );

	void Destroy();

	// Writes Outputs for a RemoteOutput name.
	HRESULT WriteOutputs( const SVString& rRemoteGroupId, SVProductInfoStruct *pProduct);

	// Gets the last RemoteOutput object in the list
	SVRemoteOutputObject* GetLastObject( const SVString& rRemoteGroupId );
	// Gets the first RemoteOutput object in the list
	SVRemoteOutputObject* GetFirstObject( const SVString& rRemoteGroupId );

	// Gets a list of RemoteOutput names 
	HRESULT GetRemoteOutputGroups( SVStringVector& rRemoteOutputIds );

	HRESULT GetPPQs( SVStringVector& rPPQs,  SVConfigurationObject* p_pConfig );
	HRESULT GetGroupNames( SVStringVector& rPPQs );

	// Deletes a single remote Output.
	HRESULT DeleteRemoteOutputEntry( const SVString& rRemoteGroupId, SVRemoteOutputObject* p_pOutputObject);

	// Deletes an entire logical RemoteOutput associated with a PPQ
	HRESULT DeleteRemoteOutput( const SVString& rRemoteGroupId );
	
	LPCTSTR GetConnectString( );	

	HRESULT SetConnectString( LPCTSTR Connection );

	// This function is used to tell if the RemoteOutput Outputs screen should be hidden
	bool IsEmpty();

	// Returns the number of Remote Output Groups.
	size_t GetRemoteOutputGroupCount();

	// Returns a copy of the SVRemoteOutputGroup class based on the Remote Group ID.
	SVRemoteOutputGroup* GetControlPar( const SVString& rRemoteGroupId );

	// returns a pointer to a SVRemoteOutputGroup class based on the Remote Group ID.
	HRESULT GetControlPar( const SVString& rRemoteGroupId, SVRemoteOutputGroup*& p_pControl );

	// Adds the defaults to a SVRemoteOutputGroup object
	HRESULT AddDefaultOutputs( const SVString& rRemoteGroupId, SVPPQObject* p_pPPQ );

	void SetupRemoteOutput(SVConfigurationObject* p_pConfig );

	void SetupRemoteOutputGroup(SVConfigurationObject* p_pConfig, SVGroupDefVect& p_rOriginalList );

	// Returns the RemoteOutput size of the first element in the starting block address array.
	long GetElementBitSize( LPCTSTR RemoteOutputId );

	// This function walks the input list and attempts to validate each input GUID 
	// by getting a pointer from the Object Manager for each GUID.
	// If an input fails then it is deleted.
	HRESULT ValidateInputs();

	HRESULT ClearUnUsedData( );

private:
	HRESULT GetValueObjectData( void* l_mem, long p_lNumBytes, SVValueObjectClass* p_ValueObject, long p_lProductIndex );

	bool RenameGroup( const SVString& rOldName, const SVString& rNewName );
	SVRemoteOutputGroupMap m_RemoteGroupParameters;	// Map of RemoteOutput parameters based on RemoteOutput ID
};


