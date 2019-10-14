//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
	SV_DECLARE_CLASS( SVRemoteOutputObject );

	SVRemoteOutputObject();
	virtual ~SVRemoteOutputObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	//For Remote Outputs these virtual methods don't do anything
	virtual HRESULT Write(const _variant_t& p_rValue) override { return S_OK; };
	virtual HRESULT Reset() override { return S_OK; };
	virtual bool IsCombined() const override { return false; };
	virtual bool GetCombinedValue() const override { return false; };

	// Get Functions
	std::string GetInputValueObjectName();
	HRESULT GetInputValueObjectGUID( GUID& p_rGUID ) const;
	SVGUID GetInputValueObjectGUID() const;

	std::string GetGroupID( ) const ;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool GetParameters(SvOi::IObjectWriter& rWriter) const;

	// Set Functions
	HRESULT SetInputObject( SVObjectClass* p_pObject );
	HRESULT SetInputObjectId( GUID p_ObjectId );
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

	// Guid of the value object used to output
	SVGUID m_InputObjectId;	

	std::string m_strGroupID;	// Which Output Group this output belongs to.
#pragma endregion Member Variables
};

typedef std::vector<SVRemoteOutputObject*> SVRemoteOutputObjectList;

