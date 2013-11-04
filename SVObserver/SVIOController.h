//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOController
//* .File Name       : $Workfile:   SVIOController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:08:58  $
//******************************************************************************

#ifndef SVIOCONTROLLER_H
#define SVIOCONTROLLER_H

#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVP_SRC/SVPLCInterfaceClass.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

#include "SVInfoStructs.h"
#include "SVPLCDataController.h"
#include "SVRemoteOutputDataController.h"
#include "SVRemoteOutputObject.h"

class SVIODoc;

class SVIOController : public SVObjectClass
{
	SV_DECLARE_CLASS( SVIOController );

public:
	SVIOController( LPCSTR ObjectName );
	SVIOController( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVIOCONTROLLER );

	virtual ~SVIOController();

	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );
	virtual BOOL SetImageDepth( long lDepth );

	virtual void ResetName();
	virtual void SetName( const CString& StrString );

	virtual DWORD processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext );

	virtual HRESULT ResetObject();
	virtual BOOL OnValidate();

	BOOL RebuildOutputList();

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	BOOL GetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	HRESULT SetModuleReady( bool p_Value );
	HRESULT SetRaidErrorBit( bool p_Value );

	SVIOEntryHostStructPtr GetModuleReady();
	SVIOEntryHostStructPtr GetRaidErrorBit();

	SVIODoc* GetIODoc() const;

	SVPLCDataController* GetPLCData();
	size_t GetPLCCount() const;
	void SetupPLC(SVConfigurationObject* p_pConfig );
	HRESULT GetPLCs( std::vector<CString>& p_astrPLCIds );
	LPCSTR GetConnectString();
	long GetQueueSize( const CString& p_strPLC );
	HRESULT GetHeartBeatAddress( CString& p_strHeartBeatAddress );
	HRESULT GetHeartBeatTime( long& p_lTime );
	HRESULT GetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC );
	HRESULT SetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC );
	CString AssociatePPQToPLC( const CString& p_strPPQ );
	HRESULT WriteOutputs( const CString& p_strPLCName, SVProductInfoStruct *pProduct);

	SVGUID GetRemoteOutputController() const;
	size_t GetRemoteOutputGroupCount() const;
	void SetupRemoteOutput( SVConfigurationObject* p_pConfig );
	HRESULT ClearRemoteOutputUnUsedData( );
	HRESULT GetRemoteOutputGroupNames( std::vector<CString>& p_astrPPQs );
	SVRemoteOutputGroup* GetRemoteOutputGroup( const CString& p_strRemoteGroupID ) const;
	size_t GetRemoteOutputGroupItemCount( const CString& p_strRemoteGroupID ) const;
	HRESULT GetRemoteOutputItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const;
	SVRemoteOutputObject* GetFirstRemoteOutputObject( const CString& p_strRemoteGroupId );
	HRESULT AddRemoteOutputItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ );
	HRESULT DeleteRemoteOutput( const CString& p_strRemoteGroupId );
	HRESULT DeleteRemoteOutputEntry( const CString& p_strRemoteGroupId, SVRemoteOutputObject* p_pOutputObject );
	HRESULT RemoteOutputValidateInputs();

protected:
	SVIOEntryHostStructPtr m_pModuleReady;
	SVIOEntryHostStructPtr m_pRaidErrorBit;

	SVPLCDataController m_PLCData;
	SVRemoteOutputDataController* m_pRemoteOutputController;

private:
	void LocalIntialize();
	void LocalDestroy();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIOController.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:08:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Jan 2013 15:04:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Oct 2012 11:25:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Sep 2012 15:19:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4.1.0   08 Jan 2013 12:15:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 16:58:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jan 2012 11:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2011 16:17:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Apr 2011 10:24:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Dec 2010 15:13:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
