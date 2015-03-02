//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for class inspection process
//******************************************************************************

#pragma once
#include "IObjectClass.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IInspectionProcess: virtual public IObjectClass
		{
		public:
			/**********
			 The method gets the interface of this ppq-object.
			***********/
			virtual IObjectClass* GetPPQInterface() const = 0;

			/**********
			 The method set default inputs.
			***********/
			virtual void SetDefaultInputs() = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;