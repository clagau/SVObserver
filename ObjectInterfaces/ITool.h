//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for SVTool class
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVProtoBuf/SVO-Enum.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#pragma endregion Includes


#pragma region Declarations
namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}
#pragma endregion Declarations

namespace SvOi
{
typedef std::pair<std::string, uint32_t> ParameterPairForML;
typedef std::vector<ParameterPairForML> ParametersForML;

	//this class is an interface. It should only have pure virtual public methods and no member variables
	class ITool
	{
	public:
		virtual ~ITool() {}

		virtual bool areAuxExtentsAvailable() const = 0;
		virtual SvUl::NameObjectIdList getAvailableAuxSourceImages() const = 0;
		virtual SvUl::NameObjectIdPair getAuxSourceImage() const = 0;
		virtual HRESULT setAuxSourceImage(uint32_t objectID) = 0;
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
	};
} //namespace SvOi
