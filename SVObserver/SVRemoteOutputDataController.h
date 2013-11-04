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

#ifndef SVREMOTEOUTPUTDATACONTROLLER_H
#define SVREMOTEOUTPUTDATACONTROLLER_H

#include <map>

#include "SVMaterialsLibrary\SVMaterialsTree.h"
#include "svobjectlibrary\svobjectclass.h"
#include "SVObjectLibrary/SVObjectNotifyTemplate.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

#include "SVObjectCommandDataJson.h"
#include "SVRemoteOutputGroup.h"
#include "SVRemoteOutputGroupAddRemoveDlg.h"
#include "SVRemoteOutputObject.h"

struct SVProductInfoStruct;

class SVRemoteOutputDataController : 
	public SVObjectClass,
	public SVObjectNotifyTemplate< SVObjectCommandDataJsonPtr >
{
	SV_DECLARE_CLASS( SVRemoteOutputDataController );

public:
	SVRemoteOutputDataController( LPCSTR ObjectName );
	SVRemoteOutputDataController( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVREMOTEOUTPUTCONTROLLER );

	virtual ~SVRemoteOutputDataController();

	virtual HRESULT ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr );

	HRESULT AddItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ );

	size_t GetItemCount( const CString& p_strRemoteGroupId );
	HRESULT GetItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem );

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL GetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent );

	BOOL SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent );

	HRESULT GetMaterials( SVMaterialsTreeAdapter& p_rMaterialsTree, SVMaterialsTreeAdapter::SVTreeContainer* p_pParent );
	HRESULT Update( SVMaterialsTreeAdapter& p_rMaterialsTree );

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

	// Gets the data associated with a RemoteOutput into a SVMaterials.
	HRESULT GetRemoteOutputControlData( SVMaterials& p_rMaterials, const CString& p_strRemoteGroupID );
	// Sets the RemoteOutputControlPar with a SVmaterials data.
	HRESULT SetRemoteOutputControlData( SVMaterials& p_rMaterials, const CString& p_strRemoteGroupID );

	// Sets the SVRemoteOutputGroup associated with RemoteOutputID with a copy of a SVRemoteOutputGroup
	HRESULT SetControlPar( const CString& p_strRemoteGroupID, SVRemoteOutputGroup* p_RemoteOutputControl );

	// Adds the defaults to a SVRemoteOutputGroup object
	HRESULT AddDefaultOutputs( CString p_strRemoteGroupID, SVPPQObject* p_pPPQ );

//	void SetupRemoteOutputPPQs(SVConfigurationObject* p_pConfig, std::vector<CString>& p_rOriginalList);
	
	void SetupRemoteOutput(SVConfigurationObject* p_pConfig );

	void SetupRemoteOutputGroup(SVConfigurationObject* p_pConfig, SVGroupDefVect& p_rOriginalList );

	// Returns the RemoteOutput size of the first element in the starting block address array.
	long GetElementBitSize( const CString& p_strRemoteOutputId );

	// This function walks the input list and attempts to validate each input GUID 
	// by getting a pointer from the Object Manager for each GUID.
	// If an input fails then it is deleted.
	HRESULT ValidateInputs();

	HRESULT GetListIndex( SVRemoteOutputObject* p_pOutput, const CString& p_strRemoteOutputGroupName, long& p_rlIndex );
	
	HRESULT ClearUnUsedData( );

private:
	HRESULT AddBranch(SVMaterialsTreeAdapter& p_rTree, 
					LPCTSTR lpszName, 
					SVMaterialsTreeAdapter::SVTreeContainer* p_pParent, 
					SVMaterialsTreeAdapter::SVTreeContainer*& p_rpBranch);
	HRESULT GetValueObjectData( void* l_mem, long p_lNumBytes, SVValueObjectClass* p_ValueObject, long p_lProductIndex );
	HRESULT GetValueObjectDataConvert2String( CString& p_rStringOut, SVValueObjectClass* p_pValueObject, long p_lProductIndex );

	bool RenameGroup( CString oldName, CString newName );
	SVRemoteOutputGroupMap m_RemoteGroupParameters;	// Map of RemoteOutput parameters based on RemoteOutput ID

};

#endif // SVREMOTEOUTPUTDATACONTROLLER_H


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputDataController.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:46:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2012 11:30:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 15:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
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

