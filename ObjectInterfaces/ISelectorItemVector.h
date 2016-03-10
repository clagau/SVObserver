//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for output info list class
//******************************************************************************

#pragma once
#pragma region Includes
#include "ISelectorItem.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

namespace Seidenader { namespace ObjectInterfaces
{
	class ISelectorItemVector
	{
	public:
		virtual ~ISelectorItemVector() {}

		//************************************
		//! Return the size of the list.
		//! \returns int
		//************************************
		//virtual int getSize() const = 0;

		//************************************
		//! Get the interface at index position.
		//! \param Index [in]
		//! \returns ISelectorItem*
		//************************************
		//virtual const ISelectorItem* getAt(int Index) const = 0;
	};

	typedef SVSharedPtr<ISelectorItemVector> ISelectorItemVectorPtr;

} /* namespace ObjectInterfaces */ } /* namespace Seidenader */

namespace SvOi = Seidenader::ObjectInterfaces;
