//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/ITRCImage.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVObjectLibrary/SVObjectAppClass.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateChildStruct.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes

class SVObjectClass;


namespace SvIe
{

#define S_NoParent                                ((HRESULT)80000L)


class SVImageClass : public SvOl::SVObjectAppClass, public SvOi::ISVImage
{
	SV_DECLARE_CLASS

public:

	enum class BufferType
	{
		Undefined,
		TRCBuffer,
		TRCChildImage,
		LocalBuffer,
		LocalChildBuffer,
	};

	explicit SVImageClass(LPCSTR ObjectName);
	SVImageClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGE );
	virtual ~SVImageClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	SVImageClass* GetParentImage() const;
	const SVImageInfoClass& GetImageInfo() const; //@TODO: Change the logic so that this is never needed outside this class

	HRESULT InitializeImage( SvPb::SVImageTypeEnum ImageType );
	
	//Set the parent and Update Image with Parentinfos
	HRESULT InitializeImage( SVImageClass* pParentImage );

	//@WARNING [Jim][8 July 2015] - Look into and evaluate this behavior.
	//-  This UpdateImage function calls RebuildStorage() which calls 
	//-  UpdateFromParentInformation() which overwrites the extents I just 
	//-  sent in anyhow????
	HRESULT UpdateImage( const SVImageExtentClass& rExtent, bool doNotRebuildStorage = false );
	//Set the parent And ImageInfoClass
	HRESULT UpdateImageSetParentAndImageInfo(uint32_t parentID, const SVImageInfoClass& rImageInfo);
	HRESULT UpdateImageType( SvPb::SVImageTypeEnum ImageType );

	virtual const double& GetLastResetTimeStamp() const;
	
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	HRESULT RebuildStorage( SvStl::MessageContainerVector *pErrorMessages=nullptr );

	const SVImageExtentClass& GetImageExtents() const;
	HRESULT GetImageExtentsToFit(SVImageExtentClass inExtent, SVImageExtentClass& rOutExtent);

	HRESULT ValidateAgainstParentExtents( const SVImageExtentClass& rExtent );
	HRESULT ValidateAgainstOutputExtents( const SVImageExtentClass& rExtent );

	bool SafeImageCopyToHandle     ( SvOi::SVImageBufferHandlePtr& p_rHandle, const SvOi::ITriggerRecordRPtr pTriggerRecord);

	HRESULT LoadImage( LPCTSTR p_szFileName, const SvOi::ITriggerRecordRWPtr& pTriggerRecord);

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	virtual void Persist( SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;
	void PersistImageAttributes( SvOi::IObjectWriter& rWriter ) const;

	const SVImageClass* const GetRootImage() const;
	HRESULT TranslateFromOutputSpaceToImage(SVImageClass* pImage, SVPoint<double> inPoint, SVPoint<double>& rOutPoint) const;
	
	//Same functionality as TranslateFromOutputSpaceToImage but uses only the extents connected to the tool 
	HRESULT TranslateFromOutputSpaceToImageFromTool(SVImageClass* pImage, SVPoint<double> inPoint, SVPoint<double>& rOutPoint) const;

	void SetTranslationOffset(double offsetX, double offsetY);

	/// Set an buffer to this image to a triggerRecord.
	/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
	/// \param pImage [in] The buffer.
	/// \param pTriggerRecord [in] The triggerRecord.
	void setImage(SvOi::ITRCImagePtr pImage, const SvOi::ITriggerRecordRWPtr& pTriggerRecord);

	/// Set the buffer to this image of the triggerRecord to invalid.
	/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
	/// \param pTriggerRecord [in] The triggerRecord.
	void resetImage(const SvOi::ITriggerRecordRWPtr& pTriggerRecord);

	/// Get a new image buffer which is not connected to this object. The buffer is blocked as long as the caller hold the handle.
	/// \returns SvOi::ITRCImagePtr
	SvOi::ITRCImagePtr getTempImageBuffer(bool createBufferExternIfNecessary = true) const;

	/// Return an pointer to a image instance (only in read only modus).
	/// \param pTriggerRecord [in] Pointer to the TriggerRecord from where the image is set.
	/// \param lockImage (default:false) [int] If true image will locked by is own and unlock if ITRCImagePtr freed. If false image will not separated lock, but by the TriggerRecord (ATTENTION: In this case, use this instance only as long as this TriggerRecord-instance existing.)
	/// \returns SvOi::ITRCImagePtr
	SvOi::ITRCImagePtr getImageReadOnly(const SvOi::ITriggerRecordR* pTriggerRecord, bool lockImage = false) const;
	SvOi::ITRCImagePtr getImageToWrite(const SvOi::ITriggerRecordRWPtr& pTriggerRecord);

	void getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const;

	virtual void setEditModeFreezeFlag(bool flag) override;
	virtual void goingOffline() override;

	virtual void copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr) override;

	BufferType getBufferType() const { return m_BufferType; };

	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;
	virtual HRESULT SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject) override;

	int getImagePosInTRC() const { return m_imagePosInTRC; };

	void setTransfermatrix(const std::vector<double>& rMatrix);

	std::string getInfoString(bool addDetails=false);

	static const std::vector<double> UnitMatrix;
#pragma region virtual method (ISVImage)
	virtual SvPb::SVImageTypeEnum GetImageType() const override;
	virtual SvOi::ISVImage* GetParentImageInterface() const override;
	virtual SvOi::SVImageBufferHandlePtr getLastImage(bool newIfNotAvailable = false) const override;
	virtual std::string getDisplayedName() const override;
	virtual long getPixelDepth() const override;
	virtual HRESULT Save(const std::string& rFilename) override; 
	virtual RECT GetOutputRectangle() const override;
	virtual void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const override;
	virtual SvPb::OverlayDesc getOverlayStruct() const override;
#pragma region virtual method (ISVImage)

protected:
	typedef std::pair< uint32_t, SVImageClass* > SVParentObjectPair;

	virtual bool DestroyImage();

	//- UpdateFromParentInformation () ---------------------------------------
	//- This function will update the Tool Image to itself.
	HRESULT UpdateFromParentInformation(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	HRESULT UpdateFromToolInformation();

	HRESULT ClearParentConnection();

	virtual HRESULT RemoveObjectConnection(uint32_t objectID ) override;

	HRESULT UpdateChild(uint32_t childID, const SVImageInfoClass& rImageInfo, bool reporterror);
	HRESULT RemoveChild(uint32_t childID );

	HRESULT UpdatePosition(bool reporterror );

	HRESULT UpdateChildren(bool reporterror);
	HRESULT RemoveChildren();

	void setImageSubType();
	bool hasStorage() const;
	uint32_t getInspectionPosInTRC() const { return m_inspectionPosInTRC; };

private:
	void init();

	void setInspectionPosForTrc();
	bool mustBeInTRC() const;
	bool UpdateBuffers(SvStl::MessageContainerVector* pErrorMessages);
	bool UpdateTRCBuffers(SvStl::MessageContainerVector *pErrorMessages, bool shouldHidden);
	bool UpdateLocalBuffer(SvStl::MessageContainerVector* pErrorMessages);
	void copyCurrent2SaveImage();

protected:
	mutable double m_LastUpdate {std::numeric_limits<double>::max()};
	mutable double m_LastReset {std::numeric_limits<double>::lowest()};

	SvPb::SVImageTypeEnum m_ImageType;
	SVImageInfoClass m_ImageInfo;
	
	typedef std::unordered_map< uint32_t, SVImageInfoClass > ImageIdInfoMap;
	ImageIdInfoMap m_ChildArrays;

private:
	int m_inspectionPosInTRC = -1; //inspection position in TriggerRecordController
	int m_imagePosInTRC = -1;
	BufferType m_BufferType {BufferType::Undefined};
	MatroxBufferChildDataStruct m_BufferStructForLocalChild;

	mutable SVParentObjectPair m_ParentImageInfo;
	SvVol::SVDoubleValueObjectClass m_width;
	SvVol::SVDoubleValueObjectClass m_height;
	SvOi::SVImageBufferHandlePtr m_localBuffer; //only set and used if m_BufferType == LocalBuffer
	SvOi::SVImageBufferHandlePtr m_savedBuffer;
	SVImageInfoClass m_ImageInfoOfSavedBuffer;

	mutable std::recursive_mutex m_imageMutex;

	bool m_removeChildCircularReference = false;
};

SVImageClass* castObjectToImage(SvOi::IObjectClass* pObject);

typedef std::set<SVImageClass*> SVImageClassPtrSet;
typedef std::vector<SVImageClass*> SVImageClassPtrVector;

} //namespace SvIe
