//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageClass
//* .File Name       : $Workfile:   SVImageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   26 Jun 2014 17:41:18  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
#include "ObjectInterfaces/ISVImage.h"
#include "SVContainerLibrary/SVVector.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageChildStruct.h"
#include "SVImageObjectClass.h"
#include "SVObjectAppClass.h"
#include "SVCameraInfoStruct.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

typedef SVVector< long > SVImageChildIndexCArray;

class SVImageClass : public SVObjectAppClass, public SvOi::ISVImage
{
	SV_DECLARE_CLASS( SVImageClass );

public:
	typedef std::pair< SVGUID, SVImageExtentClass > SVChildExtentPair;
	typedef std::deque< SVChildExtentPair > SVChildExtentDeque;

	SVImageClass( LPCSTR ObjectName );
	SVImageClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGE );
	virtual ~SVImageClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct ) override;
	virtual BOOL CloseObject() override;

	SVImageClass* GetParentImage() const;
	const GUID& GetParentImageID() const;
	const SVImageInfoClass& GetImageInfo() const; //@TODO: Change the logic so that this is never needed outside this class

	HRESULT InitializeImage( SVImageTypeEnum p_ImageType );
	HRESULT InitializeImage( SVImageClass* p_pParentImage );
	HRESULT InitializeImage( const GUID& p_rParentID );
	HRESULT InitializeImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );

	//@WARNING [Jim][8 July 2015] - Look into and evaluate this behavior.
	//-  This UpdateImage function calls RebuildStorage() which calls 
	//-  UpdateFromParentInformation() which overwrites the extents I just 
	//-  sent in anyhow????
	HRESULT UpdateImage( const SVImageExtentClass& p_rExtent, bool p_ExcludePositionCheck = false );
	HRESULT UpdateImage( const SVImageInfoClass& p_rImageInfo );
	HRESULT UpdateImage( const GUID& p_rParentID );
	HRESULT UpdateImage( const GUID& p_rParentID, const SVImageExtentClass& p_rExtent );
	HRESULT UpdateImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );
	HRESULT UpdateImage( SVImageTypeEnum p_ImageType );
	HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const SVImageInfoClass& p_rImageInfo );
	HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID );
	HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );

	virtual BOOL SetImageDepth( long lDepth );

	virtual const SVClock::SVTimeStamp& GetLastResetTimeStamp() const;
	
	virtual HRESULT ResetObject() override;

	SVImageExtentClass GetImageExtents();
	HRESULT GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent );

	HRESULT ValidateAgainstParentExtents( SVImageExtentClass& p_rsvExtent );
	HRESULT ValidateAgainstOutputExtents( const SVImageExtentClass& p_rsvExtent );
	HRESULT ValidateAgainstChildrenExtents( SVImageExtentClass& p_rsvExtent );

	HRESULT GetParentImageHandle( SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?
	HRESULT GetParentImageHandle( SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?

	// Need to deal with source and result image buckets
	virtual BOOL GetImageHandleIndex( SVImageIndexStruct& rsvIndex ) const;
	virtual BOOL SetImageHandleIndex( SVImageIndexStruct  svIndex  );

	virtual BOOL CopyImageTo( SVImageIndexStruct svIndex );

	virtual BOOL GetImageHandle( SVSmartHandlePointer& p_rHandlePtr ); //@TODO:  Change method to const?
	virtual BOOL GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle );

	BOOL SafeImageCopyToHandle     ( SVSmartHandlePointer& p_rHandle );
	BOOL SafeImageCopyToHandle     ( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle );
	BOOL SafeImageConvertToHandle  ( SVSmartHandlePointer& p_rHandle, SVImageOperationTypeEnum p_lConversionType );
	BOOL SafeImageConvertToHandle  ( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, SVImageOperationTypeEnum p_lConversionType );

	HRESULT LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent );
	HRESULT LoadImage( LPCTSTR p_szFileName, SVImageIndexStruct p_svToIndex );

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	virtual void Persist( SVObjectWriter& rWriter ) override;
	void PersistImageAttributes( SVObjectWriter& rWriter );

	virtual BOOL OnValidate() override; //@TODO:  Change method to const?

	virtual SVImageIndexStruct GetSourceImageIndex( SVDataManagerHandle* pHandle, const SVGuidSVCameraInfoStructMap& rGuidCameraMap );

	SVImageClass* GetRootImage(); //@TODO:  Change method to const?
	HRESULT TranslateFromOutputSpaceToImage(SVImageClass* p_pImage, SVExtentPointStruct p_InPt, SVExtentPointStruct& p_OutPt); //@TODO:  Change method to const?

	void GetChildExtents( SVChildExtentDeque& p_rChildExtents ) const;
	void SetTranslationOffset(double offsetX, double offsetY);

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;

#pragma region virtual method (ISVImage)
	virtual SVImageTypeEnum GetImageType() const override;
	virtual SvOi::ISVImage* GetParentImageInterface() const override;
	virtual SvOi::MatroxImageSmartHandlePtr getImageData() override;
	virtual SvOi::MatroxImageSmartHandlePtr getParentImageData() override;
	virtual SVString getDisplayedName() const override;
	virtual long getBands() const override;
	virtual long getPixelDepth() const override;
	virtual HRESULT Save(const SVString& rFilename) override; 
	virtual RECT GetOutputRectangle() const override;
#pragma region virtual method (ISVImage)

protected:
	typedef std::pair< SVGUID, SVImageClass* > SVParentObjectPair;

	virtual BOOL DestroyImage();

	//- UpdateFromParentInformation () ---------------------------------------
	//- This function will update the Tool Image to itself.
	HRESULT UpdateFromParentInformation();
	HRESULT UpdateFromToolInformation();

	HRESULT ClearParentConnection();

	virtual HRESULT RemoveObjectConnection( const GUID& p_rObjectID ) override;

	virtual SVImageObjectClassPtr GetBufferArrayPtr() const;

	HRESULT IsValidChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo ) const;
	HRESULT UpdateChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo );
	HRESULT RemoveChild( const GUID& p_rChildID );

	HRESULT UpdatePosition();

	HRESULT UpdateChildren();
	HRESULT RemoveChildren();

	HRESULT UpdateChildBuffers( SVImageObjectClassPtr p_psvChildBuffers, const SVImageInfoClass& p_rImageInfo );

	HRESULT UpdateBufferArrays( bool p_ExcludePositionCheck = false );

	HRESULT GetChildImageInfo( const GUID& p_rChildID, SVImageInfoClass& p_rImageInfo ) const;

	HRESULT GetChildImageHandle( const GUID& p_rChildID, SVSmartHandlePointer& p_rsvBufferHandle ) const;
	HRESULT GetChildImageHandle( const GUID& p_rChildID, SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer& p_rsvBufferHandle ) const;

	virtual HRESULT GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const;

	bool Lock() const;
	bool Unlock() const;

	virtual HRESULT RegisterAsSubObject();
	virtual HRESULT UnregisterAsSubObject();

	HRESULT RebuildStorage( bool p_ExcludePositionCheck );

	mutable SVClock::SVTimeStamp m_LastUpdate;
	mutable SVClock::SVTimeStamp m_LastReset;

	mutable SVParentObjectPair m_ParentImageInfo;
	SVImageTypeEnum m_ImageType;
	SVImageInfoClass m_ImageInfo;
	
	SVImageObjectClassPtr m_BufferArrayPtr;

	SVGuidImageChildMap m_ChildArrays;

private:
	void init();

	mutable bool m_bCriticalSectionCreated;
	mutable CRITICAL_SECTION m_hCriticalSection;
};

typedef SVVector <SVImageClass*, SVImageClass*> SVImageClassArray;

class SVRGBImageClass : public SVImageClass
{
public:
	virtual ~SVRGBImageClass() {}

	//Child Buffers on Color Images do not support DIB
	void setDibBufferFlag(bool flag=false) { m_ImageInfo.setDibBufferFlag(flag); }
};

