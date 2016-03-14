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
#ifndef INCL_SVOBJECTBUILDER_H
#define INCL_SVOBJECTBUILDER_H

#include "SVUtilityLibrary/SVVariantList.h"
#include "SVUtilityLibrary/SVGuidList.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectScriptEnums.h"

class SVObjectBuilder
{
public:
	static HRESULT CreateObject(const GUID& classID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID);
	static HRESULT CreateFriendObject(const GUID& classID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID);
	static HRESULT DestroyFriends(const GUID& objectID);
	static HRESULT CreateEmbeddedObject(const GUID& embeddedID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID);
	static HRESULT SetObjectValue(const GUID& ownerID, const GUID& objectID, const SVString& itemName, const _variant_t& value, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetObjectValue(const GUID& ownerID, const GUID& objectID, const SVString& itemName, const SVVariantList& values, SVObjectScriptDataObjectTypeEnum dstDataType);
	static HRESULT SetInputs(const GUID& objectID, const SVGuidList& list);
	static HRESULT SetInputs(const GUID& objectID, const SVNameGuidList& list);

	static HRESULT GetObjectDataType(const GUID& ownerID, const GUID& objectID, SVObjectScriptDataObjectTypeEnum& dataType);
};

#endif

