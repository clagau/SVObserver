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
#include "stdafx.h"
#include "TriggerHandling/SVCameraTriggerData.h"

namespace Seidenader { namespace TriggerHandling {

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
		m_timestamp.SetValue(index, timeStamp);
		m_lineInState.SetValue(index, state);
	}

	void SVCameraTriggerData::init()
	{
		// Setup Embedded Objects
		CString strName;
		strName.LoadString(IDS_OBJECTNAME_CAMERA_TRIGGER_TIMESTAMP);
		m_timestamp.SetObjectEmbedded(SVCameraTriggerTimestampGuid, this, strName);
		strName.LoadString(IDS_OBJECTNAME_CAMERA_TRIGGER_LINEINSTATE);
		m_lineInState.SetObjectEmbedded(SVCameraTriggerLineInStateGuid, this, strName);
	/* For Camera based Triggering
		RegisterEmbeddedObject(&m_timestamp, SVCameraTriggerTimestampGuid, IDS_OBJECTNAME_CAMERA_TRIGGER_TIMESTAMP, false, SVResetItemNone);
		RegisterEmbeddedObject(&m_lineInState, SVCameraTriggerLineInStateGuid, IDS_OBJECTNAME_CAMERA_TRIGGER_LINEINSTATE, false, SVResetItemNone);
	*/
		const DWORD dwAttributes = SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE;
		m_timestamp.ObjectAttributesAllowedRef() = dwAttributes;
		m_lineInState.ObjectAttributesAllowedRef() = dwAttributes;
	}

	void SVCameraTriggerData::destroy()
	{
	}

	SVIOEntryHostStructPtr SVCameraTriggerData::SetupLineStateInput(long objectDepth)
	{
		m_lineInState.SetObjectDepth(objectDepth);
		m_lineInState.SetDefaultValue( 0, false );
		m_lineInState.SetValue( 1, false );
		m_lineInState.ResetObject();

		SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject = &m_lineInState;
		pIOEntry->m_pValueParent = this;
		pIOEntry->m_ObjectType	= IO_CAMERA_DATA_INPUT;
		pIOEntry->m_PPQIndex	= 0;
		pIOEntry->m_Enabled		= true;

		return pIOEntry;
	}

	SVIOEntryHostStructPtr SVCameraTriggerData::SetupTimestampInput(long objectDepth)
	{
		m_timestamp.SetObjectDepth(objectDepth);
		m_timestamp.SetDefaultValue( 0, 0.0 );
		m_timestamp.SetValue( 1, 0.0 );
		m_timestamp.ResetObject();

		SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject = &m_timestamp;
		pIOEntry->m_pValueParent = this;
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
} /* namespace TriggerHandling */ } /* namespace Seidenader */
