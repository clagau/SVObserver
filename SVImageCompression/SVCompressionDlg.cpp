// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCompressionDlg
// * .File Name       : $Workfile:   SVCompressionDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:22:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVImageCompression.h"
#include "SVImageCompressionClass.h"
#include "SVCompressionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
This constructor creates and initializes the object for use based on a desired parent window.
*/
SVCompressionDlg::SVCompressionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVCompressionDlg::IDD, pParent)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(SVCompressionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   svmpbstrOutputImage = NULL;
}


/*
When the last reference for an automation object is released OnFinalRelease is called.  The base class will automatically delete the object.  Add additional cleanup required for your object before calling the base class.
*/
void SVCompressionDlg::OnFinalRelease()
{
	CDialog::OnFinalRelease();
}

/*
This method is used to handle the data exchange between the dialog and the application.
*/
void SVCompressionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVCompressionDlg)
//	DDX_Control(pDX, IDC_MFCPicture, svmMFCPicture);
	DDX_Control(pDX, IDC_LEADCTRL1, svmLeadControl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVCompressionDlg, CDialog)
	//{{AFX_MSG_MAP(SVCompressionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(SVCompressionDlg, CDialog)
	//{{AFX_DISPATCH_MAP(SVCompressionDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISVCompressionDlg to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {5189AC74-EE8B-4842-B8B9-CFFD67D56A25}
static const IID IID_ISVCompressionDlg =
{ 0x5189ac74, 0xee8b, 0x4842, { 0xb8, 0xb9, 0xcf, 0xfd, 0x67, 0xd5, 0x6a, 0x25 } };

BEGIN_INTERFACE_MAP(SVCompressionDlg, CDialog)
	INTERFACE_PART(SVCompressionDlg, IID_ISVCompressionDlg, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVCompressionDlg message handlers

BEGIN_EVENTSINK_MAP(SVCompressionDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(SVCompressionDlg)
	ON_EVENT(SVCompressionDlg, IDC_LEADCTRL1, 47 /* SaveBuffer */, OnSaveBufferLeadctrl1, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()



/*
This method allocated the buffer required to do the desired process.
*/
void SVCompressionDlg::OnSaveBufferLeadctrl1(long uRequiredSize) 
{
   HRESULT  hr;

   hr = AllocateSaveBuffer (uRequiredSize);
}

/*
Check to see if the current basic string exists and assign appropriate current length. Check to see if current size is large enough to meet our requirements. If it is, then do nothing.  If the current buffer is not large enough, allocate a new buffer, copy over the contents of the old buffer, and then destroy the old buffer.
*/
HRESULT SVCompressionDlg::AllocateSaveBuffer (long auRequiredSize) 
{
   long     lCurrentSize;
   HRESULT  hr;
	BSTR		l_bstrOld;

   hr = 0;


   while (1)
   {
      if (svmpbstrOutputImage == NULL)
      {
         hr = -1585;
         break;
      }
      
		l_bstrOld = *svmpbstrOutputImage;

//--- Assign proper buffer size.
      if (l_bstrOld != NULL)
      {
         lCurrentSize = SysStringByteLen (l_bstrOld);
		}
		else
		{
			lCurrentSize = 0;
		}

		if (lCurrentSize >= auRequiredSize)
		{
//------ If current buffer is already large enough, then return.
			break;
		}

//--- Allocate new buffer.
      *svmpbstrOutputImage = SysAllocStringByteLen (NULL , auRequiredSize);
      svmLeadControl.SetSaveBufferSize (auRequiredSize);

      svmLeadControl.SetSaveBufferAddress ((long) *svmpbstrOutputImage);

		if (l_bstrOld != NULL)
		{
//------ If there was an old buffer, copy the contents of the old buffer into
//------ the new buffer, and then delete the old buffer.
			memcpy (*svmpbstrOutputImage, 
					  l_bstrOld, 
					  lCurrentSize);

			if (hr < 0)
			{
				break;
			}

			SysFreeString (l_bstrOld);
			l_bstrOld = NULL;
		}
   	   
      break;
   } // while (1)

   return hr;
}

