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
#include "SVContainerLibrary/SelectorItem.h"
#pragma endregion Includes

namespace SvOi
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
		virtual SvCl::SelectorItemVectorPtr GetPPQSelectorList( const UINT Attribute ) const = 0;

		/***********
		This method gets the Interface to the Tool Set
		***********/
		virtual ITaskObject* GetToolSetInterface() const = 0;

		/***********
		This method gets the enable state for the AuxiliaryExtent
		***********/
		virtual bool getEnableAuxiliaryExtent() const = 0;

		/***********
		This method sets the enable state for the AuxiliaryExtent
		***********/
		virtual void setEnableAuxiliaryExtent(bool Enabled) = 0;

		virtual HRESULT RunOnce(ITaskObject* pTask) = 0;

		/***********
		This method gets the last product index
		***********/
		virtual long GetLastIndex() const = 0;

		/***********
		This method gets whether the inspection has a color camera
		***********/
		virtual bool IsColorCamera() const = 0;

		///Checks whether the reset state is set 
		virtual bool IsResetStateSet(unsigned long p_State) const = 0;

		/// sets the flag that forces the global extent data to update
		virtual void ForceOffsetUpdate() = 0;

		/// will global extent data be forced to update?
		virtual bool IsOffsetUpdateForced() = 0;

		/// is the new disable method set?
		virtual bool IsNewDisableMethodSet() = 0;
	};
} //namespace SvOi
