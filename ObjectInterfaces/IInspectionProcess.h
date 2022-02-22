//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for class inspection process
//******************************************************************************
#pragma once

#pragma region Includes
#include "ICommand.h"
#include "SVProtoBuf/SVRC.h"
#pragma endregion Includes

namespace SvOi
{
class ISVImage;
class IObjectClass;
class ITaskObject;
}

namespace SvPb
{
class OverlayDesc;
}

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
		virtual void fillPPQSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, const UINT attribute, SvPb::ObjectSelectorType type) const = 0;

		virtual void fillCameraSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, const UINT attribute, SvPb::ObjectSelectorType type) const = 0;

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

		virtual HRESULT RunOnce() = 0;

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
		virtual bool IsNewDisableMethodSet() const = 0;

		///Calls a command in the inspection thread
		virtual HRESULT SubmitCommand(const ICommandPtr& rCommandPtr) = 0;

		virtual void BuildValueObjectMap() = 0;

		virtual uint32_t getFirstCamera() const = 0;

		virtual HRESULT addSharedCamera(uint32_t cameraID) = 0;

		virtual HRESULT resetTool(IObjectClass& rTool) = 0;

		virtual HRESULT propagateSizeAndPosition() = 0;

		virtual bool usePropagateSizeAndPosition() const = 0;

		/// Return the Overlay struct depend of the image.
		/// \param rImage [in] Image
		/// \returns SvPb::OverlayDesc
		virtual SvPb::OverlayDesc getOverlayStruct(const SvOi::ISVImage& rImage) const = 0;

		virtual void setResultListUpdateFlag() = 0;
	};
} //namespace SvOi
