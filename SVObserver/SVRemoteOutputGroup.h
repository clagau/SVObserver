//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputGroup
//* .File Name       : $Workfile:   SVRemoteOutputGroup.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:51:52  $
//******************************************************************************

#pragma once

#include "SVMaterialsLibrary/SVMaterialsTree.h"
#include "SVObjectLibrary/SVObjectNotifyTemplate.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

#include "SVInfoStructs.h"
#include "SVObjectCommandDataJson.h"
#include "SVRemoteOutputObject.h"

class SVRemoteOutputGroup : 
	public SVObjectClass,
	public SVObjectNotifyTemplate< SVObjectCommandDataJsonPtr >,
	public SVObserverTemplate< SVProductInfoStruct >
{
public:
	SV_DECLARE_CLASS( SVRemoteOutputGroup );

	typedef std::vector< SVString > VecStringArray;

	SVRemoteOutputGroup( LPCSTR ObjectName );
	SVRemoteOutputGroup( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVREMOTEOUTPUTGROUP);

	virtual ~SVRemoteOutputGroup();

	virtual HRESULT ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr );
	virtual HRESULT ObserverUpdate( const SVProductInfoStruct& p_rData );

	BOOL Destroy();
	BOOL Create();
	BOOL Clear();
	BOOL AttachStreamManager();

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	BOOL GetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent ) const;

	HRESULT GetMaterials( SVMaterialsTreeAdapter& p_rMaterialsTree, SVMaterialsTreeAdapter::SVTreeContainer* p_pParent );
	HRESULT Update( SVMaterialsTreeAdapter& p_rMaterialsTree );

	HRESULT GetData( SVMaterials& p_rMaterials );
	HRESULT SetData( SVMaterials& p_rMaterials );

	// RemoteOutput Identifier and new output object will be managed by SVRemoteOutputGroup
	HRESULT AddOutput( SVRemoteOutputObject* p_pOutputEntry );
	size_t RemoteOutputListSize();
	HRESULT GetItem( long l_lIndex, SVRemoteOutputObject*& p_rItem );
	SVRemoteOutputObject* GetItem( long l_lIndex );
	HRESULT SortRemoteOutputList();
	SVRemoteOutputObject* GetLastObject( );
	SVRemoteOutputObject* GetFirstObject( );

	SVString GetPPQName() const;
	HRESULT SetPPQName( const SVString& p_rPPQ );

	SVString GetGroupName();

	HRESULT Delete( SVRemoteOutputObject* );

	// This function finds the objects index from the Object list.
	HRESULT Find( SVRemoteOutputObject* p_pObject, long& p_rlIndex );

	HRESULT SetGroupName( const SVString& p_rPPQ );

protected:
	SVCriticalSection m_CriticalSection;

	BOOL m_bCreated;

	SVGUID m_PPQObjectId;

	SVRemoteOutputObjectList m_RemoteOutputs;

	long m_StreamDataItems;

private:
	SVRemoteOutputGroup( const SVRemoteOutputGroup& p_rhs );

	const SVRemoteOutputGroup& operator=(const SVRemoteOutputGroup& p_rControl );

};

typedef std::map<CString, SVRemoteOutputGroup* > SVRemoteOutputGroupMap;


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputGroup.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:51:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 2012 11:30:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Sep 2012 15:26:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2012 13:48:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added function AttachStreamManager function.
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

