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
#include "SVGigeCameraROIDlg.h"
#include "SVAcquisitionClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraParamValidateClass.h"
#include "SVPPQObject.h"
#include "SVPPQConstants.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "SVStatusLibrary/MessageManager.h"
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
	, m_pFormatsParam(nullptr)
	, m_strName(strName)
	{
		m_pFormatsParam = m_wFormats.DerivedValue( m_pFormatsParam );
		m_pFormatsParam->options[ strName ].m_lHeight = height;
		m_pFormatsParam->options[ strName ].m_lWidth = width;
		m_pFormatsParam->options[ strName ].m_lHPos = 0;
		m_pFormatsParam->options[ strName ].m_lVPos = 0;
		m_pFormatsParam->strValue = strName;
	}
	virtual ~SVCameraDeviceImageFormatUpdater() {}

	virtual void SetXOffset(long xOffset) 
	{ 
		m_pFormatsParam->options[ m_strName ].m_lHPos = xOffset; 
	}
	virtual void SetYOffset(long yOffset) 
	{ 
		m_pFormatsParam->options[ m_strName ].m_lVPos = yOffset; 
	}
	virtual void SetWidth(long width) 
	{  
		m_pFormatsParam->options[ m_strName ].m_lWidth = width; 
	}
	virtual void SetHeight(long height) 
	{ 
		m_pFormatsParam->options[ m_strName ].m_lHeight = height; 
	}

	void Update(SVAcquisitionClassPtr pAcqDevice)
	{
		if (nullptr != pAcqDevice)
		{
			SVDeviceParamCollection l_params;
			l_params.SetParameter( m_wFormats );
			pAcqDevice->SetDeviceParameters(l_params);

			SVImageInfoClass infoTemp;
			pAcqDevice->GetImageInfo(&infoTemp);
		
			long width = m_pFormatsParam->options[ m_strName ].m_lWidth;
			long height = m_pFormatsParam->options[ m_strName ].m_lHeight;

			infoTemp.SetExtentProperty(SVExtentPropertyWidth, width);
			infoTemp.SetExtentProperty(SVExtentPropertyHeight, height);

			pAcqDevice->DestroyBuffers();
			pAcqDevice->CreateBuffers( infoTemp, TheSVObserverApp.GetSourceImageDepth() );
		}
	}
};

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

CSVOPropertyPageDlg::CSVOPropertyPageDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSVOPropertyPageDlg::IDD, pParent)
,	m_TriggerObj( _T(""), 0 )
,	m_eProduct( SVIM_PRODUCT_TYPE_UNKNOWN )
{
}

CSVOPropertyPageDlg::~CSVOPropertyPageDlg()
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
	if( !m_CameraObj.IsFileAcquisition() )
	{
		if( (m_CameraObj.GetCameraFile().GetLength() > 0) )
		{
			GetDlgItem( IDOK)->SetWindowText( _T("Reset"));
			m_InitialCameraFileName = m_CameraObj.GetCameraFile();
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

		// There is only GigE at this time
		LPCTSTR fileFilter = SVGigeCameraFileFilter;
		
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
									nullptr, FileName.GetDefaultPathName(), true), pRoot);
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
	ASSERT( nullptr != m_pAssistant );
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
			SVRPropertyItemCombo* pCombo = nullptr;

			pRoot->SetCanShrink(false);
			pRoot->SetLabelText(_T("Camera Advanced"));
			pRoot->SetInfoText(_T("Advanced camera properties"));
			
			ASSERT( m_eProduct != SVIM_PRODUCT_TYPE_UNKNOWN );
			if ( SvTi::SVHardwareManifest::IsDigitalSVIM( m_eProduct ) )
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
				SVGigeCameraParamValidateClass l_svValidate;
				l_svValidate.UpdateParams( m_CameraObj.GetCameraFileParamsNonConst(),
					m_CameraObj.GetCameraDeviceParamsNonConst() );
				
				CameraAdvancedHideItems();
			}// end if ( TheSVObserverApp.IsDigitalSVIM() )
			else // should never get here, perhaps there should be an exception logged/displayed instead?
			{
				pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
				if (pCombo)
				{
					pCombo->SetCtrlID(PROP_ADV_CAMERA_DIG);
					pCombo->SetLabelText(_T("Digitizer #"));
					pCombo->SetInfoText(_T("Enter the digitizer number the camera is attached to."));
					pCombo->CreateComboBox();

					CString str;
					int num = m_CameraObj.GetDigNumber();
					str.Format("%d", num);
					int iPos = pCombo->AddString(str);
					pCombo->SetItemData(iPos, num);
					pCombo->SetItemValue(num);
				}// end if (pCombo)
			}
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
								const SVCameraFormat* pFormat = nullptr;
								for (iterOption = pCamFileParam->options.begin(); iterOption != pCamFileParam->options.end(); ++iterOption)
								{
									if ( iterOption->second.m_bColor == ColorCamera )
									{
										CString sText = iterOption->second.m_strDescription.c_str();
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
			txt.Format(_T("Enter the period for the Software Trigger, in milliseconds (minimum %d milliseconds)."), SvTh::MinTimerPeriod);
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
		m_Tree.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE|SWP_NOZORDER);	
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
									_variant_t Value;
									pItem->GetItemValue( Value.GetVARIANT() );
									const SVCameraFormat* pFormat = reinterpret_cast< const SVCameraFormat*> (Value.llVal);
									if ( nullptr != pFormat )
									{
										pParam->strValue = pFormat->m_strName;
										//make sure binning parameters are set to 1 (no binning)
										if (!pFormat->m_bVariableROI)
										{
											//UpdateCameraGigeBinningParameters
											if (params.ParameterExists(DeviceParamVerticalBinning))
											{
												SVLongValueDeviceParam* pParam(nullptr);
												params.GetParameter(DeviceParamVerticalBinning).GetDerivedValue(pParam);
												pParam->lValue = 1;
												params.SetParameter(DeviceParamVerticalBinning,pParam);
											}
											if (params.ParameterExists(DeviceParamHorizontalBinning))
											{
												SVLongValueDeviceParam* pParam(nullptr);
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

									if( nullptr != l_pCombo )
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
					long iDig;
					m_Tree.FindItem(PROP_ADV_CAMERA_DIG)->GetItemValue(iDig);
					m_CameraObj.SetDigNumber(iDig); 
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
					if (lType < SvTh::MinTimerPeriod)
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
					_variant_t Value;
					pItem->GetItemValue(Value);
					const SVCameraFormat* pFormat = reinterpret_cast< const SVCameraFormat*> (Value.llVal);
					if ( nullptr != pFormat )
					{
						//@WARNING [gra][7.40][07.10.2016] SVObserver has crashed before with pFormat being not nullptr so it seems the pointer is corrupt
						if ( pFormat->m_bVariableROI )
						{
							*plResult = TRUE;
						}
					}
					else
					{
						SvStl::MessageMgrStd Exception( SvStl::LogOnly );
						Exception.setMessage( SVMSG_SVO_NULL_POINTER, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
					}
				}
			}
		}
	}
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
					_variant_t Value;
					pItem->GetItemValue(Value);
					const SVCameraFormat* pSelectedFormat = reinterpret_cast< const SVCameraFormat*> (Value.llVal);
					if ( nullptr != pSelectedFormat )
					{
						if ( pSelectedFormat->m_bVariableROI )
						{
							AdjustCameraImageFormat( pSelectedFormat->m_strName.c_str(), rw );
						}// end if ( pFormat->bVariable )
					}// end if ( pFormat )
				}// end case DeviceParamCameraFormats:
			}// end switch ( pDeviceParam->Type() )
		}// end if ( pItem->GetCtrlID() >= PROP_CAMERA_FILE_BASE )
	}// end if ( pNMPropTree->pItem )
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
		SVLongValueDeviceParam* pParam(nullptr);
		rDeviceParams.GetParameter(DeviceParamVerticalBinning).GetDerivedValue(pParam);
		dlg.SetVerticalBinningParam(pParam);
	}
	if (rDeviceParams.ParameterExists(DeviceParamHorizontalBinning))
	{
		SVLongValueDeviceParam* pParam(nullptr);
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

// For GigE ROI adjustments
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

	long l_WidthMax = format.m_lWidthMax;
	long l_HeightMax = format.m_lHeightMax;

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

		if( S_OK == pDevice->GetFileNameArraySize( l_lSize ) && l_lSize < 1 )
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
			l_WidthMax = format.m_lWidthMax / horizontalBinning;
			l_HeightMax = format.m_lHeightMax / verticalBinning;
			
			l_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, l_WidthMax);
			l_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, l_HeightMax);
		}
		else
		{
			l_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, l_WidthMax );
			l_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, l_HeightMax );
		}

		if ( S_OK == hr )
		{
			bHaveImage = true;
		}
	}

	if (IsGigeSystem())
	{
		SVCameraDeviceImageFormatUpdater imageFormatUpdater(rw, format.m_strName, l_WidthMax, l_HeightMax);
		if (S_OK == ShowGigeROIDlg(imageFormatUpdater, format, l_ImageInfo, pDevice, rCameraDeviceFormat, l_CurrentParams))
		{
			if ( bChangedAcqFormat )
			{
				SVDeviceParamCollection& rCamDeviceParams = m_CameraObj.GetCameraDeviceParamsNonConst();
				UpdateCameraGigeBinningParameters(l_CurrentParams, rCamDeviceParams);
				ApplyGigeImageFormatChange(pDevice, l_CurrentParams);
			}
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

	if ( nullptr != l_pItem )
	{
		l_pItem = l_pItem->GetChild();
	}

	if ( nullptr != l_pItem )
	{
		l_pItem = l_pItem->GetChild();
	}

	while ( nullptr != l_pItem )
	{
		SVDeviceParamEnum l_eParamId = (SVDeviceParamEnum) (l_pItem->GetCtrlID() - PROP_CAMERA_FILE_BASE);

		const SVDeviceParamCollection& l_CamFileParams = m_CameraObj.GetCameraFileParams();

		const SVDeviceParam *l_pDeviceParam = l_CamFileParams.Parameter( l_eParamId );

		if ( nullptr != l_pDeviceParam )
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
            m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->HideItem(FALSE);
			m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->HideItem(FALSE);
            break;
        }
        default:
            break;
    }//end of switch...
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

