//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCDataController
//* .File Name       : $Workfile:   SVPLCDataController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:24:20  $
//******************************************************************************


#ifndef SVPLCDATACONTROLLER_H
#define SVPLCDATACONTROLLER_H
#include "svobjectlibrary\svobjectclass.h"
#include "SVPLCOutput.h"
#include "SVPLCControlPar.h"
#include "SVPLCValidateReasons.h"
#include <map>
#include "SVMaterialsLibrary\SVMaterialsTree.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

struct SVProductInfoStruct;

class SVPLCDataController :
	public SVObjectClass
{
public:

	SV_DECLARE_CLASS( SVPLCDataController );

	SVPLCDataController(void);
	virtual ~SVPLCDataController(void);

	HRESULT AddItem( const CString& p_strPLCId, SVPLCOutputObject*& p_pNewOutput, long p_lElement, long p_lSize, GUID p_InputObjectID, long p_lBitNum, bool p_bUseBitSize );
	HRESULT RemoveItem( const CString& p_strPLCId, GUID p_PLCEntry );

	size_t GetItemCount( const CString& p_strPLCId );
	HRESULT GetItem( const CString& p_strPLCId, long l_index, SVPLCOutputObject*& p_rItem );

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL GetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent );

	BOOL SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle p_htiParent );

	HRESULT GetMaterials( SVMaterialsTreeAdapter& p_rMaterialsTree, 
							SVMaterialsTreeAdapter::SVTreeContainer* p_pParent );
	HRESULT Update( SVMaterialsTreeAdapter& p_rMaterialsTree );

	void Destroy();

	// Sorts the PLC lists by PLC element.
	HRESULT SortPLCLists();

	// Verifies that the new item will not conflict with the existing list.
//	HRESULT ValidateNew(CString p_strPLC, long p_lElement, long p_lSize, long p_lOriginalIndex, long p_lBit );
	// Verifies that the item will not overrun the buffer.
	HRESULT ValidateElement(const CString& p_strPLC, GUID p_InputObjectGuid, long p_lElement, long p_lSize, long p_lListIndex, long p_lBit );

	// Validates the starting block address list
	HRESULT ValidateBlockAddresses( const CString& p_strPLCName, SVMaterials p_BlockAddresses );


	// Writes Outputs for a plc name.
	HRESULT WriteOutputs( const CString& p_strPLCName, SVProductInfoStruct *pProduct);

	// Gets the last plc object in the list
	SVPLCOutputObject* GetLastObject( const CString& p_strPLCName );
	// Gets the first plc object in the list
	SVPLCOutputObject* GetFirstObject( const CString& p_strPLCName );
	// Calculates a next valid element number based on the last element and last size.
	bool GetNextValidElement( long& p_lNewElement,  SVPLCOutputObject* p_pPLCOutputObject );
	// Maps the ppq name to the plc name
	CString AssociatePPQToPLC( const CString& p_strPPQ );
	// Gets a list of plc names 
	HRESULT GetPLCs( std::vector<CString>& p_astrPLCIds );
	// Deletes a plc entry
	HRESULT GetPPQs( std::vector<CString>& p_astrPLCIds,  SVConfigurationObject* p_pConfig );

	HRESULT DeletePLCEntry( const CString& p_strPLCId, SVPLCOutputObject* p_pOutputObject);
	// Deletes an entire logical plc - PPQ
	HRESULT DeletePLC( const CString& p_strPLCId );
	
	HRESULT GetHeartBeatAddress( CString& p_strHeartBeatAddress );
	HRESULT SetHeartBeatAddress( CString p_strHeartBeatAddress );
	HRESULT GetHeartBeatTime( long& p_lTime );
	HRESULT SetHeartBeatTime( long p_lTime );
	LPCSTR GetConnectString( );
	HRESULT SetConnectString( LPCSTR p_strConnection );

	// This function is used to tell if the PLC Outputs screen should be hidden
	bool IsEmpty();

	// Returns the block count for a specific PLC ID.
	long GetBlockCount( const CString& p_strPLC );
	// Set Block count for a specific PLC ID.
	HRESULT SetBlockCount( const CString& p_strPLC, long p_plNewCount );
	// Returns the Queue size for a specific PLC ID
	long GetQueueSize( const CString& p_strPLC );
	// Sets the Queue size for a specific PLC ID.
	HRESULT SetQueueSize( const CString& p_strPLC, long p_plNewSize );

	// Returns the number of PLCs setup.
	size_t GetPLCCount() const;

	// Returns a copy of the SVPLCControlPar class based on the PLC ID.
	SVPLCControlPar GetControlPar( const CString& p_strPLC );

	// returns a pointer to a SVPLCControlPar class based on the PLC ID.
	HRESULT GetControlPar( const CString& p_strPLC, SVPLCControlPar** p_pControl );

	// Gets the data associated with a plc into a SVMaterials.
	HRESULT GetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC );
	// Sets the PLCControlPar with a SVmaterials data.
	HRESULT SetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC );

	// Sets the SVPLCControlPar associated with PLCID with a copy of a SVPLCControlPar
	HRESULT SetControlPar( const CString& p_strPLC, SVPLCControlPar p_PLCControl );
	// calls the function in the plc_interface dll to add an offset to a plc address.
	HRESULT AddOffsetToAddress( CString& p_strAddress, long p_lOffset, long p_lBit = -1 );
	// Adds the defaults to a SVPLCControlPar object
	HRESULT AddDefaultOutputs( SVPPQObject* p_pPPQ );

	// Gets the list index of the SVPLCOutputObject. This is used for validation so we can skip this position in our check.
	HRESULT GetListIndex( SVPLCOutputObject* p_pOutput, const CString& p_strPLCName, long& p_rlIndex );
	void SetupPLCPPQs(SVConfigurationObject* p_pConfig, std::vector<CString>& p_rOriginalList);
	void SetupPLC(SVConfigurationObject* p_pConfig );

	// Returns the plc size of the first element in the starting block address array.
	long GetElementBitSize( const CString& p_strPLCId );

	// This function walks the input list and attempts to validate each input GUID 
	// by getting a pointer from the Object Manager for each GUID.
	// If an input fails then it is deleted.
	HRESULT ValidateInputs();
	// Returns the collapse state based on the PLC ID - used for GUI
	bool GetCollapse( const CString& p_strPLCId );
	// Sets the collapse state based on the PLC ID - used for GUI
	void SetCollapse( const CString& p_strPLCId, bool p_bCollapse );

	// Calculates new element Size based on the plc element size and
	// the SVObserver data type.
	// The default element size will be 1.
	long CalcNewElementSize( const CString& l_strPLCID, SVObjectTypeEnum p_eType );

private:
	HRESULT AddBranch(SVMaterialsTreeAdapter& p_rTree, 
					LPCTSTR lpszName, 
					SVMaterialsTreeAdapter::SVTreeContainer* p_pParent, 
					SVMaterialsTreeAdapter::SVTreeContainer*& p_rpBranch);
	HRESULT GetInfoFromAddress( long& p_rlFileNum, long& p_rlElement, long& p_rlBitNum, const CString& p_szAddress );
	SVPLCDataTypeEnum GetPLCDataType( const CString& l_strAddress );
	HRESULT GetValueObjectData( void* l_mem, long p_lNumBytes, SVValueObjectClass* p_ValueObject, long p_lProductIndex );
	HRESULT GetValueObjectDataConvert2String( CString& p_rStringOut, SVValueObjectClass* p_pValueObject, long p_lProductIndex );
	HRESULT ClearUnUsedData( );

	// Gets a VARTYPE for the SVPLCTYPE
	VARTYPE VariantTypeFromPLCType( long p_lFirstDataType );

	// **** Member Variables *****
	// Heart Beat Address.
	CString m_strHeartBeatAddress;
	// Heart Beat time
	long m_lHeartBeatTime;

	// Connection String
	char m_szPLCConnectionString[256];

	SVPLCControlParMap m_PLCParameters;	// Map of plc parameters based on PLC ID

};

#endif // SVPLCDATACONTROLLER_H


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCDataController.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:24:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Oct 2012 11:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Sep 2012 15:24:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jan 2012 11:47:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jan 2009 11:51:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made plc ID parameters const reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Jan 2009 11:29:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added comments to header file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jan 2009 16:11:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved Edit / Add PLC Wizard logic to data controller class
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
 *    Rev 1.2   10 Dec 2008 10:30:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed parameter name for clarity.
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

