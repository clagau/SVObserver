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

struct SVObjectTypeInfoStruct;

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//this class is a interface. It should only have pure virtual public method and new member parameter
		class ITaskObjectListClass : virtual public ITaskObject
		{
		public:
			virtual ~ITaskObjectListClass() {}

			/**********
			 The method gets the size of tasks.
			***********/
			virtual int GetSize() const = 0;

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
			 /param context <in>.
			***********/
			virtual DWORD_PTR DestroyChildObject(ITaskObject& rObject, DWORD context) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;