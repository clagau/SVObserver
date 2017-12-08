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
#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVContainerLibrary/SVBiUniqueMap.h"
#include "SVObjectLibrary/SVObjectNotifyTemplate.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVObjectCommandDataJson.h"
#include "InspectionEngine/SVTool.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"

#pragma endregion Includes

class SVRemoteInputTool : 
	public SVToolClass,
	public SVObjectNotifyTemplate< SVObjectCommandDataJsonPtr >
{
	SV_DECLARE_CLASS( SVRemoteInputTool )

public:
	SVRemoteInputTool( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEINPUTTOOL );
	virtual ~SVRemoteInputTool();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr ) override;

	SVObjectClass* GetInputObject() const;
	HRESULT ClearInputObject();
	HRESULT SetInputObject( const SVGUID& p_rObjectId );

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

protected:
	typedef SVTQueueObject< SVObjectCommandDataJsonPtr > SVCommandQueue;

	class SVCommandQueueElement : public SVCommandTemplate
	{
	public:
		SVCommandQueueElement( const SVGUID& p_rObjectId, SVObjectCommandDataJsonPtr& p_rJsonCommandPtr );

		virtual ~SVCommandQueueElement();

		virtual HRESULT Execute() override;

	private:
		SVCommandQueueElement();

		SVGUID m_ObjectId;
		SVObjectCommandDataJsonPtr m_JsonCommandPtr;
	};

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

	HRESULT ProcessCommandQueue();

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	SVInObjectInfoStruct m_InputObjectInfo;

	SVVariantValueObjectClass m_MatchedValueId;
	SVVariantValueObjectClass m_MatchedValue;

	long m_ElementIdentifier;
	SVCommandQueue m_Commands;
	SVDataContainer m_Elements;
};

