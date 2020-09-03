//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object list class
//******************************************************************************


#pragma once
#pragma region Includes
#include "Definitions\ObjectDefines.h"
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
		/param objectID <in> ID of the object to deleted.
	***********/
	virtual void Delete(uint32_t objectID) = 0;

	/// The method inserts a task object to this task object list.
	/// \param objectBeforeID [in] the ID where the new object is to be inserted before. If SvDef::InvalidObjectId, add at the end.
	/// \param rObject [in] object to insert
	virtual void InsertBefore(uint32_t objectBeforeID, ITaskObject& rObject) = 0;

	/// The method inserts a task object to this task object list.
	/// \param pos [in] position where to insert. Is pos invalid, add at the end.
	/// \param rObject [in] object to insert.
	virtual void InsertAt(int pos, ITaskObject& rObject) = 0;

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
	/// \param objectToMoveId [in] ID of the object to move.
	/// \param preObjectId [in] ID of the object in front of the other object should moved. If this ID is SvDef::InvalidObjectId, the other object will be moved to the end of the list.
	virtual void moveTaskObject(uint32_t objectToMoveId, uint32_t preObjectId = SvDef::InvalidObjectId) = 0;

	///Fills the responseMessage  with ObjectInfos for the objects in m_TaskObjectVector
	virtual void   GetTaskObjectListInfo(SvPb::TaskObjectListResponse &rResponse) const = 0;

};
} //namespace SvOi
