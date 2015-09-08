//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object class
//******************************************************************************

#pragma once

#pragma region Includes
#include "IObjectAppClass.h"
#include "IOutputInfoListClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//this class is a interface. It should only have pure virtual public method and new member parameter
		class ITaskObject : virtual public IObjectAppClass
		{
		public:
			virtual ~ITaskObject() {}

			/**********
				The method add a input request marker to the IP.
			***********/
			virtual HRESULT AddInputRequestMarker() = 0;

			/**********
				The method start the run of the inspection (or a specified tool) once.
				/param pTool <in> Tool object which should run. If empty it use the whole inspection.
			***********/
			virtual HRESULT RunOnce(IObjectClass* pTool = nullptr) = 0;

			//************************************
			/// Return the output list of this task object.
			/// \returns Seidenader::ObjectInterfaces::IOutputInfoListClassPtr
			//************************************
			virtual IOutputInfoListClassPtr GetOutputList( ) const = 0;

			//************************************
			/// Return the output list of this task object, filtered by functor.
			/// \returns Seidenader::ObjectInterfaces::IOutputInfoListClassPtr
			//************************************
			virtual IOutputInfoListClassPtr GetOutputList(IsObjectInfoAllowed func) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;