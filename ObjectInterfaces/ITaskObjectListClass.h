//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object list class
//******************************************************************************

#pragma once
#pragma region Includes
#include "ITaskObject.h"
#pragma endregion Includes

struct SVOutObjectInfoStruct;

namespace SvOi
{
	//this class is a interface. It should only have pure virtual public method and no member variables
	class ITaskObjectListClass
	{
	public:
		virtual ~ITaskObjectListClass() {}

		/**********
			The method gets the name/Guid list of the task object of this taskObjectList.
		***********/
		virtual SvUl::NameGuidList GetTaskObjectList( ) const = 0;

		/**********
			The method deletes one object from the this task object list object.
			/param objectID <in> GUID of the object to deleted.
		***********/
		virtual void Delete(GUID& objectID) = 0;

		/**********
			The method inserts a task object to this task object list.
			/param index <in> position where to insert.
			/param rObject <in> object to insert.
			/param count <in> Default value is 1.
		***********/
		virtual void InsertAt(int index, ITaskObject& rObject, int count = 1) = 0;

		/**********
			The method destroys a child object.
			/param rObject <in> object to destroy.
			/param context <in> set which action should done. Possible flags:  SvDef::SVMFSetDefaultInputs and SvDef::SVMFResetInspection
		***********/
		virtual bool DestroyChild(ITaskObject& rObject, DWORD context) = 0;

		/**********
			The method destroys a friend object.
			/param rObject <in> object to destroy.
			/param context <in> set which action should done. Possible flags:  SvDef::SVMFSetDefaultInputs and SvDef::SVMFResetInspection
		***********/
		virtual bool DestroyFriendObject(IObjectClass& rObject, DWORD context) = 0;

		/**********
			This method removes he connection to the output objject.
			/param pOutObject <in> object to disconnect.
		***********/
		virtual void RemoveOutputObject( SVOutObjectInfoStruct* pOutObject ) = 0;
	};
} //namespace SvOi
