//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgAnalyzerPageClass
//* .File Name       : $Workfile:   TADlgAnalyzerPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   16 Dec 2014 18:04:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgAnalyzerPageClass.h"
#include "SVChildrenSetupDialog.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVSetupDialogManager.h"
#include "TADlgSheetClass.h"
#include "AnalyzerOperators/Analyzer.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(TADlgAnalyzerPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgAnalyzerPageClass)
	ON_BN_CLICKED(IDC_DETAILS_BUTTON, OnButtonDetails)
	ON_CBN_SELCHANGE(IDC_ANALYZER_COMBO, OnSelchangeCurrentAnalyzer)
	ON_BN_CLICKED(IDC_RESULT_BUTTON, OnResultButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TADlgAnalyzerPageClass::TADlgAnalyzerPageClass(uint32_t inspectionID, uint32_t taskObjectID, TADlgSheetClass* pParent)
	: CPropertyPage(TADlgAnalyzerPageClass::IDD)
	, m_pParentDialog(pParent)
	, m_pTool(nullptr)
	, m_pCurrentAnalyzer(nullptr)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
	, m_ImageController(inspectionID, taskObjectID)
{
	if (m_pParentDialog)
	{
		m_pTool = dynamic_cast<SvTo::SVToolClass*> (m_pParentDialog->GetTaskObject());

		if (m_pTool)
		{
			SvDef::SVObjectTypeInfoStruct info;
			info.m_ObjectType = SvPb::SVAnalyzerObjectType;

			m_pCurrentAnalyzer = dynamic_cast<SvAo::Analyzer*> (m_pTool->getFirstObject(info));
		}
	}
}

TADlgAnalyzerPageClass::~TADlgAnalyzerPageClass()
{
}

void TADlgAnalyzerPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgAnalyzerPageClass)
	DDX_Control(pDX, IDC_ANALYZER_COMBO, m_availableAnalyzerCombobox);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	//}}AFX_DATA_MAP
}


BOOL TADlgAnalyzerPageClass::OnSetActive()
{
	setImages();
	return CPropertyPage::OnSetActive();
}

BOOL TADlgAnalyzerPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	Log_Assert(m_pTool);
	if (m_pTool)
	{
		m_ImageController.Init();
		m_resultImageID = getFirstResultImageId(m_ImageController);
		
		const auto subType = m_pTool->GetObjectSubType();
		CWnd* pWnd;
		// Set Result button...
		switch (subType)
		{
			case SvPb::SVLinearToolObjectType:
				break;

			case SvPb::SVWindowToolObjectType:  // fall through...
			default:
				if (nullptr != (pWnd = GetDlgItem(IDC_RESULT_BUTTON)))
				{
					pWnd->EnableWindow(FALSE);
					pWnd->ShowWindow(SW_HIDE);
				}
				break;
		}

		m_dialogImage.AddTab(_T("Tool Result"));

		// Get the Image for this tool
		setImages();


		// get index of Current Analyzer
		std::string analyzerSelection = "";
		if (m_pCurrentAnalyzer)
		{
			analyzerSelection = m_pCurrentAnalyzer->GetObjectName();
		}

		SvUl::NameClassIdList availableList;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getcreatableobjectsrequest();
		pRequest->set_objectid(m_TaskObjectID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVAnalyzerObjectType);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getcreatableobjectsresponse())
		{
			const auto& rList = responseCmd.getcreatableobjectsresponse().list();
			std::transform(rList.begin(), rList.end(), std::back_inserter(availableList),
			[](const auto& item) -> SvUl::NameClassIdPair { return {item.objectname(), item.classid()}; });	
		}

		//This is to add the analyzer due to MIL License restrictions.
		if (m_pCurrentAnalyzer)
		{
			SvUl::NameClassIdList::const_iterator iter = std::find_if(availableList.begin(), availableList.end(), [&](const SvUl::NameClassIdPair& rVal)->bool
			{
				return (!rVal.first.empty() && 0 == rVal.first.compare(m_pCurrentAnalyzer->GetObjectName()));
			});
			if (availableList.end() == iter)
			{
				availableList.emplace_back(m_pCurrentAnalyzer->GetObjectName(), m_pCurrentAnalyzer->GetClassID());
				m_additionalAnalyzerId = m_pCurrentAnalyzer->GetClassID();
			}
		}

		m_availableAnalyzerCombobox.Init(availableList, analyzerSelection, _T("(No Analyzer Available)"), _T("(No Analyzer)"));

		updateButtons();

		UpdateData(FALSE); // set data to dialog
	}
	else
		// Not valid call...
		if (GetParent())
			GetParent()->SendMessage(WM_CLOSE);
		else
			SendMessage(WM_CLOSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnButtonDetails
////////////////////////////////////////////////////////////////////////////////
void TADlgAnalyzerPageClass::OnButtonDetails()
{
	if (m_pCurrentAnalyzer)
	{
		// Save ID
		uint32_t analyzerId = m_pCurrentAnalyzer->getObjectId();

		// Show Dialog
		SVSetupDialogManager::Instance().SetupDialog(m_pCurrentAnalyzer->GetClassID(), m_InspectionID, m_pCurrentAnalyzer->getObjectId(), this);

		// Restore the pointer (in case of Cancel)
		m_pCurrentAnalyzer = dynamic_cast<SvAo::Analyzer*> (SVObjectManagerClass::Instance().GetObject(analyzerId));

		if (nullptr != m_pTool)
		{
			SVObjectClass* pInspection(m_pTool->GetInspection());
			Log_Assert(nullptr != pInspection);

			if (nullptr != pInspection)
			{
				SvCmd::RunOnceSynchronous(pInspection->getObjectId());
			}

		}
	}
	else
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_Error_NoAnalyzerDetails, SvStl::SourceFileParams(StdMessageParams));
	}
}

void TADlgAnalyzerPageClass::OnSelchangeCurrentAnalyzer()
{
	CWaitCursor l_cwcMouse;

	UpdateData(TRUE); // get data from dialog

	auto classID = m_availableAnalyzerCombobox.getSelectedValue();

	// Check for valid selection
	if (SvPb::NoObjectClassId != classID)
	{
		// Check if its the same Analyzer
		if (m_pCurrentAnalyzer && m_pCurrentAnalyzer->GetClassID() != classID)
		{
			// if the Tool has an Analyzer - Close it and Delete it
			// Why? because this dialog is currently only used for selection of a single analyzer
			DestroyAnalyzer();	
		}

		// if an Analyzer does not exist - create one
		if (!m_pCurrentAnalyzer)
		{
			// and now Instantiate a new Object
			SVObjectManagerClass::Instance().ConstructObject(classID, m_pCurrentAnalyzer);

			if (m_pCurrentAnalyzer)
			{
				m_pTool->Add(dynamic_cast<SvIe::SVTaskObjectClass*>(m_pCurrentAnalyzer));
				// Ensure this object's inputs get connected
				// Fix to ensure friends get connections as well
				m_pTool->connectAllInputs();

				// And last - Create (initialize) it

				if (!m_pCurrentAnalyzer->IsCreated())
				{
					// And finally try to create the child object...
					if (!m_pTool->CreateChildObject(m_pCurrentAnalyzer, SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection))
					{
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_AnalyzerCreationFailed, SvStl::SourceFileParams(StdMessageParams));

						// Remove it from the Tool TaskObjectList ( Destruct it )
						uint32_t objectID = m_pCurrentAnalyzer->getObjectId();
						if (SvDef::InvalidObjectId != objectID)
						{
							m_pTool->Delete(objectID);
						}
						else
						{
							delete m_pCurrentAnalyzer;
						}

						m_pCurrentAnalyzer = nullptr;
					}
				}
			}
			else
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_AnalyzerInstantiationFailed, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	else // means No Analyzer
	{
		// if we had an Analyzer - delete it
		DestroyAnalyzer();
	}

	if (nullptr != m_pTool)
	{
		SVObjectClass* pInspection(m_pTool->GetInspection());
		Log_Assert(nullptr != pInspection);

		if (nullptr != pInspection)
		{
			SvCmd::RunOnceSynchronous(pInspection->getObjectId());
		}
	}

	// Update the Buttons
	updateButtons();
}

void TADlgAnalyzerPageClass::DestroyAnalyzer()
{
	if (m_pCurrentAnalyzer)
	{
		//This is to remove the analyzer due to MIL License restrictions.
		if (SvPb::NoObjectClassId != m_additionalAnalyzerId && m_pCurrentAnalyzer->GetClassID() == m_additionalAnalyzerId)
		{
			m_availableAnalyzerCombobox.remove(m_pCurrentAnalyzer->GetObjectName());
			m_additionalAnalyzerId = SvPb::NoObjectClassId;
		}

		m_pCurrentAnalyzer->DisconnectImages();

		// Close, Disconnect and Delete the Object
		m_pTool->DestroyChildObject(m_pCurrentAnalyzer, SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection);

		m_pCurrentAnalyzer = nullptr;
	}
}

void TADlgAnalyzerPageClass::setImages()
{
	IPictureDisp* pResultImage = m_ImageController.GetImage(m_resultImageID);
	m_dialogImage.setImage(pResultImage);
	m_dialogImage.Refresh();
}

////////////////////////////////////////////////////////////////////////////////
// updateButtons - Enable/Disable Buttons
////////////////////////////////////////////////////////////////////////////////
void TADlgAnalyzerPageClass::updateButtons()
{
	BOOL state = FALSE;

	if (m_pCurrentAnalyzer)
		state = TRUE;

	CWnd* pWnd;

	if (nullptr != (pWnd = GetDlgItem(IDC_DETAILS_BUTTON)))
		pWnd->EnableWindow(state);

	if (nullptr != (pWnd = GetDlgItem(IDC_RESULT_BUTTON)))
	{
		if (pWnd->IsWindowVisible())
			pWnd->EnableWindow(state);
	}
}

void TADlgAnalyzerPageClass::OnResultButton()
{
	if (m_pCurrentAnalyzer)
	{
		// Get Available Results...
		SvIe::SVClassInfoStructVector	availableResults;
		SvDef::SVObjectTypeInfoStruct		resultTypeInfo;
		resultTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
		//@TODO[MZA][7.40][14.10.2016] The getAvailableObject method should be replaced by the SvCmd::GetAvailableObjects
		m_pCurrentAnalyzer->getAvailableObjects(&availableResults, &resultTypeInfo);

		// Get Dialog Title...
		std::string Title = SvUl::LoadStdString(IDS_RESULT_ADJUSTMENT_DIALOG_TITLE);
		// Get Complete Name up to the tool level...
		Title = m_pCurrentAnalyzer->GetObjectNameToObjectType() + _T(" ") + Title;

		SVIPDoc* l_pIPDoc = nullptr;

		if (nullptr != m_pCurrentAnalyzer->GetInspection())
		{
			l_pIPDoc = m_pParentDialog->GetIPDoc();
		}

		// Prepare result adjustment dialog...
		SVChildrenSetupDialog dlg;
		dlg.m_pDocument = l_pIPDoc;
		dlg.m_AllowMultipleChildrenInstances = FALSE;
		dlg.m_pParentObject = m_pCurrentAnalyzer;
		dlg.m_pAvailableChildrenList = &availableResults;
		dlg.m_Title = Title;

		// Save id
		uint32_t analyzerId = m_pCurrentAnalyzer->getObjectId();

		// Call dialog...
		dlg.DoModal();

		// Restore the pointer (in case of Cancel)
		m_pCurrentAnalyzer = dynamic_cast<SvAo::Analyzer*> (SVObjectManagerClass::Instance().GetObject(analyzerId));
	}
}