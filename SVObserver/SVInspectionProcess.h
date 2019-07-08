//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/tokenizer.hpp>
//Moved to precompiled header: #include <boost/lexical_cast.hpp>
//Moved to precompiled header: #include <boost/type_traits.hpp>

#include "ObjectInterfaces/IInspectionProcess.h"
#include "Definitions/SVResetStruct.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "SVContainerLibrary/SVBiUniqueMap.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionEngine/SVTaskObject.h" // For SVImageClassPtrSet
#include "InspectionEngine/SVImageBuffer.h" //SVImageOverlayClass; used for getting overlay data for the ActiveX
#include "SVInfoStructs.h"
#include "SVPublishList.h"
#include "InspectionEngine/SVCameraImageTemplate.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "SVMonitorList.h"
#include "SVValueObjectLibrary/SVValueObjectClass.h"
#include "Operators/SVEquation.h"
#include "SVSharedMemoryLibrary/MonitorEntry.h"
#include "SVSharedMemoryLibrary/SMRingbuffer.h"
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

#pragma region Declarations

namespace SvIe
{
class SVCameraImageTemplate;
}
namespace SvOp
{
class SVConditionalClass;
}
class SVPPQObject;
class SVToolSetClass;
class SVResultListClass;
#pragma endregion Declarations

#define  BUFFER_IMAGE_FILENAME_LEN 1024

class SVInspectionProcess : 
	public SVObjectClass,
	public SvOi::IInspectionProcess
{
	SV_DECLARE_CLASS( SVInspectionProcess );

public:
	typedef SVBiUniqueMap<std::string, SVObjectClass* >::type SVValueObjectMap;
	typedef SVTQueueObject< SVOutputRequestInfoStruct > SVOutputRequestQueue;

	SVInspectionProcess( LPCSTR ObjectName );
	SVInspectionProcess( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINSPECTIONOBJECT );
	virtual ~SVInspectionProcess();

	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	
	virtual void ResetName() override;
	virtual void SetName( LPCTSTR StrString ) override;

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject ) override;
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject ) override;

	bool Run( SVRunStatusClass& rRunStatus );

	void SetPPQIdentifier( const SVGUID& p_rPPQId );
	const SVGUID& GetPPQIdentifier() const;
	SVPPQObject* GetPPQ() const;

	///Set sharedPointer for m_SlotManager
	void SetSlotmanager(const SvSml::RingBufferPointer& Slotmanager);
	///gets the shared Pointer in m_SlotManager
	SvSml::RingBufferPointer GetSlotmanager();

#pragma region virtual method (IInspectionProcess)
	virtual SvOi::IObjectClass* GetPPQInterface() const override;
	virtual void SetDefaultInputs() override;
	virtual std::vector<SvPb::TreeItem> GetPPQSelectorList(const UINT attribute) const override;
	virtual SvOi::ITaskObject* GetToolSetInterface() const override;
	virtual HRESULT RunOnce() override;
	virtual HRESULT SubmitCommand(const SvOi::ICommandPtr& rCommandPtr) override;
	virtual void BuildValueObjectMap() override;
	GUID getFirstCamera() const override;
	HRESULT addSharedCamera(GUID cameraID) override;
	HRESULT resetTool(SvOi::IObjectClass& rTool) override;
	virtual HRESULT propagateSizeAndPosition() override;
	virtual bool usePropagateSizeAndPosition() const override;
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

	bool CanProcess( SVProductInfoStruct *pProduct );
	HRESULT StartProcess( SVProductInfoStruct *pProduct );

	bool RebuildInspectionInputList();
	
	void RemoveCamera( const std::string& rCameraName );
	
	bool AddInputRequest( const SVObjectReference& rObjectRef, const _variant_t& rValue );
	bool AddInputRequest(SVInputRequestInfoStructPtr pInRequest);
	bool AddInputRequestMarker();

	HRESULT AddInputImageRequest(SvIe::SVImageClass* p_psvImage, BSTR& p_rbstrValue );
	HRESULT AddInputImageFileNameRequest(SvIe::SVImageClass* p_psvImage, const std::string& p_rFileName );
	HRESULT AddInputImageRequest( SVInputImageRequestInfoStructPtr p_InRequestPtr );
	HRESULT AddInputImageRequestByCameraName( const std::string& rCameraName, const std::string& rFileName);

	//************************************
	//! Checks if the configuration has conditional history attributes and resets them as they are deprecated
	//! \returns true if conditional history attributes are set
	//************************************
	bool CheckAndResetConditionalHistory();

	void DisconnectToolSetMainImage();
	void ConnectToolSetMainImage();

	/// is the new disable method set?
	bool IsNewDisableMethodSet() override;
	void SetNewDisableMethod( bool bNewDisableMethod ); 

	virtual bool getEnableAuxiliaryExtent() const override;
	virtual void setEnableAuxiliaryExtent(bool Enabled) override;

	//new GetOverlay method for use with the ActiveX
	HRESULT CollectOverlays(SvIe::SVImageClass* pImage, SVExtentMultiLineStructVector& rMultiLineArray);

	std::pair<SvTi::SVTriggerInfoStruct, SVInspectionInfoStruct> getLastProductData() const;
	void resetLastProcduct();

	HRESULT LastProductNotify();

	SVToolSetClass* GetToolSet() const;

	SVResultListClass* GetResultList() const;

	HRESULT RebuildInspection(bool shouldCreateAllObject = true);
	void ValidateAndInitialize( bool p_Validate );
	void ClearResetCounts();
	void SetResetCounts( );

	SVPublishListClass& GetPublishList();

	LPCTSTR GetDeviceName() const;
	void SetDeviceName( LPCTSTR p_szDeviceName );

	/// Update the main image of this product.
	/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
	void UpdateMainImagesByProduct(SVInspectionInfoStruct& rIpInfoStruct, SvIe::SVGuidSVCameraInfoStructMap& rCameraInfos);
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

	virtual void Persist(SvOi::IObjectWriter& rWriter) override;

	//************************************
	//! Get all active ppqVariables for the used inspection.
	//! \returns vector of objects
	//************************************
	SVObjectPtrVector getPPQVariables() const;
	
	//************************************
	//! Check if the pValueObject is an inactive ppqVarable for the used inspection.
	//! \param pValueObject [in]
	//! \returns 
	//************************************
	bool IsDisabledPPQVariable( const SVObjectClass* pObject ) const;
	
	virtual DWORD GetObjectColor() const override;

	//************************************
	//! Get the tool messages
	//! \param rInserter [out] reference to an inserter iterator
	//************************************
	void getToolMessages( SvStl::MessageContainerInserter& rInserter ) const;

	virtual bool DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo) override;

	/// sets the flag that forces the global extent data to update
	void ForceOffsetUpdate() override { m_bForceOffsetUpdate = true; }
	/// will global extent data be forced to update?
	bool IsOffsetUpdateForced() override { return m_bForceOffsetUpdate; }

	bool getInitialAuxiliaryExtents() {return m_initialAuxiliaryExtents;}

	void buildValueObjectDefList() const;

#pragma region Methods to replace processMessage
	virtual bool createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CreateChildObject( SVObjectClass* pChildObject, DWORD context = 0 ) override;
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	bool DestroyChildObject(SVObjectClass* pChildcontext);
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
	virtual bool ConnectAllInputs() override;
	virtual bool replaceObject(SVObjectClass* pObject, const GUID& rNewGuid) override;
#pragma endregion Methods to replace processMessage

	SVIOEntryStructVector m_PPQInputs;

	bool m_bForceOffsetUpdate; // Force Global Extent data to update

	/// Get the controller for the play condition equation
	/// \returns SvOi::IFormulaControllerPtr
	SvOi::IFormulaControllerPtr getRegressionTestPlayConditionController();

	/// Return true if Regression Test should go to pause because of the Play condition.
	/// \returns bool
	bool shouldPauseRegressionTestByCondition();


	/// Set Numbers of TriggerRecord for this inspection. ATTENTIONS: In error case this method throw an exception.
	/// \param newSize [in] The new size of records.
	/// \param rejectCount [in] The new max count of rejects.
	void setTriggerRecordNumbers(long newRecordSize, long rejectCount);

protected:
	
	struct WatchListElement 
	{
		WatchListElement(SVObjectReference& object, const SvSml::MonitorEntryPointer& mEntryPtr)
		{
			ObjRef = object;
			MonEntryPtr = mEntryPtr;
		}
		SVObjectReference  ObjRef;
		SvSml::MonitorEntryPointer MonEntryPtr;
	};
	
	typedef   std::unique_ptr<WatchListElement> WatchlistelementPtr;

#ifdef EnableTracking
	struct SVInspectionTrackingElement
	{
		typedef std::map< unsigned __int64, long > SVTimeCountMap;

		SVInspectionTrackingElement();
		SVInspectionTrackingElement( const SVInspectionTrackingElement& p_rObject );

		virtual ~SVInspectionTrackingElement();

		void clear();

		double m_StartTime;
		SVTimeCountMap m_Start;
		SVTimeCountMap m_End;
		SVTimeCountMap m_Duration;
	};

	struct SVInspectionTracking
	{
		typedef std::map<std::string, SVInspectionTrackingElement> SVEventTrackingMap;

		SVInspectionTracking();
		SVInspectionTracking( const SVInspectionTracking& p_rObject );

		virtual ~SVInspectionTracking();

		void clear();

		void SetStartTime();

		void EventStart( const std::string& p_rName );
		void EventEnd( const std::string& p_rName );

		double m_StartTime;
		SVEventTrackingMap m_EventCounts;
	};
#endif //EnableTracking

	typedef void ( CALLBACK * SVAPCSignalHandler )( DWORD_PTR );
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;

	typedef SVTQueueObject<SvOi::ICommandPtr> SVCommandQueue;
	typedef SVTQueueObject<SVMonitorList> SVMonitorListQueue;
	typedef SVTQueueObject<SVMonitorItemList> SVImageNameDequeQueue;
	typedef SVTQueueObject<SVInputRequestInfoStructPtr> SVInputRequestQueue;
	typedef SVTQueueObject<SVInputImageRequestInfoStructPtr> SVInputImageRequestQueue;
	typedef SVTQueueObject<SVProductInfoStruct> SVProductQueue;
	
	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual SVObjectClass* UpdateObject( const GUID &friendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner ) override;

	bool RunInspection(SVInspectionInfoStruct& rIPInfo, SvIe::SVGuidSVCameraInfoStructMap& rCameraInfos, long triggerCount, bool p_UpdateCounts = true );

	void DestroyInspection();

	HRESULT InitializeRunOnce();

	HRESULT GetInspectionValueObject( LPCTSTR Name, SVObjectReference& rObjectRef );
	HRESULT GetInspectionImage( LPCTSTR Name, SvIe::SVImageClass*& p_rRefObject );
	HRESULT GetInspectionObject( LPCTSTR Name, SVObjectReference& rObjectRef );

	bool ProcessInputRequests( bool &rForceOffsetUpdate );
	bool ProcessInputRequests( SvOi::SVResetItemEnum& rResetItem );
	bool ProcessInputImageRequests(SVInspectionInfoStruct& rIpInfoStruct, SvIe::SVGuidSVCameraInfoStructMap& rCameraInfos);

	HRESULT LastProductCopySourceImagesTo( SVProductInfoStruct *p_psvProduct );

	template<typename T>
	HRESULT SetObjectArrayValues(SVObjectReference& rObjectRef, const std::string& rValues, bool & reset);

	void SingleRunModeLoop( bool p_Refresh = false );

	HRESULT copyValues2TriggerRecord(SVRunStatusClass& rRunStatus) const;

	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	void ThreadProcess( bool& p_WaitForEvents );

	HRESULT ProcessInspection( bool& p_rProcessed, SVProductInfoStruct& p_rProduct );
	HRESULT ProcessNotifyWithLastInspected(bool& p_rProcessed, SVProductInfoStruct& p_rProduct);
	HRESULT ProcessCommandQueue( bool& p_rProcessed );
	
	///True if product is a reject
	bool isReject();
	
	void BuildWatchlist();

	SVGUID m_PPQId;

	mutable SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;
	long m_NotifyWithLastInspected;

	volatile long m_lInputRequestMarkerCount;
	SVInputRequestQueue m_InputRequests;
	SVInputImageRequestQueue m_InputImageRequests;

	SvIe::SVCameraImagePtrSet m_CameraImages;

	// Published List
	SVPublishListClass m_publishList;

	// Run status of last tool set run, if any...
	SVRunStatusClass m_runStatus;

	// Inspection pointers
	SVToolSetClass* m_pCurrentToolset;

	volatile bool m_bInspecting;

	// Inspection Queue
	SVProductQueue       m_qInspectionsQueue;

	// Map of All Value Objects
	SVValueObjectMap m_mapValueObjects;

	SvDef::SVResetStruct m_svReset;

	SvOi::IValueObjectPtrSet m_ValueObjectSet;
	
	std::string m_ToolSetCameraName;
	std::string m_DeviceName;

	SvIe::SVVirtualCamera* m_pToolSetCamera{nullptr};

#ifdef EnableTracking
	SVInspectionTracking m_InspectionTracking;
#endif

private:
	void Init();

	std::vector<_variant_t> copyValueObjectList(bool determineSize=false) const;

	HRESULT FindPPQInputObjectByName( SVObjectClass*& p_rpObject, LPCTSTR p_FullName ) const;

	HRESULT LastProductUpdate(SVProductInfoStruct *p_psvProduct);
	SVProductInfoStruct LastProductGet() const;
	
	mutable std::mutex m_LastRunMutex;
	bool m_LastRunProductNULL;
	SVProductInfoStruct m_svLastRunProduct;

	bool m_bNewDisableMethod;
	bool m_initialAuxiliaryExtents {false}; //This is only required to be able to read old configuration files with extents set

	DWORD               m_dwThreadId;

	// JMS - this variable is only used for configuration conversion.
	SvOp::SVConditionalClass* m_pToolSetConditional;
	SVCommandQueue m_CommandQueue;
	
	std::vector<WatchlistelementPtr> m_WatchListDatas;
	
	int m_StoreIndex; 
	SvSml::RingBufferPointer m_SlotManager;

	//For RegressionTest
	SvOp::SVEquationClass m_RegressionTestPlayEquation;
	SvOi::IFormulaControllerPtr m_pRegressionTestPlayEquationController;
};

typedef std::vector<SVInspectionProcess*> SVInspectionProcessVector;

namespace SVDetail
{
	template<typename T>
	struct ValueObjectTraits
	{
		typedef T value_type;
		typedef SvVol::SVValueObjectClass<T> object_type;
		inline static bool validate(const std::string &rString) { return true; }
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
		std::string Result = SvUl::Format( _T("%d"), _tcstol(rString.c_str(), nullptr, base) );
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

	typedef boost::function<bool (const std::string& rString)> Validator;
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

