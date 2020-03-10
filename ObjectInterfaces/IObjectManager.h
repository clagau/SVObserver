///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader, Markt Schwaben
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

class SVGUID;
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
//! Construct an object by the guid and return the pointer.
//! \param classID [in]
//! \returns IObjectClass*
//************************************
IObjectClass* ConstructObject(SvPb::ClassIdEnum classID);

//************************************
//! Return a object defined by it's Object ID.
//! \param rObjectId [in] Instance GUID of the object.
//! \returns IObjectClass*
//************************************
IObjectClass* getObject(const SVGUID& rObjectID);

//************************************
//! Traverses the Object DataStore, starting at a certain Object.
//! \param ObjectVisitor [in] The visitor.
//! \param SVGUID [in] starting Object.
//! \returns HRESULT
//************************************
template<typename ObjectVisitor>
HRESULT visitElements(ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID = GUID_NULL);

IObjectClass* FindObject(const SVGUID& rParentID, const SvDef::SVObjectTypeInfoStruct& rInfo);
} //namespace SvOi
