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
#include "SVObjectLibrary\SVClsIds.h"
#include "SVCameraTriggerData.h"
#pragma endregion Includes

namespace SvTh
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

	void SVCameraTriggerData::Set(long index, const NameVariantMap& rSettings)
	{
		_variant_t timeStamp;
		_variant_t lineState;
		NameVariantMap::const_iterator Iter( rSettings.end() );
		Iter = rSettings.find( _T("Timestamp") );
		if( rSettings.end() != Iter  )
		{
			timeStamp = Iter->second;
			Iter = rSettings.find( _T("LineState") );
			if( rSettings.end() != Iter  )
			{
				lineState = Iter->second;
				SetLineState(index, timeStamp.dblVal, (lineState.boolVal == VARIANT_TRUE) ? true : false);
			}
		}
	}

	void SVCameraTriggerData::SetLineState(long index, double timeStamp, bool state)
	{
		m_timestamp.SetValue( timeStamp, index );
		m_lineInState.SetValue( BOOL(state), index );
	}

	void SVCameraTriggerData::init()
	{
		// Setup Embedded Objects
		SVString Name = SvUl_SF::LoadSVString( IDS_OBJECTNAME_CAMERA_TRIGGER_TIMESTAMP );
		m_timestamp.SetObjectEmbedded(SVCameraTriggerTimestampGuid, this, Name.c_str() );
		Name = SvUl_SF::LoadSVString( IDS_OBJECTNAME_CAMERA_TRIGGER_LINEINSTATE );
		m_lineInState.SetObjectEmbedded(SVCameraTriggerLineInStateGuid, this, Name.c_str());
	/* For Camera based Triggering
		RegisterEmbeddedObject(&m_timestamp, SVCameraTriggerTimestampGuid, IDS_OBJECTNAME_CAMERA_TRIGGER_TIMESTAMP, false, SvOi::SVResetItemNone);
		RegisterEmbeddedObject(&m_lineInState, SVCameraTriggerLineInStateGuid, IDS_OBJECTNAME_CAMERA_TRIGGER_LINEINSTATE, false, SvOi::SVResetItemNone);
	*/
		const UINT cAttributes = SvOi::SV_VIEWABLE | SvOi::SV_PUBLISHABLE | SvOi::SV_ARCHIVABLE;
		m_timestamp.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
		m_lineInState.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	}

	void SVCameraTriggerData::destroy()
	{
	}

	SVIOEntryHostStructPtr SVCameraTriggerData::SetupLineStateInput(long objectDepth)
	{
		m_lineInState.SetObjectDepth(objectDepth);
		m_lineInState.SetObjectOwner(this);
		m_lineInState.SetDefaultValue(BOOL(false), false);
		m_lineInState.SetValue(BOOL(false), 1);
		m_lineInState.ResetObject();

		SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_lineInState));
		pIOEntry->m_ObjectType	= IO_CAMERA_DATA_INPUT;
		pIOEntry->m_PPQIndex	= 0;
		pIOEntry->m_Enabled		= true;

		return pIOEntry;
	}

	SVIOEntryHostStructPtr SVCameraTriggerData::SetupTimestampInput(long objectDepth)
	{
		m_timestamp.SetObjectDepth(objectDepth);
		m_timestamp.SetObjectOwner(this);
		m_timestamp.SetDefaultValue( 0.0, false );
		m_timestamp.SetValue( 0.0, 1 );
		m_timestamp.ResetObject();

		SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->setObject(dynamic_cast<SVObjectClass*> (&m_timestamp));
		pIOEntry->m_ObjectType	= IO_CAMERA_DATA_INPUT;
		pIOEntry->m_PPQIndex	= 0;
		pIOEntry->m_Enabled		= true;

		return pIOEntry;
	}

	SVString SVCameraTriggerData::GetTimestampName() const
	{
		return m_timestamp.GetName();
	}

	SVString SVCameraTriggerData::GetLineStateName() const
	{
		return m_lineInState.GetName();
	}
} //namespace SvTh
