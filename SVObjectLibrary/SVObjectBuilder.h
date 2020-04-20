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
#include "SVObjectScriptEnums.h"
#include "Definitions/ObjectDefines.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvOi
{
class IObjectClass;
}

namespace SvPb
{
	enum ClassIdEnum;
	enum EmbeddedIdEnum;
}

class SVObjectBuilder
{
public:
	static HRESULT CreateObject(SvPb::ClassIdEnum classID, uint32_t uniqueID, const std::string& name, const std::string& objectName, uint32_t ownerUniqueID);
	/// Create an object and add it to the friend list.
	/// \param classID [in] The class ID of the new object.
	/// \param uniqueID [in] The ID to be set for the new object. If this is SvDef::InvalidObjectId, the created ID stay valid.
	/// \param objectName [in] The name of the new object.
	/// \param ownerUniqueID [in] The owner of the friend list. Must not be SvDef::InvalidObjectId.
	/// \param addPreId [in] If this a valid ID of a friend in the list, the new object will be added before this.
	/// \returns HRESULT
	static SvOi::IObjectClass* CreateFriendObject(SvPb::ClassIdEnum classID, uint32_t uniqueID, const std::string& objectName, uint32_t ownerUniqueID, uint32_t addPreId = SvDef::InvalidObjectId);
	static HRESULT DestroyFriends(uint32_t objectID);
	/// Search embedded object in owner and change id and name
	/// \param embeddedID [in] Embedded Id of the object to overwrite
	/// \param uniqueID [in] New id of the object
	/// \param objectName [in] New name of the object
	/// \param ownerUniqueID [in] id of the owner
	/// \returns HRESULT
	static HRESULT OverwriteEmbeddedObject(SvPb::EmbeddedIdEnum embeddedID, uint32_t uniqueID, const std::string& objectName, uint32_t ownerUniqueID);
	static HRESULT SetObjectValue(uint32_t ownerID, uint32_t objectID, const std::string& itemName, const _variant_t& value, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetObjectValue(uint32_t ownerID, uint32_t objectID, const std::string& itemName, const std::vector<_variant_t>& values, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetInputs(uint32_t objectID, const SvDef::StringPairVector& rInputPairVector);

	static HRESULT GetObjectDataType(uint32_t ownerID, uint32_t objectID, SVObjectScriptDataObjectTypeEnum& dataType);
};

