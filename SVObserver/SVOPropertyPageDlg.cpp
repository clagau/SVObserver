//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVSecurity/SVSecurityManager.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "CameraLibrary/SVDeviceParam.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVi64ValueDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "SVFileAcquisitionDevice/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVOConfigAssistantDlg.h"
#include "SVGigeCameraROIDlg.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraParamValidateClass.h"
#include "SVPPQObject.h"
#include "SVPPQConstants.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "Triggering/SVTriggerConstants.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/AcquisitionName.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemCombo.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVRPropertyTree/SVRPropTreeItemFile.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr std::array<const char*, 5> cTriggerMode
{
	_T("Next Trigger"),
	_T("Time Delay"),
	_T("Time Delay Data Completion"),
	_T("Extended Time Delay"),
	_T("Extended Time Delay Data Completion")
};

constexpr std::array<const char*, 5> cPpqLengthInfo
{
	_T("Next Trigger Mode - in this mode the PPQ length must be 2 or greater."),
	_T("Time Delay Mode - in this mode the PPQ length must be 1."),
	_T("Time Delay && Data Completion Mode - in this mode the PPQ length must be 1."),
	_T("Extended Time Delay Mode - in this mode the PPQ length must be 1 or greater."),
	_T("Extended Time Delay && Data Completion Mode - in this mode the PPQ length must be 1 or greater.")
};

constexpr std::array<const char*, 5> cTriggerModeInfo
{
	_T("Next Trigger Mode - in this mode outputs are written on each trigger."),
	_T("Time Delay Mode - in this mode outputs are written after the specified Output Delay Time."),
	_T("Time Delay && Data Completion Mode - in this mode outputs are written after completion of the Inspection or after the specified Output Delay Time."),
	_T("Extended Time Delay Mode - in this mode outputs are written after the specified Output Delay Time. This Output Delay Time can exceed the time distance between triggers."),
	_T("Extended Time Delay && Data Completion Mode - in this mode outputs are written after completion of the Inspection or after the specified Output Delay Time. This Output Delay Time can exceed the time between triggers.")
};

constexpr std::array<const char*, 3> cTriggerType
{
	_T("Hardware"),
	_T("Software"),
	_T("Camera"),
};

constexpr const char* cGigeCameraFileFilter = _T("Camera Files (*.ogc)|*.ogc||");
constexpr const char* cSequenceCameraFileFilter = _T("SequenceCamera Files (*.seq)|*.seq||");

class SVCameraDeviceImageFormatUpdater : public ISVCameraDeviceImageFormatUpdater
{
private:
	std::string m_strName;
	SVDeviceParamWrapper m_wFormats;
	SVCameraFormatsDeviceParam* m_pFormatsParam; 
	
public:
	SVCameraDeviceImageFormatUpdater(SVDeviceParamWrapper& rw, const std::string& strName, long width, long height) 
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

	virtual void SetXOffset(long xOffset) override
	{ 
		m_pFormatsParam->options[ m_strName ].m_lHPos = xOffset; 
	}
	virtual void SetYOffset(long yOffset) override
	{ 
		m_pFormatsParam->options[ m_strName ].m_lVPos = yOffset; 
	}
	virtual void SetWidth(long width) override
	{  
		m_pFormatsParam->options[ m_strName ].m_lWidth = width; 
	}
	virtual void SetHeight(long height) override
	{ 
		m_pFormatsParam->options[ m_strName ].m_lHeight = height; 
	}

	void Update(SvIe::SVAcquisitionClassPtr pAcqDevice) override
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

			infoTemp.SetExtentProperty(SvPb::SVExtentPropertyWidth, width);
			infoTemp.SetExtentProperty(SvPb::SVExtentPropertyHeight, height);

			pAcqDevice->DestroyBuffers();
			pAcqDevice->CreateBuffers( infoTemp );
		}
	}
};

enum {IDC_PROPERTYTREE = 100};

BEGIN_MESSAGE_MAP(SVOPropertyPageDlg, CDialog)
	//{{AFX_MSG_MAP(SVOPropertyPageDlg)
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

SVOPropertyPageDlg::SVOPropertyPageDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVOPropertyPageDlg::IDD, pParent)
,	m_TriggerObj( _T(""), 0 )
,	m_eProduct( SVIM_PRODUCT_TYPE_UNKNOWN )
{
}

SVOPropertyPageDlg::~SVOPropertyPageDlg()
{
}

void SVOPropertyPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOPropertyPageDlg)
	//}}AFX_DATA_MAP
}

void SVOPropertyPageDlg::SetupCamera()
{
	SetTitle( m_CameraObj.GetCameraDisplayName().c_str() );
	if( !m_CameraObj.IsFileAcquisition() )
	{
		if(false == m_CameraObj.GetCameraFile().empty())
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
		assert( m_eProduct != SVIM_PRODUCT_TYPE_UNKNOWN );
	
		SetupFileCamera(pRoot);

		SVFileNameClass FileName;
		FileName.SetFileType(SV_CAMERA_FILE_TYPE);

		bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
		SVRPropertyItemFile* pFile = (SVRPropertyItemFile*)m_Tree.InsertItem(new SVRPropertyItemFile(bFullAccess, SVR_FILE,
			cGigeCameraFileFilter, FileName.GetDefaultPathName().c_str(), TRUE), pRoot);
		if (pFile)
		{
			pFile->SetCtrlID(PROP_AD_FILE_NAME);
			pFile->SetLabelText( _T("Camera Filename") );
			pFile->SetInfoText( _T("Select camera file for the currently selected Camera.") );
			pFile->SetItemValue( m_CameraObj.GetCameraFile().c_str() );
		}
		pFile = (SVRPropertyItemFile*)m_Tree.InsertItem(new SVRPropertyItemFile(bFullAccess, SVR_FILE,
			cSequenceCameraFileFilter, FileName.GetDefaultPathName().c_str(), TRUE), pRoot);
		if (pFile)
		{
			pFile->SetCtrlID(PROP_SEQUENCE_CAMERA_FILE_NAME);
			pFile->SetLabelText(_T("Sequence Camera Filename"));
			pFile->SetInfoText(_T("Optional: Select sequence camera file for additional camera settings."));
			pFile->SetItemValue(m_CameraObj.GetSequenceCameraFile().c_str());
		}

		// Show or Hide Real/File camera props
		ShowCameraProperties();

		pRoot->Select(true);
		pRoot->Expand();
	}
}

void SVOPropertyPageDlg::SetupFileCamera(SVRPropertyItem* pRoot)
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
	
	bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SVRPropertyItemFile* pFile = (SVRPropertyItemFile*)m_Tree.InsertItem(new SVRPropertyItemFile(bFullAccess, SVR_FILE,
									fileFilter, FileName.GetDefaultPathName().c_str(), true), pRoot);
	if (pFile)
	{
		pFile->SetCtrlID(PROP_FILECAMERA_FILENAME);
		pFile->SetLabelText(_T("Image Filename"));
		pFile->SetInfoText(_T("Select the image file for the currently selected Camera."));
		pFile->SetItemValue( m_CameraObj.GetImageFilename().c_str() );
	}

	pFile = (SVRPropertyItemFile*)m_Tree.InsertItem(new SVRPropertyItemFile(bFullAccess, SVR_FOLDER | SVR_TRAILINGSLASH,
									nullptr, FileName.GetDefaultPathName().c_str(), true), pRoot);
	if (pFile)
	{
		pFile->SetCtrlID(PROP_FILECAMERA_DIRECTORY);
		pFile->SetLabelText(_T("Image Directory"));
		pFile->SetInfoText(_T("Select the directory to load the image files from, for the currently selected Camera."));
		std::string PathName = m_CameraObj.GetImageDirectoryName();
		if( PathName.empty() )
		{
			PathName = FileName.GetDefaultPathName();
			m_CameraObj.SetImageDirectoryName( PathName );
		}
		pFile->SetItemValue( PathName.c_str() );
	}
}

void SVOPropertyPageDlg::SetupAdvancedFileCamera(SVRPropertyItem* pRoot)
{
	SetTitle( m_CameraObj.GetCameraDisplayName().c_str() );

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

void SVOPropertyPageDlg::SetupAdvancedCamera()
{
	assert( nullptr != m_pAssistant );
	SetTitle( m_CameraObj.GetCameraDisplayName().c_str() );
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
			
			assert( m_eProduct != SVIM_PRODUCT_TYPE_UNKNOWN );
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
				SVGigeCameraParamValidateClass l_svValidate;
				l_svValidate.UpdateParams( m_CameraObj.GetCameraFileParamsNonConst(),
					m_CameraObj.GetCameraDeviceParamsNonConst() );
				
				CameraAdvancedHideItems();
			}// end if ( TheSVObserverApp().IsDigitalSVIM() )
			else // should never get here, perhaps there should be an exception logged/displayed instead?
			{
				pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
				if (pCombo)
				{
					pCombo->SetCtrlID(PROP_ADV_CAMERA_DIG);
					pCombo->SetLabelText(_T("Digitizer #"));
					pCombo->SetInfoText(_T("Enter the digitizer number the camera is attached to."));
					pCombo->CreateComboBox();

					int num = m_CameraObj.GetDigNumber();
					std::string Text = SvUl::Format( _T("%d"), num );
					int iPos = pCombo->AddString( Text.c_str() );
					pCombo->SetItemData(iPos, num);
					pCombo->SetItemValue(num);
				}// end if (pCombo)
			}
		}
		pRoot->Select(true);
		pRoot->Expand();
    }// end if (pRoot)
}

void SVOPropertyPageDlg::SetupCameraDeviceParam(SVRPropertyItem* pRoot, const SVDeviceParam* pDeviceParam, const SVDeviceParam* pFileParam, bool ColorCamera)
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
						pCombo->SetCtrlID(static_cast<UINT> (PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
						std::string Label = pCamFileParam->VisualName();
						if( Label.empty() )
						{
							Label = pCamFileParam->Name();
						}
						pCombo->SetLabelText( Label.c_str() );
						pCombo->SetInfoText( pCamFileParam->Description() );
						pCombo->CreateComboBox();
						
						SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
						for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
						{
							int iPos = pCombo->AddString( iterOption->m_Description.c_str() );
							pCombo->SetItemData( iPos, iterOption->m_Value );
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
						pEdit->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
						
						std::string Label = pCamFileParam->VisualName();
						if( Label.empty() )
						{
							Label = pCamFileParam->Name();
						}
						Label = SvUl::Format( _T("%s (%s)"), Label.c_str(), pCamDeviceParam->info.sUnits.c_str() );
						pEdit->SetLabelText( Label.c_str() );
						std::string Description = SvUl::Format( _T("%s   Min = %d, Max = %d; default = %d"), pCamFileParam->Description(), pCamFileParam->GetScaledMin(), pCamFileParam->GetScaledMax(), static_cast<long> (ceil( pCamFileParam->lValue * pCamFileParam->info.multiplier)) );
						pEdit->SetInfoText( Description.c_str() );

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
					pCombo->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
					std::string Label = pCamFileParam->VisualName();
					if( Label.empty() )
					{
						Label = pCamFileParam->Name();
					}
					pCombo->SetLabelText( Label.c_str() );
					pCombo->SetInfoText( pCamFileParam->Description() );
					pCombo->CreateComboBox();
					
					SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
					for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
					{
						int iPos = pCombo->AddString( iterOption->m_Description.c_str() );
						pCombo->SetItemData( iPos, (LPARAM) iterOption->m_Value );
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
								pCombo->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
								std::string Label = pCamFileParam->VisualName();
								if( Label.empty() )
								{
									Label = pCamFileParam->Name();
								}
								pCombo->SetLabelText( Label.c_str() );
								pCombo->SetInfoText( pCamFileParam->Description() );
								pCombo->SetButtonText("ROI");
								pCombo->CreateComboBox();
									
								SVCameraFormatsDeviceParam::OptionsType::const_iterator iterOption;
								const SVCameraFormat* pFormat = nullptr;
								for (iterOption = pCamFileParam->options.begin(); iterOption != pCamFileParam->options.end(); ++iterOption)
								{
									if ( iterOption->second.m_bColor == ColorCamera )
									{
										int iPos = pCombo->AddString( iterOption->second.m_strDescription.c_str() );
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
								pCombo->SetCtrlID(static_cast<UINT>(PROP_CAMERA_FILE_BASE) + static_cast<UINT>(pCamFileParam->Type()));
								std::string Label = pCamFileParam->VisualName();
								if ( Label.empty() )
								{
									Label = pCamFileParam->Name();
								}
								pCombo->SetLabelText( Label.c_str() );
								pCombo->SetInfoText( pCamFileParam->Description() );
								pCombo->CreateComboBox();
										
								SVStringValueDeviceParam::OptionsType::const_iterator iterOption;
								int iOption = 0;
								int iOptionIndex = 0;
								for (iterOption = pCamFileParam->info.options.begin(); iterOption != pCamFileParam->info.options.end(); ++iterOption)
								{
									int iPos;
									iPos = pCombo->AddString( iterOption->m_Description.c_str() );
									++iOption;
									if ( iterOption->m_Value == pCamDeviceParam->strValue )
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

void SVOPropertyPageDlg::ShowCameraProperties()
{
	if (m_CameraObj.IsFileAcquisition())
	{
		m_Tree.FindItem(PROP_FILECAMERA_MODE)->HideItem(false);
		m_Tree.FindItem(PROP_FILECAMERA_DIRECTORY)->HideItem(false);
		m_Tree.FindItem(PROP_FILECAMERA_FILENAME)->HideItem(false);
		
		m_Tree.FindItem(PROP_AD_FILE_NAME)->HideItem(true);
		m_Tree.FindItem(PROP_SEQUENCE_CAMERA_FILE_NAME)->HideItem(true);
	}
	else
	{
		m_Tree.FindItem(PROP_AD_FILE_NAME)->HideItem(false);
		m_Tree.FindItem(PROP_SEQUENCE_CAMERA_FILE_NAME)->HideItem(false);

		m_Tree.FindItem(PROP_FILECAMERA_MODE)->HideItem(true);
		m_Tree.FindItem(PROP_FILECAMERA_DIRECTORY)->HideItem(true);
		m_Tree.FindItem(PROP_FILECAMERA_FILENAME)->HideItem(true);
	}
}

void SVOPropertyPageDlg::UpdateFileImageSize()
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

bool SVOPropertyPageDlg::ScanForImageSize(SIZE& size)
{
	bool Result( false );
	SVImageFile FileImage;
	std::string Name;

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

void SVOPropertyPageDlg::SetImageSizeEditAttributes()
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

void SVOPropertyPageDlg::SetupInspection()
{
	SetTitle( m_InspectionObj.GetInspectionName().c_str() );
			
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
			if( 0 < m_PPQObj.GetAttachedInspectionCount() )
			{
				long lSize = m_PPQObj.GetAttachedCameraCount();
				for(long l = 0; l < lSize; l++ )
				{
					std::string CameraName = m_PPQObj.GetAttachedCamera( l );
					iInsIndex = pCombo->AddString( CameraName.c_str() );
					pCombo->SetItemData( iInsIndex, l );
				}// end for
				
				long lWhere = pCombo->SelectString( -1, m_InspectionObj.GetToolsetImage().c_str() );
				/*bool bResult =*/ pCombo->SetItemValue( pCombo->GetItemData( lWhere ) );
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
			
			long lWhere = pCombo->SelectString( -1, m_InspectionObj.GetNewDisableMethodString().c_str() );
			/*bool bResult = */pCombo->SetItemValue( pCombo->GetItemData( lWhere ) );
		}// end if

		if( m_InspectionObj.GetShowAuxExtent() )
		{
			pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(),pRoot);
			if (pCombo)
			{ 
				pCombo->SetCtrlID(PROP_INS_ENABLE_AUX_EXTENT); //PROP_INS_NEW_DISABLE_METHOD);
				pCombo->SetLabelText(_T("Auxiliary Extents"));
				pCombo->SetInfoText(_T("This allows the inspection to support Auxiliary Extents (Enabling this will perform auxiliary extent calculations that could add additional time to the tool set.)"));
				pCombo->CreateComboBoxBool();
				
				pCombo->SetItemValue( m_InspectionObj.GetEnableAuxiliaryExtent() );
			}
		}
		pRoot->Select(true);
		pRoot->Expand();
	}
}

void SVOPropertyPageDlg::SetupTrigger()
{
	SetTitle( m_TriggerObj.GetTriggerDisplayName() );
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
				" Hardware, Software or Camera (PPQ Length >2)."));
			pCombo->CreateComboBox();

			for(int i=0; i < cTriggerType.size(); ++i)
			{
				int iPos = pCombo->AddString(cTriggerType[i]);
				pCombo->SetItemData(iPos, i);
			}
			
			pCombo->SetItemValue(static_cast<int> (m_TriggerObj.getTriggerType()));
		}
		pRoot->Select(true);
		pRoot->Expand();
	}
}

void SVOPropertyPageDlg::SetupAdvancedTrigger()
{
	SetTitle( m_TriggerObj.GetTriggerDisplayName() );
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
			std::string Text = SvUl::Format(_T("Enter the period for the Software Trigger (also used for Test Mode), in milliseconds (minimum %d milliseconds)."), SvTrig::MinTimerPeriod_ms);
			pEdit->SetInfoText(Text.c_str());

			pEdit->SetItemValue(m_TriggerObj.GetTimerPeriod());
		}
		pEdit = dynamic_cast<SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_ADV_TRIGGER_START_OBJECT_ID);
			pEdit->SetLabelText(_T("Start Object ID"));
			pEdit->SetInfoText(_T("Enter the Object ID with which the trigger should start with. It shall automatically increment"));
			pEdit->SetItemValue(m_TriggerObj.getObjectIDParameters().m_startObjectID);
		}
		pEdit = dynamic_cast<SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_ADV_TRIGGER_PER_OBJECT_ID);
			pEdit->SetLabelText(_T("Trigger per Object ID"));
			pEdit->SetInfoText(_T("Enter the number of trigger for each ObjectID"));
			pEdit->SetItemValue(m_TriggerObj.getObjectIDParameters().m_triggerPerObjectID);
		}
		pEdit = dynamic_cast<SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_ADV_TRIGGER_OBJECT_ID_COUNT);
			pEdit->SetLabelText(_T("Object ID count"));
			pEdit->SetInfoText(_T("Enter the number of object IDs to generate for each trigger session (-1 means continous)"));
			pEdit->SetItemValue(m_TriggerObj.getObjectIDParameters().m_objectIDCount);
		}

		pRoot->Select(true);
		pRoot->Expand();
	}
}

void SVOPropertyPageDlg::SetupPPQ()
{
	SetTitle( m_PPQObj.GetPPQName().c_str() );
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("PPQ"));
		pRoot->SetInfoText(_T("Define a PPQ"));

		int iMode = m_PPQObj.GetPPQMode();
		// PPQMode
		SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		if (pCombo)
		{
			pCombo->SetCtrlID(PROP_PPQ_MODE);
			pCombo->SetLabelText(_T("Output Mode"));
			if (0 <= iMode && cTriggerModeInfo.size() > iMode)
			{
				pCombo->SetInfoText(cTriggerModeInfo.at(iMode));
			}
			pCombo->CreateComboBox(CBS_DROPDOWNLIST);

			for(int i=0; i < cTriggerMode.size(); ++i)
			{
				int nIndex = pCombo->AddString(cTriggerMode.at(i));
				pCombo->SetItemData(nIndex, i);
			}
			pCombo->SetItemValue(iMode);
		}
		//PPQLength
		SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);

		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_PPQ_LENGTH);
			pEdit->SetLabelText(_T("Length"));
			if(0 <= iMode && cPpqLengthInfo.size() > iMode )
			{
				pEdit->SetInfoText( cPpqLengthInfo.at(iMode));
			}
			if (m_PPQObj.GetPPQMode() == 0)
			{
				if (m_PPQObj.GetPPQLength() < 2)
				{
					m_PPQObj.SetPPQLength(2);
				}
			}
			/*bool bResult = */pEdit->SetItemValue(m_PPQObj.GetPPQLength());
		}
		
		// PPQOutputDelayTime
		pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
		if (pEdit)
		{
			pEdit->SetCtrlID(PROP_PPQ_OUTPUT_DELAY_TIME);
			pEdit->SetLabelText(_T("Output Delay Time"));
			pEdit->SetInfoText(_T("All Modes - in all modes this is the number of milliseconds between the "
								  "trigger and the setting of the outputs."));
			/*bool bResult = */pEdit->SetItemValue(m_PPQObj.GetPPQOutputDelayTime());
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
			/*bool bResult = */pEdit->SetItemValue(m_PPQObj.GetPPQOutputResetDelay());
		}
		
		//add new property for "Maintain Source Image"  default to FALSE;
		// will be a new CreateComboBoxBool
		pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		if ( pCombo )
		{
			std::string InfoString = SvUl::Format( _T("Enabling this option will maintain source images and published result images "
				"throughout the length of the PPQ. Because of this, "
				"the PPQ Length can not be > %d"),TheSVObserverApp().GetMaxPPQLength());
			pCombo->SetCtrlID(PROP_PPQ_MAINTAIN_SRC_IMG);
			pCombo->SetLabelText(_T("Maintain Published Images"));
			pCombo->SetInfoText( InfoString.c_str() );
			pCombo->CreateComboBoxBool();
			pCombo->SetItemValue(m_PPQObj.GetMaintainSourceImageProperty());
		}

		pEdit = dynamic_cast <SVRPropertyItemEdit*> (m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		if (nullptr != pEdit)
		{
			pEdit->SetCtrlID(PROP_PPQ_MAX_PROCESSING_OFFSET);
			pEdit->SetLabelText(_T("Max. processing offset"));
			pEdit->SetInfoText(_T("Maximal processing offset between inspections to keep all reject data. It applies only if there are several inspections in this PPQ and a reject condition is set. "
				"Range(2 … PPQ length), 0 = use the full PPQ, Warning: A high offset increases the RAM usage."));
			pEdit->SetItemValue(m_PPQObj.GetMaxProcessingOffsetProperty());
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
			/*bool bResult = */pEdit->SetItemValue( m_PPQObj.GetInspectionTimeout() );
		}

		// Add Conditional Output
		pCombo = (SVRPropertyItemCombo*)m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot);
		if ( pCombo )
		{
			std::string InfoString = _T("Condition to evaluate to determine if Outputs will be written. "
				             "If set to Always, no evaluation is performed and the Outputs are always written.");
			pCombo->SetCtrlID(PROP_PPQ_CONDITIONAL_OUTPUT);
			pCombo->SetLabelText(_T("Conditional Output"));
			pCombo->SetInfoText( InfoString.c_str() );
			pCombo->CreateComboBox(CBS_DROPDOWNLIST);
			int nIndex = pCombo->AddString(PPQ_CONDITIONAL_OUTPUT_ALWAYS);
			pCombo->SetItemData(nIndex, nIndex);
			int selection = nIndex;
			
			const std::string& condition = m_PPQObj.GetConditionalOutputName();

			// iterate thru the inputs....
			// if the current conditional output value is not in the list, set it to always
			const SVNameObjectIdPairList& list = m_PPQObj.GetAvailableInputsForConditionalOutput();
			for (SVNameObjectIdPairList::const_iterator it = list.begin();it != list.end();++it)
			{
				std::string name = it->first;
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
// SVOPropertyPageDlg message handlers
BOOL SVOPropertyPageDlg::OnInitDialog() 
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
	
	m_Tree.SetColumn( (long) ( rc.Width() * 0.30 ) );

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

void SVOPropertyPageDlg::OnPaint() 
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
HCURSOR SVOPropertyPageDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void SVOPropertyPageDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// resize the control to always fit the dialog
	if (::IsWindow(m_Tree.GetSafeHwnd()))
		m_Tree.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE|SWP_NOZORDER);	
}

BOOL SVOPropertyPageDlg::OnEraseBkgnd(CDC* pdc) 
{
    CDialog::OnEraseBkgnd(pdc);
	// don't bother erasing the background since our control will always
	// cover the entire client area
	return TRUE;
}

void SVOPropertyPageDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
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

			assert( pDeviceParam );
			assert( pFileDeviceParam );

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
				long lValue;
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
									pItem->GetItemValue( Value );
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
												SVLongValueDeviceParam* pbinningParam(nullptr);
												params.GetParameter(DeviceParamVerticalBinning).GetDerivedValue(pbinningParam);
												pbinningParam->lValue = 1;
												params.SetParameter(DeviceParamVerticalBinning, pbinningParam);
											}
											if (params.ParameterExists(DeviceParamHorizontalBinning))
											{
												SVLongValueDeviceParam* pbinningParam(nullptr);
												params.GetParameter(DeviceParamHorizontalBinning).GetDerivedValue(pbinningParam);
												pbinningParam->lValue = 1;
												params.SetParameter(DeviceParamHorizontalBinning, pbinningParam);
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
											CString Description;
											l_pCombo->GetLBText( l_Index, Description );

											SVStringValueDeviceParam::OptionsType::const_iterator iterOption;
											for (iterOption = pParam->info.options.begin(); iterOption != pParam->info.options.end(); ++iterOption)
											{
												if( Description.GetString() == iterOption->m_Description )
												{
													pParam->strValue = iterOption->m_Value;
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
			if (SVHardwareManifest::IsMatroxGige(m_eProduct))
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
					std::string FileName;
					m_Tree.FindItem(PROP_AD_FILE_NAME)->GetItemValue( FileName );
					FileName = SvUl::Trim(FileName.c_str());
					//! Check if file exists
					if (!FileName.empty() && 0 != _access(FileName.c_str(), 0))
					{
						SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Config_CameraFileNameInvalid, {FileName}, SvStl::SourceFileParams(StdMessageParams));
						m_Tree.FindItem(PROP_AD_FILE_NAME)->SetItemValue(m_CameraObj.GetCameraFile().c_str());
						break;
					}
					m_CameraObj.SetCameraFile( FileName );
					m_CameraObj.SetCameraFileChanged();
					if( m_CameraObj.GetCameraFileChanged()  && m_InitialCameraFileName != m_CameraObj.GetCameraFile() )
					{
						GetDlgItem(IDOK)->SetWindowText(_T("OK"));
					}
					else
					{
						if( 0 < m_InitialCameraFileName.size() )
						{
							GetDlgItem(IDOK)->SetWindowText(_T("Reset"));
						}
					}
					break;
				}

				case PROP_SEQUENCE_CAMERA_FILE_NAME:
				{
					std::string FileName;
					m_Tree.FindItem(PROP_SEQUENCE_CAMERA_FILE_NAME)->GetItemValue(FileName);
					FileName = SvUl::Trim(FileName.c_str());
					//! Check if file exists
					if (!FileName.empty() && 0 != _access(FileName.c_str(), 0))
					{
						SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_FileNameInvalid, {FileName}, SvStl::SourceFileParams(StdMessageParams));
						m_Tree.FindItem(PROP_SEQUENCE_CAMERA_FILE_NAME)->SetItemValue(m_CameraObj.GetSequenceCameraFile().c_str());
						break;
					}
					m_CameraObj.SetSequenceCameraFile(FileName);
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
					std::string FileName;
					m_Tree.FindItem(ctrlID)->GetItemValue( FileName );
					m_CameraObj.SetImageFilename( FileName );
					UpdateFileImageSize();
					GetDlgItem(IDOK)->SetWindowText(_T("OK"));
					break;
				}

				case PROP_FILECAMERA_DIRECTORY:
				{
					std::string Path;
					m_Tree.FindItem(ctrlID)->GetItemValue( Path);
					m_CameraObj.SetImageDirectoryName( Path );
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
					m_TriggerObj.setTriggerType( static_cast<SvDef::TriggerType> (lType));
					break;
				}

				case PROP_ADV_TRIGGER_PERIOD:
				{
					long lType;
					m_Tree.FindItem(ctrlID)->GetItemValue(lType);

					// validate against minimum timer period
					if (lType < SvTrig::MinTimerPeriod_ms)
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

				case PROP_ADV_TRIGGER_START_OBJECT_ID:
				{
					long startObjectID {0L};
					m_Tree.FindItem(ctrlID)->GetItemValue(startObjectID);
					m_TriggerObj.getObjectIDParameters().m_startObjectID = startObjectID;
					break;
				}

				case PROP_ADV_TRIGGER_PER_OBJECT_ID:
				{
					long triggerPerObjectID {1L};
					m_Tree.FindItem(ctrlID)->GetItemValue(triggerPerObjectID);
					m_TriggerObj.getObjectIDParameters().m_triggerPerObjectID = triggerPerObjectID;
					break;
				}

				case PROP_ADV_TRIGGER_OBJECT_ID_COUNT:
				{
					long objectIDCount {-1L};
					m_Tree.FindItem(ctrlID)->GetItemValue(objectIDCount);
					m_TriggerObj.getObjectIDParameters().m_objectIDCount = objectIDCount;
					break;
				}

				case PROP_INS_TOOLSET_IMAGE:
				{
					SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*) m_Tree.FindItem( PROP_INS_TOOLSET_IMAGE );
					
					CString ImageName;
					long lIndex = pCombo->GetCurSel();
					if( lIndex != LB_ERR )
					{
						pCombo->GetLBText( lIndex, ImageName );
					}
					m_InspectionObj.SetToolsetImage( ImageName );
					break;
				}

				case PROP_INS_NEW_DISABLE_METHOD:
				{
					SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*) m_Tree.FindItem( PROP_INS_NEW_DISABLE_METHOD );
					CString DisableMethod;
					long lIndex = pCombo->GetCurSel();
					if( lIndex != LB_ERR )
					{
						pCombo->GetLBText( lIndex, DisableMethod );
					}
					m_InspectionObj.SetNewDisableMethod( DisableMethod );
					break;
				}

				case PROP_INS_ENABLE_AUX_EXTENT:
				{
					SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*) m_Tree.FindItem( PROP_INS_ENABLE_AUX_EXTENT );
					CString Enable;
					long lIndex = pCombo->GetCurSel();
					if( lIndex != LB_ERR )
					{
						pCombo->GetLBText( lIndex, Enable );
					}
					m_InspectionObj.SetEnableAuxiliaryExtent( Enable == _T("True") );
					break;
				}

				case PROP_PPQ_MODE:
				{
					long iVal;
					m_Tree.FindItem(PROP_PPQ_MODE)->GetItemValue(iVal);
					m_PPQObj.SetPPQMode(iVal);
					///cTriggerModeInfo and cPpqLengthInfo are the same size
					if (0 <= iVal && cTriggerModeInfo.size() > iVal)
					{
						m_Tree.FindItem(PROP_PPQ_LENGTH)->SetInfoText(cPpqLengthInfo.at(iVal));
       					m_Tree.FindItem(PROP_PPQ_MODE)->SetInfoText(cTriggerModeInfo.at(iVal));
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
						if (iLen > getMaxPpqLength())
						{
							iLen = getMaxPpqLength();
							m_Tree.FindItem(PROP_PPQ_LENGTH)->SetItemValue(iLen);
						}
						checkAndSetMaxProcessingOffset(iLen);
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

				case PROP_PPQ_MAX_PROCESSING_OFFSET:
				{
					int iLen;
					m_Tree.FindItem(PROP_PPQ_LENGTH)->GetItemValue(iLen);
					checkAndSetMaxProcessingOffset(iLen);
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
						m_PPQObj.SetConditionalOutputName(std::string(Value));
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

void SVOPropertyPageDlg::OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult)
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
						SvStl::MessageManager Exception(SvStl::MsgType::Log );
						Exception.setMessage( SVMSG_SVO_NULL_POINTER, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
					}
				}
			}
		}
	}
}

void SVOPropertyPageDlg::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
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
			assert( rCamDeviceParams.ParameterExists(e) );
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

static HRESULT ShowGigeROIDlg(ISVCameraDeviceImageFormatUpdater& rImageFormatUpdater, SVCameraFormat& p_rFormat, const SVImageInfoClass& p_rImageInfo, SvIe::SVAcquisitionClassPtr pDevice, SVCameraFormat& rCameraDeviceFormat, SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_FALSE;
	SVGigeCameraROIDlg dlg(rImageFormatUpdater);
	dlg.SetFormat( &p_rFormat );
	dlg.SetFormatImage( p_rImageInfo );
	dlg.SetAcquisitionDevice( pDevice );

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

static void ApplyGigeImageFormatChange(SvIe::SVAcquisitionClassPtr pDevice, const SVDeviceParamCollection& rDeviceParams)
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

	pDevice->SetDeviceParameters(l_params);
}

// For GigE ROI adjustments
HRESULT SVOPropertyPageDlg::AdjustCameraImageFormat( LPCTSTR sSelectedFormat, SVDeviceParamWrapper& rw)
{
	HRESULT hr = S_OK;

	SVDeviceParamEnum e = rw->Type();
	SVCameraFormatsDeviceParam* pCameraDeviceParam = rw.DerivedValue( pCameraDeviceParam );
	const SVDeviceParamCollection& rCameraFileParams = m_CameraObj.GetCameraFileParams();
	const SVDeviceParamWrapper& rwCameraFileFormats = rCameraFileParams.Parameter( e );
	const SVCameraFormatsDeviceParam* pCameraFileFormats = rwCameraFileFormats.DerivedValue( pCameraFileFormats );

	assert( pCameraDeviceParam->options.find( sSelectedFormat ) != pCameraDeviceParam->options.end() );
	SVCameraFormat& rCameraDeviceFormat = pCameraDeviceParam->options[sSelectedFormat];
	
	assert( pCameraFileFormats->options.find( sSelectedFormat ) != pCameraFileFormats->options.end() );
	SVCameraFormat format( pCameraFileFormats->options.find( sSelectedFormat )->second );
	format.AssignUserChangableValues( rCameraDeviceFormat );

	long l_WidthMax = format.m_lWidthMax;
	long l_HeightMax = format.m_lHeightMax;

	bool bChangedAcqFormat = false;
	SVImageInfoClass l_ImageInfo;

	std::string DeviceName = SvUl::getAcquisitionName(m_CameraObj.GetDigNumber(), m_CameraObj.IsFileAcquisition());
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	SVInspectionProcessVector aInspections;
	SVDeviceParamCollection l_CurrentParams;

	SvIe::SVAcquisitionClassPtr pDevice(SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DeviceName.c_str() ) );
	
	if( nullptr != pDevice )
	{
		long l_lSize = 0;

		if( S_OK == pDevice->GetFileNameArraySize( l_lSize ) && l_lSize < 1 )
		{
			SVFileNameArrayClass svFiles;
			SVFileNameClass svFile;

			svFile.SetFullFileName( m_CameraObj.GetCameraFile().c_str() );

			svFiles.push_back(svFile);

			pDevice->LoadFiles( svFiles );
		}

		long l_lWidth = 0;
		long l_lHeight = 0;

		SVImageInfoClass info;

		pDevice->GetImageInfo( &info );

		info.GetExtentProperty( SvPb::SVExtentPropertyWidth, l_lWidth );
		info.GetExtentProperty( SvPb::SVExtentPropertyHeight, l_lHeight );

		// disconnect buffers attached to dig
		long lInspectionCount = 0;
		SVInspectionProcess* pInspection( nullptr );
		//If the pointer is nullptr then count is 0		
		if( nullptr != pConfig ){ lInspectionCount = pConfig->GetInspectionCount( ); }


		for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )
		{
			pInspection = pConfig->GetInspection( lInspection );
			if ( nullptr != pInspection )
			{
				bool bFoundCamera=false;
				SVPPQObject* pPPQ = pInspection->GetPPQ();

				if( nullptr != pPPQ )
				{
					SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras();

					for(const auto* pCamera : cameraVector)
					{
						if ( nullptr != pCamera && pCamera->GetAcquisitionDevice() == pDevice )
						{
							bFoundCamera = true;
						}
					}
				}

				if ( bFoundCamera )
				{
					pInspection->DisconnectToolSetMainImage();
					aInspections.push_back( pInspection );
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

		if (SVHardwareManifest::IsMatroxGige(m_eProduct))
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
			
			l_ImageInfo.SetExtentProperty( SvPb::SVExtentPropertyWidth, l_WidthMax);
			l_ImageInfo.SetExtentProperty( SvPb::SVExtentPropertyHeight, l_HeightMax);
		}
		else
		{
			l_ImageInfo.SetExtentProperty( SvPb::SVExtentPropertyWidth, l_WidthMax );
			l_ImageInfo.SetExtentProperty( SvPb::SVExtentPropertyHeight, l_HeightMax );
		}
	}

	if (SVHardwareManifest::IsMatroxGige(m_eProduct))
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
		for (auto pInspection : aInspections)
		{
			pInspection->ConnectToolSetMainImage();
		}
	}
	return hr;
}

void SVOPropertyPageDlg::OnClose() 
{
	CDialog::OnClose();
}

void SVOPropertyPageDlg::CameraAdvancedHideItems()
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

void SVOPropertyPageDlg::PPQHideItems()
{
    //get PPQMode
    long lPPQMode;

    m_Tree.FindItem(PROP_PPQ_MODE)->GetItemValue(lPPQMode);
    switch(static_cast<SvDef::SVPPQOutputModeEnum>(lPPQMode))
    {
		case SvDef::SVPPQNextTriggerMode:
        {
            //NextTrigger mode - hide OutputDelay
            m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->HideItem();
			m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->HideItem();
			m_Tree.FindItem(PROP_PPQ_MAX_PROCESSING_OFFSET)->HideItem(false);
            break;
        }
        case SvDef::SVPPQTimeDelayMode:
        case SvDef::SVPPQTimeDelayAndDataCompleteMode:
		{
			m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->HideItem(false);
			m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->HideItem();
			m_Tree.FindItem(PROP_PPQ_MAX_PROCESSING_OFFSET)->HideItem();
			break;
		}

		case SvDef::SVPPQExtendedTimeDelayMode:
		case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
		{
            m_Tree.FindItem(PROP_PPQ_OUTPUT_DELAY_TIME)->HideItem(false);
			m_Tree.FindItem(PROP_PPQ_INSPECTION_TIMEOUT)->HideItem(false);
			m_Tree.FindItem(PROP_PPQ_MAX_PROCESSING_OFFSET)->HideItem();
            break;
        }
        default:
            break;
    }//end of switch...
}

void SVOPropertyPageDlg::SetDlgPage(int nID)
{
    m_nID = nID;
}

void SVOPropertyPageDlg::SetProductType(SVIMProductEnum eType)
{
    m_eProduct = eType;
}

void SVOPropertyPageDlg::SetTitle( LPCTSTR Name )
{
    std::string Label = _T("Property Settings  -  ");
	Label += Name;
    SetWindowText( Label.c_str() );
}

void SVOPropertyPageDlg::SetConfigAssistantDlg(SVOConfigAssistantDlg* pAssistant)
{
	m_pAssistant = pAssistant;
}

void SVOPropertyPageDlg::checkAndSetMaxProcessingOffset(int ppqLen)
{
	long iVal;
	m_Tree.FindItem(PROP_PPQ_MAX_PROCESSING_OFFSET)->GetItemValue(iVal);

	if (iVal < 2 && 0 != iVal)  //0 == iVal means PPQLength
	{
		iVal = 2;
		m_Tree.FindItem(PROP_PPQ_MAX_PROCESSING_OFFSET)->SetItemValue(iVal);
	}
	else if (iVal > ppqLen)
	{
		iVal = ppqLen;
		m_Tree.FindItem(PROP_PPQ_MAX_PROCESSING_OFFSET)->SetItemValue(iVal);
	}
	m_PPQObj.SetMaxProcessingOffsetProperty(iVal);
}

