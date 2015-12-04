//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for class inspection process
//******************************************************************************

#pragma once
#pragma region Includes
#include "IObjectClass.h"
#include "ITaskObject.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IInspectionProcess: virtual public IObjectClass
		{
		public:
			virtual ~IInspectionProcess() {}
			/**********
			 The method gets the interface of this ppq-object.
			***********/
			virtual IObjectClass* GetPPQInterface() const = 0;

			/**********
			 The method set default inputs.
			***********/
			virtual void SetDefaultInputs() = 0;

			/***********
			This method gets the names of the PPQ inputs
			***********/
			virtual SVStringArray GetPPQInputNames() const = 0;

			/***********
			This method gets the Interface to the Tool Set
			***********/
			virtual ITaskObject* GetToolSetInterface() const = 0;

			/***********
			This method gets the enable state for the AuxiliaryExtent (which is not a value object)
			***********/
			virtual long GetEnableAuxiliaryExtent() const = 0;

			/***********
			This method sets the enable state for the AuxiliaryExtent (which is not a value object)
			***********/
			virtual void SetEnableAuxiliaryExtent(long enabled) = 0;

			virtual HRESULT RunOnce(ITaskObject* pTask) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;