//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCommandData
//* .File Name       : $Workfile:   SVRemoteCommandData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:43:32  $
//******************************************************************************

#ifndef SVREMOTECOMMANDDATA_H
#define SVREMOTECOMMANDDATA_H

#include "SVCommandLibrary/SVCommandDataHolder.h"

class ATL_NO_VTABLE SVRemoteCommandData
:	public CComObjectRootEx< CComSingleThreadModel >
,	public CComCoClass< SVRemoteCommandData, &CLSID_SVRemoteCommandData >
,	public IDispatchImpl< ISVRemoteCommandData, &IID_ISVRemoteCommandData, &LIBID_SVObserver >
{
public:
	DECLARE_REGISTRY_RESOURCEID( IDR_SV_REMOTE_COMMAND_DATA )

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP( SVRemoteCommandData )
		COM_INTERFACE_ENTRY( ISVRemoteCommandData )
		COM_INTERFACE_ENTRY2( IDispatch, ISVRemoteCommandData )
	END_COM_MAP()

public:
	friend class SVRemoteCommand;

	SVRemoteCommandData();
	virtual ~SVRemoteCommandData();

	void SetData( const SVCommandDataHolder& p_rData );

	virtual void FinalRelease();

	STDMETHOD (GetDataInformation)( VARIANT* p_pInformation );

	STDMETHOD (GetDataType)( BSTR p_Name, long* p_pDataType );

  STDMETHOD (ConstructCommandData)(ISVRemoteCommandData** p_ppCommandData);

	STDMETHOD (GetContainer)( BSTR p_Name, ISVRemoteCommandData** p_ppContainer );
	STDMETHOD (SetContainer)( BSTR p_Name, ISVRemoteCommandData* p_pContainer );

	STDMETHOD (GetValue)( BSTR p_Name, VARIANT* p_pValue );
	STDMETHOD (SetValue)( BSTR p_Name, VARIANT p_Value );

	STDMETHOD (GetBlock)( BSTR p_Name, IStream** p_ppStream );
	STDMETHOD (SetBlock)( BSTR p_Name, IStream* p_pStream );

	STDMETHOD (GetImage)( BSTR p_Name, IStream** p_ppStream );
	STDMETHOD (SetImage)( BSTR p_Name, IStream* p_pStream );

protected:
	SVCommandDataHolder m_Data;

};

OBJECT_ENTRY_AUTO( __uuidof( SVRemoteCommandData ), SVRemoteCommandData ) 

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteCommandData.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:43:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jan 2012 15:36:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new remote functionality to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:29:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new methods and object to the SVObserver command interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/