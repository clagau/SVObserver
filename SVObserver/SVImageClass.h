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
#pragma endregion Includes

class SVExtentClass;
class SVIPDoc;
class SVImageOverlayClass;
class SVToolClass;
class SVVirtualCamera;

struct SVProductInfoStruct;

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

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct );
	virtual BOOL CloseObject();

	virtual SVImageClass* GetParentImage() const;
	virtual const GUID& GetParentImageID() const;
	virtual const SVImageInfoClass& GetImageInfo() const; //@TODO: Change the logic so that this is never needed outside this class

	virtual HRESULT InitializeImage( SVImageTypeEnum p_ImageType );
	virtual HRESULT InitializeImage( SVImageClass* p_pParentImage );
	virtual HRESULT InitializeImage( const GUID& p_rParentID );
	virtual HRESULT InitializeImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );

	//@WARNING [Jim][8 July 2015] - Look into and evaluate this behavior.
	//-  This UpdateImage function calls RebuildStorage() which calls 
	//-  UpdateFromParentInformation() which overwrites the extents I just 
	//-  sent in anyhow????
	virtual HRESULT UpdateImage( const SVImageExtentClass& p_rExtent, bool p_ExcludePositionCheck = false );
	virtual HRESULT UpdateImage( const SVImageInfoClass& p_rImageInfo );
	virtual HRESULT UpdateImage( const GUID& p_rParentID );
	virtual HRESULT UpdateImage( const GUID& p_rParentID, const SVImageExtentClass& p_rExtent );
	virtual HRESULT UpdateImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const SVImageInfoClass& p_rImageInfo );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID );
	virtual HRESULT UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo );

	virtual BOOL SetImageDepth( long lDepth );

	virtual const SVClock::SVTimeStamp& GetLastResetTimeStamp() const;
	
	virtual HRESULT ResetObject();

	virtual SVImageExtentClass GetImageExtents();
	virtual HRESULT GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent );

	virtual HRESULT ValidateAgainstParentExtents( SVImageExtentClass& p_rsvExtent );
	virtual HRESULT ValidateAgainstOutputExtents( const SVImageExtentClass& p_rsvExtent );
	virtual HRESULT ValidateAgainstChildrenExtents( SVImageExtentClass& p_rsvExtent );

	virtual HRESULT GetParentImageHandle( SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?
	virtual HRESULT GetParentImageHandle( SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?

	// Need to deal with source and result image buckets
	virtual BOOL GetImageHandleIndex( SVImageIndexStruct& rsvIndex ) const;
	virtual BOOL SetImageHandleIndex( SVImageIndexStruct  svIndex  );

	virtual BOOL CopyImageTo( SVImageIndexStruct svIndex );

	virtual BOOL GetImageHandle( SVSmartHandlePointer& p_rHandlePtr ); //@TODO:  Change method to const?
	virtual BOOL GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle );

	virtual BOOL SafeImageCopyToHandle     ( SVSmartHandlePointer& p_rHandle );
	virtual BOOL SafeImageCopyToHandle     ( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle );
	virtual BOOL SafeImageConvertToHandle  ( SVSmartHandlePointer& p_rHandle, SVImageOperationTypeEnum p_lConversionType );
	virtual BOOL SafeImageConvertToHandle  ( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, SVImageOperationTypeEnum p_lConversionType );

	virtual HRESULT LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent );
	virtual HRESULT LoadImage( LPCTSTR p_szFileName, SVImageIndexStruct p_svToIndex );

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual void Persist( SVObjectWriter& rWriter );
	virtual void PersistImageAttributes( SVObjectWriter& rWriter );

	virtual BOOL OnValidate(); //@TODO:  Change method to const?

	virtual SVImageIndexStruct GetSourceImageIndex( SVProductInfoStruct* pProduct );

	SVImageClass* GetRootImage(); //@TODO:  Change method to const?
	HRESULT TranslateFromOutputSpaceToImage(SVImageClass* p_pImage, SVExtentPointStruct p_InPt, SVExtentPointStruct& p_OutPt); //@TODO:  Change method to const?

	void GetChildExtents( SVChildExtentDeque& p_rChildExtents ) const;

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
	virtual HRESULT UpdateFromParentInformation();
	virtual HRESULT UpdateFromToolInformation();

	virtual HRESULT ClearParentConnection();

	virtual HRESULT RemoveObjectConnection( const GUID& p_rObjectID );

	virtual SVImageObjectClassPtr GetBufferArrayPtr() const;

	virtual HRESULT IsValidChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo ) const;
	virtual HRESULT UpdateChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo );
	virtual HRESULT RemoveChild( const GUID& p_rChildID );

	virtual HRESULT UpdatePosition();

	virtual HRESULT UpdateChildren();
	virtual HRESULT RemoveChildren();

	virtual HRESULT UpdateChildBuffers( SVImageObjectClassPtr p_psvChildBuffers, const SVImageInfoClass& p_rImageInfo );

	virtual HRESULT UpdateBufferArrays( bool p_ExcludePositionCheck = false );

	virtual HRESULT GetChildImageInfo( const GUID& p_rChildID, SVImageInfoClass& p_rImageInfo ) const;

	virtual HRESULT GetChildImageHandle( const GUID& p_rChildID, SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?
	virtual HRESULT GetChildImageHandle( const GUID& p_rChildID, SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer& p_rsvBufferHandle ); //@TODO:  Change method to const?

	virtual HRESULT GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const;

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	virtual bool Lock() const;
	virtual bool Unlock() const;

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

typedef std::set<SVImageClass *> SVImageClassPtrSet;

class SVRGBImageClass : public SVImageClass
{
public:
	virtual ~SVRGBImageClass() {}

	//Child Buffers on Color Images do not support DIB
	void setDibBufferFlag(bool flag=false) { m_ImageInfo.setDibBufferFlag(flag); }
};

