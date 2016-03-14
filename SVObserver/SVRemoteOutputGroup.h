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

#include "SVObjectLibrary/SVObjectNotifyTemplate.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVInfoStructs.h"
#include "SVObjectCommandDataJson.h"
#include "SVRemoteOutputObject.h"
#include "SVObjectLibrary/SVObjectXMLWriter.h"

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

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	BOOL GetParameters( SVObjectXMLWriter& rWriter ) const;

	// RemoteOutput Identifier and new output object will be managed by SVRemoteOutputGroup
	HRESULT AddOutput( SVRemoteOutputObject* p_pOutputEntry );
	size_t RemoteOutputListSize();
	HRESULT GetItem( long l_lIndex, SVRemoteOutputObject*& p_rItem );
	SVRemoteOutputObject* GetItem( long l_lIndex );
	HRESULT SortRemoteOutputList();
	SVRemoteOutputObject* GetLastObject( );
	SVRemoteOutputObject* GetFirstObject( );

	const SVGUID& GetPPQObjectId() const;
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


