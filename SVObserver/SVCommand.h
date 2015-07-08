//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommand.h
//* .File Name       : $Workfile:   SVCommand.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Feb 2014 08:05:20  $
//******************************************************************************

#ifndef SVCOMMAND_H
#define SVCOMMAND_H

#include "SVContainerLibrary/SVVector.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVActiveXLockStruct.h"	// Added by ClassView
#include "SVInfoStructs.h"

// BRW - SVImageCompression has been deprecated.
#ifndef _WIN64
class SVImageCompressionClass;
#endif
class SVValueObjectClass;
class SVInspectionProcess;

enum SVIMCOMMANDSRV_VERSION
{
	SIAC_VERSION_UNKNOWN,
	SIAC_VERSION_01_00,
	SIAC_VERSION_01_10,
	SIAC_VERSION_01_20,
	SIAC_VERSION_02_00
};// end enum

struct StreamDataStruct
{
    SVValueObjectClass *pValueObject;
	SVGUID m_InspectionID;
    CString strValueName;
	long arrayIndex;

	StreamDataStruct()
	: pValueObject( NULL ), m_InspectionID(), strValueName(), arrayIndex( 0 ) {}
};

struct PacketDataStruct
{
    SVValueObjectClass *pValueObject;
	CString strValue;
	long lState;

	PacketDataStruct()
	{
		pValueObject = NULL;
		strValue.Empty();
		lState = PRODUCT_NOT_INSPECTED;
	}// end ctor
};

struct ProductDataStruct
{
    long lProductCount;
	long lCallbackCount;
	SVVector< PacketDataStruct > arPacketData;

	ProductDataStruct()
	{
		lProductCount	= -1;
		lCallbackCount	= 0;
		arPacketData.RemoveAll();
	}// end ctor
};

#include "SVObserverCP.h"
#define SERVER_COMPRESSION_POOL_SIZE 5

/**
@SVObjectName COM Command Server

@SVObjectOverview This object process the COM commands that are sent from the SIAC software.  This object services the ability the get state information about the application, load configurations, save configurations, get parameter and image data, set parameter and image data, and create or modify OCR font files.

@SVObjectOperations This object presents operations to perform the following functions: Get Application State, Load Configuration, Save Configuration, Get Parameter Data, Get Image Data, Set Parameter Data, Set Image Data, Stream Parameter Data, Collect Condition Image and Parameter Data, and Create and Modify OCR Font Files.

*/
class ATL_NO_VTABLE CSVCommand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSVCommand, &CLSID_SVCommand>,
	public IDispatchImpl<ISVCommand, &IID_ISVCommand, &LIBID_SVObserver>,
	public IConnectionPointContainerImpl<CSVCommand>,
	public CProxy_ISVCommandEvents< CSVCommand >,
	public CProxy_ISVCommandObserverEvents< CSVCommand >
{
public:
	CSVCommand();
	
DECLARE_REGISTRY_RESOURCEID(IDR_SVCOMMAND)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSVCommand)
	COM_INTERFACE_ENTRY(ISVCommand)
	COM_INTERFACE_ENTRY2(IDispatch, ISVCommand)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CSVCommand)
	CONNECTION_POINT_ENTRY(IID__ISVCommandEvents)
	CONNECTION_POINT_ENTRY(IID__ISVCommandObserverEvents)
END_CONNECTION_POINT_MAP()

// ISVCommand
public:
	//Registers a callback function to be used by the remote 
	//command processor to deliver commands and parameters to 
	//SVObserver.
	//
	//Parameters:
//	STDMETHOD (RegisterCallback)();

	//Saves the configuration file supplied and initiates 
	//loading it into SVObserver.
	//
	//Parameters:
	//
	//XMLConfig - A BSTR containing the configuration file as 
	//well-formed XML.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (PutSVIMConfig)(BSTR XMLConfig, BSTR* pXMLError);

	//Retrieves the name of the currently loaded 
	//configuration from SVObserver.
	//
	//Parameters:
	//
	//pszConfigFileName - Pointer to a BSTR that will receive 
	//the fully qualified name of the configuration.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMConfigName)(BSTR * pszXMLData, BSTR* pXMLError);

	//Retreive a file from the SVIM.
	//
	//Parameters:
	//
	//szSourceFile - a BSTR containing the name of the file 
	//to retreive from the SVIM.
	//
	//pszXMLDestFile - Pointer to a BSTR that will receive 
	//the file data as well-formed XML
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMFile)(BSTR * pszXMLData, BSTR* pXMLError);

	//Saves a file on the SVIM.
	//
	//Parameters:
	//
	//szXMLSourceFile - a BSTR containing the file to save in 
	//well-formed XML format.
	//
	//szDestFile - A BSTR containing the name of the file 
	//that is to be saved.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (PutSVIMFile)(BSTR szXMLData, BSTR* pXMLError);

	//Sets the SVIM state to online of offline
	//
	//Parameters:
	//
	//bSVIMState - boolean that indicates if the SVIM is to 
	//be set Online (TRUE) or Offline (FALSE).
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SetSVIMState)(BSTR szXMLData, BSTR* pXMLError);

	//Retrieves the currently loaded configuration from 
	//SVObserver.
	//
	//Parameters:
	//
	//pXMLConfig - Pointer to a BSTR that will receive the 
	//configuration from SVObserver in well-formed XML format.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMConfig)(BSTR* pXMLConfig, BSTR* pXMLError);

	//Causes SVObserver to load the specified configuration.
	//
	//Parameters:
	//
	//szConfigFileName - a BSTR containing the name of the 
	//configuration to load.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (LoadSVIMConfig)(BSTR szXMLData, BSTR* pXMLError);

	//Retrieve the current inspection results from SVObserver
	//
	//Parameters:
	//
	//pXMLInspectionResults - Pointer to a BSTR that will 
	//receive the inspection results as well-formed XML.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMInspectionResults)(BSTR* pszXMLData, BSTR* pXMLError);

	//Retrieves the current operating state of the SVIM.
	//
	//Parameters:
	//
	//pXMLState - Pointer to a BSTR variable where the SVIM 
	//state will be stored as a well formed XML document.
	//
	//pXMLError - Pointer to a BSTR variable where the error 
	//information will be stored as a well formed XML 
	//document.
	//
	//Return Values:
	//
	//S_OK  if the function was successfully called. If 
	//the function returns S_OK, the pscSVIMError value 
	//can be checked to determine if the function 
	//executed successfully.
	//
	//Other - see COM documentation.
	STDMETHOD(GetSVIMState)(/*[out]*/ BSTR *pXMLState, /*[out]*/ BSTR *pXMLError);

//  The following group of functions are the same as above but they
//  do NOT use XML to wrap each function call. These functions also
//  break large files up into small blocks to decrease memory use.

	//Unpacks the configuration file supplied from PutSVIMFile and initiates loading it into SVObserver.
	//
	//Parameters:
	//
	//bstrConfigFilename - A BSTR containing the filename of the configuration file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVPutSVIMConfig)(long lOffset, long lBlockSize, BSTR *bstrFileData, BOOL bLastFlag);

	//Retrieves the name of the currently loaded configuration from SVObserver.
	//
	//Parameters:
	//
	//bstrConfigFilename - Pointer to a BSTR that will receive the fully qualified name of the configuration.
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - a BSTR that contains the file data block
	//
	//bLastFlag - BOOLEAN to determine last block of file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMConfigName)(BSTR *bstrConfigFilename);

	//Retrieve a file from the SVIM.
	//
	//Parameters:
	//
	//bstrSourceFile - a BSTR containing the name of the file to retreive from the SVIM.
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - Pointer to a BSTR that will receive the file data block
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMFile)(BSTR bstrSourceFile, long lOffset, long *lBlockSize, BSTR *bstrFileData,long *lLastPacketFlag);

	//Saves a file on the SVIM.
	//
	//Parameters:
	//
	//bstrDestFile - a BSTR containing the name of the file to write to the SVIM.
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - a BSTR that contains the file data block
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVPutSVIMFile)(BSTR bstrDestFile, long lOffset, long lBlockSize, BSTR *bstrFileData);

	//Sets the SVIM state to online of offline
	//
	//Parameters:
	//
	//bSVIMState - boolean that indicates if the SVIM is to be set Online (TRUE) or Offline (FALSE).
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVSetSVIMState)(unsigned long ulSVIMState);

	//Packs the configuration into the configuration file supplied to be read with GetSVIMFile
	//
	//Parameters:
	//
	//bstrConfigFilename - A BSTR containing the filename of the configuration file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMConfig)(long lOffset, long *lBlockSize, BSTR *bstrFileData, BOOL *bLastFlag);

	//Causes SVObserver to load the specified configuration.
	//
	//Parameters:
	//
	//bstrConfigFilename - A BSTR containing the filename of the configuration file
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - a BSTR that contains the file data block
	//
	//bLastFlag - BOOLEAN to determine last block of file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVLoadSVIMConfig)(BSTR bstrConfigFilename);

	//Retrieve the current inspection results from SVObserver
	//
	//Parameters:
	//
	//bstrInspection - BSTR that contains the name of the inpection document to get results from.
	//
	//bstrXMLResults - Pointer to a BSTR that will receive the inspection results as well-formed XML.
	//
	//bstrXMLError - Pointer to a BSTR that will receive error information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMInspectionResults)(BSTR bstrInspection, BSTR *bstrXMLResults);

	//Retrieves the current operating state of the SVIM.
	//
	//Parameters:
	//
	//ulState - Pointer to a unsigned long where the SVIM state will be stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMState)(unsigned long *ulState);

	//Sets Svim mode.
	//
	//Parameters:
	//
	//lSVIMMode - unsigned long has the SVIM mode to be set.
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVSetSVIMMode)(unsigned long lSVIMMode);

	//Gets Svim mode.
	//
	//Parameters:
	//
	//p_plSVIMMode - Pointer to an unsigned long where the new SVIM mode is stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHODIMP SVGetSVIMMode(unsigned long* p_plSVIMMode);

	//Retrieves the current server version of the COM subsystem of SVObserver
	//
	//Parameters:
	//
	//ulVersion - Pointer to an unsigned long where the server version value will be stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMVersion)(unsigned long *ulVersion);

	//Retrieves the current number of times that SVObserver has gone offline
	//Remote users can use this to determine if the SVIM has potentially been modified
	//Then they can call SVGetSVIMConfigPrint and compare the configuration to a known one
	//
	//Parameters:
	//
	//ulVersion - Pointer to an unsigned long where the offline count will be stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMOfflineCount)(unsigned long *ulOfflineCount);

	//Retrieves the current configuration print data for the loaded configuration
	//
	//Parameters:
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrConfigPrint - Pointer to a BSTR where the config print data will be stored
	//
	//bLastFlag - BOOLEAN to determine last block of file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMConfigPrint)(long lOffset, long *lBlockSize, BSTR *bstrConfigPrint, BOOL *bLastFlag);

	//Retrieves a list of images from the last inspection(s) that was run
	//
	//Parameters:
	//
	//saImageNames - A VARIANT holding a SAFEARRAY of BSTRs that contains a list of the names of the images to get
	//
	//saImages - A Pointer to a VARIANT holding a SAFEARRAY of BSTRs that contains a list of the bits of the images
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetImageList)(SAFEARRAY* psaImageNames, long lCompression, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts);

	//Runs the specified Inspection Process (IP). The correct Inputs and Source
	// Images must already contain the correct values for this inspection.
	//
	//Parameters:
	//
	//bstrNameOfIPToRun -  BSTR containing the name of the process to be 
	// executed.
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVRegisterStream)(SAFEARRAY* psaName, VARIANT vtInterface, SAFEARRAY** ppsaStatus);

	STDMETHOD(SVRunOnce)(BSTR bstrNameOfIPToRun);
	
	STDMETHOD(SVSetSourceImage)(BSTR bstrImageName, BSTR bstrImage);
	
	STDMETHOD(SVSetInputs)(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY** ppsaStatus);

    STDMETHOD(SVUnRegisterStream)(VARIANT vtInterface);

	STDMETHOD(SVGetProductDataList)(long lProcessCount, SAFEARRAY* psaNames, SAFEARRAY** ppsaData, SAFEARRAY** ppsaStatus);

	STDMETHOD(SVGetProductImageList)(long lProcessCount, SAFEARRAY* psaNames, long lCompression, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus);

    STDMETHOD(SVGetDataList)(SAFEARRAY* psaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts);

	STDMETHOD(SVGetLUT)(BSTR bstrCameraName, SAFEARRAY** ppaulLUTTable);

	STDMETHOD(SVSetLUT)(BSTR bstrCameraName, SAFEARRAY* paulLUTTable);

	STDMETHOD(SVSetImageList)(SAFEARRAY *psaNames, SAFEARRAY *psaImages, SAFEARRAY **ppsaStatus);

    STDMETHOD(SVSetToolParameterList)(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY** ppsaStatus);

	STDMETHOD(SVUnlockAllImages)();

	STDMETHOD(SVUnlockImage)(long lindex);

	STDMETHOD(SVGetLockedImage)(long lindex, long lCompression, BSTR *bsImage, BSTR *bsOverlay);

	STDMETHOD(SVLockImage)(long lProcessCount, long lindex, BSTR bsName);
	//Retrieves a count of the number of Remote Inputs that are available
	//
	//Parameters:
	//
	//lCount - A Pointer to a long that will contain the returned count
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetRemoteInputCount)(long *lCount);

	//Sets a Remote Input at the specified index to the specified value
	//
	//Parameters:
	//
	//lIndex - A long that contains the index of the Remote Input to set
	//
	//bstrValue - A BSTR that contains the value to set in the Remote Input
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVSetRemoteInput)(long lIndex, VARIANT vtValue);


	// Conditional History functions
	STDMETHOD(SVSetConditionalHistoryProperties)(BSTR bstrInspectionName, SAFEARRAY*  psaNames,  SAFEARRAY*  psaValues,  SAFEARRAY** ppsaStatus);
	STDMETHOD(SVGetConditionalHistoryProperties)(BSTR bstrInspectionName, SAFEARRAY** ppsaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus);
	STDMETHOD(SVSetConditionalHistoryList)(BSTR bstrInspectionName, SAFEARRAY*  psaValueNames,  SAFEARRAY*  psaImageNames,  SAFEARRAY*  psaConditionalNames,  SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaConditionalStatus);
	STDMETHOD(SVGetConditionalHistoryList)(BSTR bstrInspectionName, SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaConditionalStatus);
	STDMETHOD(SVGetConditionalHistoryAndClear)  (BSTR bstrInspectionName, long lCompression,
		SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays,
		SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaValues,
		SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaConditionalValues,
		SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaConditionalStatus,
		SAFEARRAY** ppsaProcessCount );
	STDMETHOD(SVGetMostRecentConditionalHistory)(BSTR bstrInspectionName, long lCompression,
		SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays,
		SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaValues,
		SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaConditionalValues,
		SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaConditionalStatus,
		long* ppsaProcessCount );





	// New functions to support Matrox Font Aalyzers - // Stage 1 runtime only, no setup.
	STDMETHOD(SVConnectFont)(long* lFontIdentifier);

	STDMETHOD(SVDisconnectFont)(long lFontIdentifier);

	STDMETHOD(SVGetCellSize)(long lFontIdentifier, long* plWidth, long* plHeight, long* plSpacing);

	STDMETHOD(SVSetCellSize)(long lFontIdentifier, long lWidth, long lHeight);

	STDMETHOD(SVGetSearchSettings)(long lFontIdentifier, long* plLength, double* pdStringAcceptance, double* pdCharacterAcceptance, BSTR* pbstrInvalidCharacter, BSTR* pbstrSearchSpeed);

	STDMETHOD(SVSetSearchSettings)(long lFontIdentifier, long lLength, double dStringAcceptance, double dCharacterAcceptance, BSTR bstrInvalidCharacter, BSTR bstrSearchSpeed);

	STDMETHOD(SVGetPositionVariance)(long lFontIdentifier, double* pdXDirection, double* pdYDirection);
 
	STDMETHOD(SVSetPositionVariance)(long lFontIdentifier, double dXDirection, double dYDirection);

	STDMETHOD(SVGetSearchAngleSettings)(long lFontIdentifier, double* pdSearchAngle, long* plInterpolation, double* pdDeltaPositive, double* pdDeltaNegative);

	STDMETHOD(SVSetSearchAngleSettings)(long lFontIdentifier, double dSearchAngle, long lInterpolation, double dDeltaPositive, double dDeltaNegative);

	STDMETHOD(SVGetCharacterEnhancementSettings)(long lFontIdentifier, long* plMorphIterations, double* pdThickenCharacters, long* plBrokenCharacters, long* plTouchingCharacters);

	STDMETHOD(SVSetCharacterEnhancementSettings)(long lFontIdentifier, long lMorphIterations, double dThickenCharacters, long lBrokenCharacters, long lTouchingCharacters);

	STDMETHOD(SVLoadFont)(long lFontIdentifier, BSTR bstrFontFile, BSTR bstrFontControls, BSTR bstrFontConstraints);

	STDMETHOD(SVSaveFont)(long lFontIdentifier, BSTR* bstrFontFile, BSTR* bstrFontControls, BSTR* bstrFontConstraints);

	STDMETHOD(SVCalibrateFont)(long lFontIdentifier, BSTR bstrCalibrateString, BSTR bstrCalibrateImage, double dXMinSize, double dXMaxSize, double dXStepSize, double dYMinSize, double dYMaxSize, double dYStepSize );

	STDMETHOD(SVReadString)(long lFontIdentifier, BSTR* bstrFoundString, BSTR bstrReadImage, double* dMatchScore );

	STDMETHOD(SVVerifyString)(long lFontIdentifier, BSTR bstrVerifyString, BSTR bstrVerifyImage, double* dMatchScore );

	STDMETHOD(SVGetConstraints)(long lFontIdentifier, long* plPosition, long* plType, BSTR* pbstrValues);

	STDMETHOD(SVSetConstraints)(long lFontIdentifier, long lPosition, long lType, BSTR bstrValues);

	STDMETHOD(SVCreateNew)(long lFontIdentifier, BSTR bstrImage);

	STDMETHOD(SVSetTrainingImage)(long lFontIdentifier, BSTR bstrImage);

	STDMETHOD(SVAddCharacter)(long lFontIdentifier, long lXPosition, long lYPosition, long lHeight, long lWidth, BSTR bstrLabel, long *lCharID);

	STDMETHOD(SVAddCharacters)(long lFontIdentifier, BSTR bstrLabelList);

	STDMETHOD(SVDeleteCharacters)(long lFontIdentifier, SAFEARRAY* psaCharIds);

	STDMETHOD(SVThresholdImage)(long lFontIdentifier, BSTR bstrImage, long lThreshold, BSTR* pbstrThresholdImage);

	STDMETHOD(SVGetFontCharacterCount)(long lFontIdentifier, long* plCharacterCount);

	STDMETHOD(SVGetFontCharacter)(long lFontIdentifier, long lIndex, BSTR* pbstrLabelImage);

	STDMETHOD(SVGetStringLength)(long p_lFontIdentifier, double* p_pdLength );

	STDMETHOD(SVSetStringLength)(long p_lFontIdentifier, double p_dLength );

	STDMETHOD(SVLoadFontImage)(long p_lFontIdentifier, BSTR bstrFontImage );

	STDMETHOD(SVGetFontCharacterList)(long p_lFontIdentifier, BSTR *bstrCharacterList, SAFEARRAY** ppsaCharIds);

	STDMETHOD(SVGetExpectedTargetCharacterSize)(long p_lFontIdentifier, long *plWidth, long *plHeight);

	STDMETHOD(SVGetFontCharacterSize)(long p_lFontIdentifier, long *plWidth, long *plHeight);
		// **********************************************************************************
		//////// *********** Transfer Definition List Functions ************** //////////////
		// **********************************************************************************

	STDMETHOD(SVGetTransferValueDefinitionList)(BSTR p_bstrInspectionName, 
														  long* p_plVersion, 
														  VARIANT* p_pvData);

	STDMETHOD(SVGetTransferImageDefinitionList)(BSTR p_bstrInspectionName, 
														  long* p_plVersion, 
														  VARIANT* p_pvData);

  STDMETHOD(SVConstructCommand)( long p_CommandType, ISVRemoteCommand **p_ppCommand );

  STDMETHOD(SVIsAvailiable)();

public:
	static BOOL ResetStreamingDataAndLockedImages();

//protected:
//  ImageToBSTR () - If alCompression = 0, then image compression will not 
//  be used.  More importantly the apCompressionObject will not be used, so if
//  alCompression = 0, then it is acceptable for apCompressionObject to equal 
//  NULL.	
	static HRESULT ImageToBSTR( SVImageInfoClass &rImageInfo, SVSmartHandlePointer ImageHandle, BSTR *pbstr
						// BRW - SVImageCompression has been deprecated.
#ifndef _WIN64
		, long alCompression, SVImageCompressionClass *apCompressionObject);
#else
		);
#endif

	static HRESULT SafeImageToBSTR( SVImageClass *p_pImage, SVImageIndexStruct p_svIndex, BSTR *pbstr
						// BRW - SVImageCompression has been deprecated.
#ifndef _WIN64
		, long alCompression, SVImageCompressionClass *apCompressionObject);
#else
		);
#endif
	static HRESULT BSTRToImage(bool bCreateNew, BSTR bstr, SVImageInfoClass& rImageInfo,SVSmartHandlePointer &rImageHandle);
	static SVMatroxBuffer CreateImageFromBSTR( BSTR bstrImage );
public:
    static HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
    static HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
protected:
    static HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv);

    static DWORD WINAPI SVStreamDataThread(LPVOID lpParam);

	virtual HRESULT StoreEventObserver( DWORD dwCookie, CComPtr< CSVCommand > p_pObserver );
	virtual HRESULT ReleaseEventObserver( DWORD dwCookie, CComPtr< CSVCommand > p_pObserver );

	HRESULT StreamingDataCallback( const SVInspectionCompleteInfoStruct& p_rData );
	HRESULT RebuildStreamingDataList();

    IStream *m_pStream;

private:
	static SVVector< SVActiveXLockStruct > m_aSVActXLock;
	static SVVector< StreamDataStruct* > m_arStreamList;
	static SVVector< ProductDataStruct* > m_arProductList;
	static SVVector< SVInspectionProcess* > m_arInspections;
	static CStringList m_cslInspectionNames;

    static volatile BOOL m_bRunStreamData;
    static volatile BOOL m_bRegisteredStream;
    static volatile DWORD m_dwStreamDataProcessId;
	static volatile long m_lLastStreamedProduct;
    static volatile HANDLE m_hStopStreamEvent;
    static volatile HANDLE m_hStreamingThread;
	static CRITICAL_SECTION m_hProductCritSect;
	static bool m_bCriticalSectionInitialized;
};// end class CSVCommand

OBJECT_ENTRY_AUTO( __uuidof(SVCommand), CSVCommand ) 

#endif //__SVCOMMAND_H_

		//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCommand.h_v  $
 * 
 *    Rev 1.3   28 Feb 2014 08:05:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   #ifndef for 64 bit platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Feb 2014 14:05:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Un-commented Image Compression for 32 bit code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 12:16:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:55:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   02 Jul 2012 16:39:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   16 Jan 2012 15:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Created object Ids and added the remote command factory to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   11 Mar 2011 13:41:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver to handle new file and ocr commands from remote systems via SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   10 Feb 2011 14:33:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   08 Dec 2010 07:45:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   09 Nov 2010 16:09:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   15 Oct 2010 15:27:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new methods and object to the SVObserver command interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   15 Dec 2009 15:13:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   03 Sep 2009 09:59:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   20 Oct 2008 11:28:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  637
 * SCR Title:  SVObserver Streaming Data Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added arrayIndex member variable to the StreamDataStruct.
 * Added static flag for Critical Section initializtion state.
 * Moved CSVCommand Constructor inplementation to the cpp file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   07 Mar 2008 11:29:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functions to support Transfer Data Definitions Lists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   25 Jul 2007 13:37:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   22 Jun 2007 10:36:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   20 Jun 2007 16:24:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * These changes also include changes to the SVObserver COM Server and Connection Point functionality.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   17 Apr 2007 08:08:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new interface method for SIAC, SVGetFontCharacterSize
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   20 Mar 2007 13:15:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added SVGetExpectedTargetCharacterSize method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   13 Mar 2007 15:47:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed GetFontCharacter to use long ID instead of a char to select the Character. Removed un-used variables.  And did some basic cleanup.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   06 Feb 2007 14:54:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added the new methods for Matrox Fonts for the ActiveX
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   19 Jan 2006 08:54:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Process count to SVGetConditionalHistoryAndClear and SVGetMostRecentConditionalHistory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   17 Jan 2006 09:52:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added:
 * SVSetConditionalHistoryProperties
 * SVGetConditionalHistoryProperties
 * SVSetConditionalHistoryList
 * SVGetConditionalHistoryList
 * SVGetConditionalHistoryAndClear
 * SVGetMostRecentConditionalHistory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   02 Nov 2005 09:07:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   21 Jun 2005 14:13:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added two new functions for changing mode through SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   18 Feb 2005 11:46:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  382
 * SCR Title:  Add locking mechanism to internal image objects to eliminate access problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added safe copy functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   14 May 2004 11:16:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  420
 * SCR Title:  Allow SVObserver to update the string length for the Matrox OCR
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new methods SVGetStringLength and SVSetStringLength to allow ActiveX to adjust the font string length.
 * Updated SVCalibrateFont and SVVerifyString to use SVSetStringLength to update string length to the supplied match string length.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   11 Feb 2004 17:30:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed compiler warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   04 Feb 2004 14:25:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  403
 * SCR Title:  Add ActiveX interfaces to allow management of Matrox fonts through the SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code to support new Matrox font setup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   17 Jun 2003 15:05:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  362
 * SCR Title:  Streaming data - not all data being sent
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Restructured the Streaming Data code. It also now makes a copy during the callback which allows the data to not remain locked to cause contention later.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   13 Jun 2003 10:23:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  362
 * SCR Title:  Streaming data - not all data being sent
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new callback method.  SVCommandRebuildListCallback which will validate the list of StreamingData objects to make sure they are still vaild.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   17 Apr 2003 17:16:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   26 Mar 2003 15:57:42   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The ImageToBSTR () definition was changed to no longer specify default values for the compression parameters.  All places where ImageToBSTR is used should posses compression capabillities.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Mar 2003 13:26:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method for SetImageList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   29 Jan 2003 14:42:48   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVGetImageList, SVGetProductImageList, ImageToBSTR, BSTRToImage, SVSetSourceImage and SVGetLockedImage methods to include the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   22 Jan 2003 13:56:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bugs in the Get(Product)ImageList and Get(Product)DataList in calls from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Jan 2003 18:37:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to support image overlays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   13 Jan 2003 13:19:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new DCOM functions to support retmote use of the Remote Inputs in the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   10 Jan 2003 17:49:52   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Changes made to Include LEADTOOLS Image compression
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Jan 2003 13:43:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  309
 * SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement the ability to lock acquired or processed images from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Jan 2003 16:40:22   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   In order to implement Image Compression, the following modifications were made...
 * 
 * Added header file references....
 * 
 *  #include "SVBStr.h"	                              // SVBStr 
 *  #include "SVImageCompressionClass.h"			
 *  #include "lead.h"                                  // CLead
 * 
 * added
 *  #define SERVER_COMPRESSION_POOL_SIZE  5
 * 
 * Changed ....
 *  CSVCommand::SVGetImageList()
 *  CSVCommand::ImageToBSTR()
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Jan 2003 15:23:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added DCom interface function for SetToolParameterList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jan 2003 15:25:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * SVGetLUT
 * SVSetLUT
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Jan 2003 12:02:28   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   The following methods were added to the SVObserver ActivX server:
 * SVSetInputs
 * SVSetSourceImage
 * SVRunOnce
 * SVGetDataList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Dec 2002 11:58:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  300
 * SCR Title:  Create an interface for SVObserver to Push Output data to a SIAC Client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Methods added:
 *  SVRegisterStream, SVUnRegisterStream - called from the ActiveX Control.  
 * SVStreamDataThread - thread that will actually stream the data back to the control.
 * SVCommandStreamDataCallback - callback from the inspection that will build packets to send to the control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Dec 2002 15:05:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include "SVInfoStructs.h"
 * changed
 * STDMETHOD(SVGetImageList)(VARIANT *pvtImageNames, VARIANT *pvtImages);
 * to
 * STDMETHOD(SVGetImageList)(SAFEARRAY* psaImageNames, SAFEARRAY** psaImages, SAFEARRAY** ppsaStatus);
 * 
 * Added
 * STDMETHOD(SVGetProductDataList)(long lProcessCount, SAFEARRAY* saNames, SAFEARRAY** psaData, SAFEARRAY** ppsaStatus);
 * 
 * STDMETHOD(SVGetProductImageList)(long lProcessCount, SAFEARRAY* saNames, long lCompression, SAFEARRAY** psaImages, SAFEARRAY** ppsaStatus);
 * 
 * Added helper functions:
 * ImageToBSTR
 * BSTRToImage
 * SafeArrayPutElementNoCopy
 * SafeArrayGetElementNoCopy
 * SafeArrayGetElementPointer
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Dec 2002 17:20:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a missing definition GetSVIMConfigPrint of SVCommand class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Nov 2002 11:05:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Version enums
 *                        SVGetSVIMVersion
 *                         SVGetSVIMOfflineCount
 *                         SVGetSVIMConfigPrint
 *                         SVGetImageList
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jun 2002 13:48:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  292
 * SCR Title:  Add new ActiveX interfaces that duplicate the existing ones with less memory
 * Checked in by:  ryoho;  Rob Yoho
 * Change Description:  
 *   added the following function:
 * SVGetSVIMConfig, SVGetSVIMConfigName, SVGetSVIMFile, SVGetSVIMInspectionResult, SVGetSVIMState, SVLoadSVIMConfig, SVPutSVIMConfig, SVPutSVIMFile, SVSetSVIMState
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2001 13:03:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added source file header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
