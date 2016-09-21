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
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#pragma endregion Includes

class SVRemoteOutputObject : public SVObjectClass
{
public:
	SV_DECLARE_CLASS( SVRemoteOutputObject );

	SVRemoteOutputObject(void);
	virtual ~SVRemoteOutputObject(void);
	
	// Get Functions
	SVString GetInputValueObjectName();
	HRESULT GetInputValueObjectGUID( GUID& p_rGUID ) const;
	SVGUID GetInputValueObjectGUID() const;

	SVValueObjectClass* GetValueObject();	

	SVString GetGroupID( ) const ;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool GetParameters( SVObjectXMLWriter& rWriter ) const;

	// Set Functions
	HRESULT SetInputObject( SVValueObjectClass* p_pObject );
	HRESULT SetInputObjectId( GUID p_ObjectId );
	HRESULT SetGroupID( const SVString& p_strGroupID );

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

private:
	// Member Variables
	// The Value Objects name.
	SVString m_strObjectName;

	// The value object
	SVValueObjectClass* m_pValueObject;	

	// Guid of the value object used to output
	SVGUID m_InputObjectId;	

	SVString m_strGroupID;	// Which Output Group this output belongs to.
};

typedef std::vector<SVRemoteOutputObject*> SVRemoteOutputObjectList;

