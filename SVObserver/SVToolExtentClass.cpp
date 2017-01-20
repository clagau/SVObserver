//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentClass
//* .File Name       : $Workfile:   SVToolExtentClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   08 May 2013 16:44:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign.hpp>
#include "SVToolExtentClass.h"
#include "SVOCore/SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#pragma endregion Includes

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVToolExtentClass::SVTranslationFilterSet SVToolExtentClass::m_LinearToolTranslations = boost::assign::list_of
	( SVExtentTranslationProfile )
	( SVExtentTranslationProfileShift )
;

SVToolExtentClass::SVToolExtentClass()
{
	Initialize();
}

SVToolExtentClass::~SVToolExtentClass()
{
	Initialize();
}

HRESULT SVToolExtentClass::ValidExtentAgainstParentImage( const SVImageExtentClass& p_rImageExtent ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage && nullptr != m_psvToolImage->GetParentImage() )
	{
		l_Status = m_psvToolImage->GetParentImage()->ValidateAgainstOutputExtents( p_rImageExtent );
	}
	else
	{
		l_Status = S_FALSE;
	}
	return l_Status;
}

HRESULT SVToolExtentClass::UpdateExtentToParentExtents( unsigned long p_ulIndex, SVImageExtentClass& p_rNewExtent )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage )
	{
		SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

		SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

		if( nullptr != l_pParent )
		{
			double l_dPosX=0, l_dPosY=0, l_dWidth, l_dHeight;
			SVImageExtentClass l_tmpExt = l_pParent->GetImageExtents();
			// Get the Width and Height form the parent extent. Set the Position point to zero, zero.
			l_Status = l_tmpExt.GetExtentProperty( SVExtentPropertyOutputHeight, l_dHeight );
			KeepPrevError( l_Status, l_tmpExt.GetExtentProperty( SVExtentPropertyOutputWidth, l_dWidth ));
			KeepPrevError( l_Status, p_rNewExtent.SetExtentProperty( SVExtentPropertyPositionPointX, l_dPosX ));
			KeepPrevError( l_Status, p_rNewExtent.SetExtentProperty( SVExtentPropertyPositionPointY, l_dPosY ));
			KeepPrevError( l_Status, p_rNewExtent.SetExtentProperty( SVExtentPropertyHeight, l_dHeight ));
			KeepPrevError( l_Status, p_rNewExtent.SetExtentProperty( SVExtentPropertyWidth, l_dWidth ));
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( S_OK == l_Status  )
		{
			if( p_rNewExtent != m_psvToolImage->GetImageExtents() )
			{
				l_Status = m_psvToolImage->UpdateImage( p_rNewExtent );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVToolExtentClass::GetParentExtent( SVImageExtentClass& p_rParent ) const
{
	HRESULT l_Status = S_OK;
	SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

	if( nullptr != l_pParent )
	{
		p_rParent = l_pParent->GetImageExtents();
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVToolExtentClass::UpdateExtentAgainstParentImage( unsigned long p_ulIndex, const SVImageExtentClass& p_rImageExtent )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage )
	{
		SVImageExtentClass l_Extent = p_rImageExtent;

		SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

		SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

		if( S_OK == l_Status  )
		{
			if( l_Extent != m_psvToolImage->GetImageExtents() )
			{
				l_Status = m_psvToolImage->UpdateImage( l_Extent );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVToolExtentClass::UpdateImageWithExtent( unsigned long p_ulIndex, SVToolExtentTypeEnum p_ToolExtentType )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage )
	{
		bool l_Update = true;

		if( l_Update )
		{
			SVImageExtentClass l_Extent;

			SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

			SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

			if( nullptr != l_pParent )
			{
				if( ( SVImageTypeLogicalAndPhysical == l_Type ) && ( SVExtentTranslationProfile == GetTranslation() ) )
				{
					l_Type = SVImageTypePhysical;

					m_psvToolImage->UpdateImage( l_Type );
				}

				if( SVImageTypeDependent == l_Type || SVImageTypeVirtual == l_Type )
				{
					l_Extent = l_pParent->GetImageExtents();

					l_Extent.SetTranslation( GetTranslation() );

					SVExtentPositionClass l_Position = l_Extent.GetPosition();

					l_Status = l_Position.SetExtentProperty( SVExtentPropertyPositionPoint, 0.0 );

					if( S_OK == l_Status  )
					{
						l_Status = l_Extent.SetPosition( l_Position );
					}

					l_Status = SetImageExtent( p_ulIndex, l_Extent );
				}
				else
				{
					l_Status = GetImageExtent( l_Extent );

					SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(m_psvToolImage->GetInspection());

					if ( nullptr != pInspection && pInspection->IsResetStateSet( SVResetAutoMoveAndResize ) )
					{
						if( l_Extent != m_psvToolImage->GetImageExtents() )
						{
							if( S_OK != m_psvToolImage->ValidateAgainstParentExtents( l_Extent ) )
							{
								l_pParent->GetImageExtentsToFit( l_Extent, l_Extent );

								SetImageExtent( p_ulIndex, l_Extent );
							}
						}
					}

					if( SVColorToolExtent == p_ToolExtentType )
					{
						SVExtentPositionClass l_Position = l_Extent.GetPosition();

						l_Status = l_Position.SetExtentProperty( SVExtentPropertyPositionPoint, 0.0 );

						if( S_OK == l_Status )
						{
							l_Status = l_Extent.SetPosition( l_Position );
						}
					}

					if( SVTransformationToolExtent == p_ToolExtentType )
					{
						double l_Value = 0.0;
						SVImageExtentClass l_InputExtent = l_pParent->GetImageExtents();

						if( S_OK == l_InputExtent.GetExtentProperty( SVExtentPropertyOutputWidth, l_Value ) )
						{
							l_Extent.SetExtentProperty( SVExtentPropertyWidth, l_Value );
						}

						if( S_OK == l_InputExtent.GetExtentProperty( SVExtentPropertyOutputHeight, l_Value ) )
						{
							l_Extent.SetExtentProperty( SVExtentPropertyHeight, l_Value );
						}

						l_Status = SetImageExtent( p_ulIndex, l_Extent );

						HRESULT l_Temp = GetImageExtent( l_Extent );

						if( S_OK == l_Status  )
						{
							l_Status = l_Temp;
						}
					}
				}
			}
			else
			{
				l_Status = GetImageExtent( l_Extent );
			}

			if( S_OK == l_Status )
			{
				if( l_Extent != m_psvToolImage->GetImageExtents() )
				{
					l_Status = m_psvToolImage->UpdateImage( l_Extent );
				}
			}

			if( S_OK == l_Status )
			{
				if( nullptr != m_psvTool )
				{
					SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(m_psvTool->GetInspection());
					if (pInspection)
					{
						pInspection->m_bForceOffsetUpdate = true;
					}
				}

				if( S_OK == l_Status )
				{
					l_Status = UpdateOffsetData( true );
				}
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

SVExtentTranslationEnum SVToolExtentClass::GetTranslation()
{
	return m_eTranslation;
}

HRESULT SVToolExtentClass::SetTranslation( SVExtentTranslationEnum p_eTranslation )
{
	HRESULT l_hrOk = S_FALSE;

	SVImageExtentClass l_svExtent;

	l_hrOk = l_svExtent.SetTranslation( p_eTranslation );

	if ( S_OK == l_hrOk )
	{
		m_eTranslation = p_eTranslation;
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::SetTranslation( SVExtentTranslationEnum p_eTranslation, long p_DataIndex )
{
	HRESULT l_hrOk = S_FALSE;

	SVTranslationFilterSet::iterator l_OldIter = m_LinearToolTranslations.find( m_eTranslation );
	SVTranslationFilterSet::iterator l_NewIter = m_LinearToolTranslations.find( p_eTranslation );

	if( ( m_eTranslation != p_eTranslation ) && ( l_OldIter != m_LinearToolTranslations.end() ) && ( l_NewIter != m_LinearToolTranslations.end() ) )
	{
		_variant_t l_PointY;
		_variant_t l_Height;

		if( ( S_OK == GetExtentValue( SVExtentPropertyPositionPointY, l_PointY.GetVARIANT() ) ) &&
		    ( S_OK == GetExtentValue( SVExtentPropertyHeight, l_Height.GetVARIANT() ) ) )
		{
			if( SVExtentTranslationProfileShift == m_eTranslation )
			{
				long l_NewY = l_PointY;

				l_NewY += static_cast< long >( l_Height ) / 2;

				l_hrOk = SetExtentValue( SVExtentPropertyPositionPointY, p_DataIndex, l_NewY );
			}
			else
			{
				long l_NewY = l_PointY;

				l_NewY -= static_cast< long >( l_Height ) / 2;

				l_hrOk = SetExtentValue( SVExtentPropertyPositionPointY, p_DataIndex, l_NewY );
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		m_eTranslation = p_eTranslation;
	}
	else
	{
		l_hrOk = SetTranslation( p_eTranslation );
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::GetExtentShape( SVExtentPropertyEnum p_eProperty, SVExtentShapeEnum &p_reValue ) const
{
	HRESULT l_hrOk = S_OK;

	p_reValue = m_eShape;

	return l_hrOk;
}

HRESULT SVToolExtentClass::GetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *&p_rpsvValue ) const
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svProperties.GetExtentObject( p_eProperty, p_rpsvValue );

	return l_hrOk;
}

HRESULT SVToolExtentClass::SetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svProperties.SetExtentObject( p_eProperty, p_psvValue );

	return l_hrOk;
}

HRESULT SVToolExtentClass::GetExtentValue( SVExtentPropertyEnum p_eProperty, VARIANT& p_rValue ) const
{
	HRESULT l_hrOk = S_OK;

	_variant_t l_Value;
	SVValueObjectClass* l_pValueObject = nullptr;

	l_Value.Attach( p_rValue );
	l_Value.Clear();

	l_hrOk = GetExtentObject( p_eProperty, l_pValueObject );

	if( nullptr != l_pValueObject )
	{
		l_hrOk = l_pValueObject->GetValue( l_Value.GetVARIANT() );
	}
	else if( S_OK == l_hrOk )
	{
		l_hrOk = E_FAIL;
	}

	p_rValue = l_Value.Detach();

	return l_hrOk;
}

HRESULT SVToolExtentClass::SetExtentValue( SVExtentPropertyEnum p_eProperty, long p_DataIndex, const _variant_t& p_rValue )
{
	HRESULT l_hrOk = S_OK;

	SVValueObjectClass* l_pValueObject = nullptr;

	l_hrOk = GetExtentObject( p_eProperty, l_pValueObject );

	if( nullptr != l_pValueObject  )
	{
		double l_Value = p_rValue;

		l_hrOk = l_pValueObject->SetValue( p_DataIndex, l_Value );
	}
	else if( S_OK == l_hrOk )
	{
		l_hrOk = E_FAIL;
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::GetImageExtent( SVImageExtentClass &p_rsvImageExtent ) const
{
	HRESULT l_hrOk = p_rsvImageExtent.Initialize();

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvImageExtent.SetTranslation( m_eTranslation );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = m_svProperties.GetProperties( p_rsvImageExtent );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvImageExtent.UpdateData();
	}
	else
	{
		p_rsvImageExtent.Initialize();
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent ) const
{
	HRESULT l_hrOk = p_rsvImageExtent.Initialize();

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvImageExtent.SetTranslation( m_eTranslation );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = m_svProperties.GetProperties( p_ulIndex, p_rsvImageExtent );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvImageExtent.UpdateData();
	}
	else
	{
		p_rsvImageExtent.Initialize();
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_OK;

	double l_dValue = 0.0;

	SVExtentTranslationEnum translation = p_svImageExtent.GetTranslation();

	if ( SVExtentTranslationUnknown != translation )
	{
		m_eTranslation = translation;
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointX, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointX, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointY, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointY, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLineX, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointEndOfLineX, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLineY, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointEndOfLineY, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyRotationAngle, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyRotationAngle, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyTranslationOffsetX, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyTranslationOffsetY, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyHeight, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyHeight, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyWidth, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyWidth, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyStartAngle, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyStartAngle, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyEndAngle, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyEndAngle, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyInnerRadius, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyInnerRadius, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyOuterRadius, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyOuterRadius, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyHeightScaleFactor, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyHeightScaleFactor, p_ulIndex, l_dValue );
	}

	if( S_OK == p_svImageExtent.GetExtentProperty( SVExtentPropertyWidthScaleFactor, l_dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyWidthScaleFactor, p_ulIndex, l_dValue );
	}
	return l_hrOk;
}

namespace local	// struct local
{
	struct IsFormula
	{
		IsFormula(SVToolExtentClass* pToolExtents) : m_pToolExtents(pToolExtents) {}
		bool operator () (SVExtentPropertyEnum eProperty)
		{
			SVExtentPropertyInfoStruct info;
			HRESULT hr = m_pToolExtents->GetExtentPropertyInfo(eProperty, info);
			if ( S_OK == hr  )
			{
				return info.bFormula || info.bHidden;
			}
			return false;
		}
		SVToolExtentClass* m_pToolExtents;
	};
}
 
HRESULT SVToolExtentClass::GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList )
{
	HRESULT hr = S_OK;

	p_rPropertyList.clear();

	SVImageExtentClass l_svExtents;
	GetImageExtent( l_svExtents );
	l_svExtents.GetExtentPropertyList( SVExtentPropertyAll, p_rPropertyList );

	SVExtentPropertyListType::iterator iter;
	// remove those that are formula
	iter = std::remove_if( p_rPropertyList.begin(), p_rPropertyList.end(), local::IsFormula(this) );
	if ( iter != p_rPropertyList.end() )
	{
		p_rPropertyList.erase( iter, p_rPropertyList.end() );
	}
	if ( SVExtentTranslationPolarUnwrap == l_svExtents.GetTranslation() )
	{
		// special case for Rotation Angle
		SVExtentPropertyInfoStruct infoStart;
		SVExtentPropertyInfoStruct infoEnd;
		GetExtentPropertyInfo( SVExtentPropertyEndAngle, infoEnd );
		GetExtentPropertyInfo( SVExtentPropertyStartAngle, infoStart );
		if ( infoStart.bFormula || infoEnd.bFormula )
		{
			iter = std::find(p_rPropertyList.begin(), p_rPropertyList.end(), SVExtentPropertyRotationAngle );
			if ( iter != p_rPropertyList.end() )
			{
				p_rPropertyList.erase( iter );
			}
		}
	}
	// objects - on ResetObject - set toolextent flag
	return hr;
}

HRESULT SVToolExtentClass::GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData )
{
	HRESULT l_svOk = S_FALSE;

	if( nullptr == m_svRootOffsetData.m_psvImage )
	{
		l_svOk = UpdateOffsetDataToImage( m_svRootOffsetData, nullptr );
	}

	p_rsvOffsetData = m_svRootOffsetData;

	if( nullptr != p_rsvOffsetData.m_psvImage )
	{
		l_svOk = S_OK;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::GetSelectedOffsetData( SVExtentOffsetStruct& p_rsvOffsetData )
{
	HRESULT l_svOk = S_FALSE;

	if( nullptr == m_psvSelectedImage )
	{
		p_rsvOffsetData = m_svRootOffsetData;
	}
	else
	{
		p_rsvOffsetData = m_svSelectedOffsetData;
	}

	if( nullptr != p_rsvOffsetData.m_psvImage )
	{
		l_svOk = S_OK;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage )
{
	HRESULT l_svOk = S_OK;

	if( nullptr != m_psvToolImage )
	{
		SVExtentOffsetStruct l_svOffsetData;

		SVImageClass* l_psvImageParent = m_psvToolImage->GetParentImage();

		if( nullptr != l_psvImageParent )
		{
			SVToolClass* l_psvToolParent = dynamic_cast<SVToolClass*>(l_psvImageParent->GetTool());

			if( l_psvImageParent != p_svToolImage && l_psvImageParent != m_psvToolImage && 
				nullptr != l_psvToolParent && l_psvToolParent != m_psvTool )
			{
				l_svOk = l_psvToolParent->UpdateOffsetDataToImage( l_svOffsetData, p_svToolImage );
			}
			else if( l_psvImageParent == p_svToolImage || nullptr == p_svToolImage )
			{
				SVImageExtentClass l_svExtent;

				l_svOk = GetImageExtent( l_svExtent );

				if( S_OK == l_svOk )
				{
					l_svOk = l_svExtent.UpdateSourceOffset( l_svOffsetData );
				}
			}
			else
			{
				l_svOk = E_FAIL;
			}

			if( S_OK == l_svOk )
			{
				if( nullptr == p_rsvOffsetData.m_psvImage )
				{
					if( nullptr != p_rsvOffsetData.m_psvRootImage )
					{
						l_svOffsetData.m_psvRootImage = p_rsvOffsetData.m_psvRootImage;
					}
					else
					{
						if( nullptr != m_svRootOffsetData.m_psvRootImage )
						{
							l_svOffsetData.m_psvRootImage = m_svRootOffsetData.m_psvRootImage;
						}
						else
						{
							l_svOffsetData.m_psvRootImage = l_psvImageParent;
						}
					}

					l_svOffsetData.m_psvImage = l_psvImageParent;
					l_svOffsetData.m_bAlwaysUpdate |= m_bAlwaysUpdate;

					if( nullptr != l_psvImageParent )
					{
						l_svOffsetData.m_csImageName = l_psvImageParent->GetCompleteName();
					}
				}
			}
		}
		p_rsvOffsetData = l_svOffsetData;
	}
	else
	{
		l_svOk = E_FAIL;
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetData( bool p_bForceUpdate )
{
	HRESULT l_svOk = S_OK;

	try
	{
		if( p_bForceUpdate ||
			m_bAlwaysUpdate ||
			m_svRootOffsetData.m_bAlwaysUpdate || 
			nullptr == m_svRootOffsetData.m_psvImage )
		{
			if( nullptr != m_psvToolImage )
			{
				SVExtentOffsetStruct l_svOffsetData;

				SVImageClass* l_psvImageParent = m_psvToolImage->GetParentImage();

				if( nullptr != l_psvImageParent )
				{
					SVToolClass* l_psvToolParent = dynamic_cast<SVToolClass*>(l_psvImageParent->GetTool());

					if( nullptr != l_psvToolParent && l_psvToolParent != m_psvTool )
					{
						l_svOk = l_psvToolParent->GetRootOffsetData( l_svOffsetData );
					}
					else
					{
						l_svOffsetData.m_psvRootImage = l_psvImageParent;
						l_svOffsetData.m_psvImage = l_psvImageParent;
						l_svOffsetData.m_csImageName = l_psvImageParent->GetCompleteName();
					}
				}
				else
				{
					l_svOffsetData.m_psvRootImage = m_psvToolImage;
					l_svOffsetData.m_psvImage = m_psvToolImage;
					l_svOffsetData.m_csImageName = m_psvToolImage->GetCompleteName();
				}

				if( S_OK == l_svOk )
				{
					SVImageExtentClass l_svExtent;

					l_svOk = GetImageExtent( l_svExtent );

					if( S_OK == l_svOk )
					{
						l_svOk = l_svExtent.UpdateSourceOffset( l_svOffsetData );
					}
				}

				if( S_OK == l_svOk )
				{
					l_svOffsetData.m_bAlwaysUpdate |= m_bAlwaysUpdate;

					m_svRootOffsetData = l_svOffsetData;
				}
			}
		}

		if( S_OK == l_svOk )
		{
			if( p_bForceUpdate ||
				m_bAlwaysUpdate ||
				m_svSelectedOffsetData.m_bAlwaysUpdate ||
				nullptr == m_svSelectedOffsetData.m_psvImage ||
				m_svSelectedOffsetData.m_psvImage != m_psvSelectedImage )
			{
				if( nullptr == m_psvSelectedImage )
				{
					m_svSelectedOffsetData.Initialize();
				}
				else
				{
					SVExtentOffsetStruct l_svOffsetData;

					l_svOk = UpdateOffsetDataToImage( l_svOffsetData, m_psvSelectedImage );

					if( S_OK == l_svOk )
					{
						m_svSelectedOffsetData = l_svOffsetData;
					}
				}
			}
		}
	}
	catch( ... )
	{
		l_svOk = S_FALSE;
	}

	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetData( bool p_bForceUpdate, SVImageClass* p_svToolImage )
{
	SetToolImage( p_svToolImage );

	return UpdateOffsetData( p_bForceUpdate );
}

HRESULT SVToolExtentClass::TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut )
{
	HRESULT l_svOk;

	SVImageExtentClass l_svExtent;

	l_svOk = GetImageExtent( l_svExtent );

	if( S_OK == l_svOk )
	{
		if( m_svRootOffsetData.m_bIsLinear )
		{
			p_svIn = SVRotatePoint( m_svRootOffsetData.m_svRotationPoint, p_svIn, m_svRootOffsetData.m_dRotationAngle );

			p_rsvOut.m_dPositionX = m_svRootOffsetData.m_svOffset.m_dPositionX + p_svIn.m_dPositionX;
			p_rsvOut.m_dPositionY = m_svRootOffsetData.m_svOffset.m_dPositionY + p_svIn.m_dPositionY;

			p_rsvOut.m_dPositionX -= m_svRootOffsetData.m_svRotationPoint.m_dPositionX;
			p_rsvOut.m_dPositionY -= m_svRootOffsetData.m_svRotationPoint.m_dPositionY;
		}
		else 
		{
			l_svOk = l_svExtent.TranslateFromOutputSpace( p_svIn, p_svIn );

			if( S_OK == l_svOk )
			{
				try
				{
					if( nullptr != m_psvToolImage )
					{
						SVImageClass* l_psvImageParent = m_psvToolImage->GetParentImage();

						if( nullptr != l_psvImageParent )
						{
							SVToolClass* l_psvToolParent = dynamic_cast<SVToolClass*>(l_psvImageParent->GetTool());

							if( nullptr != l_psvToolParent )
							{
								l_svOk = l_psvToolParent->TranslatePointToSource( p_svIn, p_svIn );
							}
						}
					}

					if( S_OK == l_svOk )
					{
						p_rsvOut = p_svIn;
					}
				}
				catch( ... )
				{
					l_svOk = S_FALSE;
				}
			}
		}
	}
	return l_svOk;
}

HRESULT SVToolExtentClass::TranslatePositionPointToSource( SVExtentPointStruct& p_rsvOut )
{
	HRESULT l_svOk = S_OK;

	if( m_svRootOffsetData.m_bIsLinear )
	{
		p_rsvOut = m_svRootOffsetData.m_svOffset;
	}
	else
	{
		try
		{
			SVImageExtentClass l_svExtent;
			SVExtentPointStruct l_svPoint;

			l_svOk = GetImageExtent( l_svExtent );

			if( S_OK == l_svOk )
			{
				l_svExtent.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svPoint );

				l_svOk = TranslatePointToSource( l_svPoint, l_svPoint );
			}

			if( S_OK == l_svOk )
			{
				p_rsvOut = l_svPoint;
			}
		}
		catch( ... )
		{
			l_svOk = S_FALSE;
		}
	}
	return l_svOk;
}

SVString SVToolExtentClass::GetAuxiliaryDrawTypeString() const
{
	SVString Result;

	// SVExtentTranslationEnum
	switch( m_eTranslation )
	{
		case SVExtentTranslationUnknown:
		{
			Result = _T("Translation Unknown");
			break;
		}
		case SVExtentTranslationNone:
		{
			Result = _T("Translation None");
			break;
		}
		case SVExtentTranslationShift:
		{
			Result = _T("Translation Shift");
			break;
		}
		case SVExtentTranslationRotate:
		{
			Result = _T("Translation Rotate");
			break;
		}
		case SVExtentTranslationFlippedRotate:
		{
			Result = _T("Translation Flipped Rotate");
			break;
		}
		case SVExtentTranslationProfile:
		{
			Result = _T("Translation Profile");
			break;
		}
		case SVExtentTranslationProfileShift:
		{
			Result = _T("Translation Profile Shift");
			break;
		}
		case SVExtentTranslationTransformShift:
		{
			Result = _T("Translation Transform Shift");
			break;
		}
		case SVExtentTranslationTransformRotate:
		{
			Result = _T("Translation Transform Rotate");
			break;
		}
		case SVExtentTranslationPolarUnwrap:
		{
			Result = _T("Translation Polar Unwrap");
			break;
		}
		case SVExtentTranslationLine:
		{
			Result = _T("Translation Line");
			break;
		}
		case SVExtentTranslationDoubleHeight:
		{
			Result = _T("Translation Double Height");
			break;
		}
		case SVExtentTranslationResize:
		{
			Result = _T("Translation Resize");
			break;
		}
		case SVExtentTranslationFlipVertical:
		{
			Result = _T("Translation Flip Vertical");
			break;
		}
		case SVExtentTranslationFlipHorizontal:
		{
			Result = _T("Translation Flip Horizontal");
			break;
		}
		case SVExtentTranslationFigureShift:
		{
			Result = _T("Translation Figure Shift");
			break;
		}
		case SVExtentTranslationLinear:
		{
			Result = _T("Translation Linear");
			break;
		}
		case SVExtentTranslationCylindricalWarpH:
		{
			Result = _T("Translation Cylindrical Warp Horizontal");
			break;
		}
		case SVExtentTranslationCylindricalWarpV:
		{
			Result = _T("Translation Cylindrical Warp Vertical");
			break;
		}
		case SVExtentTranslationHorizontalPerspective:
		{
			Result = _T("Translation Horizontal Perspective");
			break;
		}
		case SVExtentTranslationVerticalPerspective:
		{
			Result = _T("Translation Vertical Perspective");
			break;
		}
		default:
		{
			Result = _T("Translation Unknown");
			break;
		}
	}
	return Result;
}

HRESULT SVToolExtentClass::GetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const
{
	HRESULT hr = m_svProperties.GetPropertyInfo( p_eProperty, p_rInfo );
	return hr;
}

HRESULT SVToolExtentClass::SetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo )
{
	HRESULT hr = m_svProperties.SetPropertyInfo( p_eProperty, p_rInfo );
	return hr;
}

