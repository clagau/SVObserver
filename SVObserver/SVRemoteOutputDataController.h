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
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVObjectCommandDataJson.h"
#include "SVRemoteOutputGroup.h"
#include "SVRemoteOutputGroupAddRemoveDlg.h"
#include "SVIOLibrary/SVRemoteOutputObject.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

struct SVProductInfoStruct;
class SVConfigurationObject;

class SVRemoteOutputDataController : public SVObjectClass
{
	SV_DECLARE_CLASS( SVRemoteOutputDataController );

public:
	SVRemoteOutputDataController( LPCTSTR ObjectName );
	SVRemoteOutputDataController( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEOUTPUTCONTROLLER );

	virtual ~SVRemoteOutputDataController();

	HRESULT AddItem( const std::string& rRemoteGroupId, SVRemoteOutputObject*& pNewOutput, GUID InputObjectID, const std::string& rPPQ );

	size_t GetItemCount( const std::string& rRemoteGroupId );
	HRESULT GetItem( const std::string& rRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem );

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool GetParameters(SvXml::SVObjectXMLWriter& rWriter );

	BOOL SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent );

	void Destroy();

	// Writes Outputs for a RemoteOutput name.
	HRESULT WriteOutputs( const std::string& rRemoteGroupId, SVProductInfoStruct *pProduct);

	// Gets the last RemoteOutput object in the list
	SVRemoteOutputObject* GetLastObject( const std::string& rRemoteGroupId );
	// Gets the first RemoteOutput object in the list
	SVRemoteOutputObject* GetFirstObject( const std::string& rRemoteGroupId );

	// Gets a list of RemoteOutput names 
	HRESULT GetRemoteOutputGroups( SvDef::StringVector& rRemoteOutputIds );

	HRESULT GetPPQs( SvDef::StringVector& rPPQs,  SVConfigurationObject* p_pConfig );
	HRESULT GetGroupNames( SvDef::StringVector& rPPQs );

	// Deletes a single remote Output.
	HRESULT DeleteRemoteOutputEntry( const std::string& rRemoteGroupId, SVRemoteOutputObject* p_pOutputObject);

	// Deletes an entire logical RemoteOutput associated with a PPQ
	HRESULT DeleteRemoteOutput( const std::string& rRemoteGroupId );
	
	LPCTSTR GetConnectString( );	

	HRESULT SetConnectString( LPCTSTR Connection );

	// This function is used to tell if the RemoteOutput Outputs screen should be hidden
	bool IsEmpty();

	// Returns the number of Remote Output Groups.
	size_t GetRemoteOutputGroupCount();

	// Returns a copy of the SVRemoteOutputGroup class based on the Remote Group ID.
	SVRemoteOutputGroup* GetControlPar( const std::string& rRemoteGroupId );

	// returns a pointer to a SVRemoteOutputGroup class based on the Remote Group ID.
	HRESULT GetControlPar( const std::string& rRemoteGroupId, SVRemoteOutputGroup*& p_pControl );

	// Adds the defaults to a SVRemoteOutputGroup object
	HRESULT AddDefaultOutputs( const std::string& rRemoteGroupId, SVPPQObject* p_pPPQ );

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
	bool RenameGroup( const std::string& rOldName, const std::string& rNewName );
	SVRemoteOutputGroupMap m_RemoteGroupParameters;	// Map of RemoteOutput parameters based on RemoteOutput ID
};


