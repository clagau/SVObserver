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
#include "SVUtilityLibrary/SVGuidList.h"
#include "SVObjectScriptEnums.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvOi
{
class IObjectClass;
}

class SVObjectBuilder
{
public:
	static HRESULT CreateObject(const GUID& classID, const GUID& uniqueID, const std::string& name, const std::string& objectName, const GUID& ownerUniqueID);
	/// Create an object and add it to the friend list.
	/// \param classID [in] The class ID of the new object.
	/// \param uniqueID [in] The GUID to be set for the new object. If this is GUID_NULL, the created GUID stay valid.
	/// \param objectName [in] The name of the new object.
	/// \param ownerUniqueID [in] The owner of the friend list. Must not be GUID_NULL.
	/// \param rAddPreGuid [in] If this a valid GUID of a friend in the list, the new object will be added before this.
	/// \returns HRESULT
	static SvOi::IObjectClass* CreateFriendObject(const GUID& classID, const GUID& uniqueID, const std::string& objectName, const GUID& ownerUniqueID, const GUID& rAddPreGuid = GUID_NULL);
	static HRESULT DestroyFriends(const GUID& objectID);
	/// Search embedded object in owner and change Guid and name
	/// \param embeddedID [in] Embedded Id of the object to overwrite
	/// \param uniqueID [in] New Guid of the object
	/// \param objectName [in] New name of the object
	/// \param ownerUniqueID [in] Guid of the owner
	/// \returns HRESULT
	static HRESULT OverwriteEmbeddedObject(const GUID& embeddedID, const GUID& uniqueID, const std::string& objectName, const GUID& ownerUniqueID);
	static HRESULT SetObjectValue(const GUID& ownerID, const GUID& objectID, const std::string& itemName, const _variant_t& value, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetObjectValue(const GUID& ownerID, const GUID& objectID, const std::string& itemName, const std::vector<_variant_t>& values, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetInputs(const GUID& objectID, const SVGuidList& list);
	static HRESULT SetInputs(const GUID& objectID, const SvDef::StringPairVector& rInputPairVector);

	static HRESULT GetObjectDataType(const GUID& ownerID, const GUID& objectID, SVObjectScriptDataObjectTypeEnum& dataType);
};

