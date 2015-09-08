//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for object class
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectTypeInfoStruct.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//disable warning to avoid a long list of warning about of the dominate use of methods from SVObjectClass
#pragma warning(disable:4250)

		//this class is a interface. It should only have pure virtual public method and new member parameter
		class IObjectClass
		{
		public:
			virtual ~IObjectClass() {}

			/**********
			 The method gets the name of this object.
			***********/
			virtual LPCTSTR GetName() const = 0;

			/**********
			 This method gets the complete name of this object.
			***********/
			virtual SVString GetCompleteName() const = 0;

			/**********
			 The method gets the type of this object.
			***********/
			virtual const SVObjectTypeEnum& GetObjectType() const = 0;

			/**********
			 The method gets the parent or nullptr if no parent.
			***********/
			virtual const SVGUID& GetParentID() const = 0;

			/**********
			 The method gets the ancestor with a special type or nullptr if not found.
			 /param ancestorObjectType <in> 
			***********/
			virtual IObjectClass* GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) = 0;

			/**********
			The method gets the first object with the requested type.
			/param type <in> 
			***********/
			virtual IObjectClass* GetFirstObject(const SVObjectTypeInfoStruct& type) = 0;

			/**********
			This method gets the attributes allowed for the object.
			***********/
			virtual const UINT ObjectAttributesAllowed() const = 0;

			/*
			This method returns the object's Global Unique Identifier (GUID) of this object instance.
			*/
			virtual const SVGUID& GetUniqueObjectID() const = 0;

			/*
			This method returns the creation state of the object.
			*/
			virtual bool is_Created() const = 0;

			/*
			This method reset all object. 
			(By now it calls processMessage with SVM_RESET_ALL_OBJECT, that is the reason why the return value is DWORD_PTR)
			*/
			virtual DWORD_PTR resetAllObjects() = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;