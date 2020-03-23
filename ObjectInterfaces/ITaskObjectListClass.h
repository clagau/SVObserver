//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object list class
//******************************************************************************


#pragma once
#pragma region Includes
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in #include SVObjectLibrary/SVOutObjectInfoStruct.h
struct SVOutObjectInfoStruct;
namespace SvPb
{
class  TaskObjectListResponse;
};

#pragma endregion Declarations

namespace SvOi
{
class ITaskObject;

//this class is a interface. It should only have pure virtual public method and no member variables
class ITaskObjectListClass
{
public:
	virtual ~ITaskObjectListClass() {}

	/**********
		The method deletes one object from the this task object list object.
		/param objectID <in> SVGUID of the object to deleted.
	***********/
	virtual void Delete(const SVGUID& rObjectID) = 0;

	/**********
		The method inserts a task object to this task object list.
		/param index <in> position where to insert.
		/param <in> rObjectBeforeID the ID where the new object is to be inserted before
		/param rObject <in> object to insert.
		/param count <in> Default value is 1.
		***********/
	virtual void InsertBefore(const SVGUID& rObjectBeforeID, ITaskObject& rObject) = 0;

	/**********
		The method destroys a child object.
		/param rObject <in> object to destroy.
		/param context <in> set which action should done. Possible flags:  SvDef::SVMFSetDefaultInputs and SvDef::SVMFResetInspection
	***********/
	virtual bool DestroyChild(ITaskObject& rObject, DWORD context) = 0;

	/**********
		This method removes he connection to the output object.
		/param pOutObject <in> object to disconnect.
	***********/
	virtual void RemoveOutputObject(SVOutObjectInfoStruct* pOutObject) = 0;

	/// Move an object in the taskObject-list to a position in front of another object.
	/// \param objectToMoveId [in] GUID of the object to move.
	/// \param preObjectId [in] GUID of the object in front of the other object should moved. If this GUID is GUID_NULL, the other object will be moved to the end of the list.
	virtual void moveTaskObject(const SVGUID& objectToMoveId, const SVGUID& preObjectId = GUID_NULL) = 0;

	///Fills the responseMessage  with ObjectInfos for the objects in m_TaskObjectVector
	virtual void   GetTaskObjectListInfo(SvPb::TaskObjectListResponse &rResponse) const = 0;

};
} //namespace SvOi
