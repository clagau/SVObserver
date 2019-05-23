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

#pragma region Includes
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVIOLibrary/SVRemoteOutputObject.h"
#pragma endregion Includes

namespace  SvOi
{
class IObjectWriter;
}

class SVRemoteOutputGroup : public SVObjectClass
{
public:
	SV_DECLARE_CLASS( SVRemoteOutputGroup );

	SVRemoteOutputGroup( LPCTSTR ObjectName );
	SVRemoteOutputGroup( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEOUTPUTGROUP);

	virtual ~SVRemoteOutputGroup();

	BOOL Destroy();
	BOOL Create();
	BOOL Clear();

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	BOOL GetParameters(SvOi::IObjectWriter& rWriter) const;

	// RemoteOutput Identifier and new output object will be managed by SVRemoteOutputGroup
	HRESULT AddOutput( SVRemoteOutputObject* p_pOutputEntry );
	size_t RemoteOutputListSize();
	HRESULT GetItem( long l_lIndex, SVRemoteOutputObject*& p_rItem );
	SVRemoteOutputObject* GetItem( long l_lIndex ) const;
	SVRemoteOutputObject* GetLastObject( ) const;
	SVRemoteOutputObject* GetFirstObject( ) const;

	const SVGUID& GetPPQObjectId() const;
	std::string GetPPQName() const;
	HRESULT SetPPQName( const std::string& p_rPPQ );

	std::string GetGroupName() const;

	HRESULT Delete( SVRemoteOutputObject* );

	// This function finds the objects index from the Object list.
	HRESULT Find( SVRemoteOutputObject* p_pObject, long& p_rlIndex );

	HRESULT SetGroupName( const std::string& p_rPPQ );

protected:
	mutable SVCriticalSection m_CriticalSection;

	BOOL m_bCreated;

	SVGUID m_PPQObjectId;

	SVRemoteOutputObjectList m_RemoteOutputs;

private:
	SVRemoteOutputGroup( const SVRemoteOutputGroup& p_rhs );

	const SVRemoteOutputGroup& operator=(const SVRemoteOutputGroup& p_rControl );
};

typedef std::map<std::string, SVRemoteOutputGroup* > SVRemoteOutputGroupMap;


