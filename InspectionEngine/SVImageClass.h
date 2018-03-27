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
//Moved to precompiled header: #include <vector>
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVStatusLibrary/SVImageIndexStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageChildStruct.h"
#include "SVImageObjectClass.h"
#include "SVObjectAppClass.h"
#include "SVCameraInfoStruct.h"
#pragma endregion Includes

class SVImageClass : public SVObjectAppClass, public SvOi::ISVImage
{
	SV_DECLARE_CLASS( SVImageClass );

public:
	typedef std::pair< SVGUID, SVImageExtentClass > SVChildExtentPair;
	typedef std::deque< SVChildExtentPair > SVChildExtentDeque;

	SVImageClass( LPCSTR ObjectName );
	SVImageClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGE );
	virtual ~SVImageClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	SVImageClass* GetParentImage() const;
	const SVGUID& GetParentImageID() const;
	const SVImageInfoClass& GetImageInfo() const; //@TODO: Change the logic so that this is never needed outside this class

	HRESULT InitializeImage( SvDef::SVImageTypeEnum ImageType );
	HRESULT InitializeImage( SVImageClass* pParentImage );

	//@WARNING [Jim][8 July 2015] - Look into and evaluate this behavior.
	//-  This UpdateImage function calls RebuildStorage() which calls 
	//-  UpdateFromParentInformation() which overwrites the extents I just 
	//-  sent in anyhow????
	HRESULT UpdateImage( const SVImageExtentClass& rExtent, bool ExcludePositionCheck = false );
	HRESULT UpdateImage( const SVGUID& rParentID, const SVImageInfoClass& rImageInfo );
	HRESULT UpdateImage( SvDef::SVImageTypeEnum ImageType );

	virtual bool SetImageDepth( long lDepth );

	virtual const SvTl::SVTimeStamp& GetLastResetTimeStamp() const;
	
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	HRESULT RebuildStorage( bool p_ExcludePositionCheck, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	SVImageExtentClass GetImageExtents() const;
	HRESULT GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent );

	HRESULT ValidateAgainstParentExtents( SVImageExtentClass& rExtent );
	HRESULT ValidateAgainstOutputExtents( const SVImageExtentClass& rExtent );
	HRESULT ValidateAgainstChildrenExtents( SVImageExtentClass& rExtent );

	// Need to deal with source and result image buckets
	virtual bool GetImageHandleIndex( SVImageIndexStruct& rsvIndex ) const;
	virtual bool SetImageHandleIndex( SVImageIndexStruct  svIndex  );

	virtual bool CopyImageTo( SVImageIndexStruct svIndex );

	virtual bool GetImageHandle(SvOi::SVImageBufferHandlePtr& p_rHandlePtr ) const;
	virtual bool GetImageHandle( SVImageIndexStruct svIndex, SvOi::SVImageBufferHandlePtr& rHandle ) const;

	bool SafeImageCopyToHandle     (SvOi::SVImageBufferHandlePtr& p_rHandle );
	bool SafeImageCopyToHandle     ( SVImageIndexStruct p_svFromIndex, SvOi::SVImageBufferHandlePtr& p_rHandle );

	HRESULT LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent );
	HRESULT LoadImage( LPCTSTR p_szFileName, SVImageIndexStruct p_svToIndex );

	virtual HRESULT GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	virtual void Persist( SvOi::IObjectWriter& rWriter ) override;
	void PersistImageAttributes( SvOi::IObjectWriter& rWriter );

	virtual SVImageIndexStruct GetSourceImageIndex( SVDataManagerHandle* pHandle, const SVGuidSVCameraInfoStructMap& rGuidCameraMap );

	SVImageClass* GetRootImage(); //@TODO:  Change method to const?
	HRESULT TranslateFromOutputSpaceToImage(SVImageClass* p_pImage, SVExtentPointStruct p_InPt, SVExtentPointStruct& p_OutPt); //@TODO:  Change method to const?

	void GetChildExtents( SVChildExtentDeque& p_rChildExtents ) const;
	void SetTranslationOffset(double offsetX, double offsetY);

#pragma region virtual method (ISVImage)
	virtual SvDef::SVImageTypeEnum GetImageType() const override;
	virtual SvOi::ISVImage* GetParentImageInterface() const override;
	virtual SvOi::SVImageBufferHandlePtr getImageData() const override;
	virtual SvOi::SVImageBufferHandlePtr getParentImageData() const override;
	virtual std::string getDisplayedName() const override;
	virtual long getPixelDepth() const override;
	virtual HRESULT Save(const std::string& rFilename) override; 
	virtual RECT GetOutputRectangle() const override;
#pragma region virtual method (ISVImage)

protected:
	typedef std::pair< SVGUID, SVImageClass* > SVParentObjectPair;

	virtual bool DestroyImage();

	//- UpdateFromParentInformation () ---------------------------------------
	//- This function will update the Tool Image to itself.
	HRESULT UpdateFromParentInformation();
	HRESULT UpdateFromToolInformation();

	HRESULT ClearParentConnection();

	virtual HRESULT RemoveObjectConnection( const GUID& rObjectID ) override;

	virtual SVImageObjectClassPtr GetBufferArrayPtr() const;

	HRESULT IsValidChild( const SVGUID& rChildID, const SVImageInfoClass& rImageInfo ) const;
	HRESULT UpdateChild( const SVGUID& rChildID, const SVImageInfoClass& rImageInfo );
	HRESULT RemoveChild( const SVGUID& rChildID );

	HRESULT UpdatePosition();

	HRESULT UpdateChildren();
	HRESULT RemoveChildren();

	HRESULT UpdateChildBuffers( SVImageObjectClassPtr pChildBuffers, const SVImageInfoClass& rImageInfo );

	HRESULT UpdateBufferArrays( bool p_ExcludePositionCheck = false, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	HRESULT GetChildImageHandle( const SVGUID& p_rChildID, SvOi::SVImageBufferHandlePtr& p_rsvBufferHandle ) const;
	HRESULT GetChildImageHandle( const SVGUID& p_rChildID, SVImageIndexStruct p_svBufferIndex, SvOi::SVImageBufferHandlePtr& p_rsvBufferHandle ) const;

	virtual HRESULT GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const;

	bool Lock() const;
	bool Unlock() const;

	virtual HRESULT RegisterAsSubObject();
	virtual HRESULT UnregisterAsSubObject();

	virtual bool ValidateImage();

	void setImageSubType();
private:
	HRESULT GetParentImageHandle(SvOi::SVImageBufferHandlePtr& rBufferHandle) const; 
	HRESULT GetParentImageHandle(SVImageIndexStruct BufferIndex, SvOi::SVImageBufferHandlePtr& rBufferHandle) const;
	void init();

protected:
	mutable SvTl::SVTimeStamp m_LastUpdate;
	mutable SvTl::SVTimeStamp m_LastReset;

	SvDef::SVImageTypeEnum m_ImageType;
	SVImageInfoClass m_ImageInfo;
	
	SVGuidImageChildMap m_ChildArrays;

private:
	SVImageObjectClassPtr m_BufferArrayPtr;
	mutable SVParentObjectPair m_ParentImageInfo;

	mutable bool m_bCriticalSectionCreated;
	mutable CRITICAL_SECTION m_hCriticalSection;
};

typedef std::set<SVImageClass*> SVImageClassPtrSet;
typedef std::vector<SVImageClass*> SVImageClassPtrVector;

