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
#include "svobserver.h"
#include "SVRegressionFileSelectSheet.h"
#include "SVRegressionFileSelectDlg.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSVRegressionFileSelectSheet, CPropertySheet)

CSVRegressionFileSelectSheet::CSVRegressionFileSelectSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CSVRegressionFileSelectSheet::CSVRegressionFileSelectSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CSVRegressionFileSelectSheet::~CSVRegressionFileSelectSheet()
{
	int iPageCnt = GetPageCount();
	CSVRegressionFileSelectDlg *pPage;

	for (int i = iPageCnt-1; i >= 0; i-- )
	{
		pPage = (CSVRegressionFileSelectDlg*) GetPage(i);
		RemovePage(i);
		if ( pPage )
			delete pPage;
	}

	int iStructCnt = static_cast<int>(m_pRegressionList->GetCount());
}


BEGIN_MESSAGE_MAP(CSVRegressionFileSelectSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSVRegressionFileSelectSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectSheet message handlers

void CSVRegressionFileSelectSheet::CreatePages(CList<RegressionTestStruct*,RegressionTestStruct*>*RegessionList, SvIe::SVVirtualCameraPtrSet CameraList )
{
	m_pRegressionList = RegessionList;
	
	SvIe::SVVirtualCameraPtrSet::iterator iter;

	std::string PrevName( _T("Blank") );
	for ( iter = CameraList.begin(); iter != CameraList.end(); ++iter )
	{
		if ( 0 != SvUl::CompareNoCase( PrevName, std::string((*iter)->GetName()) ) )
		{
			CSVRegressionFileSelectDlg *pPage = new CSVRegressionFileSelectDlg((*iter)->GetName());
			AddPage(pPage);
			PrevName = (*iter)->GetName();
		}
	}
	
	int iPageCnt = GetPageCount();

	for ( int i = 1; i <= iPageCnt; i++ )
	{
		SetActivePage(i);
	}
	SetActivePage(0);
	
}


void CSVRegressionFileSelectSheet::OnOK()
{
	int iNumPages = GetPageCount();

	if ( !m_pRegressionList->IsEmpty() )
	{
		ClearRegressionList();
	}

	for ( int i = 0; i < iNumPages; i++ )
	{
		RegressionTestStruct *pStruct = new RegressionTestStruct;
		CSVRegressionFileSelectDlg *pPage = (CSVRegressionFileSelectDlg*)GetPage(i);
		if ( (pPage) && (pPage->GetSafeHwnd()) )
		{
			if ( pPage->GetFileSelectType() != RegNone )
			{
				pStruct->iFileMethod = pPage->GetFileSelectType();
				pStruct->Camera = pPage->GetPageName();
				pStruct->FirstFile = pPage->GetSelectedFile();
				m_pRegressionList->AddTail(pStruct);
			}
		}
		//check to make sure that if using file list that all are of same size.
	}

	try
	{
		ValidateAndFillFileList();
		for (int i = 0; i < iNumPages; i++)
		{
			CSVRegressionFileSelectDlg *pPage = (CSVRegressionFileSelectDlg*)GetPage(i);
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
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			INT_PTR result = Msg.setMessage( rExp.getMessage(), GUID_NULL, MB_YESNO);
			if (IDYES == result)
			{
				return;
			}
			else
			{
				ClearRegressionList();
				EndDialog(IDCANCEL);
				return;
			}
		}
		break;
		case SvStl::Tid_RegressionTest_WrongListSize:
		default:
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(rExp.getMessage());
			ClearRegressionList();
			return;
		}
		break;
		}
	}

	EndDialog(IDOK);
}

std::string CSVRegressionFileSelectSheet::MakeFileNameMask( const std::string& rFileName )
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
	
void CSVRegressionFileSelectSheet::ValidateAndFillFileList()
{
	long lTotalNumFiles = 0;

	int iCount = static_cast<int>(m_pRegressionList->GetCount());
	long lListCountSize = 0;

	for ( int i = 0; i < iCount; i++ )
	{
		RegressionTestStruct *pStruct;

		POSITION pos = m_pRegressionList->FindIndex(i);
		if (pos)
		{
			pStruct = m_pRegressionList->GetAt(pos);

			switch (pStruct->iFileMethod)
			{
			case RegressionFileEnum::RegSingleFile:
			{
				//check to see if a selection has been made...
				if (pStruct->FirstFile.empty())
				{
					SvDef::StringVector msgList;
					msgList.push_back(pStruct->Camera);
					SvStl::MessageContainer Exception( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoFileSelected, msgList, SvStl::SourceFileParams(StdMessageParams) );
					throw Exception;
				}
				else
				{
					//check to make sure name is a file that exists
					if (0 != ::_access(pStruct->FirstFile.c_str(), 0))
					{
						SvDef::StringVector msgList;
						msgList.push_back(pStruct->Camera);
						SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_FileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams));
						throw Exception;
					}

					if (0 != SvUl::CompareNoCase(SvUl::Right(pStruct->FirstFile.c_str(), 4), std::string(_T(".bmp"))))
					{
						SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoBmpFileSelected, SvStl::SourceFileParams(StdMessageParams));
						throw Exception;
					}
					pStruct->stdVectorFile.push_back(pStruct->FirstFile);
					SvDef::StringVector::iterator iter;

					iter = std::find(pStruct->stdVectorFile.begin(), pStruct->stdVectorFile.end(), pStruct->FirstFile);

					//set the structs starting and current position to the start
					pStruct->stdIteratorStart = iter;
					pStruct->stdIteratorCurrent = pStruct->stdIteratorStart;
					lTotalNumFiles++;
				}
			}
			break;
			case RegressionFileEnum::RegFileList:
			{
				long lNmbFiles = FillFileList(*pStruct);
				lTotalNumFiles += lNmbFiles;

				if (0 < lNmbFiles)
				{
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
					msgList.push_back(pStruct->Camera);
					SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_WrongFormat, msgList, SvStl::SourceFileParams(StdMessageParams));
					throw Exception;
				}
			}
			break;
			case RegressionFileEnum::RegSingleDirectory:
			case RegressionFileEnum::RegSubDirectories:
			{
				if (pStruct->FirstFile.empty())
				{
					SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoFilesSelected, SvStl::SourceFileParams(StdMessageParams));
					throw Exception;
				}

				long lNmbFiles = FillFileListFromDirectory(*pStruct);
				lTotalNumFiles += lNmbFiles;

				if (0 < lNmbFiles)
				{
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
					msgList.push_back(pStruct->Camera);
					SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_EmptyDirectory, msgList, SvStl::SourceFileParams(StdMessageParams));
					throw Exception;
				}
			}
			break;
			default:
				break;
			}
		}
	}

	//check to see if total num of files is > 0
	if ( lTotalNumFiles == 0 )
	{
		SvStl::MessageContainer Exception(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoFilesSelected, SvStl::SourceFileParams(StdMessageParams));
		throw Exception;
	}
}

void CSVRegressionFileSelectSheet::ClearRegressionList()
{
	int iCount = static_cast<int>(m_pRegressionList->GetCount());
	for ( int i = iCount-1; i >= 0; i-- )
	{
		POSITION pos = m_pRegressionList->FindIndex(i);
		if ( pos )
		{
			m_pRegressionList->RemoveAt(pos);
		}
	}
}

int CSVRegressionFileSelectSheet::FillFileList(RegressionTestStruct& rStruct)
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

int CSVRegressionFileSelectSheet::FillFileListFromDirectory(RegressionTestStruct& rStruct)
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

int CSVRegressionFileSelectSheet::FillFileListFromDirectory(RegressionTestStruct& rStruct, const std::string& rCurrentPath)
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
					char tmp = tmpPathName[rCurrentPath.size()+1];
					count += FillFileListFromDirectory(rStruct, tmpPathName);
				}
			}			
		}
	}

	return count;
}

BOOL CSVRegressionFileSelectSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	RegressionTestStruct *pTmpStruct;

	int iPageCnt = GetPageCount();

	for ( int i = 0; i <= iPageCnt; i++ )
	{
		SetActivePage(i);
	}
	SetActivePage(0);

	CSVRegressionFileSelectDlg *pPage;
	int iRegListCnt = static_cast<int>(m_pRegressionList->GetCount());
	if ( iRegListCnt > 0 )
	{
		for ( int l_iCount = 0; l_iCount <= iPageCnt-1; l_iCount++ )
		{
			pPage = (CSVRegressionFileSelectDlg*)GetPage(l_iCount);
			if ( pPage )
			{
				std::string TmpName = pPage->GetPageName();
				
				bool l_bFound = false;
				for ( int iPos = 0; iPos <= iRegListCnt && !l_bFound; iPos++ )
				{
					POSITION pos = m_pRegressionList->FindIndex(iPos);
					if ( pos )
					{
						pTmpStruct = m_pRegressionList->GetAt(pos);
						if ( pTmpStruct->Camera == TmpName )
						{
							pPage->SetRegressionData(pTmpStruct);
							l_bFound = true;
						}
					}
				}
			}
		}
	}
	else //No list set yet, set in first tab the mode to RegFileList
	{
		pPage = (CSVRegressionFileSelectDlg*)GetPage(0);
		if (nullptr != pPage)
		{
			RegressionTestStruct tmpStruct;
			tmpStruct.iFileMethod = RegressionFileEnum::RegFileList;
			pPage->SetRegressionData(&tmpStruct);
		}
	}	

	return bResult;
}

