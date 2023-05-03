//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputObject
//* .File Name       : $Workfile:   SVRemoteOutputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:52:42  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVOutputObject.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

namespace  SvOi
{
class IObjectWriter;
}

class SVRemoteOutputObject : public SVOutputObject
{
#pragma region Constructor
public:
	SV_DECLARE_CLASS

	SVRemoteOutputObject();
	virtual ~SVRemoteOutputObject() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	//For Remote Outputs these virtual methods don't do anything
	virtual HRESULT Write(const _variant_t&) override { return S_OK; }
	virtual HRESULT Reset() override { return S_OK; }
	virtual bool isCombined() const override { return false; }
	virtual bool isAndACK() const override { return false; }
	virtual long GetChannel() const override { return -1; }
	virtual void SetValueObjectID(uint32_t objectID, DWORD objectIDIndex = 0) override;
	virtual uint32_t GetValueObjectID(DWORD objectIDIndex = 0) const override;

	// Get Functions
	std::string GetInputValueObjectName();

	std::string GetGroupID( ) const ;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	void Persist(SvOi::IObjectWriter& rWriter) const;

	// Set Functions
	HRESULT SetGroupID( const std::string& p_strGroupID );

	bool SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	// Member Variables
	// The Value Objects name.
	std::string m_strObjectName;

	uint32_t m_valueObjectID {SvDef::InvalidObjectId};

	std::string m_strGroupID;	// Which Output Group this output belongs to.
#pragma endregion Member Variables
};

using  SVRemoteOutputObjectList = std::vector<SVRemoteOutputObject*>;

