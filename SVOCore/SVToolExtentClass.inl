//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentClass
//* .File Name       : $Workfile:   SVToolExtentClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:40:18  $
//******************************************************************************

#pragma region Includes
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#pragma endregion Includes

inline HRESULT SVToolExtentClass::Initialize()
{
	HRESULT l_svOk = S_OK;

	m_psvTool = nullptr;
	m_psvToolImage = nullptr;

	m_eImageType = SVImageTypeUnknown;

	m_eTranslation = SVExtentTranslationUnknown;
	m_eShape = SVExtentShapeUnknown;

	KeepPrevError( l_svOk, m_svProperties.Initialize() );

	m_bAlwaysUpdate = true;

	m_psvSelectedImage = nullptr;

	KeepPrevError( l_svOk, m_svRootOffsetData.Initialize() );
	KeepPrevError( l_svOk, m_svSelectedOffsetData.Initialize() );

	return l_svOk;
}

inline SVToolClass* SVToolExtentClass::GetTool() const
{
	return m_psvTool;
}

inline void SVToolExtentClass::SetTool( SVToolClass *p_psvTool )
{ 
	m_psvTool = p_psvTool;
}

inline SVImageClass* SVToolExtentClass::GetToolImage() const
{
	return m_psvToolImage;
}

inline void SVToolExtentClass::SetToolImage( SVImageClass *p_psvToolImage )
{ 
	if( m_psvToolImage != p_psvToolImage )
	{
		m_psvToolImage = p_psvToolImage;

		m_svRootOffsetData.Initialize();
		m_svSelectedOffsetData.Initialize();
	}
}

inline SVImageTypeEnum SVToolExtentClass::GetImageType() const
{
	return m_eImageType;
}

inline void SVToolExtentClass::SetImageType( SVImageTypeEnum p_eImageType )
{
	m_eImageType = p_eImageType;
}

inline SVImageClass* SVToolExtentClass::GetSelectedImage() const
{
	return m_psvSelectedImage;
}

inline void SVToolExtentClass::SetSelectedImage( SVImageClass* p_psvSelectedImage )
{
	if( m_psvSelectedImage != p_psvSelectedImage )
	{
		m_psvSelectedImage = p_psvSelectedImage;

		m_svSelectedOffsetData.Initialize();
	}
}

inline bool SVToolExtentClass::GetAlwaysUpdate() const
{
	return m_bAlwaysUpdate;
}

inline void SVToolExtentClass::SetAlwaysUpdate( bool p_bAlwaysUpdate )
{
	m_bAlwaysUpdate = p_bAlwaysUpdate;
}

