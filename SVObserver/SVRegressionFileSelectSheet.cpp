// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectSheet
// * .File Name       : $Workfile:   SVRegressionFileSelectSheet.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   04 Sep 2014 12:44:38  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRegressionFileSelectSheet.h"
#include "SVRegressionFileSelectDlg.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVOResource/resource.h"
#include "SVOGui/SelectToolForNewSourceImageDialog.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr size_t cReserveNumber = 1000;

namespace 
{
SvDef::StringVector findSubdirectories(const std::string& rParentDirectory)
{
	std::string fileMask = rParentDirectory + _T("\\*");
	CFileFind fileFinder;
	bool bFound = fileFinder.FindFile(fileMask.c_str());
	SvDef::StringVector folderList;
	folderList.reserve(cReserveNumber);
	while (bFound)
	{
		bFound = fileFinder.FindNextFile();
		if (fileFinder.IsDirectory() && !fileFinder.IsDots())
		{
			//skip directory which start with underscore (e.g. C:/images/_Name)
			std::string directoryName {fileFinder.GetFileName().GetString()};
			if (directoryName.size() > 0 && '_' != directoryName[0])
			{
				folderList.emplace_back(fileFinder.GetFilePath().GetString());
			}
		}
	}
	return folderList;
}
}


SVRegressionFileSelectSheet::SVRegressionFileSelectSheet(UINT nIDCaption, uint32_t inspectionID, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_InspectionID(inspectionID)
{
}

SVRegressionFileSelectSheet::SVRegressionFileSelectSheet(LPCTSTR pszCaption, uint32_t inspectionID, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_InspectionID(inspectionID)
{
}

SVRegressionFileSelectSheet::~SVRegressionFileSelectSheet()
{
	int iPageCnt = GetPageCount();

	for (int i = iPageCnt-1; i >= 0; i-- )
	{
		SVRegressionFileSelectDlg* pPage = dynamic_cast<SVRegressionFileSelectDlg*>(GetPage(i));
		RemovePage(i);
		if (pPage)
		{
			delete pPage;
		}
	}
}


BEGIN_MESSAGE_MAP(SVRegressionFileSelectSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSVRegressionFileSelectSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
	ON_BN_CLICKED(IDC_ADD_BTN, OnAddImage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectSheet message handlers

void SVRegressionFileSelectSheet::CreatePages(std::vector<RegressionTestStruct>* pRegessionCameraList, std::vector<RegressionTestStruct>* pRegessionImageList, const SvIe::SVVirtualCameraPtrVector&  rCameraVector)
{
	m_pRegressionCameraList = pRegessionCameraList;
	m_pRegressionImageList = pRegessionImageList;
	
	std::string PrevName( _T("Blank") );
	for (const auto* const pCamera : rCameraVector)
	{
		if ( 0 != SvUl::CompareNoCase( PrevName, std::string(pCamera->GetName()) ) )
		{
			SVRegressionFileSelectDlg *pPage = new SVRegressionFileSelectDlg(pCamera->GetName());
			AddPage(pPage);
			PrevName = pCamera->GetName();
		}
	}

	for (const auto& rData : *pRegessionImageList)
	{
		SVRegressionFileSelectDlg* pPage = new SVRegressionFileSelectDlg(rData.Name.c_str(), false, rData.objectId);
		AddPage(pPage);
	}
	
	int iPageCnt = GetPageCount();

	for ( int i = 1; i <= iPageCnt; i++ )
	{
		SetActivePage(i);
	}
	SetActivePage(0);
	
}

void SVRegressionFileSelectSheet::removeFilePage(CPropertyPage* pPage)
{
	RemovePage(pPage);
	updateActiveButton();
}

void SVRegressionFileSelectSheet::OnOK()
{
	int iNumPages = GetPageCount();

	m_pRegressionCameraList->clear();
	m_pRegressionImageList->clear();

	for ( int i = 0; i < iNumPages; i++ )
	{
		SVRegressionFileSelectDlg *pPage = dynamic_cast<SVRegressionFileSelectDlg*>(GetPage(i));
		if ( (pPage) && (pPage->GetSafeHwnd()) )
		{
			if ( pPage->GetFileSelectType() != RegNone )
			{
				RegressionTestStruct regStruct;
				regStruct.stdVectorFile.reserve(cReserveNumber);
				regStruct.iFileMethod = pPage->GetFileSelectType();
				regStruct.Name = pPage->GetPageName();
				regStruct.firstFilepath = pPage->GetSelectedFile();
				regStruct.objectId = pPage->getToolId();
				regStruct.stdVectorFolder = pPage->GetFolders();
				if (pPage->isCamera())
				{
					m_pRegressionCameraList->push_back(std::move(regStruct));
				}
				else
				{
					m_pRegressionImageList->push_back(std::move(regStruct));
				}
			}
		}
		//check to make sure that if using file list that all are of same size.
	}

	try
	{
		ValidateAndFillFileList();
		for (int i = 0; i < iNumPages; i++)
		{
			SVRegressionFileSelectDlg *pPage = dynamic_cast<SVRegressionFileSelectDlg*>(GetPage(i));
			if ((pPage) && (pPage->GetSafeHwnd()))
			{
				pPage->OnOK();
			}
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		switch (rExp.getMessage().m_AdditionalTextId)
		{
		case SvStl::Tid_RegressionTest_NoFileSelected:
		case SvStl::Tid_RegressionTest_FileNotExist:
		case SvStl::Tid_RegressionTest_WrongFormat:
		case SvStl::Tid_RegressionTest_NoFilesSelected:
		case SvStl::Tid_RegressionTest_EmptyDirectory:
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			INT_PTR result = Msg.setMessage(rExp.getMessage(), SvDef::InvalidObjectId, MB_YESNO);
			if (IDYES == result)
			{
				return;
			}
			else
			{
				m_pRegressionCameraList->clear();
				m_pRegressionImageList->clear();
				EndDialog(IDCANCEL);
				return;
			}
		}
		break;
		case SvStl::Tid_RegressionTest_WrongListSize:
		default:
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(rExp.getMessage());
			m_pRegressionCameraList->clear();
			m_pRegressionImageList->clear();
			return;
		}
		break;
		}
	}

	EndDialog(IDOK);
}

void SVRegressionFileSelectSheet::OnAddImage()
{
	auto listResponse = createToolNameList();
	
	if (0 < listResponse.list_size())
	{
		SvOg::SelectToolForNewSourceImageDialog dialog(m_InspectionID, std::move(listResponse));
		if (IDOK == dialog.DoModal())
		{
			auto objectPair = dialog.getSelectedToolNameAndId();
			RegressionTestStruct data;
			data.Name = objectPair.first;
			data.objectId = objectPair.second;
			SVRegressionFileSelectDlg* pPage = new SVRegressionFileSelectDlg(objectPair.first.c_str(), false, objectPair.second);
			AddPage(pPage);
			m_pRegressionImageList->push_back(std::move(data));
			updateActiveButton();
		}
	}
}

std::string SVRegressionFileSelectSheet::MakeFileNameMask(const std::string& rFileName )
{
	if (filenameMatchesImageFormat(rFileName, ImageFileFormat::any))
	{
		size_t positionOfRightmostUnderscore = rFileName.rfind('_');
		auto extension = SvUl::getExtension(rFileName);

		if (std::string::npos != positionOfRightmostUnderscore && extension.length()>2) 
			//we expect a filepath looking like "D:\\TEMP\\XYZ_000001.bmp" here...
		{
			return SvUl::Left(rFileName, positionOfRightmostUnderscore + 1) + _T("*") + extension;
			//... and if our expectation was met, we'll return a pattern string looking like "D:\\TEMP\\XYZ_*.bmp"
		}
	}

	return _T("");
}
	
void SVRegressionFileSelectSheet::ValidateAndFillFileList()
{
	bool isFileSet = false;
	long lListCountSize = 0;

	for ( auto& rStruct : *m_pRegressionCameraList )
	{
		isFileSet = ValidateAndFillFileList(rStruct, lListCountSize) || isFileSet;
	}

	if (false == isFileSet)
	{
		SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoFilesSelected, SvStl::SourceFileParams(StdMessageParams));
		throw Exception;
	}

	for (auto& rStruct : *m_pRegressionImageList)
	{
		isFileSet = ValidateAndFillFileList(rStruct, lListCountSize) || isFileSet;
	}
}

bool SVRegressionFileSelectSheet::ValidateAndFillFileList(RegressionTestStruct& rStruct, long& lListCountSize)
{
	bool isFileSet = false;
	switch (rStruct.iFileMethod)
	{
	case RegressionFileEnum::RegSingleFile:
	{
		//check to see if a selection has been made...
		if (rStruct.firstFilepath.empty())
		{
			SvDef::StringVector msgList;
			msgList.push_back(rStruct.Name);
			SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoFileSelected, msgList, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
		else
		{
			//check to make sure name is a file that exists
			if (0 != ::_access(rStruct.firstFilepath.c_str(), 0))
			{
				SvDef::StringVector msgList;
				msgList.push_back(rStruct.Name);
				SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_FileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams));
				throw Exception;
			}

			if (ImageFileFormat::invalid == inferredMilImageFileFormat(rStruct.firstFilepath))
			{
				SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidImageFileType, SvStl::SourceFileParams(StdMessageParams));
				throw Exception;
			}

			rStruct.stdVectorFile.push_back(rStruct.firstFilepath);
			isFileSet = true;
		}
	}
	break;
	case RegressionFileEnum::RegFileList:
	{
		long lNmbFiles = FillFileList(rStruct);
		if (0 < lNmbFiles)
		{
			isFileSet = true;
			//check number of files...
			if (lListCountSize == 0)
			{
				lListCountSize = lNmbFiles;
			}
			else // check to make sure that size matches the others
			{
				if (lNmbFiles != lListCountSize)
				{
					SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_WrongListSize, SvStl::SourceFileParams(StdMessageParams));
					throw Exception;
				}
			}
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(rStruct.Name);
			SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_WrongFormat, msgList, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
	}
	break;
	case RegressionFileEnum::RegSingleDirectory:
	case RegressionFileEnum::RegSubDirectories:
	case RegressionFileEnum::RegMultDirectories:
	{
		if (rStruct.firstFilepath.empty())
		{
			SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoFilesSelected, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}

		long lNmbFiles = FillFileListFromDirectory(rStruct);
		if (0 < lNmbFiles)
		{
			isFileSet = true;
			//check number of files...
			if (lListCountSize == 0)
			{
				lListCountSize = lNmbFiles;
			}
			else // check to make sure that size matches the others
			{
				if (lNmbFiles != lListCountSize)
				{
					SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_WrongListSize, SvStl::SourceFileParams(StdMessageParams));
					throw Exception;
				}
			}
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(rStruct.Name);
			SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_EmptyDirectory, msgList, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
	}
	break;
	default:
		break;
	}
	return isFileSet;
}

int SVRegressionFileSelectSheet::FillFileList(RegressionTestStruct& rStruct)
{
	int count = 0;
	std::string fileMask = MakeFileNameMask(rStruct.firstFilepath);
	if (fileMask.empty())
	{
		return 0;
	}
	CFileFind fileFinder;
	BOOL bFound = fileFinder.FindFile(fileMask.c_str());

	while (bFound)
	{
		bFound = fileFinder.FindNextFile();
		count++;
		rStruct.stdVectorFile.push_back(std::string(fileFinder.GetFilePath()));
	}
	return count;
}


int SVRegressionFileSelectSheet::FillFileListFromDirectory(RegressionTestStruct& rStruct)
{
	std::string currentPath = rStruct.firstFilepath;

	ImageFileFormat fileFormat = inferredMilImageFileFormat(currentPath);
	if (RegressionFileEnum::RegSingleDirectory == rStruct.iFileMethod)
	{
		if (ImageFileFormat::invalid == fileFormat)
		{
			SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidImageFileType, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
	}

	if (ImageFileFormat::invalid != fileFormat)
	{ 
		size_t Pos = currentPath.rfind('\\');
		if (std::string::npos != Pos)
		{
			currentPath = currentPath.substr(0, Pos);
		}
	}
	else
	{
		fileFormat = ImageFileFormat::any;
		//in this case we assume a directory path is contained in rStruct.firstFilepath and that the appropriate file type is bitmap
	}
	int count = collectMatchingFilesInDirectories(rStruct, currentPath, fileFormat);
	return count;
}


int SVRegressionFileSelectSheet::collectMatchingFilesInDirectories(RegressionTestStruct& rStruct, const std::string& rParentDirectory, ImageFileFormat fileFormat)
{
	int count {0};
	if (RegressionFileEnum::RegMultDirectories == rStruct.iFileMethod)
	{
		for (auto const &folder: rStruct.stdVectorFolder)
		{
			// cppcheck-suppress useStlAlgorithm ;  using std::accumulate would be more difficult to read (see below)
			count+= collectMatchingFilesInDirectory(rStruct, folder, fileFormat);
		}
		
	}
	else
	{
		count = collectMatchingFilesInDirectory(rStruct, rParentDirectory, fileFormat);

		if (RegressionFileEnum::RegSubDirectories == rStruct.iFileMethod)
		{
			SvDef::StringVector folderList = findSubdirectories(rParentDirectory);

			//StrCmpLogicalW is the sorting function used by Windows Explorer
			auto FolderCompare = [](const std::string& rLhs, const std::string& rRhs) { return ::StrCmpLogicalW(_bstr_t {rLhs.c_str()}, _bstr_t {rRhs.c_str()}) < 0; };
			std::sort(folderList.begin(), folderList.end(), FolderCompare);
			count = std::accumulate(folderList.begin(), folderList.end(), count, [this, &rStruct, fileFormat](int sum, const auto& rFolder) {return sum + collectMatchingFilesInDirectories(rStruct, rFolder, fileFormat); });
		}
	}
	return count;
}


int SVRegressionFileSelectSheet::collectMatchingFilesInDirectory(RegressionTestStruct& rStruct, const std::string& rParentDirectory, ImageFileFormat fileFormat)
{
	int count = 0;

	for (auto extension : allImageFileNameExtensions(fileFormat))
	{
		std::string fileMask = rParentDirectory + _T("\\*") + extension;
		CFileFind fileFinder;
		BOOL bFound = fileFinder.FindFile(fileMask.c_str());
		std::string firstFile;
		while (bFound)
		{
			bFound = fileFinder.FindNextFile();
			rStruct.stdVectorFile.push_back(std::string(fileFinder.GetFilePath()));
			if (0 == count)
			{
				firstFile = fileFinder.GetFilePath();
			}
			count++;
		}
		if (false == firstFile.empty())
		{
			rStruct.fileSortRange.emplace_back(firstFile, fileFinder.GetFilePath());
		}
	}

	return count;
}


BOOL SVRegressionFileSelectSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	ModifyStyle(WS_SYSMENU, WS_THICKFRAME, SWP_FRAMECHANGED);
	int iPageCnt = GetPageCount();

	for ( int i = 0; i <= iPageCnt; i++ )
	{
		SetActivePage(i);
	}
	SetActivePage(0);

	SVRegressionFileSelectDlg *pPage;
	if (m_pRegressionCameraList->size() > 0 || m_pRegressionImageList->size() > 0 )
	{
		for ( int iCount = 0; iCount <= iPageCnt-1; iCount++ )
		{
			pPage = dynamic_cast<SVRegressionFileSelectDlg*>(GetPage(iCount));
			if (pPage)
			{
				std::string TmpName = pPage->GetPageName().GetString();
				if (pPage->isCamera())
				{
					auto foundIter = std::find_if(m_pRegressionCameraList->begin(), m_pRegressionCameraList->end(), [TmpName](const auto& rRegStruct)->bool
						{
							return rRegStruct.Name == TmpName;
						});
					if (m_pRegressionCameraList->end() != foundIter)
					{
						pPage->SetRegressionData(&(*foundIter));
					}
				}
				else
				{
					auto foundIter = std::find_if(m_pRegressionImageList->begin(), m_pRegressionImageList->end(), [TmpName](const auto& rRegStruct)->bool
						{
							return rRegStruct.Name == TmpName;
						});
					if (m_pRegressionImageList->end() != foundIter)
					{
						pPage->SetRegressionData(&(*foundIter));
					}
				}
			}
		}

	}

	if (0 < SvOg::getToolsWithReplaceableSourceImage(m_InspectionID).list_size())
	{
		CRect rectWnd, rect;
		GetWindowRect(rectWnd);
		SetWindowPos(NULL, 0, 0, rectWnd.Width(), rectWnd.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		GetDlgItem(IDOK)->GetWindowRect(rect);
		m_addImageButton.CreateEx(WS_EX_CLIENTEDGE, _T("Button"), "Add Image", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, CRect{ 10, rectWnd.Height() - 70, 90, rectWnd.Height() - 44 }, this, IDC_ADD_BTN);

		m_addImageButton.SetFont(GetFont());
		CenterWindow();
	}
	SetupDynamicLayout();
	return bResult;
}

SvPb::GetToolsWithReplaceableSourceImageResponse SVRegressionFileSelectSheet::createToolNameList() const
{
	SvPb::GetToolsWithReplaceableSourceImageResponse listResponse = SvOg::getToolsWithReplaceableSourceImage(m_InspectionID);
	auto* pList = listResponse.mutable_list();
	int iNumPages = GetPageCount();
	for (int i = 0; i < iNumPages; i++)
	{
		SVRegressionFileSelectDlg* pPage = dynamic_cast<SVRegressionFileSelectDlg*>(GetPage(i));
		if (nullptr != pPage && false == pPage->isCamera())
		{
			std::string name{ pPage->GetPageName() };
			auto iter = std::find_if(pList->begin(), pList->end(), [name](const auto& rNamePair) { return rNamePair.objectname() == name; });
			if (pList->end() != iter)
			{
				pList->erase(iter);
			}
		}
	}
	return listResponse;
}

void SVRegressionFileSelectSheet::updateActiveButton()
{
	GetDlgItem(IDC_ADD_BTN)->EnableWindow(0 < createToolNameList().list_size());
}

void SVRegressionFileSelectSheet::SetupDynamicLayout()
{
	EnableDynamicLayout(TRUE);

	CRect clientRect;
	GetClientRect(&clientRect);

	constexpr int gripperSize = 12;
	CRect gripperRect(clientRect.right - gripperSize, clientRect.bottom - gripperSize, clientRect.right, clientRect.bottom);

	m_Gripper.Create(SBS_SIZEGRIP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, gripperRect, this, IDC_RESIZE_GRIPPER);

	auto pManager = GetDynamicLayout();
	if (pManager != nullptr)
	{
		pManager->Create(this);

		for (CWnd* child = GetWindow(GW_CHILD); child; child = child->GetWindow(GW_HWNDNEXT))
		{

			// All buttons need to be moved 100% in all directions
			if (child->SendMessage(WM_GETDLGCODE) & DLGC_BUTTON)
			{
				pManager->AddItem(child->GetSafeHwnd(),
					CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());
			}
			else // This will be the main tab control which needs to be stretched in both directions
			{
				pManager->AddItem(child->GetSafeHwnd(),
					CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
			}

		}
	}
}
