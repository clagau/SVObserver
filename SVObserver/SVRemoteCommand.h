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
#pragma once

#pragma region Includes
#include "SVCommandLibrary/SVCommandLibraryEnums.h"
#include "SVCommandLibrary/SVCommandTemplate.h"
#pragma endregion Includes

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

