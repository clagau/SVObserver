//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object class
//******************************************************************************

#pragma once
#include "IObjectAppClass.h"
#include "IOutputInfoListClass.h"
#include "SVUtilityLibrary\SVSharedPtr.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		typedef SVSharedPtr< IOutputInfoListClass > IOutputInfoListClassSmartPointer;

		//this class is a interface. It should only have pure virtual public method and new member parameter
		class ITaskObject : virtual public IObjectAppClass
		{
		public:
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
			/// \returns Seidenader::ObjectInterfaces::IOutputInfoListClassSmartPointer
			//************************************
			virtual IOutputInfoListClassSmartPointer GetOutputList( ) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;