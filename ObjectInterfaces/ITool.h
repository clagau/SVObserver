//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for SVTool class
//******************************************************************************

#pragma once
#pragma region Includes
#include "Definitions/SVExtentEnums.h"
#include "SVUtilityLibrary/NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in #include "SVUtilityLibrary\SVGUID.h"
class SVGUID;
namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}
#pragma endregion Declarations

namespace SvOi
{
typedef std::pair<std::string, SVGUID> ParameterPairForML;
typedef std::vector<ParameterPairForML> ParametersForML;

	//this class is a interface. It should only have pure virtual public method and no member variables
	class ITool
	{
	public:
		virtual ~ITool() {}

		virtual bool areAuxExtentsAvailable() const = 0;
		virtual SvUl::NameGuidList getAvailableAuxSourceImages() const = 0;
		virtual SvUl::NameGuidPair getAuxSourceImage() const = 0;
		virtual HRESULT setAuxSourceImage(const SVGUID& rObjectID) = 0;
		virtual void SetToolImage( const SVGUID& rObjectID ) = 0;
		virtual HRESULT UpdateImageWithExtent() = 0;
		virtual bool SetFirstInputImageName( LPCTSTR FirstName) = 0;
		virtual long getToolPosition() const = 0;
		// getExtentProperty cannot be const because GetImageExtents is not const
		virtual HRESULT getExtentProperty(const SvDef::SVExtentPropertyEnum& rExtentProperty, double& rValue) = 0;
	virtual ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const = 0;
		/// After adding tool, this method do the post-action (e.g. BuildValueObjectMap and SetDefaultFormula)
		virtual void finishAddTool() = 0;
	};
} //namespace SvOi
