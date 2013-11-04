//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCommand
//* .File Name       : $Workfile:   SVRemoteCommand.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:42:58  $
//******************************************************************************

#ifndef SVREMOTECOMMAND_H
#define SVREMOTECOMMAND_H

#include "SVCommandLibrary/SVCommandLibraryEnums.h"
#include "SVCommandLibrary/SVCommandTemplate.h"

class ATL_NO_VTABLE SVRemoteCommand
:	public CComObjectRootEx< CComSingleThreadModel >
,	public CComCoClass< SVRemoteCommand, &CLSID_SVRemoteCommand >
,	public IDispatchImpl< ISVRemoteCommand, &IID_ISVRemoteCommand, &LIBID_SVObserver >
{
public:
	DECLARE_REGISTRY_RESOURCEID( IDR_SV_REMOTE_COMMAND )

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP( SVRemoteCommand )
		COM_INTERFACE_ENTRY( ISVRemoteCommand )
		COM_INTERFACE_ENTRY2( IDispatch, ISVRemoteCommand )
	END_COM_MAP()

public:
	SVRemoteCommand();
	virtual ~SVRemoteCommand();

	virtual void FinalRelease();

	void SetCommand( SVCommandTemplatePtr p_Command );

	STDMETHOD (GetCommandType)(long* p_CommandType);

  STDMETHOD (ConstructCommandData)(ISVRemoteCommandData** p_ppCommandData);

	STDMETHOD (GetAttributes)(ISVRemoteCommandData** p_ppAttributes);
	STDMETHOD (SetAttributes)(ISVRemoteCommandData* p_pAttributes);

	STDMETHOD (Execute)();

	STDMETHOD (GetResults)(ISVRemoteCommandData** p_ppResults);

protected:
	SVCommandTemplatePtr m_Command;

};

OBJECT_ENTRY_AUTO( __uuidof( SVRemoteCommand ), SVRemoteCommand ) 

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteCommand.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:42:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:29:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new methods and object to the SVObserver command interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/