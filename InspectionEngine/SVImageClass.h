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
#include "SVObjectAppClass.h"
#include "TriggerRecordController/IImage.h"
#include "TriggerRecordController/ITriggerRecordRW.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes

class SVObjectClass;
class SVOutputInfoListClass;

namespace SvIe
{

#define S_NoParent                                ((HRESULT)80000L)

class SVImageClass : public SVObjectAppClass, public SvOi::ISVImage
{
	SV_DECLARE_CLASS( SVImageClass );

public:

	SVImageClass( LPCSTR ObjectName );
	SVImageClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGE );
	virtual ~SVImageClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	SVImageClass* GetParentImage() const;
	const SVImageInfoClass& GetImageInfo() const; //@TODO: Change the logic so that this is never needed outside this class

	HRESULT InitializeImage( SvDef::SVImageTypeEnum ImageType );
	HRESULT InitializeImage( SVImageClass* pParentImage );

	//@WARNING [Jim][8 July 2015] - Look into and evaluate this behavior.
	//-  This UpdateImage function calls RebuildStorage() which calls 
	//-  UpdateFromParentInformation() which overwrites the extents I just 
	//-  sent in anyhow????
	HRESULT UpdateImage( const SVImageExtentClass& rExtent, bool doNotRebuildStorage = false );
	HRESULT UpdateImage( const SVGUID& rParentID, const SVImageInfoClass& rImageInfo );
	HRESULT UpdateImage( SvDef::SVImageTypeEnum ImageType );

	virtual const double& GetLastResetTimeStamp() const;
	
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	HRESULT RebuildStorage( SvStl::MessageContainerVector *pErrorMessages=nullptr );

	const SVImageExtentClass& GetImageExtents() const;
	HRESULT GetImageExtentsToFit(SVImageExtentClass inExtent, SVImageExtentClass& rOutExtent);

	HRESULT ValidateAgainstParentExtents( const SVImageExtentClass& rExtent );
	HRESULT ValidateAgainstOutputExtents( const SVImageExtentClass& rExtent );

	bool SafeImageCopyToHandle     ( SvOi::SVImageBufferHandlePtr& p_rHandle, const SvTrc::ITriggerRecordRPtr pTriggerRecord);

	HRESULT LoadImage( LPCTSTR p_szFileName, const SvTrc::ITriggerRecordRWPtr& pTriggerRecord);

	virtual HRESULT GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	virtual void Persist( SvOi::IObjectWriter& rWriter ) override;
	void PersistImageAttributes( SvOi::IObjectWriter& rWriter );

	const SVImageClass* const GetRootImage() const;
	HRESULT TranslateFromOutputSpaceToImage(SVImageClass* pImage, SVPoint<double> inPoint, SVPoint<double>& rOutPoint) const;

	void SetTranslationOffset(double offsetX, double offsetY);

	/// Set an buffer to this image to a triggerRecord.
	/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
	/// \param pImage [in] The buffer.
	/// \param pTriggerRecord [in] The triggerRecord.
	void setImage(SvTrc::IImagePtr pImage, const SvTrc::ITriggerRecordRWPtr& pTriggerRecord);

	/// Set the buffer to this image of the triggerRecord to invalid.
	/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
	/// \param pTriggerRecord [in] The triggerRecord.
	void resetImage(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord);
	SvTrc::IImagePtr getLastImage(bool newIfNotAvailable = false) const;

	/// Get a new image buffer which is not connected to this object. The buffer is blocked as long as the caller hold the handle.
	/// \returns SvTrc::IImagePtr
	SvTrc::IImagePtr getTempImageBuffer(bool createBufferExternIfNecessary = true) const;

	/// Return an pointer to a image instance (only in read only modus).
	/// \param pTriggerRecord [in] Pointer to the TriggerRecord from where the image is set.
	/// \param lockImage (default:false) [int] If true image will locked by is own and unlock if IImagePtr freed. If false image will not separated lock, but by the TriggerRecord (ATTENTION: In this case, use this instance only as long as this TriggerRecord-instance existing.)
	/// \returns SvTrc::IImagePtr
	SvTrc::IImagePtr getImageReadOnly(const SvTrc::ITriggerRecordR* pTriggerRecord, bool lockImage = false) const;
	SvTrc::IImagePtr getImageToWrite(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord);

	void GetOutputList(SVOutputInfoListClass& p_rOutputInfoList);

	virtual HRESULT GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0) const override;

	bool isChildImageInTRC() const { return m_isChildImageInTRC; };

#pragma region virtual method (ISVImage)
	virtual SvDef::SVImageTypeEnum GetImageType() const override;
	virtual SvOi::ISVImage* GetParentImageInterface() const override;
	virtual SvOi::SVImageBufferHandlePtr getImageData() const override;
	virtual std::string getDisplayedName() const override;
	virtual long getPixelDepth() const override;
	virtual HRESULT Save(const std::string& rFilename) override; 
	virtual RECT GetOutputRectangle() const override;
	virtual SvPb::OverlayDesc getOverlayStruct() const override;
#pragma region virtual method (ISVImage)

protected:
	typedef std::pair< SVGUID, SVImageClass* > SVParentObjectPair;

	virtual bool DestroyImage();

	//- UpdateFromParentInformation () ---------------------------------------
	//- This function will update the Tool Image to itself.
	HRESULT UpdateFromParentInformation(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	HRESULT UpdateFromToolInformation();

	HRESULT ClearParentConnection();

	virtual HRESULT RemoveObjectConnection( const GUID& rObjectID ) override;

	HRESULT UpdateChild( const SVGUID& rChildID, const SVImageInfoClass& rImageInfo);
	HRESULT RemoveChild( const SVGUID& rChildID );

	HRESULT UpdatePosition( );

	HRESULT UpdateChildren();
	HRESULT RemoveChildren();

	bool Lock() const;
	bool Unlock() const;

	void setImageSubType();
private:
	void init();

	void setInspectionPosForTrc();
	bool UpdateTRCBuffers(SvStl::MessageContainerVector *pErrorMessages);

protected:
	mutable double m_LastUpdate;
	mutable double m_LastReset;

	SvDef::SVImageTypeEnum m_ImageType;
	SVImageInfoClass m_ImageInfo;
	
	typedef std::map< SVGUID, SVImageInfoClass > SVGuidImageChildMap; 
	SVGuidImageChildMap m_ChildArrays;

private:
	int m_inspectionPosInTRC = -1; //inspection position in TriggerRecordController
	int m_imagePosInTRC = -1;
	bool m_isChildImageInTRC = false;
	mutable SVParentObjectPair m_ParentImageInfo;
	SvVol::SVDoubleValueObjectClass m_width;
	SvVol::SVDoubleValueObjectClass m_height;

	mutable bool m_bCriticalSectionCreated;
	mutable CRITICAL_SECTION m_hCriticalSection;
	SVObjectPtrVector m_embeddedList;
};

typedef std::set<SVImageClass*> SVImageClassPtrSet;
typedef std::vector<SVImageClass*> SVImageClassPtrVector;

} //namespace SvIe
