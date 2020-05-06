//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraTriggerData
//* .File Name       : $Workfile:   SVCameraTriggerData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:48:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCameraTriggerData.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TriggerHandling/TriggerDispatcher.h"
#pragma endregion Includes

namespace SvTi
{
	SVCameraTriggerData::SVCameraTriggerData()
	: SVObjectClass(nullptr, IDS_OBJECTNAME_CAMERA_TRIGGER_DATA)
	{
		init();
	}

	SVCameraTriggerData::~SVCameraTriggerData()
	{
		destroy();
	}

	void SVCameraTriggerData::Set(const IntVariantMap& rSettings)
	{
		_variant_t timeStamp;
		_variant_t lineState;
		IntVariantMap::const_iterator Iter( rSettings.end() );
		Iter = rSettings.find(SvTh::TriggerDataEnum::TimeStamp);
		if( rSettings.end() != Iter  )
		{
			timeStamp = Iter->second;
			Iter = rSettings.find(SvTh::TriggerDataEnum::LineState);
			if( rSettings.end() != Iter  )
			{
				lineState = Iter->second;
				SetLineState(timeStamp.dblVal, (lineState.boolVal == VARIANT_TRUE) ? true : false);
			}
		}
	}

	void SVCameraTriggerData::SetLineState(double timeStamp, bool state)
	{
		m_timestamp.SetValue( timeStamp);
		m_lineInState.SetValue( BOOL(state));
	}

	void SVCameraTriggerData::init()
	{
		// Setup Embedded Objects
		std::string Name = SvUl::LoadStdString( IDS_OBJECTNAME_CAMERA_TRIGGER_TIMESTAMP );
		m_timestamp.SetObjectEmbedded(SvPb::CameraTriggerTimestampEId, this, Name.c_str() );
		Name = SvUl::LoadStdString( IDS_OBJECTNAME_CAMERA_TRIGGER_LINEINSTATE );
		m_lineInState.SetObjectEmbedded(SvPb::CameraTriggerLineInStateEId, this, Name.c_str());
	/* For Camera based Triggering
		RegisterEmbeddedObject(&m_timestamp, SvPb::CameraTriggerTimestampEId, IDS_OBJECTNAME_CAMERA_TRIGGER_TIMESTAMP, false, SvOi::SVResetItemNone);
		RegisterEmbeddedObject(&m_lineInState, SvPb::CameraTriggerLineInStateEId, IDS_OBJECTNAME_CAMERA_TRIGGER_LINEINSTATE, false, SvOi::SVResetItemNone);
	*/
		const UINT cAttributes = SvPb::viewable | SvPb::publishable | SvPb::archivable;
		m_timestamp.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
		m_lineInState.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	}

	void SVCameraTriggerData::destroy()
	{
	}

	SVIOEntryHostStructPtr SVCameraTriggerData::SetupLineStateInput()
	{
		m_lineInState.SetObjectOwner(this);
		m_lineInState.SetDefaultValue(BOOL(false), false);
		m_lineInState.SetValue(BOOL(false));
		m_lineInState.ResetObject();

		SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();
		pIOEntry->setLinkedObject(dynamic_cast<SVObjectClass*> (&m_lineInState));
		pIOEntry->m_ObjectType	= IO_CAMERA_DATA_INPUT;
		pIOEntry->m_PPQIndex	= 0;
		pIOEntry->m_Enabled		= true;

		return pIOEntry;
	}

	SVIOEntryHostStructPtr SVCameraTriggerData::SetupTimestampInput()
	{
		m_timestamp.SetObjectOwner(this);
		m_timestamp.SetDefaultValue( 0.0, false );
		m_timestamp.SetValue(0.0);
		m_timestamp.ResetObject();

		SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();
		pIOEntry->setLinkedObject(dynamic_cast<SVObjectClass*> (&m_timestamp));
		pIOEntry->m_ObjectType	= IO_CAMERA_DATA_INPUT;
		pIOEntry->m_PPQIndex	= 0;
		pIOEntry->m_Enabled		= true;

		return pIOEntry;
	}

	std::string SVCameraTriggerData::GetTimestampName() const
	{
		return m_timestamp.GetName();
	}

	std::string SVCameraTriggerData::GetLineStateName() const
	{
		return m_lineInState.GetName();
	}
} //namespace SvTi
