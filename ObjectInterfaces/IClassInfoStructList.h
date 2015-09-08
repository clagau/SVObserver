//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for class info struct list
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IClassInfoStructList
		{
		public:
			virtual ~IClassInfoStructList() {}

			/**********
			 The method gets the size of the list.
			***********/
			virtual long Size() const = 0;
			/**********
			 The method gets the class name of a object on a requested position.
			 /param index <in> the index of the object. Must be between 0 and size-1.
			***********/
			virtual CString GetClassName(int index) const = 0;
			/**********
			 The method gets the class id of a object on a requested position.
			 /param index <in> the index of the object. Must be between 0 and size-1.
			***********/
			virtual GUID GetClassID(int index) const = 0;
		};

		/**********
		  The method create a class info struct list. It have to be deleted by calling the method deleteSVClassInfoStructList.
		  /return pointer to this object. It HAVE TO be deleted by the caller calling the method deleteSVClassInfoStructList, if the object is not used anymore.
		***********/
		IClassInfoStructList* createSVClassInfoStructList();

		/**********
		  The method delete a object create by method createSVClassInfoStructList.
		  /param pObject <in> pointer to the object.
		***********/
		void deleteSVClassInfoStructList( IClassInfoStructList* pObject );
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;