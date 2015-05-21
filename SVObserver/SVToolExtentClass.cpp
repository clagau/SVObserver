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

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign.hpp>
#include "SVToolExtentClass.h"
#include "SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVValueObject.h"

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

	if( m_psvToolImage != NULL && m_psvToolImage->GetParentImage() != NULL )
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

	if( m_psvToolImage != NULL )
	{

		SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

		SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

		if( l_pParent != NULL )
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

		if( l_Status == S_OK )
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

	if( l_pParent != NULL )
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

	if( m_psvToolImage != NULL )
	{
		SVImageExtentClass l_Extent = p_rImageExtent;

		SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

		SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

		if( l_Status == S_OK )
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

	if( m_psvToolImage != NULL )
	{
		bool l_Update = true;

		if( l_Update )
		{
			SVImageExtentClass l_Extent;

			SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

			SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

			if( l_pParent != NULL )
			{
				if( ( l_Type == SVImageTypeLogical ) && ( GetTranslation() == SVExtentTranslationProfile ) )
				{
					l_Type = SVImageTypePhysical;

					m_psvToolImage->UpdateImage( l_Type );
				}

				if( l_Type == SVImageTypeDependent || l_Type == SVImageTypeVirtual )
				{
					l_Extent = l_pParent->GetImageExtents();

					l_Extent.SetTranslation( GetTranslation() );

					SVExtentPositionClass l_Position = l_Extent.GetPosition();

					l_Status = l_Position.SetExtentProperty( SVExtentPropertyPositionPoint, 0.0 );

					if( l_Status == S_OK )
					{
						l_Status = l_Extent.SetPosition( l_Position );
					}

					l_Status = SetImageExtent( p_ulIndex, l_Extent );
				}
				else
				{
					l_Status = GetImageExtent( l_Extent );

					SVInspectionProcess* pInspection = m_psvToolImage->GetInspection();

					if ( nullptr != pInspection && pInspection->IsResetStateSet( SVResetAutoMoveAndResize ) )
					{
						if( l_Extent != m_psvToolImage->GetImageExtents() )
						{
							if( m_psvToolImage->ValidateAgainstParentExtents( l_Extent ) != S_OK )
							{
								l_pParent->GetImageExtentsToFit( l_Extent, l_Extent );

								SetImageExtent( p_ulIndex, l_Extent );
							}
						}
					}

					if( p_ToolExtentType == SVColorToolExtent )
					{
						SVExtentPositionClass l_Position = l_Extent.GetPosition();

						l_Status = l_Position.SetExtentProperty( SVExtentPropertyPositionPoint, 0.0 );

						if( l_Status == S_OK )
						{
							l_Status = l_Extent.SetPosition( l_Position );
						}
					}

					if( p_ToolExtentType == SVTransformationToolExtent )
					{
						double l_Value = 0.0;
						SVImageExtentClass l_InputExtent = l_pParent->GetImageExtents();

						if( l_InputExtent.GetExtentProperty( SVExtentPropertyOutputWidth, l_Value ) == S_OK )
						{
							l_Extent.SetExtentProperty( SVExtentPropertyWidth, l_Value );
						}

						if( l_InputExtent.GetExtentProperty( SVExtentPropertyOutputHeight, l_Value ) == S_OK )
						{
							l_Extent.SetExtentProperty( SVExtentPropertyHeight, l_Value );
						}

						l_Status = SetImageExtent( p_ulIndex, l_Extent );

						HRESULT l_Temp = GetImageExtent( l_Extent );

						if( l_Status == S_OK )
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

			if( l_Status == S_OK )
			{
				if( l_Extent != m_psvToolImage->GetImageExtents() )
				{
					if( p_ToolExtentType == SVRebuidOnResizeToolExtent )
					{
						l_Status = m_psvToolImage->UpdateImage( l_Extent, true );
					}
					else
					{
						l_Status = m_psvToolImage->UpdateImage( l_Extent );
					}
				}
			}

			if( l_Status == S_OK )
			{
				if( m_psvTool != NULL && m_psvTool->GetInspection() != NULL )
				{
					m_psvTool->GetInspection()->m_bForceOffsetUpdate = true;
				}

				if( l_Status == S_OK )
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

	if ( l_hrOk == S_OK )
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

		if( ( GetExtentValue( SVExtentPropertyPositionPointY, l_PointY.GetVARIANT() ) == S_OK ) &&
		    ( GetExtentValue( SVExtentPropertyHeight, l_Height.GetVARIANT() ) == S_OK ) )
		{
			if( m_eTranslation == SVExtentTranslationProfileShift )
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
	SVValueObjectClass* l_pValueObject = NULL;

	l_Value.Attach( p_rValue );
	l_Value.Clear();

	l_hrOk = GetExtentObject( p_eProperty, l_pValueObject );

	if( l_pValueObject != NULL )
	{
		l_hrOk = l_pValueObject->GetValue( l_Value.GetVARIANT() );
	}
	else if( l_hrOk == S_OK )
	{
		l_hrOk = E_FAIL;
	}

	p_rValue = l_Value.Detach();

	return l_hrOk;
}

HRESULT SVToolExtentClass::SetExtentValue( SVExtentPropertyEnum p_eProperty, long p_DataIndex, const _variant_t& p_rValue )
{
	HRESULT l_hrOk = S_OK;

	SVValueObjectClass* l_pValueObject = NULL;

	l_hrOk = GetExtentObject( p_eProperty, l_pValueObject );

	if( l_pValueObject != NULL )
	{
		double l_Value = p_rValue;

		l_hrOk = l_pValueObject->SetValue( p_DataIndex, l_Value );
	}
	else if( l_hrOk == S_OK )
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT SVToolExtentClass::GetImageExtent( SVImageExtentClass &p_rsvImageExtent ) const
{
	HRESULT l_hrOk = p_rsvImageExtent.Initialize();

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvImageExtent.SetTranslation( m_eTranslation );
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = m_svProperties.GetProperties( p_rsvImageExtent );
	}

	if ( l_hrOk == S_OK )
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

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvImageExtent.SetTranslation( m_eTranslation );
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = m_svProperties.GetProperties( p_ulIndex, p_rsvImageExtent );
	}

	if ( l_hrOk == S_OK )
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

	if ( p_svImageExtent.GetTranslation() != SVExtentTranslationUnknown )
	{
		m_eTranslation = p_svImageExtent.GetTranslation();
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointX, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyPositionPointX, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointY, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyPositionPointY, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLineX, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyPositionPointEndOfLineX, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLineY, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyPositionPointEndOfLineY, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyRotationAngle, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyRotationAngle, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyTranslationOffsetX, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyTranslationOffsetY, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyHeight, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyHeight, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyWidth, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyWidth, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyStartAngle, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyStartAngle, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyEndAngle, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyEndAngle, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyInnerRadius, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyInnerRadius, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
	}

	if( p_svImageExtent.GetExtentProperty( SVExtentPropertyOuterRadius, l_dValue ) == S_OK )
	{
		SVValueObjectClass *l_psvValueObject = NULL;

		if ( GetExtentObject( SVExtentPropertyOuterRadius, l_psvValueObject ) == S_OK && l_psvValueObject != NULL )
		{
			HRESULT hr = l_psvValueObject->SetValue( p_ulIndex, l_dValue );
			if ( l_hrOk == S_OK )
				l_hrOk = hr;
		}
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
			if ( hr == S_OK )
				return info.bFormula || info.bHidden;
			else
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
		p_rPropertyList.erase( iter, p_rPropertyList.end() );

	if ( l_svExtents.GetTranslation() == SVExtentTranslationPolarUnwrap )
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
				p_rPropertyList.erase( iter );
		}
	}
	// objects - on ResetObject - set toolextent flag


	return hr;
}


HRESULT SVToolExtentClass::GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData )
{
	HRESULT l_svOk = S_FALSE;

	if( m_svRootOffsetData.m_psvImage == NULL )
	{
		l_svOk = UpdateOffsetDataToImage( m_svRootOffsetData, NULL );
	}

	p_rsvOffsetData = m_svRootOffsetData;

	if( p_rsvOffsetData.m_psvImage != NULL )
	{
		l_svOk = S_OK;
	}

	return l_svOk;
}

HRESULT SVToolExtentClass::GetSelectedOffsetData( SVExtentOffsetStruct& p_rsvOffsetData )
{
	HRESULT l_svOk = S_FALSE;

	if( m_psvSelectedImage == NULL )
	{
		p_rsvOffsetData = m_svRootOffsetData;
	}
	else
	{
		p_rsvOffsetData = m_svSelectedOffsetData;
	}

	if( p_rsvOffsetData.m_psvImage != NULL )
	{
		l_svOk = S_OK;
	}

	return l_svOk;
}

HRESULT SVToolExtentClass::UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage )
{
	HRESULT l_svOk = S_OK;

	if( m_psvToolImage != NULL )
	{
		SVExtentOffsetStruct l_svOffsetData;

		SVImageClass* l_psvImageParent = m_psvToolImage->GetParentImage();

		if( l_psvImageParent != NULL )
		{
			SVToolClass* l_psvToolParent = l_psvImageParent->GetTool();

			if( l_psvImageParent != p_svToolImage && l_psvImageParent != m_psvToolImage && 
				l_psvToolParent != NULL && l_psvToolParent != m_psvTool )
			{
				l_svOk = l_psvToolParent->UpdateOffsetDataToImage( l_svOffsetData, p_svToolImage );
			}
			else if( l_psvImageParent == p_svToolImage || p_svToolImage == NULL )
			{
				SVImageExtentClass l_svExtent;

				l_svOk = GetImageExtent( l_svExtent );

				if( l_svOk == S_OK )
				{
					l_svOk = l_svExtent.UpdateSourceOffset( l_svOffsetData );
				}
			}
			else
			{
				l_svOk = E_FAIL;
			}

			if( l_svOk == S_OK )
			{
				if( p_rsvOffsetData.m_psvImage == NULL )
				{
					if( p_rsvOffsetData.m_psvRootImage != NULL )
					{
						l_svOffsetData.m_psvRootImage = p_rsvOffsetData.m_psvRootImage;
					}
					else
					{
						if( m_svRootOffsetData.m_psvRootImage != NULL )
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

					if( l_psvImageParent != NULL )
					{
						l_svOffsetData.m_csImageName = l_psvImageParent->GetCompleteObjectName();
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
			m_svRootOffsetData.m_psvImage == NULL )
		{
			if( m_psvToolImage != NULL )
			{
				SVExtentOffsetStruct l_svOffsetData;

				SVImageClass* l_psvImageParent = m_psvToolImage->GetParentImage();

				if( l_psvImageParent != NULL )
				{
					SVToolClass* l_psvToolParent = l_psvImageParent->GetTool();

					if( l_psvToolParent != NULL && l_psvToolParent != m_psvTool )
					{
						l_svOk = l_psvToolParent->GetRootOffsetData( l_svOffsetData );
					}
					else
					{
						l_svOffsetData.m_psvRootImage = l_psvImageParent;
						l_svOffsetData.m_psvImage = l_psvImageParent;
						l_svOffsetData.m_csImageName = l_psvImageParent->GetCompleteObjectName();
					}
				}
				else
				{
					l_svOffsetData.m_psvRootImage = m_psvToolImage;
					l_svOffsetData.m_psvImage = m_psvToolImage;
					l_svOffsetData.m_csImageName = m_psvToolImage->GetCompleteObjectName();
				}

				if( l_svOk == S_OK )
				{
					SVImageExtentClass l_svExtent;

					l_svOk = GetImageExtent( l_svExtent );

					if( l_svOk == S_OK )
					{
						l_svOk = l_svExtent.UpdateSourceOffset( l_svOffsetData );
					}
				}

				if( l_svOk == S_OK )
				{
					l_svOffsetData.m_bAlwaysUpdate |= m_bAlwaysUpdate;

					m_svRootOffsetData = l_svOffsetData;
				}
			}
		}

		if( l_svOk == S_OK )
		{
			if( p_bForceUpdate ||
				m_bAlwaysUpdate ||
				m_svSelectedOffsetData.m_bAlwaysUpdate ||
				m_svSelectedOffsetData.m_psvImage == NULL ||
				m_svSelectedOffsetData.m_psvImage != m_psvSelectedImage )
			{
				if( m_psvSelectedImage == NULL )
				{
					m_svSelectedOffsetData.Initialize();
				}
				else
				{
					SVExtentOffsetStruct l_svOffsetData;

					l_svOk = UpdateOffsetDataToImage( l_svOffsetData, m_psvSelectedImage );

					if( l_svOk == S_OK )
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

	if( l_svOk == S_OK )
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

			if( l_svOk == S_OK )
			{
				try
				{
					if( m_psvToolImage != NULL )
					{
						SVImageClass* l_psvImageParent = m_psvToolImage->GetParentImage();

						if( l_psvImageParent != NULL )
						{
							SVToolClass* l_psvToolParent = l_psvImageParent->GetTool();

							if( l_psvToolParent != NULL )
							{
								l_svOk = l_psvToolParent->TranslatePointToSource( p_svIn, p_svIn );
							}
						}
					}

					if( l_svOk == S_OK )
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

			if( l_svOk == S_OK )
			{
				l_svExtent.GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_svPoint );

				l_svOk = TranslatePointToSource( l_svPoint, l_svPoint );
			}

			if( l_svOk == S_OK )
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

HRESULT SVToolExtentClass::GetAuxilliaryDrawTypeString( CString& p_strDrawType )
{
	HRESULT l_hrOk = S_OK;

	// SVExtentTranslationEnum
	switch( m_eTranslation )
	{
		case SVExtentTranslationUnknown :
		{
			p_strDrawType = _T("Translation Unknown");
			break;
		}
		case SVExtentTranslationNone :
		{
			p_strDrawType = _T("Translation None");
			break;
		}
		case SVExtentTranslationShift :
		{
			p_strDrawType = _T("Translation Shift");
			break;
		}
		case SVExtentTranslationRotate :
		{
			p_strDrawType = _T("Translation Rotate");
			break;
		}
		case SVExtentTranslationFlippedRotate :
		{
			p_strDrawType = _T("Translation Flipped Rotate");
			break;
		}
		case SVExtentTranslationProfile :
		{
			p_strDrawType = _T("Translation Profile");
			break;
		}
		case SVExtentTranslationProfileShift :
		{
			p_strDrawType = _T("Translation Profile Shift");
			break;
		}
		case SVExtentTranslationTransformShift :
		{
			p_strDrawType = _T("Translation Transform Shift");
			break;
		}
		case SVExtentTranslationTransformRotate :
		{
			p_strDrawType = _T("Translation Transform Rotate");
			break;
		}
		case SVExtentTranslationPolarUnwrap :
		{
			p_strDrawType = _T("Translation Polar Unwrap");
			break;
		}
		case SVExtentTranslationBuildReference :
		{
			p_strDrawType = _T("Translation Build Reference");
			break;
		}
		case SVExtentTranslationLine :
		{
			p_strDrawType = _T("Translation Line");
			break;
		}
		case SVExtentTranslationDoubleHeight :
		{
			p_strDrawType = _T("Translation Double Height");
			break;
		}
		case SVExtentTranslationFlipVertical :
		{
			p_strDrawType = _T("Translation Flip Vertical");
			break;
		}
		case SVExtentTranslationFlipHorizontal :
		{
			p_strDrawType = _T("Translation Flip Horizontal");
			break;
		}
		case SVExtentTranslationFigureShift	:
		{
			p_strDrawType = _T("Translation Figure Shift");
			break;
		}
		case SVExtentTranslationLinear :
		{
			p_strDrawType = _T("Translation Linear");
			break;
		}
		case SVExtentTranslationCylindricalWarpH :
		{
			p_strDrawType = _T("Translation Cylindrical Warp Horizontal");
			break;
		}
		case SVExtentTranslationCylindricalWarpV :
		{
			p_strDrawType = _T("Translation Cylindrical Warp Vertical");
			break;
		}
		case SVExtentTranslationHorizontalPerspective :
		{
			p_strDrawType = _T("Translation Horizontal Perspective");
			break;
		}
		case SVExtentTranslationVerticalPerspective :
		{
			p_strDrawType = _T("Translation Vertical Perspective");
			break;
		}
		default:
		{
			p_strDrawType = _T("Translation Unknown");;
			break;
		}
	}
	return l_hrOk;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolExtentClass.cpp_v  $
 * 
 *    Rev 1.2   08 May 2013 16:44:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   26 Apr 2013 06:37:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified UpdateExtentToParentExtents to only copy the output width and output height from the parent extent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:21:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   19 Apr 2013 09:14:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added UpdateExtentToParentExtents.
 * Added GetParentExtent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   18 Apr 2013 16:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  821
 * SCR Title:  Fix issue with Linear Tool when Enabling Rotation
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Linear tool Extent Translation funtioanlity to Tool Extent Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:42:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   11 Jan 2013 15:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new extent functionality that does not rebuild the image buffers on a positional change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   15 Aug 2012 17:07:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with automatic move and resize logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   09 Aug 2012 15:58:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue when changing camera image size and the automatic moving/resizing tools.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   02 May 2011 16:12:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with virtual and dependent images to change position point to (0,0) when updating extents from parent image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 May 2011 15:59:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with virtual and dependent images to change position point to (0,0) when updating extents from parent image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   05 Apr 2011 12:31:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix automatic moving/resizing issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   30 Mar 2011 16:28:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Mar 2011 12:19:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   18 Mar 2011 07:37:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Mar 2011 13:59:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   14 Mar 2011 08:07:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to perform a partial validation of extents to prevent crashing or causing load failure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   11 Mar 2011 14:33:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Mar 2011 10:39:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix issues with not assigning input image to extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   28 Oct 2010 14:16:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Apr 2008 09:09:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Jan 2006 17:36:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Sep 2005 10:14:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified TranslatePointToSource
 * Modified UpdateSourceOffset
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Sep 2005 11:19:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   moved logic for setting bIsSourceOffsetValid flag from local UpdateSourceOffset to SVImageExtentClass::UpdateSourceOffset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Sep 2005 12:07:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix bug with un-initialized HResult that caused Auxilliary Extents to be wrong.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Sep 2005 15:47:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated to prevent infinite recursion in the UpdateSourceOffset method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Sep 2005 12:04:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New functions to support Auxilliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jun 2005 08:33:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2005 14:27:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new element for hiding rotation angle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:11:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
