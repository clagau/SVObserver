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
#pragma once

#pragma region Includes
#include "SVCommandLibrary/SVCommandDataHolder.h"
#pragma endregion Includes

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

