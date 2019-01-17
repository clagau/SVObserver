//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputTool
//* .File Name       : $Workfile:   SVRemoteInputTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   13 Nov 2014 10:10:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVRemoteInputTool.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInspectionProcess.h"

SV_IMPLEMENT_CLASS( SVRemoteInputTool, SVRemoteInputToolGuid )

SVRemoteInputTool::SVRemoteInputTool( SVObjectClass* POwner, int StringResourceID )
: SVToolClass( POwner, StringResourceID )
, m_ElementIdentifier( 0 )
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVRemoteInputToolObjectType;

	// Register an empty input object
	m_InputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputObjectInfo, _T( "RemoteInputValue" ) );

	// Remove Embedded Extents
	removeEmbeddedExtents();

	RegisterEmbeddedObject( &m_MatchedValueId, SVRemoteInputToolMatchedValueIdGuid, IDS_OBJECTNAME_MATCHEDVALUEID, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_MatchedValue, SVRemoteInputToolMatchedValueGuid, IDS_OBJECTNAME_MATCHEDVALUE, false, SvOi::SVResetItemNone );

	m_MatchedValueId.SetDefaultValue( _variant_t( 0 ), true );
	m_MatchedValue.SetDefaultValue( _variant_t( _T( "" ) ), true );

	// Set default inputs and outputs
	addDefaultInputObjects();

}

SVRemoteInputTool::~SVRemoteInputTool()
{
}

bool SVRemoteInputTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_Status = SVToolClass::CreateObject(rCreateStructure);

	//l_Status &= ( S_OK == SVInputStreamManager::Instance().InsertInputStream( GetCompleteName().c_str(), GetUniqueObjectID() ) );

	m_isCreated = l_Status;

	m_MatchedValueId.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_MatchedValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	return l_Status;
}

bool SVRemoteInputTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	SvOl::ValidateInput(m_InputObjectInfo);

	return Result && ValidateLocal(pErrorMessages);
}

HRESULT SVRemoteInputTool::ClearInputObject()
{
	HRESULT l_Status = S_OK;

	m_InputObjectInfo.SetInputObject( nullptr );

	return l_Status;
}

HRESULT SVRemoteInputTool::SetInputObject(const std::string& rGuidName)
{
	HRESULT l_Status = S_OK;

	SVObjectReference ObjectRef{ rGuidName };
	m_InputObjectInfo.SetInputObject(ObjectRef);

	if( !( m_InputObjectInfo.IsConnected() ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVObjectReference SVRemoteInputTool::GetInputObject() const
{
	if( m_InputObjectInfo.IsConnected() && nullptr != m_InputObjectInfo.GetInputObjectInfo().getObject())
	{
		return m_InputObjectInfo.GetInputObjectInfo().GetObjectReference();
	}
	return SVObjectReference();
}

bool SVRemoteInputTool::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = SVToolClass::onRun( rRunStatus, pErrorMessages );

	std::string MatchString;
	std::string MatchedString;
	long l_MatchedStringId = 0;
	long l_Identifier = 0;

	Result = Result && ValidateLocal(pErrorMessages);

	if( Result )
	{

		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (m_InputObjectInfo.GetInputObjectInfo().getObject());
		if( nullptr !=  pValueObject )
		{
				Result = ( S_OK == pValueObject->getValue( MatchString ) );
		}
		else
		{
			Result = false;
		}


		if (!Result)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if( Result )
	{
		SVDataContainer::index_iterator< from >::type l_Iter = m_Elements.get< from >().find( SVDataElement( MatchString ) );

		if( l_Iter != m_Elements.get< from >().end() )
		{
			rRunStatus.SetPassed();

			l_Identifier = l_Iter->second;

			MatchedString = l_Iter->first.m_ElementData;
			l_MatchedStringId = l_Iter->first.m_ElementIdentifier;
		}
	}

	m_MatchedValueId.SetValue(l_MatchedStringId);
	m_MatchedValue.SetValue(_variant_t(MatchedString.c_str()));

	if( Result && ( l_Identifier != 0 ) )
	{
		SVDataContainer::index_iterator< to >::type l_Iter = m_Elements.get< to >().begin();

		while( l_Iter != m_Elements.get< to >().end() )
		{
			bool l_Done = ( l_Iter->second == l_Identifier );

			SVDataContainer::index_iterator< to >::type l_Temp = l_Iter++;

			m_Elements.get< to >().erase( l_Temp );

			if( l_Done )
			{
				break;
			}
		}
	}
	else
	{
		rRunStatus.SetFailed();
	}

	return Result;
}

SVRemoteInputTool::SVDataElement::SVDataElement()
: m_ElementData( _T( "" ) )
, m_ElementIdentifier( 0 )
{
}

SVRemoteInputTool::SVDataElement::SVDataElement( const std::string& p_rElementData, long p_ElementIdentifier )
: m_ElementData( p_rElementData )
, m_ElementIdentifier( p_ElementIdentifier )
{
}

SVRemoteInputTool::SVDataElement::SVDataElement( const std::string& p_rElementData )
: m_ElementData( p_rElementData )
, m_ElementIdentifier( 0 )
{
}

SVRemoteInputTool::SVDataElement::SVDataElement( const SVDataElement& p_rObject )
: m_ElementData( p_rObject.m_ElementData )
, m_ElementIdentifier( p_rObject.m_ElementIdentifier )
{
}

SVRemoteInputTool::SVDataElement::~SVDataElement()
{
}

bool SVRemoteInputTool::SVDataElement::operator==( const SVDataElement& p_rObject ) const
{
	return ( m_ElementData == p_rObject.m_ElementData );
}

bool SVRemoteInputTool::SVDataElement::operator<( const SVDataElement& p_rObject ) const
{
	return ( m_ElementData < p_rObject.m_ElementData );
}

bool SVRemoteInputTool::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if ( !m_InputObjectInfo.IsConnected() || nullptr == m_InputObjectInfo.GetInputObjectInfo().getObject() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	
	return true;
}
