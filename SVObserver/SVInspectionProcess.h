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
#include "ObjectInterfaces/IToolSet.h"
#include "Definitions/SVResetStruct.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVContainerLibrary/SVBiUniqueMap.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVDataManagerLibrary/SVDataManagerIndexArrayHandle.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectSubmitCommandFacade.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVStatusLibrary/SVImageIndexStruct.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionEngine/SVTaskObject.h" // For SVImageClassPtrSet
#include "InspectionEngine/SVImageBuffer.h" //SVImageOverlayClass; used for getting overlay data for the ActiveX
#include "SVInfoStructs.h"
#include "SVInspectionProcessResetStruct.h"
#include "SVPublishList.h"
#include "SVCameraImageTemplate.h"
#include "SVVirtualCamera.h"
#include "SVMonitorList.h"
#include "SVValueObjectLibrary/SVValueObjectClass.h"
#include "InspectionEngine/SVEquation.h"
#include "SVSharedMemoryLibrary/MonitorEntry.h"
#include "SVSharedMemoryLibrary/SMRingbuffer.h"
#pragma endregion Includes

#pragma region Declarations
class SVCameraImageTemplate;
class SVConditionalClass;
class SVDisplayObject;
class SVPPQObject;
class SVToolSetClass;
class SVTaskObjectClass;
class SVResultListClass;
#pragma endregion Declarations

#define  BUFFER_IMAGE_FILENAME_LEN 1024

class SVInspectionProcess : 
	public SVObjectClass,
	public SVObjectSubmitCommandFacade,
	public SVObserverTemplate< SVAddTool >,
	public SVObserverTemplate< SVDeleteTool >,
	public SvOi::IInspectionProcess
{
	friend class SVCommandInspectionExtentUpdater; // For access to RunOnce()
	SV_DECLARE_CLASS( SVInspectionProcess );

public:
	typedef SVBiUniqueMap<std::string, SVObjectClass* >::type SVValueObjectMap;
	typedef SVTQueueObject< SVOutputRequestInfoStruct > SVOutputRequestQueue;
	//************************************
	//! FunctionPointer as Argument for LoopOverTool
	//! negative return values indicate an error 
	//************************************
	typedef int  (*pToolFunc) (SVObjectClass *pObject );

	SVInspectionProcess( LPCSTR ObjectName );
	SVInspectionProcess( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINSPECTIONOBJECT );
	virtual ~SVInspectionProcess();

	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	
	virtual void SetObjectDepth( int NewObjectDepth ) override;
	virtual void SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;
	virtual bool SetImageDepth( long lDepth ) override;

	virtual void ResetName() override;
	virtual void SetName( LPCTSTR StrString ) override;

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject ) override;
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject ) override;

	bool Run( SVRunStatusClass& rRunStatus );

	virtual HRESULT ObserverUpdate( const SVAddTool& p_rData ) override;
	virtual HRESULT ObserverUpdate( const SVDeleteTool& p_rData ) override;

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
	virtual SvCl::SelectorItemVectorPtr GetPPQSelectorList( const UINT Attribute ) const override;
	virtual SvOi::ITaskObject* GetToolSetInterface() const override;
	virtual HRESULT RunOnce(SvOi::ITaskObject* pTask) override;
	virtual long GetLastIndex() const  override;
	virtual bool AddInputRequest( const SVGUID& ObjectRef, const _variant_t& rValue, long Index ) override;
	virtual bool AddInputRequestMarker() override;
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

	HRESULT AddInputImageRequest( SVImageClass* p_psvImage, BSTR& p_rbstrValue );
	HRESULT AddInputImageFileNameRequest( SVImageClass* p_psvImage, const std::string& p_rFileName );
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

	virtual long GetEnableAuxiliaryExtent() const override;
	virtual void SetEnableAuxiliaryExtent( long p_lEnableAuxiliaryExtents ) override;

	//new GetOverlay method for use with the ActiveX
	HRESULT CollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray);

	SVProductInfoStruct LastProductGet( SVDataManagerLockTypeEnum p_LockType ) const;
	HRESULT LastProductUpdate( SVProductInfoStruct *p_psvProduct );

	HRESULT LastProductNotify();

	HRESULT GetNextAvailableIndexes( SVInspectionInfoStruct& p_rInspectionInfo, SVDataManagerLockTypeEnum p_LockType ) const;

	void DumpDMInfo( LPCTSTR p_szName ) const;

	SVToolSetClass* GetToolSet() const;

	SVResultListClass* GetResultList() const;

	virtual void SetInvalid() override;

	HRESULT RebuildInspection();
	void ValidateAndInitialize( bool p_Validate, bool p_IsNew );
	void ClearResetCounts();
	void SetResetCounts( );

	SVPublishListClass& GetPublishList();

	LPCTSTR GetDeviceName() const;
	void SetDeviceName( LPCTSTR p_szDeviceName );

	int UpdateMainImagesByProduct( SVProductInfoStruct* p_psvProduct );
	virtual bool IsColorCamera() const override;

	LPCTSTR GetToolsetImage();
	void SetToolsetImage( const std::string& rToolsetImage );

	SVCameraImageTemplate* GetToolSetMainImage();

	SVVirtualCamera* GetFirstCamera() const;
	SVVirtualCamera* GetFirstPPQCamera() const;
	HRESULT GetCameras( SVVirtualCameraPtrSet& p_rCameras ) const;
	HRESULT GetPPQCameras( SVVirtualCameraPtrSet& p_rCameras ) const;
	HRESULT GetCamerasForLut( SVVirtualCameraPtrSet& p_rCameras ) const;
	HRESULT GetCamerasForLightReference( SVVirtualCameraPtrSet& p_rCameras ) const;

	HRESULT AddSharedCamera( SVVirtualCamera* pCamera );

	HRESULT GetMainImages( const std::string& rCameraName, SVCameraImagePtrSet& rMainImages ) const;

	HRESULT RemoveImage(SVImageClass* pImage);

	virtual void Persist(SvOi::IObjectWriter& rWriter) override;

	long GetResultDataIndex() const;

	//************************************
	//! Get all active ppqVariables for the used inspection.
	//! \returns vector of objects
	//************************************
	SVObjectPtrVector getPPQVariables() const;
	
	//************************************
	//! Check if the pValueObject is a active ppqVarable for the used inspection.
	//! \param pValueObject [in]
	//! \returns 
	//************************************
	bool IsEnabledPPQVariable( const SVObjectClass* pObject ) const;
	
	//************************************
	//! Check if the pValueObject is an inactive ppqVarable for the used inspection.
	//! \param pValueObject [in]
	//! \returns 
	//************************************
	bool IsDisabledPPQVariable( const SVObjectClass* pObject ) const;
	
	virtual DWORD GetObjectColor() const override;

	//************************************
	//! calls pf for all object in Inspection 
	//! \param pf [in] function pointer 
	//! \param counter [out] sum over results from pf
	//! \returns bool true if all call from pf are not negativ
	//************************************
	bool   LoopOverTools(pToolFunc pf, int& counter ); 
	
	//************************************
	//! Get the tool messages
	//! \param rInserter [out] reference to an inserter iterator
	//************************************
	void getToolMessages( SvStl::MessageContainerInserter& rInserter ) const;

	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo ) override;

	/// sets the flag that forces the global extent data to update
	void ForceOffsetUpdate() override { m_bForceOffsetUpdate = true; }
	/// will global extent data be forced to update?
	bool IsOffsetUpdateForced() override { return m_bForceOffsetUpdate; }

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

	SvDef::StringVector& getViewedInputNames() { return m_arViewedInputNames; };

	/// Get the controller for the play condition equation
	/// \returns SvOi::IFormulaControllerPtr
	SvOi::IFormulaControllerPtr getRegressionTestPlayConditionController();

	/// Return true if Regression Test should go to pause because of the Play condition.
	/// \returns bool
	bool shouldPauseRegressionTestByCondition();
	 
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

		SvTl::SVTimeStamp m_StartTime;
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

		SvTl::SVTimeStamp m_StartTime;
		SVEventTrackingMap m_EventCounts;
	};
#endif //EnableTracking

	typedef void ( CALLBACK * SVAPCSignalHandler )( DWORD_PTR );
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;

	typedef SVTQueueObject< SVCommandTemplatePtr > SVCommandQueue;
	typedef SVTQueueObject< SVMonitorList > SVMonitorListQueue;
	typedef SVTQueueObject< SVMonitorItemList > SVImageNameDequeQueue;
	typedef SVTQueueObject< SVInputRequestInfoStructPtr > SVInputRequestQueue;
	typedef SVTQueueObject< SVInputImageRequestInfoStructPtr > SVInputImageRequestQueue;
	typedef SVTQueueObject< SVProductInfoStruct > SVProductQueue;
	
	virtual HRESULT SubmitCommand( const SVCommandTemplatePtr& p_rCommandPtr ) override;

	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual SVObjectClass* UpdateObject( const GUID &friendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner ) override;

	bool RunOnce( SVToolClass* p_psvTool = nullptr );
	bool RunInspection( long lResultDataIndex, SVImageIndexStruct svResultImageIndex, SVProductInfoStruct *pProduct, bool p_UpdateCounts = true );

	void DestroyInspection();

	HRESULT InitializeRunOnce();

	HRESULT BuildValueObjectMap();

	HRESULT GetInspectionValueObject( LPCTSTR Name, SVObjectReference& rObjectRef );
	HRESULT GetInspectionImage( LPCTSTR Name, SVImageClass*& p_rRefObject );
	HRESULT GetInspectionObject( LPCTSTR Name, SVObjectReference& rObjectRef );

	bool AddInputRequest( SVInputRequestInfoStructPtr p_pInRequest );

	bool ProcessInputRequests( bool &rForceOffsetUpdate );
	bool ProcessInputRequests( SvOi::SVResetItemEnum& rResetItem, SVStdMapSVToolClassPtrSVInspectionProcessResetStruct &rToolMap );
	bool ProcessInputImageRequests( SVProductInfoStruct *p_psvProduct );

	HRESULT ReserveNextResultImage( SVProductInfoStruct *p_pProduct, SVDataManagerLockTypeEnum p_eLockType, bool p_ClearOtherInspections = false );

	HRESULT LastProductCopySourceImagesTo( SVProductInfoStruct *p_psvProduct );

	HRESULT SetSourceImagesTo( SVProductInfoStruct *p_psvProduct );

	HRESULT RestoreCameraImages();

	template<typename T>
	HRESULT SetObjectArrayValues(SVObjectReference& rObjectRef, const std::string& rValues, bool & reset);

	void SingleRunModeLoop( bool p_Refresh = false );

	HRESULT CopyForward( SVRunStatusClass& rRunStatus );

	HRESULT CreateResultImageIndexManager() const;

	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	void ThreadProcess( bool& p_WaitForEvents );

	HRESULT ProcessInspection( bool& p_rProcessed, SVProductInfoStruct& p_rProduct );
	HRESULT ProcessNotifyWithLastInspected(bool& p_rProcessed, SVProductInfoStruct& rProduct);
	HRESULT ProcessCommandQueue( bool& p_rProcessed );
	
	///True if product i a reject
	bool   CopyToWatchlist(SVProductInfoStruct& LastProduct);
	
	void  BuildWatchlist();
	
	SVGUID m_PPQId;

	// DataManager index handles
	mutable SVSmartIndexArrayHandlePtr m_pResultImageCircleBuffer;

	mutable SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;
	long m_NotifyWithLastInspected;

	volatile long m_lInputRequestMarkerCount;
	SVInputRequestQueue m_InputRequests;
	SVInputImageRequestQueue m_InputImageRequests;

	SVCameraImagePtrSet m_CameraImages;

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
	SVImageClassPtrSet m_ImageObjectSet;

	std::string m_ToolSetCameraName;
	std::string m_DeviceName;

#ifdef EnableTracking
	SVInspectionTracking m_InspectionTracking;
#endif

private:
	void Init();

	HRESULT FindPPQInputObjectByName( SVObjectClass*& p_rpObject, LPCTSTR p_FullName ) const;

	SVCriticalSectionPtr m_LastRunLockPtr;
	bool m_LastRunProductNULL;
	SVProductInfoStruct m_svLastRunProduct;

	bool                m_bNewDisableMethod;
	long                m_lEnableAuxiliaryExtents;

	DWORD               m_dwThreadId;

	// JMS - this variable is only used for configuration conversion.
	SVConditionalClass* m_pToolSetConditional;
	SVCommandQueue m_CommandQueue;
	

	std::vector<WatchlistelementPtr>  m_WatchListImages;
	std::vector<WatchlistelementPtr> m_WatchListDatas;
	
	int m_StoreIndex; 
	SvSml::RingBufferPointer m_SlotManager;

	SvDef::StringVector m_arViewedInputNames;

	//For RegressionTest
	SVEquationClass m_RegressionTestPlayEquation;
	SvOi::IFormulaControllerPtr m_pRegressionTestPlayEquationController;
};

typedef std::vector<SVInspectionProcess*> SVInspectionProcessVector;

namespace SVDetail
{
	template<typename T>
	struct ValueObjectTraits
	{
		typedef T value_type;
		typedef SVValueObjectClass<T> object_type;
		inline static bool validate(const std::string &rString) { return true; }
	};

	template<>
	struct ValueObjectTraits<CFile>
	{
		typedef std::string value_type;
		typedef SVValueObjectClass<std::string> object_type;
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
	ASSERT(rObjectRef.getObject()->IsCreated() && rObjectRef.getObject()->IsValid() && rObjectRef.isArray() && rObjectRef.isEntireArray());
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

