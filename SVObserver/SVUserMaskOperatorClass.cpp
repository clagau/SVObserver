//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUserMaskOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 15:04:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVUserMaskOperatorClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/MatroxImageData.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVOCore/SVTool.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVGlobal.h" // For ConvertToHex
#include "SVInspectionProcess.h"
#include "SVObserver.h"
#include "SVToolSet.h"
#include "SVShapeMaskHelperClass.h"
#include "SVOCore/SVUnaryImageOperatorList.h"

SV_IMPLEMENT_CLASS( SVUserMaskOperatorClass, SVUserMaskOperatorClassGuid )

SVUserMaskOperatorClass::SVUserMaskOperatorClass( SVObjectClass* POwner, int StringResourceID )
						:SVUnaryImageOperatorClass( POwner, StringResourceID ) 
{
	init();
}

SVUserMaskOperatorClass::~SVUserMaskOperatorClass()
{
	CloseObject();
}

void SVUserMaskOperatorClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVUserMaskOperatorObjectType;

	m_inObjectInfo.SetInputObjectType( SVImageObjectType );
	m_inObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inObjectInfo, _T( "UserMaskImage" ) );


	// RRRRGGGGHHHHHH who created this stupid object structure????
	SVShapeMaskHelperClass* pShapeHelper = new SVShapeMaskHelperClass(this);
	m_guidShapeHelper = pShapeHelper->GetUniqueObjectID();

	bool bAddFriend = AddFriend( pShapeHelper->GetUniqueObjectID() );
	ASSERT( bAddFriend );



	RegisterEmbeddedObject( &m_Data.bvoActivated, SVMaskEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemIP );
	RegisterEmbeddedObject( &m_Data.dwvoMaskType, SVMaskUseImageMaskGuid, IDS_MASK_TYPE, false, SvOi::SVResetItemIP );
	RegisterEmbeddedObject( &m_Data.evoCurrentMaskOperator, SVMaskOperatorGuid, IDS_OBJECTNAME_MASK_OPERATOR, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.evoFillArea, SVMaskFillAreaGuid, IDS_OBJECTNAME_MASK_FILL_AREA, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.lvoFillColor, SVMaskFillColorGuid, IDS_OBJECTNAME_MASK_FILL_COLOR, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.evoDrawCriteria, SVDrawCriteriaObjectGuid, IDS_OBJECTNAME_DRAW_CRITERIA, false, SvOi::SVResetItemOwner );

	SVEnumerateVector vec;
	vec.push_back( SVEnumeratePair(_T( "AND" ), SVImageAnd ) );
	vec.push_back( SVEnumeratePair(_T( "OR" ), SVImageOr ) );	
	vec.push_back( SVEnumeratePair(_T( "XOR" ), SVImageXOr ) );	
	vec.push_back( SVEnumeratePair(_T( "NOR" ), SVImageNor ) );	
	vec.push_back( SVEnumeratePair(_T( "XNOR" ), SVImageXNor ) );	
	vec.push_back( SVEnumeratePair(_T( "NAND" ), SVImageNand ) );	
	vec.push_back( SVEnumeratePair(_T( "ADD" ), SVImageAddSaturation ) );
	vec.push_back( SVEnumeratePair(_T( "SUB" ), SVImageSubSaturation ) );
	vec.push_back( SVEnumeratePair(_T( "MULT" ), SVImageMulSaturation ) );
	vec.push_back( SVEnumeratePair(_T( "DIV" ), SVImageDiv ) );
	vec.push_back( SVEnumeratePair(_T( "MIN" ), SVImageMin ) );	
	vec.push_back( SVEnumeratePair(_T( "MAX" ), SVImageMax ) );	
	vec.push_back( SVEnumeratePair(_T( "PASS" ), SVImagePass ) );
	m_Data.evoCurrentMaskOperator.SetEnumTypes(vec);
	m_Data.evoCurrentMaskOperator.SetDefaultValue(SVImageAnd, true);

	vec.clear();
	vec.push_back( SVEnumeratePair(_T("Mask Area"), 0 ) );
	vec.push_back( SVEnumeratePair(_T("Background"), 1 ) );
	m_Data.evoFillArea.SetEnumTypes(vec);
	m_Data.evoFillArea.SetDefaultValue(0l, true);	// mask area is default

	m_Data.lvoFillColor.SetDefaultValue(0, true);	// black default

	vec.clear();
	vec.push_back( SVEnumeratePair(_T("None"), SVNone ) );
	vec.push_back( SVEnumeratePair(_T("Non-Black"), SVNonBlackPixels ) );
	vec.push_back( SVEnumeratePair(_T("Non-White"), SVNonWhitePixels ) );
	m_Data.evoDrawCriteria.SetEnumTypes( vec );
	m_Data.evoDrawCriteria.SetDefaultValue( SVNone, true );

	m_Data.bvoActivated.SetDefaultValue( BOOL(false), true);
	m_Data.dwvoMaskType.SetDefaultValue(MASK_TYPE_STATIC, true);




	m_MaskBufferInfo.SetOwnerImage( SV_GUID_NULL );
	m_MaskBufferInfo.SetOwner( SV_GUID_NULL );


	// Set default inputs and outputs
	addDefaultInputObjects();
}

bool SVUserMaskOperatorClass::CloseObject()
{
	bool bOk = S_OK == DestroyLocalImageBuffer();

	if ( m_isCreated )
	{
		m_isCreated = false;

		bOk = SVUnaryImageOperatorClass::CloseObject();
	}

	return bOk;
}

bool SVUserMaskOperatorClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject(rCreateStructure);

	const UINT cAttributes = SvOi::SV_PRINTABLE | SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE;
	// Set / Reset Printable Flag
	m_Data.bvoActivated.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_Data.evoDrawCriteria.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.dwvoMaskType.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.evoCurrentMaskOperator.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.evoFillArea.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.lvoFillColor.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	m_isCreated = bOk;

	return m_isCreated;
}

bool SVUserMaskOperatorClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVUnaryImageOperatorClass::ResetObject(pErrorMessages);

	DWORD dwMaskType;
	m_Data.dwvoMaskType.GetValue( dwMaskType );
	BOOL bActive = false;
	m_Data.bvoActivated.GetValue(bActive);

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	ASSERT( pShapeHelper );
	if ( pShapeHelper )
	{
		Result = pShapeHelper->ResetObject(pErrorMessages) && Result;

		SvOi::SetAttributeType AddRemoveType = (dwMaskType == MASK_TYPE_SHAPE) ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		pShapeHelper->SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_VIEWABLE, AddRemoveType );
		pShapeHelper->SetObjectAttributesSet( SvOi::SV_PRINTABLE, AddRemoveType );
		pShapeHelper->SetObjectAttributesAllowed( SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
		pShapeHelper->SetObjectAttributesSet( SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
		AddRemoveType = (dwMaskType == MASK_TYPE_SHAPE) && bActive ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_Data.evoFillArea.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_VIEWABLE, AddRemoveType  );
		m_Data.evoFillArea.SetObjectAttributesSet( SvOi::SV_PRINTABLE, AddRemoveType );
		m_Data.lvoFillColor.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_VIEWABLE, AddRemoveType  );
		m_Data.lvoFillColor.SetObjectAttributesSet( SvOi::SV_PRINTABLE, AddRemoveType  );

		if ( !bActive )
		{
			//turn off items.
			m_Data.evoFillArea.SetObjectAttributesSet( SvOi::SV_PRINTABLE| SvOi::SV_VIEWABLE, SvOi::SetAttributeType::RemoveAttribute );
			m_Data.lvoFillColor.SetObjectAttributesSet( SvOi::SV_PRINTABLE| SvOi::SV_VIEWABLE, SvOi::SetAttributeType::RemoveAttribute );
		}
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoShapeHelper, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	// if image mask or overlays are turned on, set to true
	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_Data.evoDrawCriteria.GetValue( l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	m_bUseOverlays = ( l_eCriteria != SVNone || dwMaskType == MASK_TYPE_IMAGE );

	if( S_OK != CreateLocalImageBuffer() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

HRESULT SVUserMaskOperatorClass::GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct )
{
	long lValue=0;
	m_Data.evoFillArea.GetValue( lValue );
	rsvFillStruct.bFillMaskArea = (lValue == 0 ? true : false);
	m_Data.lvoFillColor.GetValue( lValue );
	rsvFillStruct.rgbFillColor = RGB(lValue, lValue, lValue);

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		pShapeHelper->GetFillProperties( rsvFillStruct );
	}
	return S_OK;
}

SVShapeMaskHelperClass* SVUserMaskOperatorClass::GetShapeHelper()
{
	SVShapeMaskHelperClass* pMaskHelper = nullptr;

	// Get Friend Object
	for( size_t i = 0; i < m_friendList.size(); i++ )
	{
		const SVObjectInfoStruct& friendObjectInfo = m_friendList[i];
		if( pMaskHelper = dynamic_cast<SVShapeMaskHelperClass*> (friendObjectInfo.m_pObject) )
		{
			m_guidShapeHelper = pMaskHelper->GetUniqueObjectID();
			break;
		}
	}

	//!! temp workaround for 4.60 Beta 4
	if ( nullptr == pMaskHelper )
	{
		SVShapeMaskHelperClass* pShapeHelper = new SVShapeMaskHelperClass(this);
		m_guidShapeHelper = pShapeHelper->GetUniqueObjectID();

		bool bAddFriend = AddFriend( pShapeHelper->GetUniqueObjectID() );
		ASSERT( bAddFriend );

		if( CreateChildObject(pShapeHelper) )
		{
			pMaskHelper = pShapeHelper;
		}
	}

	ASSERT( pMaskHelper );
	return pMaskHelper;
}

HRESULT SVUserMaskOperatorClass::GetCancelData(SVInputRequestStructMap& rMap)
{
	HRESULT hr = S_OK;

	rMap.Add( &m_Data.evoCurrentMaskOperator );
	rMap.Add( &m_Data.bvoActivated );
	rMap.Add( &m_Data.dwvoMaskType );
	rMap.Add( &m_Data.evoFillArea );
	rMap.Add( &m_Data.lvoFillColor );
	rMap.Add( &m_Data.evoDrawCriteria );

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		pShapeHelper->GetCancelData( rMap );
	}

	return hr;
}

bool SVUserMaskOperatorClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	DWORD dwMaskType = 0;
	m_Data.dwvoMaskType.GetValue( dwMaskType );

	if (MASK_TYPE_IMAGE == dwMaskType)
	{
		SVImageClass* pImage = getMaskInputImage();
		if (nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
		{
			Result = true;
		}
	}
	else
	{
		if (nullptr != m_pCurrentUIOPL)
		{
			SVImageClass* pImage = m_pCurrentUIOPL->getInputImage();
			if (nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
			{
				Result = true;
			}
		}
	}

	return Result;
}

HRESULT SVUserMaskOperatorClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructVector &p_MultiLineArray )
{
	HRESULT l_hr = S_FALSE;
	if ( m_bUseOverlays )
	{
		SVExtentMultiLineStruct	l_MultiLine;

		BuildMaskLines( l_MultiLine );
		l_MultiLine.m_Color = RGB( 0, 255, 255 );
		l_MultiLine.m_ObjectID = SVUserMaskOperatorClassGuid;
		p_MultiLineArray.Add( l_MultiLine );
		l_hr = S_OK;
	}
	return l_hr;
}

HRESULT SVUserMaskOperatorClass::BuildMaskLines( SVExtentMultiLineStruct& p_MultiLine  )
{
	HRESULT l_hr = S_OK;

	// Multi Lines instead of points.
	p_MultiLine.Initialize();

	// Get Value Objects.
	BOOL Activated;
	m_Data.bvoActivated.GetValue( Activated );

	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_Data.evoDrawCriteria.GetValue(l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	DWORD dwMaskType;
	m_Data.dwvoMaskType.GetValue( dwMaskType );

	SVImageClass* pInputImage = getMaskInputImage();
	SVImageExtentClass l_svExtents;
	if( l_eCriteria != SVNone && Activated && !m_MaskBufferHandlePtr.empty() &&
		( MASK_TYPE_IMAGE != dwMaskType || nullptr != pInputImage ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		m_MaskBufferHandlePtr->GetData( l_MilHandle );

		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		if (pTool)
		{
			pTool->GetImageExtent( l_svExtents );

			RECT l_rec;

			HRESULT l_hr = m_MaskBufferInfo.GetOutputRectangle( l_rec );
			// User Mask draw with lines 

			HRESULT l_Code;
		
			LPVOID pSrcHostBuffer = nullptr;
			l_Code = SVMatroxBufferInterface::GetHostAddress( &pSrcHostBuffer, l_MilHandle.GetBuffer() );
			long l_lSrcBytes;
			l_Code = SVMatroxBufferInterface::Get( l_MilHandle.GetBuffer(), SVPitchByte, l_lSrcBytes );
			unsigned char* pSrcLine = ( unsigned char* )pSrcHostBuffer;

			long l_lSkip = 1;
		
			SVExtentPointStruct l_svPoint;
			SVExtentLineStruct l_Line;
			l_Line.m_svPointArray.Add( l_svPoint );
			l_Line.m_svPointArray.Add( l_svPoint );

			l_Line.m_dwColor = RGB( 0, 255, 255 );

			for( int l_iRow = 0 ; l_iRow < l_rec.bottom ; l_iRow += l_lSkip )
			{
				SVExtentPointStruct l_svStartPoint;

				bool l_bDrewLastPixel = false;

				for( int l_iCol = 0 ; l_iCol < l_rec.right ; l_iCol++ )
				{
					switch( l_eCriteria )
					{
						case  SVNonBlackPixels :
						{
							if( pSrcLine[l_iCol] != 0 ) 
							{
								if( ! l_bDrewLastPixel )
								{
									l_bDrewLastPixel = true;
									l_svStartPoint.m_dPositionX = l_iCol;
									l_svStartPoint.m_dPositionY = l_iRow;
									l_svExtents.TranslateFromOutputSpace( l_svStartPoint, l_svStartPoint );
								}
							}
							else if( l_bDrewLastPixel )
							{
								l_bDrewLastPixel = false;
								AddLine( l_iCol, l_iRow, l_svStartPoint, l_svExtents, l_Line, p_MultiLine  );
							}
							break;
						}
						case SVNonWhitePixels :
						{
							if( pSrcLine[l_iCol] != 255 )
							{							
								if( ! l_bDrewLastPixel )
								{
									l_bDrewLastPixel = true;
									l_svStartPoint.m_dPositionX = l_iCol;
									l_svStartPoint.m_dPositionY = l_iRow;
									l_svExtents.TranslateFromOutputSpace( l_svStartPoint, l_svStartPoint );
								}
							}
							else if( l_bDrewLastPixel ) 
							{
								l_bDrewLastPixel = false;
								AddLine( l_iCol, l_iRow, l_svStartPoint, l_svExtents, l_Line, p_MultiLine  );
							}
							break;
						}
					}
				}//for( int l_iCol = 0 ; l_iCol < l_rec.right ; l_iCol++ )

				if( l_bDrewLastPixel )  // Finish the last line if the pixels were the same to the end.
				{
					AddLine( l_rec.right-1, l_iRow, l_svStartPoint, l_svExtents, l_Line, p_MultiLine );
				}

				pSrcLine += l_lSrcBytes*l_lSkip;
			}//for( int l_iRow = 0 ; l_iRow < l_rec.bottom ; l_iRow += l_lSkip )

			p_MultiLine.m_Color = RGB( 0, 255, 255 );
		}
		else
		{
			l_hr = E_POINTER;
		}
	}

	return l_hr;
}

HRESULT SVUserMaskOperatorClass::AddLine(int p_iCol, int p_iRow, SVExtentPointStruct& p_svStartPoint, SVImageExtentClass& p_svExtent, SVExtentLineStruct& p_Line, SVExtentMultiLineStruct& p_MultiLine  )
{
	HRESULT l_hr = S_OK;

	SVExtentPointStruct l_svEndPoint;
	l_svEndPoint.m_dPositionX = p_iCol;
	l_svEndPoint.m_dPositionY = p_iRow;
	p_svExtent.TranslateFromOutputSpace( l_svEndPoint, l_svEndPoint);
	p_Line.m_svPointArray[0] = p_svStartPoint ;
	p_Line.m_svPointArray[1] = l_svEndPoint ;
	p_MultiLine.m_svLineArray.Add( p_Line );
	return l_hr;
}
	
HRESULT SVUserMaskOperatorClass::CreateLocalImageBuffer()
{
	HRESULT l_hrOk = S_OK;

	SVImageClass *l_psvImage = getReferenceImage();

	if( nullptr != l_psvImage )
	{
		SVImageInfoClass l_MaskBufferInfo = l_psvImage->GetImageInfo();

		bool bImageInfoChanged = (l_MaskBufferInfo != m_MaskBufferInfo);
		if ( bImageInfoChanged )
		{
			m_MaskBufferInfo = l_MaskBufferInfo;
			l_hrOk = DestroyLocalImageBuffer();
			l_hrOk = SVImageProcessingClass::CreateImageBuffer( m_MaskBufferInfo, m_MaskBufferHandlePtr );
		}

		if( S_OK == l_hrOk )
		{
			if( ! Refresh() )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVUserMaskOperatorClass::DestroyLocalImageBuffer()
{
	HRESULT l_hrOk = S_OK;

	m_MaskBufferHandlePtr.clear();

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Refresh
// -----------------------------------------------------------------------------
// .Description : Refresh mask buffer...
//              :
////////////////////////////////////////////////////////////////////////////////
bool SVUserMaskOperatorClass::Refresh()
{
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

	if( nullptr != pInspection )
	{
		DWORD dwMaskType;
		m_Data.dwvoMaskType.GetValue( dwMaskType );
		SVShapeMaskHelperClass* pShape = GetShapeHelper();
		if ( nullptr != pShape && ( MASK_TYPE_SHAPE == dwMaskType ) )
		{
			pShape->Refresh();
			return true;
		}
		else
		{
			// Init buffer to 0...
			// &&&
			if ( S_OK == SVImageProcessingClass::InitBuffer( m_MaskBufferHandlePtr ) )
			{
				HDC dc;
				// Get DC...
				dc = SVImageProcessingClass::CreateBufferDC( m_MaskBufferInfo, m_MaskBufferHandlePtr );
				if(dc != (HDC)0)        // && dc != (HDC)-1)
				{
					// Draw mask...
					RECT rect;

					SVImageExtentClass ex = m_MaskBufferInfo.GetExtents();

					ex.GetOutputRectangle( rect );

					m_graphixObject.Draw( dc, rect );

	#ifdef _DEBUG999
				{
					// Dump the images to .bmp files
					try
					{
						HRESULT l_Code = 
							SVMatroxBufferInterface::Export( m_MaskBufferHandlePtr.milImage,
						( char* )_T("C:\\Temp\\MaskBuffer.bmp"),
						SVFileBitmap );

					}
					catch(...)
					{
					}

				}
	#endif //_DEBUG

					// Release DC...
					return S_OK == SVImageProcessingClass::DestroyBufferDC( m_MaskBufferHandlePtr, dc );
				}// end if(dc != (HDC)0) 
			}// end if ( S_OK == TheSVObserverApp.mpsvImaging->InitBuffer( m_MaskBufferHandle ) )
		}// end else if not shape
	}// end if( GetIPDoc() )
	return false;
}

void SVUserMaskOperatorClass::Persist( SVObjectWriter& rWriter )
{
	SVTaskObjectClass::Persist(rWriter);
	
	// Add Script of SVGraphixClass as a hex string
	// Prepare Archive...
	CMemFile maskStorage;
	CArchive maskArchive( &maskStorage, CArchive::store );

	m_graphixObject.Serialize( maskArchive );

	// flush the archive
	maskArchive.Flush();

	// get the size
	DWORD len = static_cast<unsigned long>(maskStorage.GetLength());
	
	// position to the beginning
	maskStorage.SeekToBegin();

	// create a buffer
	LPBYTE pBuff = new BYTE[len];
	
	// get the data
	maskStorage.Read( pBuff, len );

	SVString hexMaskDataStr;

	// Convert to hex String
	::SVConvertToHexString( len, pBuff, hexMaskDataStr );

	delete [] pBuff;

	_variant_t value;
	value.SetString(hexMaskDataStr.c_str());
	rWriter.WriteAttribute(scMaskDataTag, value);
	value.Clear();
}

HRESULT SVUserMaskOperatorClass::GetObjectValue( const SVString& rValueName, _variant_t& rValue ) const
{
	HRESULT hr = S_OK;

	if( _T("MaskData") == rValueName )
	{
		// Add Script of SVGraphixClass as a hex string
		// Prepare Archive...
		CMemFile maskStorage;
		CArchive maskArchive( &maskStorage, CArchive::store );

		m_graphixObject.Serialize( maskArchive );

		// flush the archive
		maskArchive.Flush();

		// get the size
		DWORD len = static_cast<unsigned long>(maskStorage.GetLength());
		
		// position to the beginning
		maskStorage.SeekToBegin();

		// create a buffer
		std::vector< unsigned char > l_Buffer( len );
		
		// get the data
		maskStorage.Read( &( l_Buffer[0] ), static_cast<UINT>(l_Buffer.size()) );

		SVSAFEARRAY l_SafeArray;

		for( size_t i = 0; i < l_Buffer.size(); i++ )
		{
			_variant_t l_Value = l_Buffer[ i ];

			l_SafeArray.Add( l_Value );
		}

		rValue = l_SafeArray;
	}
	else
	{
		hr = SVUnaryImageOperatorClass::GetObjectValue( rValueName, rValue );
	}

	return hr;
}

// Restoration of trivial members
HRESULT SVUserMaskOperatorClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	bool bOk = false;
	
	SvCl::SVObjectSVStringArrayClass StringArray;
	SvCl::SVObjectLongArrayClass svLongArray;

	if ( ( bOk = pDataObject->GetAttributeData( _T("MaskOperator"), svLongArray ) ) )
	{
		for( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_Data.evoCurrentMaskOperator.SetValue(svLongArray[i]);
		}
	}
	else if ( ( bOk = pDataObject->GetAttributeData( _T("MaskData"), StringArray ) ) )
	{
		DWORD len;
		LPBYTE pBuff;
		for( int i = 0; i < StringArray.GetSize(); i++ )
		{
			// Get the data back to binary
			if( ::SVConvertFromHexString( len, &pBuff, StringArray[i] ) )
			{
				CMemFile maskStorage;
			
				// write the data to the memory file
				maskStorage.Write( pBuff, len );
				maskStorage.Flush();
				maskStorage.SeekToBegin();

				delete [] pBuff;

				// Create a CArchive to load from
				CArchive memArchive( &maskStorage, CArchive::load );

				// call SVGraphixClass::Serialize()
				m_graphixObject.Serialize( memArchive );
			}
		}
	}
	else
	{
		hr = SVUnaryImageOperatorClass::SetObjectValue( pDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns false, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVUserMaskOperatorClass::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	BOOL bActive;
	m_Data.bvoActivated.GetValue( bActive );

	if( bActive && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );

		if( !m_MaskBufferHandlePtr.empty() )
		{
			SVImageBufferHandleImage l_MaskMilHandle;
			m_MaskBufferHandlePtr->GetData( l_MaskMilHandle );

			HRESULT MatroxCode;

			DWORD dwMaskType = MASK_TYPE_STATIC;
			m_Data.dwvoMaskType.GetValue( dwMaskType );

			if ( dwMaskType == MASK_TYPE_IMAGE )
			{
				SVImageClass* l_pMaskInputImage = getMaskInputImage();
				SVImageClass* l_pRefImage = getReferenceImage();

				if ( nullptr != l_pRefImage &&
					nullptr != l_pMaskInputImage )
				{
					SVExtentPointStruct l_svPoint;

					SVSmartHandlePointer l_MaskInputBuffer;

					SVImageExtentClass l_svExtents = l_pRefImage->GetImageExtents();

					if ( S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPoint ) &&
						l_pMaskInputImage->GetImageHandle( l_MaskInputBuffer ) && !( l_MaskInputBuffer.empty() ) )
					{
						if ( S_OK != l_pMaskInputImage->ValidateAgainstOutputExtents( l_svExtents ) )
						{
							MatroxCode = SVMatroxBufferInterface::ClearBuffer( l_MaskMilHandle.GetBuffer(), 0.0 );

							if (S_OK != MatroxCode)
							{
								if (nullptr != pErrorMessages)
								{
									SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateBufferFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
									pErrorMessages->push_back(Msg);
								}
								// Signal that something was wrong...
								SetInvalid();
								rRunStatus.SetInvalid();
								return false;
							}
						}

						SVImageBufferHandleImage l_MaskMilBuffer;
						l_MaskInputBuffer->GetData( l_MaskMilBuffer );

						MatroxCode = SVMatroxBufferInterface::CopyBuffer( l_MaskMilHandle.GetBuffer(), l_MaskMilBuffer.GetBuffer(), (long)-l_svPoint.m_dPositionX, (long)-l_svPoint.m_dPositionY );

						if (S_OK != MatroxCode)
						{
							if (nullptr != pErrorMessages)
							{
								SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
								pErrorMessages->push_back(Msg);
							}
							// Signal that something was wrong...
							SetInvalid();
							rRunStatus.SetInvalid();
							return false;
						}
					}
					else
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
						// Signal that something was wrong...
						SetInvalid();
						rRunStatus.SetInvalid();
						return false;
					}
				}
			}// end if ( dwMaskType == MASK_TYPE_IMAGE )

			else if ( dwMaskType == MASK_TYPE_SHAPE )
			{
				SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
				pShapeHelper->onRun(First, RInputImageHandle, ROutputImageHandle, rRunStatus);
			}

			long lMaskOperator = SVImageAnd;
			m_Data.evoCurrentMaskOperator.GetValue( lMaskOperator );

			MatroxCode = SVMatroxImageInterface::Arithmetic(l_OutMilHandle.GetBuffer(),
				First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				l_MaskMilHandle.GetBuffer(), 
				static_cast<SVImageOperationTypeEnum>(lMaskOperator) );

			if (S_OK != MatroxCode)
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunArithmeticFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
				// Signal that something was wrong...
				SetInvalid();
				rRunStatus.SetInvalid();
				return false;
			}

#ifdef _DEBUG999
			{
				// Dump the images to .bmp files
				try
				{
					MatroxCode = SVMatroxBufferInterface::Export( m_MaskBufferHandlePtr.milImage,
						_T("C:\\Temp\\Mask.bmp"),
						SVFileBitmap );


					MatroxCode = SVMatroxBufferInterface::Export( RInputImageHandle.milImage,
						_T("C:\\Temp\\Input.bmp"),
						SVFileBitmap);


					MatroxCode = SVMatroxBufferInterface::Export( ROutputImageHandle.milImage,
						_T("C:\\Temp\\Output.bmp"),
						SVFileBitmap);

				}
				catch(...)
				{
				}

			}
#endif //_DEBUG999
			SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
			if ( pShapeHelper )
			{
				DWORD dwShapeColor = m_statusColor.GetDefaultValue();
				if ( MASK_TYPE_SHAPE == dwMaskType )
				{
					dwShapeColor = rRunStatus.GetStatusColor();
				}
				pShapeHelper->m_statusColor.SetValue(dwShapeColor);
			}
			// Success...
			return true;
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		// Signal that something was wrong...
		SetInvalid();
		rRunStatus.SetInvalid();
		SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
		if ( pShapeHelper )
		{
			DWORD dwShapeColor = m_statusColor.GetDefaultValue();
			pShapeHelper->m_statusColor.SetValue(dwShapeColor);
		}
		return false;
	}

	// Was deactivated, but everything is ok !!!
	// So just return false, don't invalidate !!!
	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		DWORD dwShapeColor = m_statusColor.GetDefaultValue();
		pShapeHelper->m_statusColor.SetValue(dwShapeColor);
	}
	return false;
}

SVImageClass* SVUserMaskOperatorClass::getMaskInputImage() const
{
	if( m_inObjectInfo.IsConnected() && nullptr != m_inObjectInfo.GetInputObjectInfo().m_pObject )
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when m_pObject is not nullptr then it is a SVImageClass
		return static_cast<SVImageClass*> (m_inObjectInfo.GetInputObjectInfo().m_pObject);
	}
	return nullptr;
}

bool SVUserMaskOperatorClass::hasToAskFriendForConnection( const SVObjectTypeInfoStruct& rInfo, SVObjectClass*& rPOwner ) const
{
	bool Result(true);
	if ( SVImageObjectType == rInfo.ObjectType )
	{
		rPOwner = GetInspection();
		Result = false;
	}
	return Result;
}

#pragma region IMask
SvOi::MatroxImageSmartHandlePtr SVUserMaskOperatorClass::GetReferenceImage() const
{
	SvOi::MatroxImageSmartHandlePtr handlePtr;
	SVImageClass* pImage = const_cast<SVUserMaskOperatorClass*>(this)->getReferenceImage();
	if (pImage)
	{
		if (SvOi::SVImageTypeEnum::SVImageTypePhysical == pImage->GetImageType())
		{
			handlePtr = pImage->GetParentImageInterface()->getImageData();
		}
		else
		{
			handlePtr = pImage->getParentImageData();
		}
	}
	return handlePtr;
}

SvOi::MatroxImageSmartHandlePtr SVUserMaskOperatorClass::GetMaskImage() const
{
	SvOi::MatroxImageSmartHandlePtr data(new MatroxImageData(m_MaskBufferHandlePtr));
	return data;
}

HRESULT SVUserMaskOperatorClass::Import(const SVString& filename)
{
	HRESULT hr(S_OK);
	CFile maskFile; 

	if (maskFile.Open(filename.c_str(), CFile::modeRead))
	{
		CArchive archive(&maskFile, CArchive::load);
		m_graphixObject.Serialize(archive); 
		archive.Close();
		maskFile.Close();

		Refresh(); 
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

HRESULT SVUserMaskOperatorClass::Export(const SVString& filename)
{
	HRESULT hr(S_OK);
	CFile maskFile; 

	if (maskFile.Open(filename.c_str(), CFile::modeCreate | CFile::modeWrite))
	{
		CArchive archive(&maskFile, CArchive::store);
		m_graphixObject.Serialize(archive); 
		archive.Close();
		maskFile.Close();
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}
	
HGLOBAL SVUserMaskOperatorClass::GetMaskData() const
{
	return m_graphixObject.GetGraphixData();
}

bool SVUserMaskOperatorClass::SetMaskData(HGLOBAL hGlobal)
{
	return m_graphixObject.SetGraphixData(hGlobal) ? true : false;
}
#pragma endregion IMask

