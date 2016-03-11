//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPropertyPageDlg
//* .File Name       : $Workfile:   SVOPropertyPageDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   18 Sep 2014 13:42:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOPropertyPageDlg.h"
#include "SVObserver.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOMFCLibrary/SVDeviceParam.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVOMFCLibrary/SVCustomDeviceParam.h"
#include "SVOMFCLibrary/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVOConfigAssistantDlg.h"
#include "SVCameraFormat7Dlg.h"
#include "SVGigeCameraROIDlg.h"
#include "SVAcquisitionClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SV1394CameraFileLibrary/SV1394CameraParamValidateClass.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraParamValidateClass.h"
#include "SVPPQObject.h"
#include "SVSoftwareTriggerDefaults.h"
#include "SVPPQConstants.h"
#include "SVHardwareManifest.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class SVCameraDeviceImageFormatUpdater : public ISVCameraDeviceImageFormatUpdater
{
private:
	SVString m_strName;
	SVDeviceParamWrapper m_wFormats;
	SVCameraFormatsDeviceParam* m_pFormatsParam; 
	
public:
	SVCameraDeviceImageFormatUpdater(SVDeviceParamWrapper& rw, const SVString& strName, long width, long height) 
	: m_wFormats(rw) 
	, m_pFormatsParam(NULL)
	, m_strName(strName)
	{
		m_pFormatsParam = m_wFormats.DerivedValue( m_pFormatsParam );
		m_pFormatsParam->options[ strName ].lHeight = height;
		m_pFormatsParam->options[ strName ].lWidth = width;
		m_pFormatsParam->options[ strName ].lHPos = 0;
		m_pFormatsParam->options[ strName ].lVPos = 0;
		m_pFormatsParam->strValue = strName;
	}
	virtual ~SVCameraDeviceImageFormatUpdater() {}

	virtual void SetXOffset(long xOffset) 
	{ 
		m_pFormatsParam->options[ m_strName ].lHPos = xOffset; 
	}
	virtual void SetYOffset(long yOffset) 
	{ 
		m_pFormatsParam->options[ m_strName ].lVPos = yOffset; 
	}
	virtual void SetWidth(long width) 
	{  
		m_pFormatsParam->options[ m_strName ].lWidth = width; 
	}
	virtual void SetHeight(long height) 
	{ 
		m_pFormatsParam->options[ m_strName ].lHeight = height; 
	}

	void Update(SVAcquisitionClassPtr pAcqDevice)
	{
		if (pAcqDevice != NULL)
		{
			SVDeviceParamCollection l_params;
			l_params.SetParameter( m_wFormats );
			pAcqDevice->SetDeviceParameters(l_params);

			SVImageInfoClass infoTemp;
			pAcqDevice->GetImageInfo(&infoTemp);
		
			long width = m_pFormatsParam->options[ m_strName ].lWidth;
			long height = m_pFormatsParam->options[ m_strName ].lHeight;

			infoTemp.SetExtentProperty(SVExtentPropertyWidth, width);
			infoTemp.SetExtentProperty(SVExtentPropertyHeight, height);

			pAcqDevice->DestroyBuffers();
			pAcqDevice->CreateBuffers( infoTemp, TheSVObserverApp.GetSourceImageDepth() );
		}
	}
};

const CString CorecoCameraFileFilter = _T("Camera Files (*.cca)|*.cca|All Files (*.*)|*.*||");
const CString SV1394CameraFileFilter = _T("Camera Files (*.odc)|*.odc|All Files (*.*)|*.*||");
const CString SVGigeCameraFileFilter = _T("Camera Files (*.ogc)|*.ogc||");

enum {IDC_PROPERTYTREE = 100};

BEGIN_MESSAGE_MAP(CSVOPropertyPageDlg, CDialog)
	//{{AFX_MSG_MAP(CSVOPropertyPageDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
    ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTYTREE, OnItemChanged)
    ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_PROPERTYTREE, OnItemQueryShowButton)
    ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_PROPERTYTREE, OnItemButtonClick)
END_MESSAGE_MAP()

CSVOPropertyPageDlg::CSVOPropertyPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSVOPropertyPageDlg::IDD, pParent)
,	m_TriggerObj( _T(""), 0 )
,	m_eProduct( SVIM_PRODUCT_TYPE_UNKNOWN )
{
}

void CSVOPropertyPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVOPropertyPageDlg)
	//}}AFX_DATA_MAP
}

void CSVOPropertyPageDlg::SetupCamera()
{
	SetTitle(m_CameraObj.GetCameraDisplayName());
	if( (m_eProduct != SVIM_PRODUCT_FULL) && (m_eProduct != SVIM_PRODUCT_05) )
	{
		if( !m_CameraObj.IsFileAcquisition() )
		{
			if( (m_CameraObj.GetCameraFile().GetLength() > 0) )
			{
				GetDlgItem( IDOK)->SetWindowText( _T("Reset"));
				m_InitialCameraFileName = m_CameraObj.GetCameraFile();
			}
		}
	}
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Camera"));
		pRoot->SetInfoText(_T("Define a Camera"));
		ASSERT( m_eProduct != SVIM_PRODUCT_TYPE_UNKNOWN );
	
		SetupFileCamera(pRoot);

		SVFileNameClass FileName;
		FileName.SetFileType(SV_CAMERA_FILE_TYPE);

		// determine if GigE is present
		LPCTSTR fileFilter = SVHardwareManifest::IsDigitalSVIM( m_eProduct ) ? 
			(SVHardwareManifest::IsMatroxGige(m_eProduct) ? SVGigeCameraFileFilter : SV1394CameraFileFilter) : CorecoCameraFileFilter;
		
		bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
		SVRPropertyItemFile* pFile = (SVRPropertyItemFile*)m_Tree.InsertItem(new SVRPropertyItemFile(bFullAccess, SVR_FILE,
				fileFilter, FileName.GetDefaultPathName(), TRUE), pRoot);
		if (pFile)
		{
			pFile->SetCtrlID(PROP_AD_FILE_NAME);
			pFile->SetLabelText(_T("Camera Filename"));
			pFile->SetInfoText(_T("Select camera file for the currently selected Camera."));
			pFile->SetItemValue(m_CameraObj.GetCameraFile());
		}
		
		// Show or Hide Real/File camera props
		ShowCameraProperties();

		pRoot->Select(true);
		pRoot->Expand();
	}
}

void CSVOPropertyPageDlg::SetupFileCamera(SVRPropertyItem* pRoot)
{
	SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
	if (pCombo)
	{
		pCombo->SetCtrlID( PROP_CAMERA_TYPE_FILE_OR_REAL );
		pCombo->SetLabelText( _T("Acquisition Type") );
		pCombo->SetInfoText(_T("This item indicates the type of Acquisition. (Hardware Acquisition or File Acquisition)"));
		pCombo->CreateComboBox();
		
		int iInsIndex = pCombo->AddString( _T( "Hardware Acquisition" ) );
		pCombo->SetItemData( iInsIndex, 0 );
		
		iInsIndex = pCombo->AddString( _T( "File Acquisition" ) );
		pCombo->SetItemData( iInsIndex, 1 );
		
		pCombo->SetItemValue(m_CameraObj.IsFileAcquisition() ? 1 : 0);
	}

	pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
	if (pCombo)
	{
		pCombo->SetCtrlID( PROP_CAMERA_COLOR );
		pCombo->SetLabelText( _T("Color Camera") );
		pCombo->SetInfoText(_T("This item indicates that the camera is a color camera"));
		pCombo->CreateComboBox();

		int iInsIndex = pCombo->AddString( _T( "No" ) );
		pCombo->SetItemData( iInsIndex, 0 );

		iInsIndex = pCombo->AddString( _T( "Yes" ) );
		pCombo->SetItemData( iInsIndex, 1 );

		pCombo->SetItemValue(m_CameraObj.IsColor() ? 1 : 0);
	}

	pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
	if (pCombo)
	{
		pCombo->SetCtrlID( PROP_FILECAMERA_MODE );
		pCombo->SetLabelText( _T("File Loading Mode") );
		pCombo->SetInfoText(_T("This specifies the File Loading mode to use. (Single File, Continuous Load, or Single Iteration)"));
		pCombo->CreateComboBox();
		int iInsIndex = pCombo->AddString( DeviceParamFileAcqSingleFile_String );
		pCombo->SetItemData( iInsIndex, SingleFileMode ); 
		
		iInsIndex = pCombo->AddString( DeviceParamFileAcqContinuousLoad_String );
		pCombo->SetItemData( iInsIndex, ContinuousMode );
		
		iInsIndex = pCombo->AddString( DeviceParamFileAcqSingleIteration_String );
		pCombo->SetItemData( iInsIndex, SingleIterationMode );

		pCombo->SetItemValue(m_CameraObj.GetFileLoadingMode());
	}

	LPCTSTR fileFilter = _T("Image Files (*.bmp)|*.bmp||");
	SVFileNameClass FileName;
	FileName.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
	
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SVRPropertyItemFile* pFile = (SVRPropertyItemFile*)m_Tree.InsertItem(new SVRPropertyItemFile(bFullAccess, SVR_FILE,
									fileFilter, FileName.GetDefaultPathName(), true), pRoot);
	if (pFile)
	{
		pFile->SetCtrlID(PROP_FILECAMERA_FILENAME);
		pFile->SetLabelText(_T("Image Filename"));
		pFile->SetInfoText(_T("Select the image file for the currently selected Camera."));
		pFile->SetItemValue(m_CameraObj.GetImageFilename());
	}

	pFile = (SVRPropertyItemFile*)m_Tree.InsertItem(new SVRPropertyItemFile(bFullAccess, SVR_FOLDER | SVR_TRAILINGSLASH,
									NULL, FileName.GetDefaultPathName(), true), pRoot);
	if (pFile)
	{
		pFile->SetCtrlID(PROP_FILECAMERA_DIRECTORY);
		pFile->SetLabelText(_T("Image Directory"));
		pFile->SetInfoText(_T("Select the directory to load the image files from, for the currently selected Camera."));
		SVString dirName = m_CameraObj.GetImageDirectoryName();
		if (dirName.empty())
		{
			dirName = FileName.GetDefaultPathName();
			m_CameraObj.SetImageDirectoryName(dirName);
		}
		pFile->SetItemValue(dirName.c_str());
	}
}

void CSVOPropertyPageDlg::SetupAdvancedFileCamera(SVRPropertyItem* pRoot)
{
	SetTitle(m_CameraObj.GetCameraDisplayName());

	SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
	if (pCombo)
	{
		pCombo->SetCtrlID( PROP_ADV_FILECAMERA_IMAGESIZEEDITMODE );
		pCombo->SetLabelText( _T("Image Size Mode") );
		pCombo->SetInfoText(_T("This specifies the Image Size Editing mode to use. (Use Image Size from File or User Editable)"));
		pCombo->CreateComboBox();
		
		int iInsIndex = pCombo->AddString( _T( "Use Image Size from File" ) );
		pCombo->SetItemData( iInsIndex, 0 );
		
		iInsIndex = pCombo->AddString( _T( "User Editable" ) );
		pCombo->SetItemData( iInsIndex, 1 );
		
		pCombo->SetItemValue(m_CameraObj.IsFileImageSizeEditModeFileBased() ? 0 : 1);
	}

	SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
	if (pEdit)
	{
		pEdit->SetCtrlID(PROP_ADV_FILECAMERA_IMAGEWIDTH);
		pEdit->SetLabelText(DeviceParamFileAcqImageWidth_String);
		pEdit->SetInfoText(_T("Specifies the Width for the File Acquisition Image (minimum of 4)."));
	}
	pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
	if (pEdit)
	{
		pEdit->SetCtrlID(PROP_ADV_FILECAMERA_IMAGEHEIGHT);
		pEdit->SetLabelText(DeviceParamFileAcqImageHeight_String);
		pEdit->SetInfoText(_T("Specifies the Height for the File Acquisition Image (minimum of 4)."));
	}
	UpdateFileImageSize();
	SetImageSizeEditAttributes();
}

void CSVOPropertyPageDlg::SetupAdvancedCamera()
{
	ASSERT( m_pAssistant != NULL );
	SetTitle(m_CameraObj.GetCameraDisplayName());
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		if (m_CameraObj.IsFileAcquisition())
		{
			SetupAdvancedFileCamera(pRoot);
		}
		else
		{
			SVRPropertyItemCombo* pCombo = NULL;

			pRoot->SetCanShrink(false);
			pRoot->SetLabelText(_T("Camera Advanced"));
			pRoot->SetInfoText(_T("Advanced camera properties"));
			
			ASSERT( m_eProduct != SVIM_PRODUCT_TYPE_UNKNOWN );
			if ( SVHardwareManifest::IsDigitalSVIM( m_eProduct ) )
			{
				// load options based on camera file
				const SVDeviceParamCollection& rCameraFileParams = m_CameraObj.GetCameraFileParams();
				const SVDeviceParamCollection& rCameraDeviceParams = m_CameraObj.GetCameraDeviceParams();
				
				// use sorted access
				SVDeviceParamIndexer index( rCameraFileParams.mapParameters );
				SVDeviceParamIndexer::iterator iter;
				for (iter = index.begin(); iter != index.end(); ++iter)
				{
					const SVDeviceParamWrapper& rCamFileParam = rCameraFileParams.Parameter( *iter );
					const SVDeviceParamWrapper& rCamDeviceParam = rCameraDeviceParams.Parameter( *iter );

					if ( !rCamFileParam.IsValid() || !rCamDeviceParam.IsValid() )
						continue;
					
					const int iDetailLevel = 0;
					if ( rCamFileParam->DetailLevel() > iDetailLevel )
						continue;

					const SVDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
					const SVDeviceParam* pCamDeviceParam = rCamDeviceParam.DerivedValue(pCamDeviceParam);

					switch ( rCamFileParam->DataType() )
					{
						case DeviceDataTypeBool:
						//case DeviceDataTypei64: // No editing of I64 at this time
						case DeviceDataTypeLong:
						case DeviceDataTypeString:
						case DeviceDataTypeComplex:
							SetupCameraDeviceParam(pRoot, pCamDeviceParam, pCamFileParam, m_CameraObj.IsColor());
							break;

						case DeviceDataTypeCustom:
						{
							// swap the Container for the real parameters
							const SVCustomDeviceParam* pCustomFileParam = rCamFileParam.DerivedValue(pCustomFileParam);
							const SVCustomDeviceParam* pCustomDeviceParam = rCamDeviceParam.DerivedValue(pCustomDeviceParam);
							if (pCustomFileParam && pCustomDeviceParam)
							{
								const SVDeviceParamWrapper& rFileParam = pCustomFileParam->GetHeldParam();
								const SVDeviceParamWrapper& rDeviceParam = pCustomDeviceParam->GetHeldParam();

								const SVDeviceParam* pFileParam = rFileParam.DerivedValue(pFileParam);
								const SVDeviceParam* pDeviceParam = rDeviceParam.DerivedValue(pDeviceParam);
								SetupCameraDeviceParam(pRoot, pDeviceParam, pFileParam, m_CameraObj.IsColor());
							}
						}
						break;

					default:
						break;
					}
				}
				SV1394CameraParamValidateClass l_svValidate;
				l_svValidate.UpdateParams( m_CameraObj.GetCameraFileParamsNonConst(),
					m_CameraObj.GetCameraDeviceParamsNonConst() );
				
				CameraAdvancedHideItems();
			}// end if ( TheSVObserverApp.IsDigitalSVIM() )
			else
			{
				pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
				if (pCombo)
				{
					int iNum = 0;
					pCombo->SetCtrlID(PROP_ADV_CAMERA_DIG);
					pCombo->SetLabelText(_T("Digitizer #"));
					pCombo->SetInfoText(_T("Enter the digitizer number the camera is attached to."));
					pCombo->CreateComboBox();
					//based on board fill in combo
					switch(m_eProduct)
					{
					case SVIM_PRODUCT_FULL:
						{
							iNum = 4;
							break;
						}
					case SVIM_PRODUCT_05:
						{
							iNum = 2;
							break;
						}
					case SVIM_PRODUCT_RGB_MONO:
						{
							iNum = 1;
							break;
						}
					case SVIM_PRODUCT_RGB_COLOR:
						{
							iNum = 1;
							break;
						}
					default:
						{
							break;
						}
					}
					CString sFormat;
					int iPos;
					for (int i = 0; i < iNum; i++)
					{
						sFormat.Empty();
						sFormat.Format("%d",i);
						iPos = pCombo->AddString(sFormat);
						pCombo->SetItemData(iPos,i);
					}
					pCombo->SetItemValue(m_CameraObj.GetDigNumber());
					
				}// end if (pCombo)
			}// end if ( m_eProduct == SVIM_BOARD_1394 ) else
				
			if (m_eProduct == SVIM_PRODUCT_RGB_MONO)
			{
				pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
				if (pCombo)
				{  
					CString sBands;
					int iBandPos;
					pCombo->SetCtrlID(PROP_ADV_CAMERA_BANDS);
					pCombo->SetLabelText(_T("Band #"));
					pCombo->SetInfoText(_T("Enter the band that the camera will use."));
					pCombo->CreateComboBox();
					for (int iC = 0; iC < 3; iC++)
					{
						sBands.Empty();
						sBands.Format("%d",iC);
						iBandPos = pCombo->AddString(sBands);
						pCombo->SetItemData(iBandPos,iC);
					}
					pCombo->SetItemValue(m_CameraObj.GetBandNumber());
				}
			}// end if (m_eProduct == SVIM_PRODUCT_RGB_MONO)
		}
		pRoot->Select(true);
		pRoot->Expand();
    }// end if (pRoot)
}

void CSVOPropertyPageDlg::SetupCameraDeviceParam(SVRPropertyItem* pRoot, const SVDeviceParam* pDeviceParam, const SVDeviceParam* pFileParam, bool ColorCamera)
{
	if (pDeviceParam && pFileParam)
	{
		switch ( pFileParam->DataType() )
		{
			case DeviceDataTypeLong:
			{
				const SVLongValueDeviceParam* pCamFileParam = dynamic_cast<const SVLongValueDeviceParam *>(pFileParam);
				const SVLongValueDeviceParam* pCamDeviceParam = dynamic_cast<const SVLongValueDeviceParam *>(pDeviceParam);
				if ( pCamFileParam->info.options.size() > 0 )
				{
					SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
					if (pCombo)
					{
						pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + pCamFileParam->Type() );
						CString sLabel = pCamFileParam->VisualName();
						if ( sLabel == _T("") )
							sLabel = pCamFileParam->Name();
						pCombo->SetLabelText( sLabel );
						pCombo->SetInfoText( CString(pCamFileParam->Description()) );
						pCombo->CreateComboBox();
						
						SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
						for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
						{
							CString sText = iterOption->strDescription.c_str();
							int iPos = pCombo->AddString( sText );
							pCombo->SetItemData( iPos, iterOption->value );
						}
						if ( pCamFileParam->info.options.size() == 1 )
						{
							pCombo->ReadOnly( true );
						}
						pCombo->SetItemValue( pCamDeviceParam->lValue );
					}
				}
				else
				{
					// do a spin control bounded by pParam->info.min / info.max;
					SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
					if (pEdit)
					{
						pEdit->SetCtrlID( PROP_CAMERA_FILE_BASE + pCamFileParam->Type() );
						CString sLabel = pCamFileParam->VisualName();
						if ( sLabel == _T("") )
							sLabel = pCamFileParam->Name();
						sLabel = sLabel + " (" + pCamDeviceParam->info.sUnits + ")";
						pEdit->SetLabelText( sLabel );
						CString sDescription = CString(pCamFileParam->Description());
						sDescription += CString(_T("   Min = ")) + AsString(pCamFileParam->GetScaledMin() ) + CString(_T(", Max = ")) + AsString( pCamFileParam->GetScaledMax() ) + CString(_T("; default = ")) + AsString((long )ceil( pCamFileParam->lValue * pCamFileParam->info.multiplier) );
						pEdit->SetInfoText( sDescription );
										
						pEdit->SetItemValue( pCamDeviceParam->GetScaledValue() );
					}
				}
				break;
			}
			case DeviceDataTypeBool:
			{
				const SVBoolValueDeviceParam* pCamFileParam = dynamic_cast<const SVBoolValueDeviceParam *>(pFileParam);
				const SVBoolValueDeviceParam* pCamDeviceParam = dynamic_cast<const SVBoolValueDeviceParam *>(pDeviceParam);
				
				SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo *)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
				if (pCombo)
				{
					pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + pCamFileParam->Type() );
					CString sLabel = pCamFileParam->VisualName();
					if ( sLabel == _T("") )
						sLabel = pCamFileParam->Name();
					pCombo->SetLabelText( sLabel );
					pCombo->SetInfoText( CString(pCamFileParam->Description()) );
					pCombo->CreateComboBox();
					
					SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
					for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
					{
						CString sText = iterOption->strDescription.c_str();
						int iPos = pCombo->AddString( sText );
						pCombo->SetItemData( iPos, (LPARAM) iterOption->value );
					}
					if ( pCamFileParam->info.options.size() == 1 )
					{
						pCombo->ReadOnly( true );
					}
					pCombo->SetItemValue( pCamDeviceParam->bValue );
				}
				break;
			}
			case DeviceDataTypeString:
			{
				switch ( pFileParam->Type() )
				{
					case DeviceParamCameraFormats:
					{
						const SVCameraFormatsDeviceParam* pCamFileParam = dynamic_cast<const SVCameraFormatsDeviceParam *>(pFileParam);
						const SVCameraFormatsDeviceParam* pCamDeviceParam = dynamic_cast<const SVCameraFormatsDeviceParam *>(pDeviceParam);
						if ( pCamFileParam->options.size() > 0 )
						{
							SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo *)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
							if (pCombo)
							{
								pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + pCamFileParam->Type() );
								CString sLabel = pCamFileParam->VisualName();
								if ( sLabel == _T("") )
									sLabel = pCamFileParam->Name();
								pCombo->SetLabelText( sLabel );
								pCombo->SetInfoText( CString(pCamFileParam->Description()) );
								pCombo->SetButtonText("ROI");
								pCombo->CreateComboBox();
									
								SVCameraFormatsDeviceParam::OptionsType::const_iterator iterOption;
								const SVCameraFormat* pFormat=NULL;
								for (iterOption = pCamFileParam->options.begin(); iterOption != pCamFileParam->options.end(); ++iterOption)
								{
									if ( iterOption->second.bColor == ColorCamera )
									{
										CString sText = iterOption->second.strDescription.c_str();
										int iPos = pCombo->AddString( sText );
										pCombo->SetItemData( iPos, reinterpret_cast<DWORD_PTR>(&(iterOption->second)) );
										if ( pCamDeviceParam->strValue == iterOption->second.m_strName )
										{
											pFormat = &(iterOption->second);
										}
									}
								}
								if ( pCamFileParam->options.size() == 1 )
								{
									pCombo->ReadOnly( true );
								}
								pCombo->SetItemValue( reinterpret_cast<DWORD_PTR>(pFormat) );
							}
						}// end if ( pCamFileParam->options.size() > 0 )
					}// end case DeviceParamCameraFormats:
					break;
									
					default:
					{
						const SVStringValueDeviceParam* pCamFileParam = dynamic_cast<const SVStringValueDeviceParam *>(pFileParam);
						const SVStringValueDeviceParam* pCamDeviceParam = dynamic_cast<const SVStringValueDeviceParam *>(pDeviceParam);
						if ( pCamFileParam->info.options.size() > 0 )
						{
							SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo *)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
							if (pCombo)
							{
								pCombo->SetCtrlID( PROP_CAMERA_FILE_BASE + pCamFileParam->Type() );
								CString sLabel = pCamFileParam->VisualName();
								if ( sLabel == _T("") )
									sLabel = pCamFileParam->Name();
								pCombo->SetLabelText( sLabel );
								pCombo->SetInfoText( CString(pCamFileParam->Description()) );
								pCombo->CreateComboBox();
										
								SVStringValueDeviceParam::OptionsType::const_iterator iterOption;
								int iOption = 0;
								int iOptionIndex = 0;
								for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
								{
									CString sText = iterOption->strDescription.c_str();
									int iPos;
									iPos = pCombo->AddString( sText );
									++iOption;
									if ( iterOption->value == pCamDeviceParam->strValue )
										iOptionIndex = iOption;
									pCombo->SetItemData( iPos, iOption );
								}
								if ( pCamFileParam->info.options.size() == 1 )
								{
									pCombo->ReadOnly( true );
								}
								pCombo->SetItemValue( iOptionIndex );
							}
						}
					}// end default:
					break;
				}// end switch ( pFileParam->Type() )
			}
			case DeviceDataTypeComplex:
			{
				// put a "..." button to pull up custom dialog (e.g. LUT or LR)
				break;
			}
			default:
			break;
		}// end switch ( w->DataType() )
	}
}

void CSVOPropertyPageDlg::ShowCameraProperties()
{
	if (m_CameraObj.IsFileAcquisition())
	{
		m_Tree.FindItem(PROP_FILECAMERA_MODE)->HideItem(false);
		m_Tree.FindItem(PROP_FILECAMERA_DIRECTORY)->HideItem(false);
		m_Tree.FindItem(PROP_FILECAMERA_FILENAME)->HideItem(false);
		
		m_Tree.FindItem(PROP_AD_FILE_NAME)->HideItem(true);
	}
	else
	{
		m_Tree.FindItem(PROP_AD_FILE_NAME)->HideItem(false);

		m_Tree.FindItem(PROP_FILECAMERA_MODE)->HideItem(true);
		m_Tree.FindItem(PROP_FILECAMERA_DIRECTORY)->HideItem(true);
		m_Tree.FindItem(PROP_FILECAMERA_FILENAME)->HideItem(true);
	}
}

void CSVOPropertyPageDlg::UpdateFileImageSize()
{
	long width = m_CameraObj.GetFileImageWidth();
	long height = m_CameraObj.GetFileImageHeight();

	// determine if width/height has been user entered or if should scan for width/height from file
	if (m_CameraObj.IsFileImageSizeEditModeFileBased())
	{
		// Scan for Image Size
		SIZE size;

		// if file or directory specified
		// grab image size from the file (Single File mode) or first file in the directory (Contiunuous Load mode)
		if (ScanForImageSize(size))
		{
			width = size.cx;
			height = size.cy;
			
			m_CameraObj.SetFileImageWidth(width);
			m_CameraObj.SetFileImageHeight(height);
		}
	}
	// update control values if created
	SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.FindItem(PROP_ADV_FILECAMERA_IMAGEWIDTH);
	if (pEdit)
	{
		pEdit->SetItemValue(width);
	}
	
	pEdit = (SVRPropertyItemEdit*)m_Tree.FindItem(PROP_ADV_FILECAMERA_IMAGEHEIGHT);
	if (pEdit)
	{
		pEdit->SetItemValue(height);
	}
}

bool CSVOPropertyPageDlg::ScanForImageSize(SIZE& size)
{
	bool Result( false );
	SVImageFile FileImage;
	SVString Name;

	if ( 0 == m_CameraObj.GetFileLoadingMode() ) // use File
	{
		Name = m_CameraObj.GetImageFilename();
	}
	else
	{
		Name = m_CameraObj.GetImageDirectoryName();
	}

	if( S_OK == SVImageFileLoader::LoadFirstFile( Name.c_str(), _T("*.bmp"), FileImage ) )
	{
		size = FileImage.GetSize();
		Result = true;
	}
	return Result;
}

void CSVOPropertyPageDlg::SetImageSizeEditAttributes()
{
	if (m_CameraObj.IsFileImageSizeEditModeFileBased())
	{
		m_Tree.FindItem(PROP_ADV_FILECAMERA_IMAGEWIDTH)->ReadOnly( true );
		m_Tree.FindItem(PROP_ADV_FILECAMERA_IMAGEHEIGHT)->ReadOnly( true );
	}
	else
	{
		m_Tree.FindItem(PROP_ADV_FILECAMERA_IMAGEWIDTH)->ReadOnly( false );
		m_Tree.FindItem(PROP_ADV_FILECAMERA_IMAGEHEIGHT)->ReadOnly( false );
	}
}

void CSVOPropertyPageDlg::SetupInspection()
{
	SetTitle(m_InspectionObj.GetInspectionName());
			
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Inspection"));
		pRoot->SetInfoText(_T("Define an Inspection"));
		
		SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		int iInsIndex;
		if (pCombo)
		{ 
			pCombo->SetCtrlID(PROP_INS_TOOLSET_IMAGE);
			pCombo->SetLabelText(_T("Toolset Image"));
			pCombo->SetInfoText(_T("This specifies which Camera from the attached PPQ to use as the main Toolset Image."));
			pCombo->CreateComboBox(CBS_DROPDOWNLIST);
			// enter the Cameras available on the PPQ that this inspection is attached to...
			
			CString strName;
			long lSize;
			long l;
			
			if( 0 < m_PPQObj.GetAttachedInspectionCount() )
			{
				lSize = m_PPQObj.GetAttachedCameraCount();
				for( l = 0; l < lSize; l++ )
				{
					strName = m_PPQObj.GetAttachedCamera( l );
					iInsIndex = pCombo->AddString( strName );
					pCombo->SetItemData( iInsIndex, l );
				}// end for
				
				long lWhere = pCombo->SelectString( -1, m_InspectionObj.GetToolsetImage() );
				bool bResult = pCombo->SetItemValue( pCombo->GetItemData( lWhere ) );
			}// end if
			else
			{
				pCombo->SetInfoText(_T("This inspection is not yet attached to a PPQ."));
			}
		}
		
		pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
		if (pCombo)
		{ 
			pCombo->SetCtrlID(PROP_INS_NEW_DISABLE_METHOD);
			pCombo->SetLabelText(_T("Disable Method"));
			pCombo->SetInfoText(_T("This specifies the disable method to use. Method 1 is the traditional method that guarantees previous results, even for disabled tools and toolsets. Method 2 guarantees previous results only for conditionally disabled tools and non-disabled tools and toolsets."));
			pCombo->CreateComboBox(CBS_DROPDOWNLIST);
			// enter the Cameras available on the PPQ that this inspection is attached to...
			
			iInsIndex = pCombo->AddString( _T( "Method 1" ) );
			pCombo->SetItemData( iInsIndex, 0 );
			
			iInsIndex = pCombo->AddString( _T( "Method 2" ) );
			pCombo->SetItemData( iInsIndex, 1 );
			
			long lWhere = pCombo->SelectString( -1, m_InspectionObj.GetNewDisableMethod() );
			bool bResult = pCombo->SetItemValue( pCombo->GetItemData( lWhere ) );
		}// end if

		if( m_InspectionObj.GetShowAuxExtent() )
		{
			pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
			if (pCombo)
			{ 
				pCombo->SetCtrlID(PROP_INS_ENABLE_AUX_EXTENT); //PROP_INS_NEW_DISABLE_METHOD);
				pCombo->SetLabelText(_T("Auxiliary Extent"));
				pCombo->SetInfoText(_T("This allows the inspection to support Auxiliary Extents (Enabling this will perform auxiliary extent calculations that could add additional time to the tool set.)"));
				pCombo->CreateComboBoxBool();
				
				pCombo->SetItemValue( m_InspectionObj.GetEnableAuxiliaryExtent() );
			}
		}
		pRoot->Select(true);
		pRoot->Expand();
	}
}

void CSVOPropertyPageDlg::SetupTrigger()
{
	SetTitle(CString(m_TriggerObj.GetTriggerDisplayName()));
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Trigger"));
		pRoot->SetInfoText(_T("Define a Trigger"));
		
		SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		
		if (pCombo)
		{
			pCombo->SetCtrlID(PROP_TRG_TYPE);
			pCombo->SetLabelText(_T("Trigger Type"));
			pCombo->SetInfoText(_T("This item indicates the type of trigger."
				" Hardware, Software."));
			pCombo->CreateComboBox();

			int iPos = pCombo->AddString(_T("Hardware"));
			pCombo->SetItemData(iPos, 0);

			iPos = pCombo->AddString(_T("Software"));
			pCombo->SetItemData(iPos, 1);
			
			pCombo->SetItemValue(m_TriggerObj.IsSoftwareTrigger() ? 1 : 0);
		}
		pRoot->Select(true);
		pRoot->Expand();
	}
}

void CSVOPropertyPageDlg::SetupAdvancedTrigger()
{
	SetTitle(CString(m_TriggerObj.GetTriggerDisplayName()));
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Trigger - Advanced"));
		pRoot->SetInfoText(_T("Advanced Properties for a Trigger"));

		SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_ADV_TRIGGER_PERIOD);
			pEdit->SetLabelText(_T("Timer Period"));
			CString txt;
			txt.Format(_T("Enter the period for the Software Trigger, in milliseconds (minimum %d milliseconds)."), SVSoftwareTriggerDefaults::MinTimerPeriod);
			pEdit->SetInfoText(txt);
			
			pEdit->SetItemValue(m_TriggerObj.GetTimerPeriod());
		}
		pRoot->Select(true);
		pRoot->Expand();
	}
}

void CSVOPropertyPageDlg::SetupPPQ()
{
	SetTitle(m_PPQObj.GetPPQName());
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("PPQ"));
		pRoot->SetInfoText(_T("Define a PPQ"));

		// PPQMode
		SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		if (pCombo)
		{
			long lMode = (long)m_PPQObj.GetPPQMode();
			int	nIndex;
			pCombo->SetCtrlID(PROP_PPQ_MODE);
			pCombo->SetLabelText(_T("Output Mode"));
			switch (lMode)
			{//InfoText needs to be set here as well as in OnItemChanged
			case 0:
				{//next trigger
					pCombo->SetInfoText(_T("Next Trigger Mode - in this mode outputs are written on each trigger." ));
					break;
				}
			case 1:
				{//time delay
					pCombo->SetInfoText(_T("Time Delay Mode - in this mode outputs are written after the specified "
						"Output Delay Time."));
					break;
				}
			case 2:
				{//time data complete
					pCombo->SetInfoText(_T("Time Delay && Data Completion Mode - in this mode outputs are written "
						"after the specified Output Delay Time and completion of the Inspection."));
					break;
				}
			case 3:
				{//extended time delay
					pCombo->SetInfoText(_T("Extended Time Delay Mode - in this mode outputs are written after the "
						"specified Output Delay Time. This Output Delay Time can exceed the time "
						"distance between triggers."));
					break;
				}
			default:
				{
					break;
				}
			}
			pCombo->CreateComboBox(CBS_DROPDOWNLIST);
			nIndex = pCombo->AddString(PPQ_NEXT_TRG_STR);
			pCombo->SetItemData(nIndex, 0);
			nIndex = pCombo->AddString(PPQ_TIME_DELAY_STR);
			pCombo->SetItemData(nIndex, 1);
			nIndex = pCombo->AddString(PPQ_TIME_DATA_COMPLETE);
			pCombo->SetItemData(nIndex, 2);
			nIndex = pCombo->AddString(PPQ_EXTENDED_TIME_DELAY);
			pCombo->SetItemData(nIndex,3);
			pCombo->SetItemValue(lMode);
			
		}
		//PPQLength
		SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
		int iMode = m_PPQObj.GetPPQMode();
		CString sInfoTxt;
		switch (iMode)
		{
		case 0: //next trigger
			{   
				sInfoTxt = _T("Next Trigger Mode - in this mode the PPQ length must be 2 or greater.");
				break;
			}
		case 1: //time delay
			{
				sInfoTxt = _T("Time Delay Mode - in this mode the PPQ length must be 1.");
				break;
			}
		case 2: //time delay data complete
			{
				sInfoTxt = _T("Time Delay && Data Completion Mode - in this mode the PPQ length must be 1.");
				break;
			}
		case 3: //extended time delay
			{
				sInfoTxt = _T("Extended Time Delay Mode - in this mode the PPQ length must be 1 or greater.");
				break;
			}
		default:
			{
				break;
			}
		}

		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_PPQ_LENGTH);
			pEdit->SetLabelText(_T("Length"));
			pEdit->SetInfoText(sInfoTxt);
			if (m_PPQObj.GetPPQMode() == 0)
			{
				if (m_PPQObj.GetPPQLength() < 2)
				{
					m_PPQObj.SetPPQLength(2);
				}
			}
			bool bResult = pEdit->SetItemValue(m_PPQObj.GetPPQLength());
		}
		
		// PPQOutputDelayTime
		pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_PPQ_OUTPUT_DELAY_TIME);
			pEdit->SetLabelText(_T("Output Delay Time"));
			pEdit->SetInfoText(_T("All Modes - in all modes this is the number of milliseconds between the "
								  "trigger and the setting of the outputs."));
			bool bResult = pEdit->SetItemValue(m_PPQObj.GetPPQOutputDelayTime());
		}
		
		// PPQOutputResetDelayTime
		pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_PPQ_OUTPUT_RESET_DELAY);
			pEdit->SetLabelText(_T("Output Reset Delay Time"));
			pEdit->SetInfoText(_T("All Modes - in all modes this is the number of milliseconds between the "
								  "setting of the outputs and the resetting of the outputs to their defaults. "
									"A value of 0 means there will be no reset."));
			bool bResult = pEdit->SetItemValue(m_PPQObj.GetPPQOutputResetDelay());
		}
		
		//add new property for "Maintain Source Image"  default to FALSE;
		// will be a new CreateComboBoxBool
		pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		if ( pCombo )
		{
			CString sInfoString;
			sInfoString.Format("Enabling this option will maintain source images and published result images "
				"throughout the length of the PPQ. Because of this, "
				"the PPQ Length can not be > %d",TheSVObserverApp.GetMaxPPQLength()); //GetSourceImageDepth() - 2 );
			pCombo->SetCtrlID(PROP_PPQ_MAINTAIN_SRC_IMG);
			pCombo->SetLabelText(_T("Maintain Published Images"));
			pCombo->SetInfoText(sInfoString);
			pCombo->CreateComboBoxBool();
			pCombo->SetItemValue(m_PPQObj.GetMaintainSourceImageProperty());
		}
		
		// add "Inspection Timeout" property
		pEdit = dynamic_cast <SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		if ( pEdit )
		{
			pEdit->SetCtrlID(PROP_PPQ_INSPECTION_TIMEOUT);
			pEdit->SetLabelText(_T("Reserved Inspection Threshold"));
			pEdit->SetInfoText(_T("Unit: milliseconds. "
			                      "Time offset before the end of the Extended Time Delay after which an inspection will not start. "
			                      "Set the value to 0 to disable this feature." ));
			bool bResult = pEdit->SetItemValue( m_PPQObj.GetInspectionTimeout() );
		}

		// Add Conditional Output
		pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		if ( pCombo )
		{
			CString sInfoString;
			sInfoString = _T("Condition to evaluate to determine if Outputs will be written. "
				             "If set to Always, no evaluation is performed and the Outputs are always written.");
			pCombo->SetCtrlID(PROP_PPQ_CONDITIONAL_OUTPUT);
			pCombo->SetLabelText(_T("Conditional Output"));
			pCombo->SetInfoText(sInfoString);
			pCombo->CreateComboBox(CBS_DROPDOWNLIST);
			int nIndex = pCombo->AddString(PPQ_CONDITIONAL_OUTPUT_ALWAYS);
			pCombo->SetItemData(nIndex, nIndex);
			int selection = nIndex;
			
			const SVString& condition = m_PPQObj.GetConditionalOutputName();

			// iterate thru the inputs....
			// if the current conditional output value is not in the list, set it to always
			const SVNameGuidPairList& list = m_PPQObj.GetAvailableInputsForConditionalOutput();
			for (SVNameGuidPairList::const_iterator it = list.begin();it != list.end();++it)
			{
				SVString name = it->first;
				nIndex = pCombo->AddString(name.c_str());
				pCombo->SetItemData(nIndex, nIndex);
				if (name == condition)
				{
					selection = nIndex;
				}
			}
			pCombo->SetItemValue(selection);
		}
		pRoot->Select(true);
		pRoot->Expand();
	}
	PPQHideItems();
}

/////////////////////////////////////////////////////////////////////////////
// CSVOPropertyPageDlg message handlers
BOOL CSVOPropertyPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CRect rc;
	
	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;
	
	// Init the control's size to cover the entire client area
	GetDlgItem(IDC_PROPERTY_TREE)->GetWindowRect(rc);
	ScreenToClient(rc);
	
	// Create SVRPropTree control
	m_Tree.Create(dwStyle, rc, this, IDC_PROPERTYTREE);
	
	// Create a root item (root items should always be SVRPropertyItem object since they
	// can not have properties
	
	bool bResult = false;
	
	m_Tree.SetColumn( (long) ( rc.Width() * 0.30 ) );

	CString	rKey;
	
	switch(m_nID)
	{
		case VIRTUAL_CAMERA_DLG:
			SetupCamera();
			break;

		case VIRTUAL_CAMERA_ADV:
			SetupAdvancedCamera();
            break;

		case VIRTUAL_INSPECT_DLG:
			SetupInspection();
			break;

		case VIRTUAL_TRIGGER_DLG:
			SetupTrigger();
			break;
			
		case VIRTUAL_TRIGGER_ADV:
			SetupAdvancedTrigger();
			break;
			
		case VIRTUAL_PPQ_DLG:
			SetupPPQ();
			break;
	}

	m_Tree.SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSVOPropertyPageDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
    {
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
    }
	else
    {
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSVOPropertyPageDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSVOPropertyPageDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// resize the control to always fit the dialog
	if (::IsWindow(m_Tree.GetSafeHwnd()))
		m_Tree.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE|SWP_NOZORDER);	
}

BOOL CSVOPropertyPageDlg::OnEraseBkgnd(CDC* pdc) 
{
    CDialog::OnEraseBkgnd(pdc);
	// don't bother erasing the background since our control will always
	// cover the entire client area
	return TRUE;
}

void CSVOPropertyPageDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;
	*plResult = S_OK;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
		{
			SVDeviceParamEnum e = (SVDeviceParamEnum) (pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);

			SVDeviceParamCollection& params = m_CameraObj.GetCameraDeviceParamsNonConst();
			const SVDeviceParamCollection& l_CamFileParams = m_CameraObj.GetCameraFileParams();
			SVDeviceParamWrapper& w = params.GetParameter(e);
			const SVDeviceParamWrapper& wCF = l_CamFileParams.Parameter( e );

			SVDeviceParam* pDeviceParam = w;
			const SVDeviceParam* pFileDeviceParam = wCF;

			ASSERT( pDeviceParam );
			ASSERT( pFileDeviceParam );

			long lValue;

			// If Custom get real DeviceParam
			if (pDeviceParam && pDeviceParam->DataType() == DeviceDataTypeCustom)
			{
				// swap the Container for the real parameters
				const SVCustomDeviceParam* pCustomFileParam = wCF.DerivedValue(pCustomFileParam);
				SVCustomDeviceParam* pCustomParam = w.DerivedValue(pCustomParam);

				pDeviceParam = pCustomParam->GetHeldParam();
				pFileDeviceParam = pCustomParam->GetHeldParam();
			}
			if (pDeviceParam && pFileDeviceParam)
			{
				switch ( pDeviceParam->DataType() )
				{
					case DeviceDataTypeLong:
					{
						SVLongValueDeviceParam* pParam = dynamic_cast<SVLongValueDeviceParam *>( pDeviceParam );
						const SVLongValueDeviceParam* pCFParam = dynamic_cast<const SVLongValueDeviceParam *>( pFileDeviceParam );
						if (pParam && pCFParam)
						{
							pItem->GetItemValue(lValue);
							if ( ( pCFParam->info.max == 0 && pCFParam->info.min == 0 )
								|| (lValue >= pCFParam->GetScaledMin() && lValue <= pCFParam->GetScaledMax() ) )
							{
								pParam->SetScaledValue(lValue);
								long lRecalcValue = pParam->GetScaledValue();
								if( lRecalcValue != lValue)
								{
									pItem->SetItemValue(lRecalcValue);
								}
							}
							else
							{
								MessageBeep(MB_ICONASTERISK);
								*plResult = S_FALSE;
							}
						}
						break;
					}
					case DeviceDataTypei64:
					{
						// this does nothing so we can't edit I64 values....
						//SVi64ValueDeviceParam* pParam = w.DerivedValue( pParam );
						break;
					}
					case DeviceDataTypeBool:
					{
						SVBoolValueDeviceParam* pParam = dynamic_cast<SVBoolValueDeviceParam*>( pDeviceParam );
						if (pParam)
						{
							pItem->GetItemValue(lValue);
							*pParam = lValue != 0;
						}
						break;
					}
					case DeviceDataTypeString:
					{
						switch ( pDeviceParam->Type() )
						{
							case DeviceParamCameraFormats:
							{
								SVCameraFormatsDeviceParam* pParam = dynamic_cast<SVCameraFormatsDeviceParam*>( pDeviceParam  );
								if (pParam)
								{
									pItem->GetItemValue(lValue);
									const SVCameraFormat* pFormat = (const SVCameraFormat*) lValue;
									if ( nullptr != pFormat )
									{
										pParam->strValue = pFormat->m_strName;
										//make sure binning parameters are set to 1 (no binning)
										if (!pFormat->bVariableROI)
										{
											//UpdateCameraGigeBinningParameters
											if (params.ParameterExists(DeviceParamVerticalBinning))
											{
												SVLongValueDeviceParam* pParam(NULL);
												params.GetParameter(DeviceParamVerticalBinning).GetDerivedValue(pParam);
												pParam->lValue = 1;
												params.SetParameter(DeviceParamVerticalBinning,pParam);
											}
											if (params.ParameterExists(DeviceParamHorizontalBinning))
											{
												SVLongValueDeviceParam* pParam(NULL);
												params.GetParameter(DeviceParamHorizontalBinning).GetDerivedValue(pParam);
												pParam->lValue = 1;
												params.SetParameter(DeviceParamHorizontalBinning,pParam);
											}										
										}
									}
								}
							}
							break;

							default:
							{
								SVStringValueDeviceParam* pParam = dynamic_cast<SVStringValueDeviceParam*>( pDeviceParam  );
								if (pParam)
								{
									SVRPropertyItemCombo* l_pCombo = dynamic_cast< SVRPropertyItemCombo* >( pItem );

									if( l_pCombo != NULL )
									{
										int l_Index = l_pCombo->GetCurSel();
										if( -1 < l_Index )
										{
											CString l_Desc;

											l_pCombo->GetLBText( l_Index, l_Desc );

											SVStringValueDeviceParam::OptionsType::const_iterator iterOption;
											for (iterOption = pParam->info.options.begin(); iterOption != pParam->info.options.end(); ++iterOption)
											{
												CString sText = iterOption->strDescription.c_str();

												if( l_Desc == sText )
												{
													pParam->strValue = iterOption->value;
													break;
												}
											}
										}
									}
								}
							}
							break;
						}// end switch ( pDeviceParam->Type() )
					}// end case DeviceDataTypeString:

					case DeviceDataTypeComplex:
					{
						// put a "..." button to pull up custom dialog (e.g. LUT or LR)
						break;
					}

					default:
						break;
				}// end switch ( w->DataType() )
			}
			if (IsGigeSystem())
			{
				SVGigeCameraParamValidateClass l_svValidate;

				if ( l_svValidate.ShouldUpdateParams( e ) )
				{
					l_svValidate.UpdateParams( m_CameraObj.GetCameraFileParamsNonConst(),
												 m_CameraObj.GetCameraDeviceParamsNonConst() );

					CameraAdvancedHideItems();
				}
			}
			else
			{
				SV1394CameraParamValidateClass l_svValidate;

				if ( l_svValidate.ShouldUpdateParams( e ) )
				{
					l_svValidate.UpdateParams( m_CameraObj.GetCameraFileParamsNonConst(),
												 m_CameraObj.GetCameraDeviceParamsNonConst() );

					CameraAdvancedHideItems();
				}
			}
		}
		else
		{
			UINT ctrlID = pNMPropTree->pItem->GetCtrlID();
			switch (ctrlID)
			{
				case PROP_AD_FILE_NAME:
				{
					CString sFileName;
					m_Tree.FindItem(PROP_AD_FILE_NAME)->GetItemValue(sFileName);
					sFileName.TrimLeft();
					sFileName.TrimRight();
					m_CameraObj.SetCameraFile(sFileName);
					m_CameraObj.SetCameraFileChanged();
					if( m_CameraObj.GetCameraFileChanged()  && m_InitialCameraFileName.Compare( m_CameraObj.GetCameraFile()) != 0 )
						GetDlgItem(IDOK)->SetWindowText(_T("OK"));
					else
					{
						if( m_InitialCameraFileName.GetLength() > 0 )
							GetDlgItem(IDOK)->SetWindowText(_T("Reset"));
					}
					break;
				}

				case PROP_ADV_CAMERA_DIG:
				{
					int iBands;
					long iDig;
					m_Tree.FindItem(PROP_ADV_CAMERA_DIG)->GetItemValue(iDig);
					m_CameraObj.SetDigNumber(iDig); 
					if (m_eProduct == SVIM_PRODUCT_RGB_MONO)
					{
						m_Tree.FindItem(PROP_ADV_CAMERA_BANDS)->GetItemValue(iBands);
						m_CameraObj.SetBandNumber(iBands);
					}
					break;
				}
				
				// for File Acquisition
				case PROP_CAMERA_TYPE_FILE_OR_REAL:
					{
						long lMode;
						m_Tree.FindItem(ctrlID)->GetItemValue(lMode);
						m_CameraObj.SetFileAcquisitionMode((lMode) ? true : false);
						if( lMode == TRUE)
						{
							GetDlgItem(IDOK)->SetWindowText(_T("OK"));
						}
						ShowCameraProperties();
						break;
					}

				case PROP_CAMERA_COLOR:
					{
						long lMode;
						m_Tree.FindItem(ctrlID)->GetItemValue(lMode);
						m_CameraObj.SetIsColor((lMode) ? true : false);
						break;
					}

				case PROP_FILECAMERA_FILENAME:
				{
					CString filename;
					m_Tree.FindItem(ctrlID)->GetItemValue(filename);
					m_CameraObj.SetImageFilename(SVString(filename));
					UpdateFileImageSize();
					GetDlgItem(IDOK)->SetWindowText(_T("OK"));
					break;
				}

				case PROP_FILECAMERA_DIRECTORY:
				{
					CString directory;
					m_Tree.FindItem(ctrlID)->GetItemValue(directory);
					m_CameraObj.SetImageDirectoryName(SVString(directory));
					UpdateFileImageSize();
					break;
				}

				case PROP_FILECAMERA_MODE:
				{
					long lMode;
					m_Tree.FindItem(ctrlID)->GetItemValue(lMode);
					m_CameraObj.SetFileLoadingMode(lMode);
					UpdateFileImageSize();
					break;
				}

				case PROP_ADV_FILECAMERA_IMAGESIZEEDITMODE:
				{
					long lMode;
					m_Tree.FindItem(ctrlID)->GetItemValue(lMode);
					m_CameraObj.SetFileImageSizeEditModeFileBased(lMode ? false : true);
					SetImageSizeEditAttributes();
					UpdateFileImageSize();
					break;
				}

				case PROP_ADV_FILECAMERA_IMAGEWIDTH:
				{
					long lWidth;
					m_Tree.FindItem(ctrlID)->GetItemValue(lWidth);
					// validate against minimum
					if (lWidth < 4)
					{
						MessageBeep(MB_ICONASTERISK);
						*plResult = S_FALSE;
					}
					else
					{
						m_CameraObj.SetFileImageWidth(lWidth);
					}
					break;
				}

				case PROP_ADV_FILECAMERA_IMAGEHEIGHT:
				{
					long lHeight;
					m_Tree.FindItem(ctrlID)->GetItemValue(lHeight);
					// validate against minimum
					if (lHeight < 4)
					{
						MessageBeep(MB_ICONASTERISK);
						*plResult = S_FALSE;
					}
					else
					{
						m_CameraObj.SetFileImageHeight(lHeight);
					}
					break;
				}

				case PROP_TRG_TYPE:
				{
					UINT lType = 0;
					SVRPropertyItem* l_Item = m_Tree.FindItem( ctrlID );
					l_Item->GetItemValue( lType );
					m_TriggerObj.SetSoftwareTrigger( lType ? true : false );
					break;
				}

				case PROP_ADV_TRIGGER_PERIOD:
				{
					long lType;
					m_Tree.FindItem(ctrlID)->GetItemValue(lType);

					// validate against minimum timer period
					if (lType < SVSoftwareTriggerDefaults::MinTimerPeriod)
					{
						MessageBeep(MB_ICONASTERISK);
						*plResult = S_FALSE;
					}
					else
					{
						m_TriggerObj.SetTimerPeriod(lType);
					}
					break;
				}

				case PROP_INS_TOOLSET_IMAGE:
				{
					CString sImage;
					SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*) m_Tree.FindItem( PROP_INS_TOOLSET_IMAGE );
					
					long lIndex = pCombo->GetCurSel();
					if( lIndex != LB_ERR )
						pCombo->GetLBText( lIndex, sImage );
					m_InspectionObj.SetToolsetImage( sImage );
					break;
				}

				case PROP_INS_NEW_DISABLE_METHOD:
				{
					CString sDisable;
					long lIndex;
					SVRPropertyItemCombo* pCombo;

					pCombo = (SVRPropertyItemCombo*) m_Tree.FindItem( PROP_INS_NEW_DISABLE_METHOD );
					lIndex = pCombo->GetCurSel();
					if( lIndex != LB_ERR )
						pCombo->GetLBText( lIndex, sDisable );
					m_InspectionObj.SetNewDisableMethod( sDisable );
					break;
				}

				case PROP_INS_ENABLE_AUX_EXTENT:
				{
					CString sEnable;
					long lIndex;
					SVRPropertyItemCombo* pCombo;

					pCombo = (SVRPropertyItemCombo*) m_Tree.FindItem( PROP_INS_ENABLE_AUX_EXTENT );
					lIndex = pCombo->GetCurSel();
					if( lIndex != LB_ERR )
						pCombo->GetLBText( lIndex, sEnable );
					m_InspectionObj.SetEnableAuxiliaryExtent( sEnable == _T("True") );
					break;
				}

				case PROP_PPQ_MODE:
				{
					long iVal;
					m_Tree.FindItem(PROP_PPQ_MODE)->GetItemValue(iVal);
					m_PPQObj.SetPPQMode(iVal);
					switch (iVal)
					{// InfoTxt for PPQ_MODE and Length needs to be the same
					 // as it is in the OnInitDlg
						case 0:
						{//next trigger
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetInfoText(_T("Next Trigger Mode - in this mode the PPQ length must be 2 or greater."));
        					m_Tree.FindItem(PROP_PPQ_MODE)->SetInfoText(_T("Next Trigger Mode - in this mode outputs are written on each trigger." ));
							break;
						}
						case 1:
						{//time delay
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetInfoText(_T("Time Delay Mode - in this mode the PPQ length must be 1."));
        					m_Tree.FindItem(PROP_PPQ_MODE)->SetInfoText(_T("Time Delay Mode - in this mode outputs are written after the specified "
																		   "Output Delay Time." ));
							break;
						}
						case 2:
						{//time data complete
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetInfoText(_T("Time Delay && Data Completion Mode - in this mode the PPQ length must be 1."));
        					m_Tree.FindItem(PROP_PPQ_MODE)->SetInfoText(_T("Time Delay && Data Completion Mode - in this mode outputs are written "
																		   "after the specified Output Delay Time and completion of the Inspection." ));
							break;
						}
						case 3:
						{//extended time delay
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetInfoText(_T("Extended Time Delay Mode - in this mode the PPQ length must be 1 or greater."));
        					m_Tree.FindItem(PROP_PPQ_MODE)->SetInfoText(_T("Extended Time Delay Mode - in this mode outputs are written after the "
																		   "specified Output Delay Time. This Output Delay Time can exceed the time "
																		   "distance between triggers." ));
							break;
						}
						default:
						{
							break;
						}
					}
					//get the PPQLength
					int iPPQLength;
					m_Tree.FindItem(PROP_PPQ_LENGTH)->GetItemValue(iPPQLength);
    
					if (iVal == 0) //it is NextTrigger
					{//make sure PPQLength is > 2)
						if (iPPQLength < 2)
						{
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(2);
							iPPQLength = 2;
						}                        
					}
					if ( (iVal == 1) || (iVal == 2) )
					{ //make sure value is 1
						if (iPPQLength != 1)
						{
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(1);
							iPPQLength = 1;
						}
					}
					if ( iVal == 3 )
					{
						if (iPPQLength < 1)
						{
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(1);
							iPPQLength = 1;
						}
					}
					m_PPQObj.SetPPQLength(iPPQLength);

					PPQHideItems();
					//m_Tree.FindItem(PROP_PPQ_MODE)->HideItem();
					break;
				}

				case PROP_PPQ_LENGTH:
				{
					long lPPQMode;
					CString sTxt;
					int iLen;
					m_Tree.FindItem(PROP_PPQ_LENGTH)->GetItemValue(iLen);
					m_Tree.FindItem(PROP_PPQ_MODE)->GetItemValue(lPPQMode);
                
					if (lPPQMode == 0/*NextTrigger*/)
					{
						//see if iLen is > 2
						if (iLen < 2)
						{
							iLen = 2;
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(iLen);
						}
						if (iLen > 300)
						{
							iLen = 300;
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(iLen);
						}
					}
					if ( (lPPQMode == 1) || (lPPQMode == 2) )
					{
						//mode is either TimeDelay or TimeDelayDataCompletion
						//only allow value of 1...
						if (iLen != 1)
						{
							iLen = 1;
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(iLen);
						}
					}
					if (lPPQMode == 3)
					{
						if (iLen < 1)
						{
							iLen = 1;
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(iLen);
						}
					}
					m_PPQObj.SetPPQLength(iLen);
					PPQHideItems();
					break;
				}

				case PROP_PPQ_OUTPUT_RESET_DELAY:
				{
					PPQHideItems();
					long lDelay;
					m_Tree.FindItem(PROP_PPQ_OUTPUT_RESET_DELAY)->GetItemValue(lDelay);
					if (lDelay < 0)
					{
						lDelay = 0;
						m_Tree.FindItem(PROP_PPQ_OUTPUT_RESET_DELAY)->SetItemValue(lDelay);
					}
					m_PPQObj.SetPPQOutputResetDelay(lDelay);
					break;
				}

				case PROP_PPQ_OUTPUT_DELAY_TIME:
				{
					int iTime;  
					//value can either be 0 or >=10
					m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->GetItemValue(iTime);
					if (iTime < 0)
					{ //set value back to 0
						iTime = 0;
						m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->SetItemValue(iTime);
					}
					m_PPQObj.SetPPQOutputDelayTime(iTime);
					PPQHideItems();
					break;
				}

				case PROP_PPQ_MAINTAIN_SRC_IMG:
				{
					long iVal;
					m_Tree.FindItem(PROP_PPQ_MAINTAIN_SRC_IMG)->GetItemValue(iVal);
					m_PPQObj.SetMaintainSourceImageProperty( iVal != 0 );
					break;
				}

				case PROP_PPQ_INSPECTION_TIMEOUT:
				{
					long lVal;
					m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->GetItemValue(lVal);
					m_PPQObj.SetInspectionTimeout(lVal);
					break;
				}

				case PROP_PPQ_CONDITIONAL_OUTPUT:
				{
					SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo *)m_Tree.FindItem(PROP_PPQ_CONDITIONAL_OUTPUT);
					long lIndex = pCombo->GetCurSel();
					if (lIndex != LB_ERR)
					{
						CString Value;
						pCombo->GetLBText(lIndex, Value);
						m_PPQObj.SetConditionalOutputName(SVString(Value));
					}
					break;
				}

				default:
				{
					break;
				}
			}// end switch (pNMPropTree->pItem->GetCtrlID())
		}// end if ( pNMPropTree->pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE ) else
	}// end if ( pNMPropTree->pItem )
}

void CSVOPropertyPageDlg::OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = FALSE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
		{

			SVDeviceParamEnum e = (SVDeviceParamEnum) (pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);
			const SVDeviceParamCollection& params = m_CameraObj.GetCameraFileParams();
			const SVDeviceParamWrapper& w = params.Parameter(e);
			const SVDeviceParam* pDeviceParam = w;
			switch ( pDeviceParam->Type() )
			{
				case DeviceParamCameraFormats:
				{
					long lValue;
					pItem->GetItemValue(lValue);
					const SVCameraFormat* pFormat = (const SVCameraFormat*) lValue;
					if ( pFormat )
					{
						if ( pFormat->bVariableROI )
						{
							*plResult = TRUE;
						}// end if ( pFormat->bVariable )
					}// end if ( pFormat )
				}// end case DeviceParamCameraFormats:
			}// end switch ( pDeviceParam->Type() )
		}// end if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
	}// end if ( pNMPropTree->pItem )
}

void CSVOPropertyPageDlg::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
		{
			SVDeviceParamEnum e = (SVDeviceParamEnum) (pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);
			SVDeviceParamCollection& rCamDeviceParams = m_CameraObj.GetCameraDeviceParamsNonConst();
			ASSERT( rCamDeviceParams.ParameterExists(e) );
			SVDeviceParamWrapper& rw = rCamDeviceParams.GetParameter(e);
			SVDeviceParam* pDeviceParam = rw;
			switch ( pDeviceParam->Type() )
			{
				case DeviceParamCameraFormats:
				{
					long lValue;
					pItem->GetItemValue(lValue);
					const SVCameraFormat* pSelectedFormat = (const SVCameraFormat*) lValue;
					if ( pSelectedFormat )
					{
						if ( pSelectedFormat->bVariableROI )
						{
							AdjustCameraImageFormat( pSelectedFormat->m_strName.c_str(), rw );
						}// end if ( pFormat->bVariable )
					}// end if ( pFormat )
				}// end case DeviceParamCameraFormats:
			}// end switch ( pDeviceParam->Type() )
		}// end if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
	}// end if ( pNMPropTree->pItem )
}

static HRESULT ShowFormat7Dlg( SVCameraFormat& p_rFormat, const SVImageInfoClass& p_rImageInfo, SVAcquisitionClassPtr p_pDevice, SVCameraFormat& rCameraDeviceFormat )
{
	HRESULT hr = S_FALSE;
	SVCameraFormat7Dlg dlg;
	dlg.SetFormat( &p_rFormat );
	dlg.SetFormat7Image( p_rImageInfo );
	dlg.SetAcquisitionDevice( p_pDevice );
	if ( IDOK == dlg.DoModal() )
	{
		rCameraDeviceFormat.AssignUserChangableValues( p_rFormat );
		hr = S_OK;
	}
	return hr;
}

static HRESULT ShowGigeROIDlg(ISVCameraDeviceImageFormatUpdater& rImageFormatUpdater, SVCameraFormat& p_rFormat, const SVImageInfoClass& p_rImageInfo, SVAcquisitionClassPtr p_pDevice, SVCameraFormat& rCameraDeviceFormat, SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_FALSE;
	SVGigeCameraROIDlg dlg(rImageFormatUpdater);
	dlg.SetFormat( &p_rFormat );
	dlg.SetFormatImage( p_rImageInfo );
	dlg.SetAcquisitionDevice( p_pDevice );

	// check if binning is supported
	// if so get value, min and max for vertical and horizontal
	if (rDeviceParams.ParameterExists(DeviceParamVerticalBinning))
	{
		SVLongValueDeviceParam* pParam(NULL);
		rDeviceParams.GetParameter(DeviceParamVerticalBinning).GetDerivedValue(pParam);
		dlg.SetVerticalBinningParam(pParam);
	}
	if (rDeviceParams.ParameterExists(DeviceParamHorizontalBinning))
	{
		SVLongValueDeviceParam* pParam(NULL);
		rDeviceParams.GetParameter(DeviceParamHorizontalBinning).GetDerivedValue(pParam);
		dlg.SetHorizontalBinningParam(pParam);
	}

	if ( IDOK == dlg.DoModal() )
	{
		rCameraDeviceFormat.AssignUserChangableValues( p_rFormat );
		hr = S_OK;
	}
	return hr;
}

static void UpdateCameraGigeBinningParameters(const SVDeviceParamCollection& rDeviceParams, SVDeviceParamCollection& rCamDeviceParams)
{
	if (rDeviceParams.ParameterExists(DeviceParamVerticalBinning))
	{
		SVDeviceParamWrapper l_VerticalBinningParam = rDeviceParams.Parameter(DeviceParamVerticalBinning);
		rCamDeviceParams.SetParameter(l_VerticalBinningParam); 
	}
	if (rDeviceParams.ParameterExists(DeviceParamHorizontalBinning))
	{
		SVDeviceParamWrapper l_HorizontalBinningParam = rDeviceParams.Parameter(DeviceParamHorizontalBinning);
		rCamDeviceParams.SetParameter(l_HorizontalBinningParam); 
	}
}

static void ApplyGigeImageFormatChange(SVAcquisitionClassPtr pAcqDevice, const SVDeviceParamCollection& rDeviceParams)
{
	SVDeviceParamCollection l_params;
	if (rDeviceParams.ParameterExists(DeviceParamVerticalBinning))
	{
		SVDeviceParamWrapper l_VerticalBinningParam = rDeviceParams.Parameter(DeviceParamVerticalBinning);
		l_params.SetParameter(l_VerticalBinningParam);
	}
	if (rDeviceParams.ParameterExists(DeviceParamHorizontalBinning))
	{
		SVDeviceParamWrapper l_HorizontalBinningParam = rDeviceParams.Parameter(DeviceParamHorizontalBinning);
		l_params.SetParameter(l_HorizontalBinningParam);
	}
	SVDeviceParamWrapper wFormats = rDeviceParams.Parameter( DeviceParamCameraFormats );
	l_params.SetParameter( wFormats );

	pAcqDevice->SetDeviceParameters(l_params);
}

// For 1394 Format 7 and GigE ROI adjustments
HRESULT CSVOPropertyPageDlg::AdjustCameraImageFormat( LPCTSTR sSelectedFormat, SVDeviceParamWrapper& rw)
{
	HRESULT hr = S_OK;

	SVDeviceParamEnum e = rw->Type();
	SVCameraFormatsDeviceParam* pCameraDeviceParam = rw.DerivedValue( pCameraDeviceParam );
	const SVDeviceParamCollection& rCameraFileParams = m_CameraObj.GetCameraFileParams();
	const SVDeviceParamWrapper& rwCameraFileFormats = rCameraFileParams.Parameter( e );
	const SVCameraFormatsDeviceParam* pCameraFileFormats = rwCameraFileFormats.DerivedValue( pCameraFileFormats );

	ASSERT( pCameraDeviceParam->options.find( sSelectedFormat ) != pCameraDeviceParam->options.end() );
	SVCameraFormat& rCameraDeviceFormat = pCameraDeviceParam->options[sSelectedFormat];
	
	ASSERT( pCameraFileFormats->options.find( sSelectedFormat ) != pCameraFileFormats->options.end() );
	SVCameraFormat format( pCameraFileFormats->options.find( sSelectedFormat )->second );
	format.AssignUserChangableValues( rCameraDeviceFormat );

	long l_WidthMax = format.lWidthMax;
	long l_HeightMax = format.lHeightMax;

	bool bHaveImage = false;
	bool bChangedAcqFormat = false;
	CString sCurrentFormat;
	SVImageInfoClass l_ImageInfo;

	CString sDeviceName = m_pAssistant->BuildDigName( m_CameraObj );
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	SVInspectionProcessArray aInspections;
	SVDeviceParamCollection l_CurrentParams;

	SVAcquisitionClassPtr pDevice( SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( sDeviceName ) );
	
	if( nullptr != pDevice )
	{
		long l_lSize = 0;

		if( pDevice->GetFileNameArraySize( l_lSize ) == S_OK && l_lSize < 1 )
		{
			SVFileNameArrayClass svFiles;
			SVFileNameClass svFile;

			CString sCameraFileName = m_CameraObj.GetCameraFile();

			svFile.SetFullFileName(sCameraFileName);

			svFiles.Add(svFile);

			pDevice->LoadFiles( svFiles );
		}

		long l_lWidth = 0;
		long l_lHeight = 0;

		SVImageInfoClass info;

		pDevice->GetImageInfo( &info );

		info.GetExtentProperty( SVExtentPropertyWidth, l_lWidth );
		info.GetExtentProperty( SVExtentPropertyHeight, l_lHeight );

		// disconnect buffers attached to dig
		long lInspectionCount = 0;
		SVInspectionProcess* pInspection( nullptr );
		//If the pointer is nullptr then count is 0		
		if( nullptr != pConfig ){ lInspectionCount = pConfig->GetInspectionCount( ); }


		for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )
		{
			bool bHasCamera = false;
			pInspection = pConfig->GetInspection( lInspection );
			if ( nullptr != pInspection )
			{
				bool bFoundCamera=false;
				SVPPQObject* pPPQ = pInspection->GetPPQ();

				if( nullptr != pPPQ )
				{
					std::deque< SVVirtualCamera* > l_Cameras;

					pPPQ->GetCameraList( l_Cameras );

					std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

					while( l_Iter != l_Cameras.end() )
					{
						SVVirtualCamera* pCamera = ( *l_Iter );

						if ( nullptr != pCamera && pCamera->mpsvDevice == pDevice )
						{
							bFoundCamera = true;
						}

						++l_Iter;
					}
				}

				if ( bFoundCamera )
				{
					pInspection->DisconnectToolSetMainImage();
					aInspections.Add( pInspection );
				}
			}
		}

		// get Current format
		pDevice->GetDeviceParameters( l_CurrentParams );

		// Update Image format for max image
		SVCameraDeviceImageFormatUpdater imageFormatUpdater(rw, format.m_strName, l_WidthMax, l_HeightMax);
		imageFormatUpdater.Update(pDevice);

		bChangedAcqFormat = true;

		// Get image info for single grab buffer
		pDevice->GetImageInfo(&l_ImageInfo);	//&&& this won't necessarily work with cameras that support color

		if (IsGigeSystem())
		{
			// Adjust for Binning
			long verticalBinning = 1;
			long horizontalBinning = 1;

			if (l_CurrentParams.ParameterExists(DeviceParamVerticalBinning))
			{
				const SVLongValueDeviceParam* pParam = l_CurrentParams.Parameter(DeviceParamVerticalBinning).DerivedValue(pParam); 
				if (pParam && pParam->lValue > 0)
				{
					verticalBinning = pParam->lValue;
				}
			}
			if (l_CurrentParams.ParameterExists(DeviceParamHorizontalBinning))
			{
				const SVLongValueDeviceParam* pParam = l_CurrentParams.Parameter(DeviceParamHorizontalBinning).DerivedValue(pParam);
				if (pParam && pParam->lValue > 0)
				{
					horizontalBinning = pParam->lValue; 
				}
			}
			l_WidthMax = format.lWidthMax / horizontalBinning;
			l_HeightMax = format.lHeightMax / verticalBinning;
			
			l_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, l_WidthMax);
			l_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, l_HeightMax);
		}
		else
		{
			l_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, l_WidthMax );
			l_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, l_HeightMax );
		}

		if ( hr == S_OK )
		{
			bHaveImage = true;
		}
	}

	if (IsGigeSystem())
	{
		SVCameraDeviceImageFormatUpdater imageFormatUpdater(rw, format.m_strName, l_WidthMax, l_HeightMax);
		if (ShowGigeROIDlg(imageFormatUpdater, format, l_ImageInfo, pDevice, rCameraDeviceFormat, l_CurrentParams) == S_OK)
		{
			if ( bChangedAcqFormat )
			{
				SVDeviceParamCollection& rCamDeviceParams = m_CameraObj.GetCameraDeviceParamsNonConst();
				UpdateCameraGigeBinningParameters(l_CurrentParams, rCamDeviceParams);
				ApplyGigeImageFormatChange(pDevice, l_CurrentParams);
			}
		}
	}
	else
	{
		ShowFormat7Dlg(format, l_ImageInfo, pDevice, rCameraDeviceFormat);
		if ( bChangedAcqFormat )
		{
			// realloc in original format
			SVDeviceParamCollection l_params;
			SVDeviceParamWrapper wFormats = l_CurrentParams.Parameter( DeviceParamCameraFormats );
			l_params.SetParameter( wFormats );
			pDevice->SetDeviceParameters(l_params);
		}
	}
	
	if ( bChangedAcqFormat )
	{
		// reconnect buffers
		for ( long lInspection = 0; lInspection < aInspections.GetSize(); lInspection++ )
		{
			aInspections.GetAt(lInspection)->ConnectToolSetMainImage();
		}
	}
	return hr;
}

void CSVOPropertyPageDlg::OnClose() 
{
	CDialog::OnClose();
}

void CSVOPropertyPageDlg::CameraAdvancedHideItems()
{
	SVRPropertyItem* l_pItem = m_Tree.GetRootItem();

	if ( l_pItem != NULL )
	{
		l_pItem = l_pItem->GetChild();
	}

	if ( l_pItem != NULL )
	{
		l_pItem = l_pItem->GetChild();
	}

	while ( l_pItem != NULL )
	{
		SVDeviceParamEnum l_eParamId = (SVDeviceParamEnum) (l_pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);

		const SVDeviceParamCollection& l_CamFileParams = m_CameraObj.GetCameraFileParams();

		const SVDeviceParam *l_pDeviceParam = l_CamFileParams.Parameter( l_eParamId );

		if ( l_pDeviceParam != NULL )
		{
			l_pItem->HideItem( 0 < l_pDeviceParam->DetailLevel() );
		}

		l_pItem = l_pItem->GetSibling();
	}
}

void CSVOPropertyPageDlg::PPQHideItems()
{
    //get PPQMode
    long lPPQMode;

    m_Tree.FindItem(PROP_PPQ_MODE)->GetItemValue(lPPQMode);
    switch(lPPQMode)
    {
        case 0:
        {
            //NextTrigger mode - hide OutputDelay
            //m_Tree.FindItem(PROP_PPQ_OUTPUT_RESET_DELAY)->HideItem();
            m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->HideItem();
			m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->HideItem();
            break;
        }
        case 1: //timedelay
        case 2: //timedelaydatacomplete
		{
			m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->HideItem(FALSE);
			m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->HideItem();
			break;
		}

        case 3: //extendedtimedelay
        {
            //m_Tree.FindItem(PROP_PPQ_OUTPUT_RESET_DELAY)->HideItem(FALSE);
            m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->HideItem(FALSE);
			m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->HideItem(FALSE);
            break;
        }
        default:
            break;
    }//end of switch...
}

void CSVOPropertyPageDlg::TriggerHideItems()
{
}

void CSVOPropertyPageDlg::SetDlgPage(int nID)
{
    m_nID = nID;
}

void CSVOPropertyPageDlg::SetProductType(SVIMProductEnum eType)
{
    m_eProduct = eType;
}

void CSVOPropertyPageDlg::SetTitle(CString sName)
{
    CString sTmp = _T("Property Settings  -  ")+sName;
    SetWindowText(sTmp);
}

void CSVOPropertyPageDlg::SetConfigAssistantDlg(CSVOConfigAssistantDlg* pAssistant)
{
	m_pAssistant = pAssistant;
}

bool CSVOPropertyPageDlg::IsGigeSystem() const
{
	return ( m_eProduct == SVIM_PRODUCT_X2_GD1A 
		|| m_eProduct == SVIM_PRODUCT_X2_GD1A_COLOR
		|| m_eProduct == SVIM_PRODUCT_X2_GD2A 
		|| m_eProduct == SVIM_PRODUCT_X2_GD2A_COLOR
		|| m_eProduct == SVIM_PRODUCT_X2_GD4A 
		|| m_eProduct == SVIM_PRODUCT_X2_GD4A_COLOR
		|| m_eProduct == SVIM_PRODUCT_X2_GD8A 
		|| m_eProduct == SVIM_PRODUCT_X2_GD8A_COLOR
		|| m_eProduct == SVIM_PRODUCT_X2_GD8A_NONIO 
		|| m_eProduct == SVIM_PRODUCT_X2_GD8A_NONIO_COLOR) ? true : false;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOPropertyPageDlg.cpp_v  $
 * 
 *    Rev 1.6   18 Sep 2014 13:42:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised due to changes in SVRPropertyItemFile which now uses SVLibrary/SVFileDialog for security purposes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Aug 2014 12:05:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  926
 * SCR Title:  Fix issue with binning staying on (SVO-323)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed OnChangeItem.  If switching out of custom ROI mode, turn binning off.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 11:23:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified setupCameraDeviceParam to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Oct 2013 12:05:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Jun 2013 15:26:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.77   16 May 2013 18:27:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to fix acquisition image size issue when disabling binning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2013 19:46:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.76   10 May 2013 10:58:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Renamed Format7Dlg to AdjustCameraImageFormat
 * Added ShowFormat7Dlg function
 * Added ShowGigeROIDlg function
 * Added UpdateGigeCameraBinningParameters function
 * Added ApplyGigeImageFormatChange function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:11:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.75   01 Apr 2013 16:11:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SetupCameraDeviceParam method
 * Revised SetuipAdvancedCamera to call SetupCameraDeviceParam
 * Revised OnItemChanged to handle custom device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.74   21 Jan 2013 11:33:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Conditional output for PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.73   16 Jan 2013 16:37:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated to use new funcitonality in the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.72   10 Jan 2013 16:18:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.71   13 Dec 2012 10:48:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (SetupAdvancedCamera) Added check for SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsGigeSystem) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   04 Oct 2012 10:49:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   13 Sep 2012 13:18:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  790
 * SCR Title:  Fix property page in the configuration assistant when dealing with GD4A Color
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added the product type SVIM_PRODUCT_X2_GD4A_COLOR to the list of tests for color in SetupAdvancedCamera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68.1.1   12 Dec 2012 19:33:54   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (SetupAdvancedCamera) Added check for SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsGigeSystem) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68.1.0   08 Nov 2012 15:14:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  790
 * SCR Title:  Fix property page in the configuration assistant when dealing with GD4A Color
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added the product type SVIM_PRODUCT_X2_GD4A_COLOR to the list of tests for color in SetupAdvancedCamera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   13 Aug 2012 11:39:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   09 Jul 2012 13:45:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X2_GD4A option for compatibility with this legacy product type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   02 Jul 2012 17:24:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   02 Jul 2012 14:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2B-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   02 Jul 2012 14:06:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed configuration type SVIM X2-GD4A.
 * 
 * Added new configuration type SVIM X2-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   19 Jun 2012 13:22:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   16 Sep 2011 16:14:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   12 Aug 2011 14:16:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added 4 Camera GigE System.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   11 Aug 2011 07:28:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  735
 * SCR Title:  Add Gige Color System
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added system type and product type to support GigE color cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   02 Aug 2011 07:26:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  734
 * SCR Title:  Clarify OK Button on Camera Properties Dialog on Digital Systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Text on OK button to Reset when files are present but not changed. When dialog is used for Digital or GigE Camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   22 Mar 2011 08:01:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   16 Mar 2011 13:45:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   18 Feb 2011 09:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   10 Feb 2011 15:05:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   08 Dec 2010 13:15:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   05 Nov 2010 11:08:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   04 Nov 2010 14:08:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   27 Jul 2010 10:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Frixed problem for string type when using a combo box item on property page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   07 Jul 2010 16:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   15 Feb 2010 12:37:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   16 Dec 2009 14:05:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   10 Nov 2009 12:47:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Gige Color system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   03 Sep 2009 10:35:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   30 Jul 2009 12:14:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   17 Jun 2009 08:54:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsGigeSystem method
 * Revised to support Gige System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   03 Jun 2009 09:59:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   28 Apr 2009 09:20:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  658
 * SCR Title:  Update SVObserver to handle the new model number info
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed code to correctly handle model numbers with no hardware (Laptop version)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   20 Jan 2009 10:40:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for the X2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   05 Dec 2008 15:26:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisiiton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   21 Oct 2008 14:21:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added validation check for image width and height against minimum values for File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   21 Oct 2008 13:20:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Timer period minimum value validation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   07 Oct 2008 16:14:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the Software Trigger Timer period property description.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   03 Oct 2008 17:35:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period for the Software Timer Trigger
 * Added check for Minimum Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 Sep 2008 14:58:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for FileAcqusition setup.
 * Revised SetupCamera to support FileAcquisition.
 * Revised SetupAdvancedCamera to support FileAcquisition.
 * Revised ShowCameraProperties to support FileAcquisition.
 * Revised OnItemChanged to support FileAcquisition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   10 Sep 2008 17:08:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support the general and advanced properties of Software Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   24 Jul 2007 15:17:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   22 Jan 2007 09:29:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new property Inspection Timeout Threshold that will get turned on if the PPQ Mode is Extended Time Delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   29 Nov 2006 15:42:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  583
 * SCR Title:  Add Single Grab capability to the Format7 Dialog in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Format7Dlg method in support of Intek single grab functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   04 Aug 2006 08:55:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  575
 * SCR Title:  Fix Bug dealing with Image locking that causes run once to fail
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed compile errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   04 Aug 2006 08:06:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  575
 * SCR Title:  Fix Bug dealing with Image locking that causes run once to fail
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Replaced function GetSourceImageDepth to GetMaxPPQLength where needed to allow source image depth to ge greater than the ppq.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   28 Mar 2006 14:39:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  550
 * SCR Title:  Fix Format7 problem when changing width of image using the Intek driver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update method name for setting an image for the format7 dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   28 Mar 2006 14:05:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed invalid param bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   19 Jan 2006 11:20:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to hide Aux Extent property when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   08 Nov 2005 16:08:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new Intek 1394 Driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   02 Nov 2005 09:10:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed wording from "Maintain Source Images" to "Maintain Published Images"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   24 Oct 2005 09:40:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   22 Aug 2005 10:24:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   17 Feb 2005 15:09:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   23 Jul 2004 08:55:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnItemChanged and OnInitDialog methods to use the new 1394 camera validation method CameraAdvancedHideItems to show/hide parameters that are not valid when certain trigger modes are used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   12 Jul 2004 11:52:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed tree control code that is no longer necessary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   03 Feb 2004 17:28:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed field name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   12 Jan 2004 10:48:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  404
 * SCR Title:  Change Camera Parameters that represent time to use standard units
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed OnInitDialog() to display units and scaled values.  Changed OnItemChanged(..) to call SetScaledValue(long).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   12 Nov 2003 13:52:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   filter available formats to only allow matching color or b/w options
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   12 Nov 2003 13:50:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Board -> Product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   15 Oct 2003 13:17:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes to edit configuration to track camera file changes so that the default light reference will be read from the camera file only initially or when changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   06 Oct 2003 09:47:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem with OnItemChanged min/max check
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   06 Oct 2003 09:08:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for outside min/max range
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   17 Sep 2003 09:14:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Format 7 handling problem
 * changed parameter display to use VisualName
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Aug 2003 13:43:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SetCanShrink for each "root" item
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Aug 2003 09:04:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed dead code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Aug 2003 12:48:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified code to pop up Format 7 dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Aug 2003 13:51:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Jul 2003 09:48:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include "SVImagingDeviceParams.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Jul 2003 10:51:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for Format7 dialog
 * added support for conditional "..." button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Jul 2003 08:56:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DetailLevel support
 * added CameraFormats param support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2003 10:28:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SetConfigAssistantDlg method and updated SetBoardType, OnItemChanged and OnInitDialog methods for using the new 1394 cameras and new Trigger interfaces.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 13:27:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2003 13:27:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new PPQ property. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
