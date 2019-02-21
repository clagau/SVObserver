//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputTool
//* .File Name       : $Workfile:   SVRemoteInputTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:40:04  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVBiUniqueMap.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVTool.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVRemoteInputTool : public SVToolClass
{
	SV_DECLARE_CLASS( SVRemoteInputTool )

public:
	SVRemoteInputTool( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEINPUTTOOL );
	virtual ~SVRemoteInputTool();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVObjectReference GetInputObject() const;
	HRESULT ClearInputObject();
	HRESULT SetInputObject(const std::string& rGuidName);

protected:
	struct SVDataElement
	{
		SVDataElement();
		SVDataElement( const std::string& p_rElementData, long p_ElementIdentifier );
		SVDataElement( const std::string& p_rElementData );
		SVDataElement( const SVDataElement& p_rObject );

		virtual ~SVDataElement();

		bool operator==( const SVDataElement& p_rObject ) const;
		bool operator<( const SVDataElement& p_rObject ) const;

		std::string m_ElementData;
		long m_ElementIdentifier;

	};

	typedef SVBiUniqueMap< SVDataElement, long >::type SVDataContainer;

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	SvOl::SVInObjectInfoStruct m_InputObjectInfo;

	SvVol::SVVariantValueObjectClass m_MatchedValueId;
	SvVol::SVVariantValueObjectClass m_MatchedValue;

	long m_ElementIdentifier;
	SVDataContainer m_Elements;
};

} //namespace SvTo