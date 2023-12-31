//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcess
//* .File Name       : $Workfile:   SVInspectionProcess.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.14  $
//* .Check In Date   : $Date:   12 Feb 2015 02:58:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/SVResetStruct.h"
#include "InspectionEngine/RunStatus.h"
#include "InspectionEngine/SVTaskObject.h" // For SVImageClassPtrSet
#include "SVInfoStructs.h"
#include "InspectionEngine/SVCameraImageTemplate.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVEquation.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/MonitorEntry.h"
#include "SVValueObjectLibrary/SVValueObjectClass.h"
#include "SVDisplayObject.h"
#pragma endregion Includes

#pragma region Declarations

namespace SvIe
{
class SVCameraImageTemplate;
}
namespace SvOp
{
class SVConditional;
}
class SVPPQObject;
class SVToolSet;
class SVResultList;
#pragma endregion Declarations

#define  BUFFER_IMAGE_FILENAME_LEN 1024


class SVInspectionProcess : 
	public SVObjectClass,
	public SvOi::IInspectionProcess
{
	SV_DECLARE_CLASS

public:
	using InspectionProcessFunction = std::function<void()>;
	typedef std::unordered_map<std::string, SVObjectClass*> SVValueObjectMap;

	enum InspectionFunction
	{
		Inspection,
		NotifyLastInspected,
		ProcessCommand,
		Size
	};

	explicit SVInspectionProcess( LPCSTR ObjectName );
	SVInspectionProcess( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINSPECTIONOBJECT );
	virtual ~SVInspectionProcess();

	virtual bool resetAllObjects(SvStl::MessageContainerVector* pErrorMessages = nullptr, int nResetDepth = 0) override;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	
	virtual void ResetName() override;
	virtual void SetName( LPCTSTR StrString ) override;

	bool CreateObject( );
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject ) override;
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject ) override;

	bool Run( SvIe::RunStatus& rRunStatus );

	void SetPPQIdentifier(uint32_t PPQId);
	uint32_t GetPPQIdentifier() const;
	SVPPQObject* GetPPQ() const;

	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;

#pragma region virtual method (IInspectionProcess)
	virtual SvOi::IObjectClass* GetPPQInterface() const override;
	virtual void SetDefaultInputs() override;
	virtual void fillPPQSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, const UINT attribute, SvPb::ObjectSelectorType type) const override;
	virtual void fillCameraSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, const UINT attribute, SvPb::ObjectSelectorType type) const override;
	virtual SvOi::ITaskObject* GetToolSetInterface() const override;
	virtual HRESULT RunOnce() override;
	virtual HRESULT SubmitCommand(const SvOi::ICommandPtr& rCommandPtr) override;
	uint32_t getFirstCamera() const override;
	HRESULT addSharedCamera(uint32_t cameraID) override;
	HRESULT resetTool(SvOi::IObjectClass& rTool) override;
	HRESULT resetToolAndDependends(SvOi::IObjectClass* pTool) ;
	virtual HRESULT propagateSizeAndPosition() override;
	virtual bool usePropagateSizeAndPosition() const override;
	virtual SvPb::OverlayDesc getOverlayStruct(const SvOi::ISVImage& rImage) const override;
	virtual void setResultListUpdateFlag() override;
	virtual SvOi::IObjectClass* GetIObjectClassPtr()  override;
#pragma endregion virtual method (IInspectionProcess)

	bool IsCameraInInspection( const std::string& rCameraName ) const;

	///Checks whether the reset state is set
	bool IsResetStateSet( unsigned long p_State ) const;
	void AddResetState( unsigned long p_State );
	void RemoveResetState( unsigned long p_State );

	bool CreateInspection( LPCTSTR szDocName );

	// Runtime Functions
	bool CanGoOnline();
	bool CanRegressionGoOnline();
	bool GoOnline();
	bool GoOffline();

	///If images or inputs are missing, CanProcess will return  false, 
	bool CanProcess( SVProductInfoStruct *pProduct, CantProcessEnum& rReason );
	
	HRESULT StartProcess( SVProductInfoStruct *pProduct );

	void RebuildInspectionInputList(const SVIOEntryHostStructPtrVector& rUsedInputs);
	
	void RemoveCamera( const std::string& rCameraName );
	
	bool AddInputRequest( const SVObjectReference& rObjectRef, const _variant_t& rValue );
	bool AddInputRequest(SVInputRequestInfoStructPtr pInRequest);
	bool AddInputRequestMarker();
	bool AddInputRequestResetAllObject();

	HRESULT AddInputImageRequest(SvIe::SVImageClass* p_psvImage, BSTR& p_rbstrValue );
	HRESULT AddInputImageFileNameRequest(SvIe::SVImageClass* p_psvImage, const std::string& p_rFileName );
	HRESULT AddInputImageRequest( SVInputImageRequestInfoStructPtr p_InRequestPtr );
	HRESULT AddInputImageRequestByCameraName( const std::string& rCameraName, const std::string& rFileName);
	void AddInputImageRequestToTool(const std::string& rName, uint32_t toolId, const std::string& rFileName);

	void DisconnectToolSetMainImage();
	void ConnectToolSetMainImage();

	/// is the new disable method set?
	bool IsNewDisableMethodSet() const override;
	void SetNewDisableMethod( bool bNewDisableMethod ); 

	virtual bool getEnableAuxiliaryExtent() const override;
	virtual void setEnableAuxiliaryExtent(bool Enabled) override;

	DWORD GetObjectIdIndex() const;
	void SetObjectIdIndex(DWORD objectIdIndex);

	//new GetOverlay method for use with the ActiveX
	HRESULT getOverlays(SvIe::SVImageClass& rImage, SVExtentMultiLineStructVector& rMultiLineArray);

	std::pair<SvTrig::SVTriggerInfoStruct, SVInspectionInfoStruct> getLastProductData() const;
	void resetLastProduct();

	void LastProductNotify();

	SVToolSet* GetToolSet() const;

	SVResultList* GetResultList() const;

	HRESULT RebuildInspection(bool shouldCreateAllObject = true);
	void ValidateAndInitialize( bool p_Validate );
	virtual void resetCounterDirectly() override;
	void resetCounterSynchronous();
	void requestResetAllObjects();

	LPCTSTR GetDeviceName() const;
	void SetDeviceName( LPCTSTR p_szDeviceName );

	/// Update the main image of this product.
	/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
	void UpdateMainImagesByProduct(SVInspectionInfoStruct& rIpInfoStruct, const SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos);
	virtual bool IsColorCamera() const override;

	LPCTSTR GetToolsetImage();
	void SetToolsetImage( const std::string& rToolsetImage );

	SvIe::SVCameraImageTemplate* GetToolSetMainImage();

	SvOi::IObjectClass* getFirstPPQCamera() const;
	SvIe::SVVirtualCameraPtrVector GetCameras() const;
	SvIe::SVVirtualCameraPtrVector GetCamerasForLut() const;
	SvIe::SVVirtualCameraPtrVector GetCamerasForLightReference() const;

	HRESULT GetMainImages( const std::string& rCameraName, SvIe::SVCameraImagePtrSet& rMainImages ) const;

	HRESULT RemoveImage(SvIe::SVImageClass* pImage);

	virtual void Persist(SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;

	//************************************
	//! Get all active ppqVariables for the used inspection.
	//! \returns vector of objects
	//************************************
	SVObjectPtrVector getPPQVariables() const;
	
	virtual DWORD GetObjectColor() const override;

	//************************************
	//! Get the tool messages
	//! \param rInserter [out] reference to an inserter iterator
	//************************************
	void getToolMessages( SvStl::MessageContainerInserter& rInserter ) const;

	/// sets the flag that forces the global extent data to update
	void ForceOffsetUpdate() override { m_bForceOffsetUpdate = true; }
	/// will global extent data be forced to update?
	bool IsOffsetUpdateForced() override { return m_bForceOffsetUpdate; }

	bool getInitialAuxiliaryExtents() {return m_initialAuxiliaryExtents;}

	void buildValueObjectData();

	void setIOObjectIdMap(std::map<std::string, uint32_t>&& ioObjectMap);

	void attachObserver(SVDisplayObject* pDisplayObject);
	void detachObserver(SVDisplayObject* pDisplayObject);
	template< typename SVDataType >
	void updateObserver(const SVDataType& rData) 
	{
		for (auto* pObject : m_observerList)
		{
			if (pObject)
			{
				pObject->ObserverUpdate(rData);
			}
		}
	};

#pragma region Methods to replace processMessage
	virtual bool createAllObjects();
	virtual bool CreateChildObject( SVObjectClass* pChildObject, DWORD context = 0 ) override;
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	bool DestroyChildObject(SVObjectClass* pChildcontext);
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
	virtual bool connectAllInputs() override;
	virtual void disconnectAllInputs() override;
	virtual bool replaceObject(SVObjectClass* pObject, uint32_t newId) override;
#pragma endregion Methods to replace processMessage

	/// Get the controller for the play condition equation
	/// \returns SvOi::IFormulaControllerPtr
	SvOi::IFormulaControllerPtr getRegressionTestPlayConditionController();

	/// Return true if Regression Test should go to pause because of the Play condition.
	/// \returns bool
	bool shouldPauseRegressionTestByCondition();

	void setTrcPos(int pos) { m_trcPos = pos; };
	int getTrcPos() const { return m_trcPos; };

	bool isProcessActive() const { return m_processActive; };

protected:
	
	struct WatchListElement 
	{
		WatchListElement(const SVObjectReference& object, const SvSml::MonitorEntryPointer& mEntryPtr)
			: ObjRef(object)
			, MonEntryPtr(mEntryPtr)
		{
		}
		SVObjectReference  ObjRef;
		SvSml::MonitorEntryPointer MonEntryPtr;
	};
	
	typedef   std::unique_ptr<WatchListElement> WatchlistelementPtr;
	typedef SVTQueueObject<SvOi::ICommandPtr> SVCommandQueue;
	typedef SVTQueueObject<SVInputRequestInfoStructPtr> SVInputRequestQueue;
	typedef SVTQueueObject<SVInputImageRequestInfoStructPtr> SVInputImageRequestQueue;
	typedef SVTQueueObject<SVProductInfoStruct> SVProductQueue;

	bool RunInspection(SVInspectionInfoStruct& rIPInfo, const SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos, long triggerCount, bool p_UpdateCounts = true );

	void DestroyInspection();

	HRESULT InitializeRunOnce();

	bool ProcessInputRequests( bool &rForceOffsetUpdate );
	bool ProcessInputRequests( SvOi::SVResetItemEnum& rResetItem );
	bool ProcessInputImageRequests(SVInspectionInfoStruct& rIpInfoStruct, const SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos);

	HRESULT LastProductCopySourceImagesTo( SVProductInfoStruct *p_psvProduct );

	template<typename T>
	HRESULT SetObjectArrayValues(SVObjectReference& rObjectRef, const std::string& rValues, bool & reset);

	void SingleRunModeLoop( bool p_Refresh = false );

	HRESULT copyValues2TriggerRecord(SvIe::RunStatus& rRunStatus);

	static void __stdcall ProcessCallback(ULONG_PTR pCaller);

	HRESULT ProcessInspection();
	HRESULT ProcessNotifyWithLastInspected();
	HRESULT ProcessCommandQueue();
	
	///True if product is a reject
	bool isReject(SvOi::ITriggerRecordRPtr pTriggerRecord);
	
	void BuildWatchlist();

private:
	void Init();

	HRESULT FindPPQInputObjectByName(SVObjectClass*& p_rpObject, LPCTSTR p_FullName) const;

	void LastProductUpdate(const SVProductInfoStruct& rProduct);
	SVProductInfoStruct LastProductGet() const;

	SvOi::SVResetItemEnum  AddToResetIds(SvOi::SVResetItemEnum eResetItem, SVObjectClass* pObject, std::back_insert_iterator<std::vector<uint32_t>>  ResetIdIt);
	
	std::vector<std::shared_ptr<SvOi::IValueObject>> m_InputObjects;

	bool m_bForceOffsetUpdate{true}; // Force Global Extent data to update

	uint32_t m_PPQId{SvDef::InvalidObjectId};

	mutable SvSyl::SVThread m_processThread;
	std::atomic_bool m_NotifyWithLastInspected;

	std::atomic<long>  m_lInputRequestMarkerCount{0L};
	SVInputRequestQueue m_InputRequests;
	SVInputImageRequestQueue m_InputImageRequests;

	SvIe::SVCameraImagePtrSet m_CameraImages;

	// Run status of last tool set run, if any...
	SvIe::RunStatus m_runStatus;

	// Inspection pointers
	SVToolSet* m_pCurrentToolset{nullptr};

	SvDef::SVResetStruct m_svReset;

	SvOi::IValueObjectPtrSet m_ValueObjectSet;
	
	std::string m_ToolSetCameraName;
	std::string m_DeviceName;

	SvIe::SVVirtualCamera* m_pToolSetCamera{nullptr};

	mutable std::mutex m_inspectionMutex;
	bool m_lastRunProductValid{ true };
	SVProductInfoStruct m_lastRunProduct;
	std::map<std::string, uint32_t> m_ioObjectMap;

	bool m_bNewDisableMethod{false};
	bool m_initialAuxiliaryExtents {false}; //This is only required to be able to read old configuration files with extents set

	// JMS - this variable is only used for configuration conversion.
	SvOp::SVConditional* m_pToolSetConditional{nullptr};
	SVCommandQueue m_CommandQueue;
	
	std::vector<SvSml::MonitorEntryPointer> m_WatchListDatas;
	
	int m_StoreIndex{-1}; 

	//For RegressionTest
	SvOp::SVEquation m_RegressionTestPlayEquation;
	SvOi::IFormulaControllerPtr m_pRegressionTestPlayEquationController;
	///TRC and Memory block data
	int m_trcPos = -1;
	long m_memValueDataOffset{0L};			///Is the current memory data offset 
#ifdef TRACE_MEMORYBLOCK
	long m_memValueDataOffsetPrev {0L};
#endif 
	SvOi::IValueObjectPtrSet m_updateValueObjectSet; //The value objects which need updating
	std::vector<uint8_t> m_valueData;		///The memory block used to store all value objects of the inspection
	std::atomic_bool m_resetting{false};

	std::atomic_bool m_offlineRequest{ false };
	std::atomic_bool m_processActive{ false };
	SVProductInfoStruct m_product{};
	std::array <InspectionProcessFunction, InspectionFunction::Size> m_processFunctions;
	std::vector<SVDisplayObject*> m_observerList;
};

typedef std::vector<SVInspectionProcess*> SVInspectionProcessVector;

namespace SVDetail
{
	template<typename T>
	struct ValueObjectTraits
	{
		typedef T value_type;
		typedef SvVol::SVValueObjectClass<T> object_type;
		inline static bool validate(const std::string&) { return true; }
	};

	template<>
	struct ValueObjectTraits<CFile>
	{
		typedef std::string value_type;
		typedef SvVol::SVValueObjectClass<std::string> object_type;
		inline static bool validate(const std::string& rString)
		{
			CFileStatus rStatus;
			return CFile::GetStatus( rString.c_str(), rStatus ) != 0 && 0L <= rStatus.m_size;
		}
	};

	template<typename T>
	inline T str2int(const std::string& rString)
	{
		int base = (rString[0] == '0' && toupper(rString[1]) == 'X')?16:10;
		return static_cast<T>(_tcstol(rString.c_str(), nullptr, base));
	}

	template<>
	inline std::string str2int(const std::string& rString)
	{
		int base = (rString[0] == '0' && toupper(rString[1]) == 'X')?16:10;
		std::string Result = std::format( _T("{}"), _tcstol(rString.c_str(), nullptr, base) );
		return Result;
	}

	template<typename T>
	inline T str2(const std::string& rString)
	{
		if (boost::is_integral<T>::value)
		{
			return str2int<T>(rString);
		}
		else
		{
			return boost::lexical_cast<T>(rString);
		}
	}

	using Validator = std::function<bool (const std::string& rString)>;
}

template<typename T>
inline HRESULT Parse(std::vector<T> & vec, const std::string& rValues, SVDetail::Validator validate)
{
	const TCHAR * escape = _T("\\");
	const TCHAR * separator = _T(",");
	const TCHAR * quote = _T("`\'");
	if( rValues.empty())
	{
		return S_FALSE;
	}
	typedef boost::escaped_list_separator<TCHAR> Sep;
	typedef boost::tokenizer<Sep, std::string::const_iterator, std::string> Tokenizer;
	Sep sep(escape, separator, quote);
	Tokenizer tok(rValues, sep);
	try
	{
		for (Tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
		{
			if (validate(*it))
			{
				vec.push_back(SVDetail::str2<T>(*it));
			}
			else
			{
				return E_INVALIDARG;
			}
		}
	}
	catch(std::exception & )
	{
		return E_FAIL;
	}
	return S_OK;
}

template<typename T>
inline HRESULT SVInspectionProcess::SetObjectArrayValues(SVObjectReference& rObjectRef, const std::string& rValues, bool & reset)
{
	ASSERT(rObjectRef.getObject()->IsCreated() && rObjectRef.isArray() && rObjectRef.isEntireArray());
	typedef typename SVDetail::ValueObjectTraits<T> Traits;
	typedef typename SVDetail::ValueObjectTraits<T>::value_type Type;
	typedef typename SVDetail::ValueObjectTraits<T>::object_type ObjectType;
	std::vector<Type> vec;
	ObjectType* pObjectType = dynamic_cast<ObjectType*> (rObjectRef.getObject());
	if( nullptr != pObjectType )
	{
		HRESULT hr = Parse(vec, rValues, Traits::validate);
		if (S_OK == hr)
		{
			hr = pObjectType->SetArrayValues(vec);
		}
		reset = S_OK == hr;
		return hr;
	}
	else
	{
		return TYPE_E_TYPEMISMATCH;
	}
}

