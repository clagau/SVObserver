//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCControlPar
//* .File Name       : $Workfile:   SVPLCControlPar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:23:56  $
//******************************************************************************

#pragma once
#include "SVPLCOutput.h"
#include "SVMaterialsLibrary/SVMaterialsTree.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
class SVPLCManager;

class SVPLCControlPar
{
public:
	typedef std::vector<CString> VecStringArray;
	SVPLCControlPar(void);
	~SVPLCControlPar(void);
	void Destroy();

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	BOOL GetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent ) const;

	HRESULT GetMaterials( SVMaterialsTreeAdapter& p_rMaterialsTree, SVMaterialsTreeAdapter::SVTreeContainer* p_pParent );
	HRESULT Update( SVMaterialsTreeAdapter& p_rMaterialsTree );

	HRESULT GetData( SVMaterials& p_rMaterials );
	HRESULT SetData( SVMaterials& p_rMaterials );

	long GetBlockCount( void );
	long SetBlockCount( long p_lBlockCount );
	long GetQueueSize( void );
	long SetQueueSize( long p_lQueueSize );

	// PLC Identifier and new output object will be managed by SVPLCControlPar
	HRESULT AddOutput( SVPLCOutputObject* p_pOutputEntry );
	HRESULT RemoveOutput( GUID p_pOutputId );
	size_t PLCListSize();
	HRESULT GetItem( long l_lIndex, SVPLCOutputObject*& p_rItem );
	SVPLCOutputObject* GetItem( long l_lIndex );
	HRESULT SortPLCList();
	SVPLCOutputObject* GetLastObject( );
	SVPLCOutputObject* GetFirstObject( );
	long GetElementBitSize();

	HRESULT Delete( SVPLCOutputObject* );

	// This is the size in elements of the whole block of PLC Elements.
	long GetBlockSize();
	HRESULT GetBlockAddresses( VecStringArray& p_Addresses );
	HRESULT SetBlockAddresses(  VecStringArray p_Addresses );
	CString GetBlockAddress( int p_lIndex );

	// This function finds the objects index from the Object list.
	HRESULT Find( SVPLCOutputObject* p_pObject, long& p_rlIndex );

	// Used to expand or collapse the View 
	bool GetCollapse();
	// Used to expand or collapse the View 
	void SetCollapse( bool p_bCollapse );
	long m_lStartingAddressCounter; // used to keep track of the current block address

private:
	bool m_bCollapseView; // Used to collapse the view

	long m_lBlockSize;	// This is what you get if you walk the list 
						// and add each elements size.
	// Calculate Total Block size based on the last element + last element size
	HRESULT CalculateBlockSize();
	long m_lBlockCount;
	long m_lQueueSize;
	SVPLCOutputObjectList m_PLCOutputs;
	VecStringArray m_strBlockAddresses;
};

typedef std::map<CString, SVPLCControlPar > SVPLCControlParMap;


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCControlPar.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:23:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 2012 11:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Sep 2012 15:24:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2009 08:47:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved current Address Counter variable from the SVPLCDataController to the SVPLCControlPar class.
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

