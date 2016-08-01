//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectBuilder
//* .File Name       : $Workfile:   SVObjectBuilder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:46:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVVariantList.h"
#include "SVUtilityLibrary/SVGuidList.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectScriptEnums.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

class SVObjectBuilder
{
public:
	static HRESULT CreateObject(const GUID& classID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID);
	/// Create an object and add it to the friend list.
	/// \param classID [in] The class ID of the new object.
	/// \param uniqueID [in] The GUID to be set for the new object. If this is SV_GUID_NULL, the created GUID stay valid.
	/// \param objectName [in] The name of the new object.
	/// \param ownerUniqueID [in] The owner of the friend list. Must not be SV_GUID_NULL.
	/// \param rAddPreGuid [in] If this a valid GUID of a friend in the list, the new object will be added before this.
	/// \returns HRESULT
	static HRESULT CreateFriendObject(const GUID& classID, const GUID& uniqueID, const SVString& objectName, const GUID& ownerUniqueID, const GUID& rAddPreGuid = SV_GUID_NULL);
	static HRESULT DestroyFriends(const GUID& objectID);
	static HRESULT CreateEmbeddedObject(const GUID& embeddedID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID);
	static HRESULT SetObjectValue(const GUID& ownerID, const GUID& objectID, const SVString& itemName, const _variant_t& value, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetObjectValue(const GUID& ownerID, const GUID& objectID, const SVString& itemName, const SVVariantList& values, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetInputs(const GUID& objectID, const SVGuidList& list);
	static HRESULT SetInputs(const GUID& objectID, const SVNameGuidList& list);

	static HRESULT GetObjectDataType(const GUID& ownerID, const GUID& objectID, SVObjectScriptDataObjectTypeEnum& dataType);
};

