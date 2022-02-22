///******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
///*****************************************************************************
/// IObjectManager is the interface to get/set the objects
///              By now there are only function to call intern static methods.
///******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
namespace SvDef
{
struct SVObjectTypeInfoStruct;
}

namespace SvOi
{
class IObjectClass;
class ISVImage;
}
#pragma endregion Declarations


namespace SvOi
{
//************************************
//! Return a object defined by dotted name.
//! \param rFullName [in] Name of the object.
//! \returns IObjectClass*
//************************************
IObjectClass* getObjectByDottedName(const std::string& rFullName);

//************************************
//! Construct an object by the classId and return the pointer.
//! \param classID [in]
//! \returns IObjectClass*
//************************************
IObjectClass* ConstructObject(SvPb::ClassIdEnum classID);

//************************************
//! Return a object defined by it's Object ID.
//! \param objectId [in] Id of the object.
//! \returns IObjectClass*
//************************************
IObjectClass* getObject(uint32_t objectID);

void fillObjectList(std::back_insert_iterator<std::vector<IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, uint32_t startingObjectID, bool addHidden = false, bool stopIfClosed = false);

IObjectClass* FindObject(uint32_t parentID, const SvDef::SVObjectTypeInfoStruct& rInfo);
} //namespace SvOi
