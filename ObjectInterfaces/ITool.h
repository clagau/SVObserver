//******************************************************************************
/// COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for SVTool class
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVProtoBuf/SVO-Enum.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes



#pragma region Declarations
namespace SvStl
{
class MessageContainer;
using MessageContainerVector = std::vector<MessageContainer>;
}
class SVImageExtentClass;
namespace  SvTo
{
class SVToolExtentClass;
}


#pragma endregion Declarations

namespace SvOi
{
using ParameterPairForML = std::pair<std::string, uint32_t>;
using ParametersForML = std::vector<ParameterPairForML>;

	//this class is an interface. It should only have pure virtual public methods and no member variables
	class ITool
	{
	public:
		virtual ~ITool() {}

		virtual bool areAuxExtentsAvailable() const = 0;
		virtual SvPb::GetAvailableAncestorImagesResponse getAvailableAncestorImages() const = 0;
		virtual void SetToolImage(uint32_t objectID ) = 0;
		virtual uint32_t GetToolImage() const = 0;
		virtual HRESULT UpdateImageWithExtent() = 0;
		virtual bool SetFirstInputImageName( LPCTSTR FirstName) = 0;
		virtual long getToolPosition() const = 0;
		virtual void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const = 0;
		virtual HRESULT getParentExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const = 0;
		virtual ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const = 0;
		/// After adding tool, this method do the post-action (e.g. BuildValueObjectMap and SetDefaultFormula)
		virtual void finishAddTool() = 0;
		virtual SvPb::EAutoSize getAutoSizeEnabled() const = 0;
		virtual bool canResizeToParent() const = 0;
		virtual HRESULT updateExtentFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eAction, long dx, long dy) = 0;
		virtual HRESULT setExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double value) = 0;
		virtual HRESULT setExtentList(const ::google::protobuf::RepeatedPtrField<::SvPb::ExtentParameter> param ) = 0;
		virtual HRESULT setExtentToParent() = 0;
		virtual bool isAllowedLocation(const SvPb::SVExtentLocationPropertyEnum Location, SvPb::SVExtentDirectionsEnum Direction = SvPb::SVExtentDirectionBoth) const = 0;
		virtual SvPb::InspectionCmdResponse getInvalidDependencies() const = 0;
		virtual std::vector<std::string> getToolAdjustNameList() const = 0;
		virtual bool isToolActive() const = 0;
		virtual SVImageExtentClass& GetImageExtentRef() = 0;
		virtual SvTo::SVToolExtentClass& GetToolExtent() = 0;
		virtual bool WasEnabled() const =0;
	};
} //namespace SvOi
