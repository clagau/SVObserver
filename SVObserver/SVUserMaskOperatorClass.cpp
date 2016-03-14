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
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/MatroxImageData.h"
#include "SVTool.h"
#include "SVImageProcessingClass.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVGlobal.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVInspectionProcess.h"
#include "SVObserver.h"
#include "SVToolSet.h"
#include "SVShapeMaskHelperClass.h"
#include "SVUnaryImageOperatorList.h"

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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVUserMaskOperatorObjectType;

	m_inObjectInfo.SetInputObjectType( SVImageObjectType );
	m_inObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inObjectInfo, _T( "UserMaskImage" ) );


	// RRRRGGGGHHHHHH who created this stupid object structure????
	SVShapeMaskHelperClass* pShapeHelper = new SVShapeMaskHelperClass(this);
/*	
	SVClassInfoStruct infoShapeHelper;
	infoShapeHelper.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;
	infoShapeHelper.ObjectTypeInfo.SubType	= SVShapeMaskHelperObjectType;
	infoShapeHelper.ClassId = SVShapeMaskHelperClassGuid;
	infoShapeHelper.ClassName.LoadString( IDS_CLASSNAME_SHAPE_MASK_HELPER );
	SVShapeMaskHelperClass* pShapeHelper = dynamic_cast <SVShapeMaskHelperClass*> (infoShapeHelper.Construct());
	pShapeHelper->SetObjectOwner(this);
*/
	m_guidShapeHelper = pShapeHelper->GetUniqueObjectID();

	BOOL bAddFriend = AddFriend( pShapeHelper->GetUniqueObjectID() );
	ASSERT( bAddFriend );



	RegisterEmbeddedObject( &m_Data.bvoActivated, SVMaskEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SVResetItemIP );
	RegisterEmbeddedObject( &m_Data.dwvoMaskType, SVMaskUseImageMaskGuid, IDS_MASK_TYPE, false, SVResetItemIP );
	RegisterEmbeddedObject( &m_Data.evoCurrentMaskOperator, SVMaskOperatorGuid, IDS_OBJECTNAME_MASK_OPERATOR, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.evoFillArea, SVMaskFillAreaGuid, IDS_OBJECTNAME_MASK_FILL_AREA, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.lvoFillColor, SVMaskFillColorGuid, IDS_OBJECTNAME_MASK_FILL_COLOR, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.evoDrawCriteria, SVDrawCriteriaObjectGuid, IDS_OBJECTNAME_DRAW_CRITERIA, false, SVResetItemOwner );

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
	m_Data.evoCurrentMaskOperator.SetDefaultValue(SVImageAnd, TRUE);

	vec.clear();
	vec.push_back( SVEnumeratePair(_T("Mask Area"), 0 ) );
	vec.push_back( SVEnumeratePair(_T("Background"), 1 ) );
	m_Data.evoFillArea.SetEnumTypes(vec);
	m_Data.evoFillArea.SetDefaultValue((long)0, TRUE);	// mask area is default

	m_Data.lvoFillColor.SetDefaultValue(0, TRUE);	// black default

	vec.clear();
	vec.push_back( SVEnumeratePair(_T("None"), SVNone ) );
	vec.push_back( SVEnumeratePair(_T("Non-Black"), SVNonBlackPixels ) );
	vec.push_back( SVEnumeratePair(_T("Non-White"), SVNonWhitePixels ) );
	m_Data.evoDrawCriteria.SetEnumTypes( vec );
	m_Data.evoDrawCriteria.SetDefaultValue( SVNone, TRUE );

	m_Data.bvoActivated.SetDefaultValue( FALSE, TRUE );
	m_Data.dwvoMaskType.SetDefaultValue(MASK_TYPE_STATIC, TRUE);




	m_MaskBufferInfo.SetOwnerImage( SV_GUID_NULL );
	m_MaskBufferInfo.SetOwner( SV_GUID_NULL );


	// Set default inputs and outputs
	addDefaultInputObjects();
}

BOOL SVUserMaskOperatorClass::CloseObject()
{
	BOOL bOk = DestroyLocalImageBuffer() == S_OK;

	if ( isCreated )
	{
		isCreated = FALSE;

		bOk = SVUnaryImageOperatorClass::CloseObject();
	}

	return bOk;
}

BOOL SVUserMaskOperatorClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVUnaryImageOperatorClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	m_Data.bvoActivated.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_Data.evoDrawCriteria.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE ;

	m_Data.dwvoMaskType.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_Data.evoCurrentMaskOperator.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_Data.evoFillArea.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_Data.lvoFillColor.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;

	isCreated = bOk;

	return isCreated;
}

HRESULT SVUserMaskOperatorClass::ResetObject()
{
	HRESULT l_hrOk = SVUnaryImageOperatorClass::ResetObject();

	DWORD dwMaskType;
	m_Data.dwvoMaskType.GetValue( dwMaskType );
	BOOL bActive = FALSE;
	m_Data.bvoActivated.GetValue(bActive);

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	ASSERT( pShapeHelper );
	if ( pShapeHelper )
	{
		l_hrOk = pShapeHelper->ResetObject();

		SetBits( pShapeHelper->ObjectAttributesSetRef(),     SV_PRINTABLE, dwMaskType == MASK_TYPE_SHAPE );
		SetBits( pShapeHelper->ObjectAttributesSetRef(),     SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE, true );
		SetBits( m_Data.evoFillArea.ObjectAttributesSetRef(), SV_PRINTABLE, ( (dwMaskType == MASK_TYPE_SHAPE) && bActive) );
		SetBits( m_Data.lvoFillColor.ObjectAttributesSetRef(), SV_PRINTABLE, ( (dwMaskType == MASK_TYPE_SHAPE) && bActive)  );
		// ARGH 
		SetBits( pShapeHelper->ObjectAttributesAllowedRef(), SV_PRINTABLE | SV_VIEWABLE, dwMaskType == MASK_TYPE_SHAPE );
		SetBits( pShapeHelper->ObjectAttributesAllowedRef(), SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE, true );
		SetBits( m_Data.evoFillArea.ObjectAttributesAllowedRef(), SV_PRINTABLE | SV_VIEWABLE, ( (dwMaskType == MASK_TYPE_SHAPE) && bActive)  );
		SetBits( m_Data.lvoFillColor.ObjectAttributesAllowedRef(), SV_PRINTABLE | SV_VIEWABLE, ( (dwMaskType == MASK_TYPE_SHAPE) && bActive)  );

		if ( !bActive )
		{
			//turn off items.
			SetBits( m_Data.evoFillArea.ObjectAttributesSetRef(), SV_PRINTABLE| SV_VIEWABLE, ( (dwMaskType == MASK_TYPE_SHAPE) && bActive) );
			SetBits( m_Data.lvoFillColor.ObjectAttributesSetRef(), SV_PRINTABLE| SV_VIEWABLE, ( (dwMaskType == MASK_TYPE_SHAPE) && bActive)  );
		}

	}
	// if image mask or overlays are turned on, set to true
	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_Data.evoDrawCriteria.GetValue( l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	m_bUseOverlays = ( l_eCriteria != SVNone || dwMaskType == MASK_TYPE_IMAGE );

	if( CreateLocalImageBuffer() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
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
	for( size_t i = 0; i < friendList.size(); i++ )
	{
		const SVObjectInfoStruct& friendObjectInfo = friendList[i];
		if( pMaskHelper = dynamic_cast<SVShapeMaskHelperClass*> (friendObjectInfo.PObject) )
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

		BOOL bAddFriend = AddFriend( pShapeHelper->GetUniqueObjectID() );
		ASSERT( bAddFriend );

		if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pShapeHelper), NULL ) == SVMR_SUCCESS )
		{
			pMaskHelper = pShapeHelper;
		}
	}

	ASSERT( pMaskHelper );
	return pMaskHelper;
}

// ISVCancel
bool SVUserMaskOperatorClass::CanCancel()
{
	return true;
}

// ISVCancel
HRESULT SVUserMaskOperatorClass::GetCancelData(SVCancelData*& p_rpData)
{
	HRESULT hr = S_OK;
	SVMaskOperatorCancelData* pData = new SVMaskOperatorCancelData;
	p_rpData = pData;

	*pData = m_Data;

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		HRESULT hrShape = pShapeHelper->GetCancelData( pData->pShapeData );
	}

	return hr;
}

// ISVCancel
HRESULT SVUserMaskOperatorClass::SetCancelData(SVCancelData* p_pData)
{
	HRESULT hr = SV_FALSE;
	SVMaskOperatorCancelData* pData = dynamic_cast <SVMaskOperatorCancelData*> (p_pData);
	ASSERT( pData );
	if ( pData )
	{
		hr = S_OK;
		SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
		if ( pShapeHelper )
		{
			hr = pShapeHelper->SetCancelData( pData->pShapeData );
		}
		if ( hr == S_OK )
		{
			m_Data = *pData;
			m_Data.pShapeData = NULL;

			hr = ResetObject();
		}
	}
	return hr;
}

// ISVCancel2
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

HRESULT SVUserMaskOperatorClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	DWORD dwMaskType = 0;
	m_Data.dwvoMaskType.GetValue( dwMaskType );

	if ( dwMaskType == MASK_TYPE_IMAGE )
	{
		if ( p_psvImage != NULL && p_psvImage == getMaskInputImage() )
		{
			l_hrOk = S_OK;
		}
	}
	else
	{
		if( p_psvImage == m_pCurrentUIOPL->getInputImage() )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVUserMaskOperatorClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
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
	bool l_bActivated;
	m_Data.bvoActivated.GetValue( l_bActivated );

	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_Data.evoDrawCriteria.GetValue(l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	DWORD dwMaskType;
	m_Data.dwvoMaskType.GetValue( dwMaskType );

	SVImageClass* pInputImage = getMaskInputImage();
	SVImageExtentClass l_svExtents;
	if( l_eCriteria != SVNone && l_bActivated && !m_MaskBufferHandlePtr.empty() &&
		( dwMaskType != MASK_TYPE_IMAGE || pInputImage != NULL ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		m_MaskBufferHandlePtr->GetData( l_MilHandle );

		GetTool()->GetImageExtent( l_svExtents );

		RECT l_rec;

		HRESULT l_hr = m_MaskBufferInfo.GetOutputRectangle( l_rec );
		// User Mask draw with lines 

		
		SVMatroxBufferInterface::SVStatusCode l_Code;
		
		LPVOID pSrcHostBuffer = NULL;
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

	if( l_psvImage != NULL )
	{
		SVImageInfoClass l_MaskBufferInfo = l_psvImage->GetImageInfo();

		bool bImageInfoChanged = (l_MaskBufferInfo != m_MaskBufferInfo);
		if ( bImageInfoChanged )
		{
			m_MaskBufferInfo = l_MaskBufferInfo;
			l_hrOk = DestroyLocalImageBuffer();
			l_hrOk = SVImageProcessingClass::Instance().CreateImageBuffer( m_MaskBufferInfo, m_MaskBufferHandlePtr );
		}

		if( l_hrOk == S_OK )
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
BOOL SVUserMaskOperatorClass::Refresh()
{
	SVInspectionProcess* pInspection = GetInspection();

	if( nullptr != pInspection )
	{
		DWORD dwMaskType;
		m_Data.dwvoMaskType.GetValue( dwMaskType );
		SVShapeMaskHelperClass* pShape = GetShapeHelper();
		if ( pShape != NULL && ( dwMaskType == MASK_TYPE_SHAPE) )
		{
			pShape->Refresh();
			return TRUE;
		}
		else
		{
			// Init buffer to 0...
			// &&&
			if ( S_OK == SVImageProcessingClass::Instance().InitBuffer( m_MaskBufferHandlePtr ) )
			{
				HDC dc;
				// Get DC...
				dc = SVImageProcessingClass::Instance().CreateBufferDC( m_MaskBufferInfo, m_MaskBufferHandlePtr );
				if(dc != (HDC)0)        // && dc != (HDC)-1)
				{
	#ifdef _DEBUG //999
					if(dc == (HDC)(-1))
					{
					}
	#endif //_DEBUG
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
						SVMatroxBufferInterface::SVStatusCode l_Code = 
							SVMatroxBufferInterface::Export( m_MaskBufferHandlePtr.milImage,
						( char* )_T("C:\\Temp\\MaskBuffer.bmp"),
						SVFileBitmap );

					}
					catch(...)
					{
					}

				}
	#endif //_DEBUG
					//
					// Check to see if a new DIB image had to be created in order.
					// to get a HDC for it from the MIL library 6.0.
					//
					//if(GetSystem()->imageDIB_MIL &&
					//  (GetSystem()->imageDIB_MIL != maskBufferInfo.HImageBuffer.milImage))
					//{
					//    MbufFree(maskBufferInfo.HImageBuffer.milImage);
					//    maskBufferInfo.HImageBuffer.milImage = GetSystem()->imageDIB_MIL;
					//    GetSystem()->imageDIB_MIL = M_NULL;
					//}

					// Release DC...
					return S_OK == SVImageProcessingClass::Instance().DestroyBufferDC( m_MaskBufferHandlePtr, dc );
				}// end if(dc != (HDC)0)        // && dc != (HDC)-1)
			}// end if ( S_OK == TheSVObserverApp.mpsvImaging->InitBuffer( m_MaskBufferHandle ) )
		}// end else if not shape
	}// end if( GetIPDoc() )
	return FALSE;
}

#ifdef USE_OBJECT_SCRIPT
// Get parseable script of this object.
// This script is human readable and is used to make this 
// object persistent. Use TheObjectScriptParser parse function
// to build in runtime an object based on this script.
// Be careful, if this object already exists, it would be overwritten.
void SVUserMaskOperatorClass::GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString script;

	// preallocate 16K
	script.GetBuffer(16384);  
	script.ReleaseBuffer(); 
	
	SVUnaryImageOperatorClass::GetObjectScript( script, RStrAliasTable, Indent );

	CString nameStrDelimiter = _T( "'" );

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	// Overwrite task object termination termination...
	int last = script.ReverseFind( TCHAR( '}' ) );
	if( last >= 0 )
		script = script.Left( last );

	// 
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetName();

	// Get the Data Values (Member Info, Values)
	CString tmp;

	/* // This is now a registered value object

	// Add mask operator as trivial members
	long lMaskOperator = M_AND;
	m_currentMaskOperator.GetValue(0, lMaskOperator);
	tmp.Format( "LONG %d", lMaskOperator );
	script += strIndent + objectTag + _T( ".MaskOperator" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );
	*/

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

	CString hexMaskDataStr;

	// Convert to hex String
	::SVConvertToHexString( len, pBuff, hexMaskDataStr );

	delete [] pBuff;

	tmp.Format( "STRING \"%s\"", hexMaskDataStr );
	script += strIndent + objectTag + _T( ".MaskData" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );
	// Add termination...
	script += strIndent + _T( "};" );

	script.FreeExtra();

	RStrScript += script;
}
#endif

void SVUserMaskOperatorClass::Persist( SVObjectWriter& rWriter )
{
	SVTaskObjectClass::Persist(rWriter);
	
	CString tmp;

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

	CString hexMaskDataStr;

	// Convert to hex String
	::SVConvertToHexString( len, pBuff, hexMaskDataStr );

	delete [] pBuff;

	tmp.Format( "%s", hexMaskDataStr );
	_variant_t value;
	value.SetString(tmp);
	rWriter.WriteAttribute(scMaskDataTag, value);
	value.Clear();
}

HRESULT SVUserMaskOperatorClass::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "MaskData" ) )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

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

		l_TempVariant = l_SafeArray;

		l_TempVariant.Detach();
	}
	else
	{
		hr = SVUnaryImageOperatorClass::GetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVUserMaskOperatorClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "MaskData" ) )
	{
		if( ( p_rVariantValue.vt & VT_ARRAY ) == VT_ARRAY )
		{
			SVSAFEARRAY l_SafeArray( p_rVariantValue );

			// create a buffer
			std::vector< unsigned char > l_Buffer( l_SafeArray.size() );

			for( size_t i = 0; hr == S_OK && i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;

				if( l_SafeArray.GetElement( i, l_Value ) == S_OK )
				{
					l_Buffer[ i ] = l_Value;
				}
				else
				{
					hr = E_FAIL;
				}
			}

			if( hr == S_OK )
			{
				CMemFile maskStorage;
			
				// write the data to the memory file
				maskStorage.Write( &( l_Buffer[ 0 ] ), static_cast<UINT>(l_Buffer.size()) );
				maskStorage.Flush();
				maskStorage.SeekToBegin();

				// Create a CArchive to load from
				CArchive memArchive( &maskStorage, CArchive::load );

				// call SVGraphixClass::Serialize()
				m_graphixObject.Serialize( memArchive );
			}
		}
		else
		{
			hr = E_FAIL;
		}
	}
	else
	{
		hr = SVUnaryImageOperatorClass::SetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

// Restoration of trivial members
HRESULT SVUserMaskOperatorClass::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	
	SvCl::SVObjectCStringArrayClass svCStringArray;
	SvCl::SVObjectLongArrayClass svLongArray;

	if ( ( bOk = PDataObject->GetAttributeData( "MaskOperator", svLongArray ) ) )
	{
		for( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_Data.evoCurrentMaskOperator.SetValue( 1, svLongArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "MaskData", svCStringArray ) ) )
	{
		DWORD len;
		LPBYTE pBuff;
		for( int i = 0; i < svCStringArray.GetSize(); i++ )
		{
			CString hexMaskDataStr = svCStringArray[i];
			
			// Get the data back to binary
			if( ::SVConvertFromHexString( len, &pBuff, hexMaskDataStr ) )
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
		hr = SVUnaryImageOperatorClass::SetObjectValue( PDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

BOOL SVUserMaskOperatorClass::ConnectAllInputs()
{
	DWORD_PTR dwResult = SVMR_NOT_PROCESSED;
	SVInputInfoListClass inputList;
	
	// Add the defaults
	addDefaultInputObjects(TRUE, &inputList);
	
	// tell friends to connect...
	for (size_t j = 0; j < friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = friendList[j];
		::SVSendMessage(rFriend.UniqueObjectID, SVM_CONNECT_ALL_INPUTS, NULL, NULL);
	}

	// find our inputs
	for (int i = 0; i < inputList.GetSize(); ++ i)
	{
		SVInObjectInfoStruct* pInInfo = inputList.GetAt(i);
		if (pInInfo)
		{
			// Is not yet connected...
			if (!pInInfo->IsConnected())
			{
				if (pInInfo->GetInputObjectInfo().UniqueObjectID == SVInvalidGUID)
				{
					// Input Object is not set...Try to get one...
					SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;
					
					SVObjectClass* pOwner = GetOwner();
					SVObjectClass* pRequestor = pInInfo->PObject;
					SVObjectClass* pObject = NULL;
					BOOL bSuccess = FALSE;
					
					if ( info.ObjectType == SVImageObjectType )
					{
						pOwner = GetInspection();
					}
					else
					{
						// Ask first friends...
						for (size_t j = 0; j < friendList.size(); ++ j)
						{
							const SVObjectInfoStruct& rFriend = friendList[j];
							pObject = reinterpret_cast<SVObjectClass *>(::SVSendMessage(rFriend.UniqueObjectID, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info)));
							if (pObject)
							{
								// Connect input ...
								pInInfo->SetInputObject( pObject->GetUniqueObjectID() );
								bSuccess = TRUE;
								break;
							}
						}
					}
					
					// Then ask owner...
					if (! bSuccess)
					{
						while (pOwner)
						{
							SVInspectionProcess* pInspection =  GetInspection();
							// if color system & pOwner == SVToolSetClass
							if (nullptr != pInspection && pInspection->IsColorCamera() && (SV_IS_KIND_OF(pOwner, SVToolSetClass)) && info.ObjectType == SVImageObjectType)
							{
								pObject = reinterpret_cast<SVObjectClass *>(::SVSendMessage(pOwner, SVM_GET_IMAGE_BAND0_OBJECT, reinterpret_cast<DWORD_PTR>(pRequestor), reinterpret_cast<DWORD_PTR>(&info)));
							}
							else
							{
								pObject = reinterpret_cast<SVObjectClass *>(::SVSendMessage(pOwner, SVM_GETFIRST_OBJECT, reinterpret_cast<DWORD_PTR>(pRequestor), reinterpret_cast<DWORD_PTR>(&info)));
							}
							if (pObject)
							{
								// Connect input ...
								pInInfo->SetInputObject( pObject->GetUniqueObjectID() );
								break;
							}
							else
							{
								pOwner = pOwner->GetOwner();
								pRequestor = pRequestor->GetOwner();
							}
						}
					}
				}
				
				// Finally try to connect...
				DWORD_PTR dwConnectResult = ::SVSendMessage(pInInfo->GetInputObjectInfo().UniqueObjectID, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(pInInfo), NULL);

				dwResult = dwConnectResult | dwResult;
			}
			else
			{
				dwResult = SVMR_SUCCESS;
			}
		}
		else
		{
			ASSERT(0);
			dwResult = SVMR_NO_SUCCESS;
		}
	}
	return (dwResult == SVMR_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVUserMaskOperatorClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
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
			
			SVMatroxImageInterface::SVStatusCode l_Code;

			DWORD dwMaskType = MASK_TYPE_STATIC;
			m_Data.dwvoMaskType.GetValue( dwMaskType );

			if ( dwMaskType == MASK_TYPE_IMAGE )
			{
				SVImageClass* l_pMaskInputImage = getMaskInputImage();
				SVImageClass* l_pRefImage = getReferenceImage();

				if ( l_pRefImage != NULL &&
				     l_pMaskInputImage != NULL )
				{
					SVExtentPointStruct l_svPoint;

					SVSmartHandlePointer l_MaskInputBuffer;

					SVImageExtentClass l_svExtents = l_pRefImage->GetImageExtents();

					if ( l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPoint ) == S_OK &&
					     l_pMaskInputImage->GetImageHandle( l_MaskInputBuffer ) && !( l_MaskInputBuffer.empty() ) )
					{
						if ( l_pMaskInputImage->ValidateAgainstOutputExtents( l_svExtents ) != S_OK )
						{
							l_Code = SVMatroxBufferInterface::ClearBuffer( l_MaskMilHandle.GetBuffer(), 0.0 );

							if( l_Code != SVMEE_STATUS_OK )
							{
								// Signal that something was wrong...
								SetInvalid();
								RRunStatus.SetInvalid();
								return FALSE;
							}
						}

						SVImageBufferHandleImage l_MaskMilBuffer;
						l_MaskInputBuffer->GetData( l_MaskMilBuffer );

						l_Code = SVMatroxBufferInterface::CopyBuffer( l_MaskMilHandle.GetBuffer(), l_MaskMilBuffer.GetBuffer(), (long)-l_svPoint.m_dPositionX, (long)-l_svPoint.m_dPositionY );

						if( l_Code != SVMEE_STATUS_OK )
						{
							// Signal that something was wrong...
							SetInvalid();
							RRunStatus.SetInvalid();
							return FALSE;
						}
					}
					else
					{
						// Signal that something was wrong...
						SetInvalid();
						RRunStatus.SetInvalid();
						return FALSE;
					}
				}
			}// end if ( dwMaskType == MASK_TYPE_IMAGE )

			else if ( dwMaskType == MASK_TYPE_SHAPE )
			{
				SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
				BOOL bShapeOnRun = pShapeHelper->onRun(First, RInputImageHandle, ROutputImageHandle, RRunStatus);
			}

			long lMaskOperator = SVImageAnd;
			m_Data.evoCurrentMaskOperator.GetValue( lMaskOperator );

			l_Code = SVMatroxImageInterface::Arithmetic(l_OutMilHandle.GetBuffer(),
				( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				l_MaskMilHandle.GetBuffer(), 
						static_cast<SVImageOperationTypeEnum>(lMaskOperator) );

			if( l_Code != SVMEE_STATUS_OK )
			{
				// Signal that something was wrong...
				SetInvalid();
				RRunStatus.SetInvalid();
				return FALSE;
			}

#ifdef _DEBUG999
            {
                // Dump the images to .bmp files
				try
				{
					l_Code = SVMatroxBufferInterface::Export( m_MaskBufferHandlePtr.milImage,
						_T("C:\\Temp\\Mask.bmp"),
						SVFileBitmap );


					l_Code = SVMatroxBufferInterface::Export( RInputImageHandle.milImage,
						_T("C:\\Temp\\Input.bmp"),
						SVFileBitmap);


					l_Code = SVMatroxBufferInterface::Export( ROutputImageHandle.milImage,
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
				DWORD dwShapeColor;
				statusColor.GetDefaultValue(dwShapeColor);
				if ( dwMaskType == MASK_TYPE_SHAPE )
				{
					dwShapeColor = RRunStatus.GetStatusColor();
				}
				pShapeHelper->statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwShapeColor );
			}
			// Success...
			return TRUE;
        }

		// Signal that something was wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
		SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
		if ( pShapeHelper )
		{
			DWORD dwShapeColor;
			statusColor.GetDefaultValue(dwShapeColor);
			pShapeHelper->statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwShapeColor );
		}
		return FALSE;
	}

	// Was deactivated, but everything is ok !!!
	// So just return FALSE, don't invalidate !!!
	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		DWORD dwShapeColor;
		statusColor.GetDefaultValue(dwShapeColor);
		pShapeHelper->statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwShapeColor );
	}
	return FALSE;
}

SVImageClass* SVUserMaskOperatorClass::getMaskInputImage()
{
	if( m_inObjectInfo.IsConnected() && 
		m_inObjectInfo.GetInputObjectInfo().PObject )
	{
		return dynamic_cast< SVImageClass* > ( m_inObjectInfo.GetInputObjectInfo().PObject );
	}
	return NULL;

}

BOOL SVUserMaskOperatorClass::OnValidate()
{
	BOOL bValidate = SVUnaryImageOperatorClass::OnValidate();
	if ( bValidate )
	{
		SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
		if ( pShapeHelper )
		{
			DWORD dwMaskType = MASK_TYPE_STATIC;
			m_Data.dwvoMaskType.GetValue( dwMaskType );
			if ( dwMaskType == MASK_TYPE_SHAPE )
			{
				BOOL bShapeValidate = pShapeHelper->OnValidate();
				bValidate = bValidate && bShapeValidate;
			}
		}
	}
	return bValidate;
}

#pragma region IMask
SvOi::MatroxImageSmartHandlePtr SVUserMaskOperatorClass::GetReferenceImage() const
{
	SvOi::MatroxImageSmartHandlePtr handlePtr;
	SVImageClass* pImage = const_cast<SVUserMaskOperatorClass*>(this)->getReferenceImage();
	if (pImage)
	{
		if (SVImageTypePhysical == pImage->GetImageType())
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

