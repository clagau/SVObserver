// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOResource/resource.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVOGui/SelectToolForNewSourceImageDialog.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(SVRegressionFileSelectSheet, CPropertySheet)

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
				regStruct.iFileMethod = pPage->GetFileSelectType();
				regStruct.Name = pPage->GetPageName();
				regStruct.FirstFile = pPage->GetSelectedFile();
				regStruct.objectId = pPage->getToolId();
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
	if (0 < SvOg::getToolsWithReplaceableSourceImage(m_InspectionID).list_size())
	{
		SvOg::SelectToolForNewSourceImageDialog dialog(m_InspectionID);
		if (IDOK == dialog.DoModal())
		{
			auto objectPair = dialog.getSelectedTool();
			std::string SelectedItems = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult();
			SVObjectReference objectRef{ SelectedItems };
			RegressionTestStruct data;
			data.Name = objectPair.first;
			data.objectId = objectPair.second;
			SVRegressionFileSelectDlg* pPage = new SVRegressionFileSelectDlg(objectPair.first.c_str(), false, objectPair.second);
			AddPage(pPage);
			m_pRegressionImageList->push_back(std::move(data));
		}
	}
}

std::string SVRegressionFileSelectSheet::MakeFileNameMask( const std::string& rFileName )
{
	std::string Result;
	size_t FileNameLength = rFileName.size();

	//check to see if last 4 chars = .bmp 
	if( 0 == SvUl::CompareNoCase( SvUl::Right(rFileName, 4), std::string( _T(".bmp") ) ) )
	{
		Result = SvUl::Left( rFileName, FileNameLength - 4 );
	
		size_t Pos = Result.rfind('_');
		if ( std::string::npos != Pos )
		{
			Result = SvUl::Left( rFileName, Pos+1 );
			Result = Result + _T("*.bmp");
		}
	}

	return Result;
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
		if (rStruct.FirstFile.empty())
		{
			SvDef::StringVector msgList;
			msgList.push_back(rStruct.Name);
			SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoFileSelected, msgList, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
		else
		{
			//check to make sure name is a file that exists
			if (0 != ::_access(rStruct.FirstFile.c_str(), 0))
			{
				SvDef::StringVector msgList;
				msgList.push_back(rStruct.Name);
				SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_FileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams));
				throw Exception;
			}

			if (0 != SvUl::CompareNoCase(SvUl::Right(rStruct.FirstFile.c_str(), 4), std::string(_T(".bmp"))))
			{
				SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoBmpFileSelected, SvStl::SourceFileParams(StdMessageParams));
				throw Exception;
			}
			rStruct.stdVectorFile.push_back(rStruct.FirstFile);
			SvDef::StringVector::iterator iter;

			iter = std::find(rStruct.stdVectorFile.begin(), rStruct.stdVectorFile.end(), rStruct.FirstFile);

			//set the structs starting and current position to the start
			rStruct.stdIteratorStart = iter;
			rStruct.stdIteratorCurrent = rStruct.stdIteratorStart;
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
	{
		if (rStruct.FirstFile.empty())
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
	std::string fileMask = MakeFileNameMask(rStruct.FirstFile);
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
	std::sort(rStruct.stdVectorFile.begin(), rStruct.stdVectorFile.end());
	//find the starting position
	SvDef::StringVector::iterator iter;

	iter = std::find(rStruct.stdVectorFile.begin(), rStruct.stdVectorFile.end(), rStruct.FirstFile);

	//set the structs starting and current position to the start
	rStruct.stdIteratorStart = iter;
	rStruct.stdIteratorCurrent = rStruct.stdIteratorStart;
	return count;
}

int SVRegressionFileSelectSheet::FillFileListFromDirectory(RegressionTestStruct& rStruct)
{
	std::string currentPath = rStruct.FirstFile;
	if (RegressionFileEnum::RegSingleDirectory == rStruct.iFileMethod)
	{
		//For Single Directory a bmp-File is selected. For this reason the filename has to be removed.
		if (0 != SvUl::CompareNoCase(SvUl::Right(currentPath.c_str(), 4), std::string(_T(".bmp"))))
		{
			SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoBmpFileSelected, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
	}

	if (0 == SvUl::CompareNoCase(SvUl::Right(currentPath.c_str(), 4), std::string(_T(".bmp"))))
	{
		size_t Pos = currentPath.rfind('\\');
		if (std::string::npos != Pos)
		{
			currentPath = currentPath.substr(0, Pos);
		}
	}
	int count = FillFileListFromDirectory(rStruct, currentPath);

	//set the structs starting and current position to the start
	rStruct.stdIteratorStart = rStruct.stdVectorFile.begin();
	rStruct.stdIteratorCurrent = rStruct.stdIteratorStart;
	return count;
}

int SVRegressionFileSelectSheet::FillFileListFromDirectory(RegressionTestStruct& rStruct, const std::string& rCurrentPath)
{
	int count = 0;

	std::string fileMask = rCurrentPath + _T("\\*.bmp");
	CFileFind fileFinder;
	BOOL bFound = fileFinder.FindFile(fileMask.c_str());

	while (bFound)
	{
		bFound = fileFinder.FindNextFile();
		count++;
		rStruct.stdVectorFile.push_back(std::string(fileFinder.GetFilePath()));
	}

	if (RegressionFileEnum::RegSubDirectories == rStruct.iFileMethod)
	{
		fileMask = rCurrentPath + _T("\\*");
		bFound = fileFinder.FindFile(fileMask.c_str());

		while (bFound)
		{
			bFound = fileFinder.FindNextFile();
			if (fileFinder.IsDirectory() && !fileFinder.IsDots())
			{
				//skip directory which start with underscore (e.g. C:/images/_Name)
				std::string tmpPathName(fileFinder.GetFilePath());
				if (tmpPathName.size() > rCurrentPath.size()+1 && '_' != tmpPathName[rCurrentPath.size()+1])
				{
					count += FillFileListFromDirectory(rStruct, tmpPathName);
				}
			}			
		}
	}

	return count;
}

BOOL SVRegressionFileSelectSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	int iPageCnt = GetPageCount();

	for ( int i = 0; i <= iPageCnt; i++ )
	{
		SetActivePage(i);
	}
	SetActivePage(0);

	SVRegressionFileSelectDlg *pPage;
	if (m_pRegressionCameraList->size() > 0 || m_pRegressionImageList->size() > 0 )
	{
		for ( int l_iCount = 0; l_iCount <= iPageCnt-1; l_iCount++ )
		{
			pPage = dynamic_cast<SVRegressionFileSelectDlg*>(GetPage(l_iCount));
			if (pPage)
			{
				std::string TmpName = pPage->GetPageName();
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
	else //No list set yet, set in first tab the mode to RegFileList
	{
		pPage = dynamic_cast<SVRegressionFileSelectDlg*>(GetPage(0));
		if (nullptr != pPage)
		{
			RegressionTestStruct tmpStruct;
			tmpStruct.iFileMethod = RegressionFileEnum::RegFileList;
			pPage->SetRegressionData(&tmpStruct);
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
	return bResult;
}

