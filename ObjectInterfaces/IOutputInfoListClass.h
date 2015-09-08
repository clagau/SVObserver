//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for output info list class
//******************************************************************************

#pragma once
#pragma region Includes
#include "IObjectInfoStruct.h"
#include <boost\function.hpp>
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		typedef boost::function<bool (const IObjectInfoStruct& rInfo)> IsObjectInfoAllowed;

		class IOutputInfoListClass
		{
		public:
			virtual ~IOutputInfoListClass() {}

			//************************************
			//! Return the size of the list.
			//! \returns int
			//************************************
			virtual int GetSize() const = 0;

			//************************************
			//! Get the output interface at index position.
			//! \param index [in,out]
			//! \returns IObjectInfoStruct*
			//************************************
			virtual IObjectInfoStruct* GetInterfaceAt(int index) = 0;
		};
		typedef SVSharedPtr<IOutputInfoListClass> IOutputInfoListClassPtr;
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;