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
#include "SVImageClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/SVResetStruct.h"
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

HRESULT SVToolExtentClass::ValidExtentAgainstParentImage( const SVImageExtentClass& rImageExtent ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage && nullptr != m_psvToolImage->GetParentImage() )
	{
		l_Status = m_psvToolImage->GetParentImage()->ValidateAgainstOutputExtents( rImageExtent );
	}
	else
	{
		l_Status = S_FALSE;
	}
	return l_Status;
}

HRESULT SVToolExtentClass::UpdateExtentToParentExtents( SVImageExtentClass& rNewExtent )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage )
	{
		SVImageClass* pParent = m_psvToolImage->GetParentImage();

		if( nullptr != pParent )
		{
			double dPosX( 0.0 ), dPosY( 0.0 ), dWidth( 0.0 ), dHeight( 0.0 );
			SVImageExtentClass TempExt = pParent->GetImageExtents();
			// Get the Width and Height form the parent extent. Set the Position point to zero, zero.
			l_Status = TempExt.GetExtentProperty( SVExtentPropertyOutputHeight, dHeight );
			KeepPrevError( l_Status, TempExt.GetExtentProperty( SVExtentPropertyOutputWidth, dWidth ));
			KeepPrevError( l_Status, rNewExtent.SetExtentProperty( SVExtentPropertyPositionPointX, dPosX ));
			KeepPrevError( l_Status, rNewExtent.SetExtentProperty( SVExtentPropertyPositionPointY, dPosY ));
			KeepPrevError( l_Status, rNewExtent.SetExtentProperty( SVExtentPropertyHeight, dHeight ));
			KeepPrevError( l_Status, rNewExtent.SetExtentProperty( SVExtentPropertyWidth, dWidth ));
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( S_OK == l_Status  )
		{
			if( rNewExtent != m_psvToolImage->GetImageExtents() )
			{
				l_Status = m_psvToolImage->UpdateImage( rNewExtent );
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

HRESULT SVToolExtentClass::UpdateExtentAgainstParentImage( const SVImageExtentClass& p_rImageExtent )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage )
	{
		SVImageExtentClass l_Extent = p_rImageExtent;

		SvOi::SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

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

HRESULT SVToolExtentClass::UpdateImageWithExtent( SVToolExtentTypeEnum p_ToolExtentType )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_psvToolImage )
	{
		bool l_Update = true;

		if( l_Update )
		{
			SVImageExtentClass l_Extent;

			SvOi::SVImageTypeEnum l_Type = m_psvToolImage->GetImageType();

			SVImageClass* l_pParent = m_psvToolImage->GetParentImage();

			if( nullptr != l_pParent )
			{
				if( ( SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical == l_Type ) && ( SVExtentTranslationProfile == GetTranslation() ) )
				{
					l_Type = SvOi::SVImageTypeEnum::SVImageTypePhysical;

					m_psvToolImage->UpdateImage( l_Type );
				}

				if( SvOi::SVImageTypeEnum::SVImageTypeDependent == l_Type || SvOi::SVImageTypeEnum::SVImageTypeVirtual == l_Type )
				{
					l_Extent = l_pParent->GetImageExtents();

					l_Extent.SetTranslation( GetTranslation() );

					SVExtentPositionClass l_Position = l_Extent.GetPosition();

					l_Status = l_Position.SetExtentProperty( SVExtentPropertyPositionPoint, 0.0 );

					if( S_OK == l_Status  )
					{
						l_Status = l_Extent.SetPosition( l_Position );
					}

					l_Status = SetImageExtent( l_Extent );
				}
				else
				{
					l_Status = GetImageExtent( l_Extent );

					SvOi::IInspectionProcess* pInspection = m_psvToolImage->GetInspectionInterface();

					if ( nullptr != pInspection && pInspection->IsResetStateSet( SvOi::SVResetAutoMoveAndResize ) )
					{
						if( l_Extent != m_psvToolImage->GetImageExtents() )
						{
							if( S_OK != m_psvToolImage->ValidateAgainstParentExtents( l_Extent ) )
							{
								l_pParent->GetImageExtentsToFit( l_Extent, l_Extent );

								SetImageExtent( l_Extent );
							}
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

						l_Status = SetImageExtent( l_Extent );

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
					SvOi::IInspectionProcess* pInspection = m_psvTool->GetInspectionInterface();
					if (pInspection)
					{
						pInspection->ForceOffsetUpdate();
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

HRESULT SVToolExtentClass::SetTranslation(SVExtentTranslationEnum eTranslation)
{
	HRESULT l_hrOk = S_FALSE;

	SVImageExtentClass l_svExtent;

	l_hrOk = l_svExtent.SetTranslation(eTranslation);

	if (S_OK == l_hrOk)
	{
		m_eTranslation = eTranslation;
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::SetLinearTranslation(SVExtentTranslationEnum eTranslation)
{
	HRESULT l_hrOk = S_FALSE;

	SVTranslationFilterSet::iterator l_OldIter = m_LinearToolTranslations.find(m_eTranslation);
	SVTranslationFilterSet::iterator l_NewIter = m_LinearToolTranslations.find(eTranslation);

	if ((m_eTranslation != eTranslation) && (l_OldIter != m_LinearToolTranslations.end()) && (l_NewIter != m_LinearToolTranslations.end()))
	{
		_variant_t PointY;
		_variant_t Height;

		if ((S_OK == GetExtentValue(SVExtentPropertyPositionPointY, PointY)) &&
			(S_OK == GetExtentValue(SVExtentPropertyHeight, Height)))
		{
			double NewY = static_cast<double> (PointY);
			double Offset = static_cast<double>(Height) / 2.0;

			NewY += (SVExtentTranslationProfileShift == m_eTranslation) ? Offset : -Offset;
			l_hrOk = SetExtentValue(SVExtentPropertyPositionPointY, NewY);
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		m_eTranslation = eTranslation;
	}
	else
	{
		l_hrOk = SetTranslation(eTranslation);
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::GetExtentShape( SVExtentPropertyEnum p_eProperty, SVExtentShapeEnum &p_reValue ) const
{
	HRESULT l_hrOk = S_OK;

	p_reValue = m_eShape;

	return l_hrOk;
}

HRESULT SVToolExtentClass::GetExtentObject( SVExtentPropertyEnum p_eProperty, SvOi::IValueObject*& rpValueObject ) const
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svProperties.GetExtentObject( p_eProperty, rpValueObject );

	return l_hrOk;
}

HRESULT SVToolExtentClass::SetExtentObject( SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svProperties.SetExtentObject( p_eProperty, pValueObject );

	return l_hrOk;
}

HRESULT SVToolExtentClass::GetExtentValue( SVExtentPropertyEnum p_eProperty, _variant_t& rValue ) const
{
	HRESULT l_hrOk = S_OK;

	SvOi::IValueObject* pValueObject( nullptr );

	rValue.Clear();

	l_hrOk = GetExtentObject( p_eProperty, pValueObject );

	if( nullptr != pValueObject )
	{
		l_hrOk = pValueObject->getValue( rValue );
	}
	else if( S_OK == l_hrOk )
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT SVToolExtentClass::SetExtentValue( SVExtentPropertyEnum p_eProperty, const _variant_t& rValue )
{
	HRESULT l_hrOk = S_OK;

	SvOi::IValueObject* pValueObject( nullptr );

	l_hrOk = GetExtentObject( p_eProperty, pValueObject );

	if( nullptr != pValueObject  )
	{
		l_hrOk = pValueObject->setValue( rValue );
	}
	else if( S_OK == l_hrOk )
	{
		l_hrOk = E_FAIL;
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::GetImageExtent( SVImageExtentClass &rImageExtent ) const
{
	HRESULT l_hrOk = rImageExtent.Initialize();

	if ( S_OK == l_hrOk )
	{
		l_hrOk = rImageExtent.SetTranslation( m_eTranslation );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = m_svProperties.GetProperties( rImageExtent );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = rImageExtent.UpdateData();
	}
	else
	{
		rImageExtent.Initialize();
	}
	return l_hrOk;
}

HRESULT SVToolExtentClass::SetImageExtent( const SVImageExtentClass& rImageExtent )
{
	HRESULT l_hrOk = S_OK;

	double dValue = 0.0;

	SVExtentTranslationEnum translation = rImageExtent.GetTranslation();

	if ( SVExtentTranslationUnknown != translation )
	{
		m_eTranslation = translation;
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyPositionPointX, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointX, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyPositionPointY, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointY, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLineX, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointEndOfLineX, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLineY, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyPositionPointEndOfLineY, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyRotationAngle, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyRotationAngle, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetX, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyTranslationOffsetX, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetY, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyTranslationOffsetY, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyHeight, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyHeight, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyWidth, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyWidth, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyStartAngle, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyStartAngle, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyEndAngle, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyEndAngle, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyInnerRadius, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyInnerRadius, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyOuterRadius, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyOuterRadius, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyHeightScaleFactor, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyHeightScaleFactor, dValue );
	}

	if( S_OK == rImageExtent.GetExtentProperty( SVExtentPropertyWidthScaleFactor, dValue ) )
	{
		l_hrOk &= SetExtentValue( SVExtentPropertyWidthScaleFactor, dValue );
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

HRESULT SVToolExtentClass::UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* pToolImage )
{
	HRESULT l_svOk = S_OK;

	if( nullptr != m_psvToolImage )
	{
		SVExtentOffsetStruct l_svOffsetData;

		SVImageClass* pImageParent = m_psvToolImage->GetParentImage();

		if( nullptr != pImageParent )
		{
			SVToolClass* pToolParent = dynamic_cast<SVToolClass*>(pImageParent->GetTool());

			if( pImageParent != pToolImage && pImageParent != m_psvToolImage && 
				nullptr != pToolParent && pToolParent != m_psvTool )
			{
				l_svOk = pToolParent->UpdateOffsetDataToImage( l_svOffsetData, pToolImage );
			}
			else if( pImageParent == pToolImage || nullptr == pToolImage )
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
							l_svOffsetData.m_psvRootImage = pImageParent;
						}
					}

					l_svOffsetData.m_psvImage = pImageParent;
					l_svOffsetData.m_bAlwaysUpdate |= m_bAlwaysUpdate;

					if( nullptr != pImageParent )
					{
						l_svOffsetData.m_csImageName = pImageParent->GetCompleteName();
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

				SVImageClass* pImageParent = m_psvToolImage->GetParentImage();

				if( nullptr != pImageParent )
				{
					SVToolClass* pToolParent = dynamic_cast<SVToolClass*>(pImageParent->GetTool());

					if( nullptr != pToolParent && pToolParent != m_psvTool )
					{
						l_svOk = pToolParent->GetRootOffsetData( l_svOffsetData );
					}
					else
					{
						l_svOffsetData.m_psvRootImage = pImageParent;
						l_svOffsetData.m_psvImage = pImageParent;
						l_svOffsetData.m_csImageName = pImageParent->GetCompleteName();
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
					SVImageExtentClass Extent;

					l_svOk = GetImageExtent( Extent );

					if( S_OK == l_svOk )
					{
						l_svOk = Extent.UpdateSourceOffset( l_svOffsetData );
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
						SVImageClass* pImageParent = m_psvToolImage->GetParentImage();

						if( nullptr != pImageParent )
						{
							SVToolClass* pToolParent = dynamic_cast<SVToolClass*>(pImageParent->GetTool());

							if( nullptr != pToolParent )
							{
								l_svOk = pToolParent->TranslatePointToSource( p_svIn, p_svIn );
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

