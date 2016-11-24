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
#include "ISelectorItemVector.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IInspectionProcess
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
			This method gets the PPQ Variables selector list
			***********/
			virtual ISelectorItemVectorPtr GetPPQSelectorList( const UINT Attribute ) const = 0;

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

			/***********
			This method gets the last product index
			***********/
			virtual long GetLastIndex() const = 0;

			/***********
			This method gets whether the inspection has a color camera
			***********/
			virtual bool IsColorCamera() const = 0;

			/***********
			This method adds an input request
			***********/
			virtual bool AddInputRequest( const SVGUID& rGuid, const _variant_t& rValue ) = 0;

			/***********
			This method adds an input request marker
			***********/
			virtual bool AddInputRequestMarker() = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;