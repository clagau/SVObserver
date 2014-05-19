//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTwoImagePage
//* .File Name       : $Workfile:   SVToolAdjustmentDialogTwoImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 14:37:58  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolAdjustmentDialogTwoImagePage.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVGageTool.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVImageArithmetic.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTwoImagePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogTwoImagePageClass)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelChangeCombo2)
	ON_CBN_SELCHANGE(IDC_OPERATOR_COMBO, OnSelchangeOperatorCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogTwoImagePageClass::SVToolAdjustmentDialogTwoImagePageClass( SVToolAdjustmentDialogSheetClass* Parent ) 
	: CPropertyPage(SVToolAdjustmentDialogTwoImagePageClass::IDD)
{
	pFirstSourceImage  = NULL;
	pSecondSourceImage = NULL;
	pFirstImageInputInfo  = NULL;
	pSecondImageInputInfo = NULL;
	pArithmeticOperator = NULL;
	pParentDialog      = Parent;
	firstOldIndex      = -1;
	secondOldIndex	   = -1;

	pToolSet = NULL;
	pTool = NULL;

	//{{AFX_DATA_INIT(SVToolAdjustmentDialogTwoImagePageClass)
	//}}AFX_DATA_INIT
}

SVToolAdjustmentDialogTwoImagePageClass::~SVToolAdjustmentDialogTwoImagePageClass()
{
}

HRESULT SVToolAdjustmentDialogTwoImagePageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool != NULL )
	{
		UpdateData( TRUE ); // get data from dialog

		long lOperator = ( long ) operatorCtrl.GetItemData( operatorCtrl.GetCurSel() );

		l_hrOk = AddInputRequest( pArithmeticOperator, lOperator );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( pTool );
		}

		if( lOperator == SV_IMGOP_DOUBLE_HEIGHT || lOperator == SV_IMGOP_FLIP_VERTICAL || lOperator == SV_IMGOP_FLIP_HORIZONTAL )
		{
			secondAvailableSourceImageListBoxCtl.EnableWindow( FALSE );
			secondImageCtrl.ShowWindow( SW_HIDE );
		}// end if
		else
		{
			secondAvailableSourceImageListBoxCtl.EnableWindow( TRUE );
			secondImageCtrl.ShowWindow( SW_SHOW );
		}// end else

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogTwoImagePageClass::refresh()
{
	SetInspectionData();

	firstImageCtrl.refresh();
	secondImageCtrl.refresh();
}

void SVToolAdjustmentDialogTwoImagePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogTwoImagePageClass)
	DDX_Control(pDX, IDC_OPERATOR_COMBO, operatorCtrl);
	DDX_Control(pDX, IDC_SECOND_IMAGE, secondImageCtrl);
	DDX_Control(pDX, IDC_FIRST_IMAGE, firstImageCtrl);
	DDX_Control(pDX, IDC_COMBO1, firstAvailableSourceImageListBoxCtl);
	DDX_Control(pDX, IDC_COMBO2, secondAvailableSourceImageListBoxCtl);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogTwoImagePageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if( pParentDialog && ( pTool = pParentDialog->GetTool() ) )
	{
		SetTaskObject( pTool );

		pToolSet = ( SVToolSetClass * ) pTool->GetAncestor( SVToolSetObjectType );
		if( pToolSet &&	pToolSet->getCurrentImage() )
		{
			// First clean up image list
			imageList.RemoveAll();

			SVInObjectInfoStruct* l_psvImageInfo = NULL;
			SVInObjectInfoStruct* l_psvLastImageInfo = NULL;

			// Try to find two image inputs...
			// NOTE: 
			//		 We want to find the two image inputs of the image arithmetic object !!!
			//		 Be careful, if you want to reuse this page for other dual image inputs,
			//		 do a type check!
			//		 Otherwise you could get the wrong image input ( i.e. any other object
			//		 which lives in the tool object and has his own image input, like the 
			//		 in place image operator list image input in the image tool!!! )
			// RO_22Mar2000
			while( l_psvImageInfo == NULL && pTool->FindNextInputImageInfo( l_psvImageInfo, l_psvLastImageInfo ) == S_OK )
			{
				SVImageInfoClass ImageInfo;

				if( l_psvImageInfo != NULL && l_psvImageInfo->ObjectTypeInfo.ObjectType == SVImageArithmeticObjectType )
				{
					if( pFirstImageInputInfo == NULL )
					{
						pFirstImageInputInfo = l_psvImageInfo;

						if( pFirstImageInputInfo->IsConnected() )
							pFirstSourceImage = dynamic_cast <SVImageClass*> (l_psvImageInfo->GetInputObjectInfo().PObject);

						l_psvLastImageInfo = l_psvImageInfo;
						l_psvImageInfo = NULL;
					}
					else
					{
						pSecondImageInputInfo = l_psvImageInfo;

						if( pSecondImageInputInfo->IsConnected() )
							pSecondSourceImage = dynamic_cast <SVImageClass*> (l_psvImageInfo->GetInputObjectInfo().PObject);
					}
				}
				else
				{
					l_psvLastImageInfo = l_psvImageInfo;
					l_psvImageInfo = NULL;
				}
			}

			// Find all available Images...
			SVObjectTypeInfoStruct imageObjectInfo;
			imageObjectInfo.ObjectType = SVImageObjectType;

			SVGetObjectDequeByTypeVisitor l_Visitor( imageObjectInfo );

			SVObjectManagerClass::Instance().VisitElements( l_Visitor, pToolSet->GetUniqueObjectID() );

			SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

			for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
			{
				SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

				if( pImage != NULL )
				{
					SVImageInfoClass ImageInfo = pImage->GetImageInfo();

					if( ( pImage->ObjectAttributesAllowed() & SV_HIDDEN ) == 0 )
					{
						long l_lBandNumber = 1;

						ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );

						#define __REMOVE_GAGE_TOOL_IMAGE
						#ifdef __REMOVE_GAGE_TOOL_IMAGE   //27 Oct 1999 - frb.
							//
							// Check for unwanted images here.
							//
							SVToolClass * pImageOwnerTool = dynamic_cast<SVToolClass *>(ImageInfo.GetOwner());
							if(pImageOwnerTool)
							{
								if(!SV_IS_KIND_OF(pImageOwnerTool, SVGageToolClass))
								{
									// Insert Image into combo box...
									if ( l_lBandNumber == 1)
									{
										imageList.Add( pImage );
									}
								}
							}
							else
							{
								// Insert Image into combo box...
								if ( l_lBandNumber == 1)
								{
									imageList.Add( pImage );
								}
							}
						#else
							// Insert Image into combo box...
							if ( l_lBandNumber == 1)
							{
								imageList.Add( pImage );
							}
						#endif
					}
				}
			}


			firstImageCtrl.UpdateImageInfo( pFirstSourceImage );
			firstImageCtrl.refresh();

			secondImageCtrl.UpdateImageInfo( pSecondSourceImage );
			secondImageCtrl.refresh();

			// Init first source image list...
			firstOldIndex  = firstAvailableSourceImageListBoxCtl.init( &imageList, pFirstSourceImage );

			// Init second source image list...
			secondOldIndex = secondAvailableSourceImageListBoxCtl.init( &imageList, pSecondSourceImage );


			// Get Current Arithmetic Operator...
			SVObjectTypeInfoStruct operatorObjectInfo;
			operatorObjectInfo.ObjectType = SVLongValueObjectType;
			operatorObjectInfo.EmbeddedID = SVArithmeticOperatorObjectGuid;
			pArithmeticOperator = reinterpret_cast<SVLongValueObjectClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&operatorObjectInfo) ));
			// Fill Arithmetic Combo...
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "AND" ) ), static_cast<DWORD_PTR>(SVImageAnd) );
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "OR" ) ),  static_cast<DWORD_PTR>(SVImageOr) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "XOR" ) ), static_cast<DWORD_PTR>(SVImageXOr) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "NOR" ) ), static_cast<DWORD_PTR>(SVImageNor) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "XNOR" ) ), static_cast<DWORD_PTR>(SVImageXNor) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "NAND" ) ), static_cast<DWORD_PTR>(SVImageNand) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "ADD" ) ), static_cast<DWORD_PTR>(SVImageAddSaturation) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "SUB" ) ), static_cast<DWORD_PTR>(SVImageSubSaturation) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "MULT" ) ), static_cast<DWORD_PTR>(SVImageMulSaturation) );
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "DIV" ) ), static_cast<DWORD_PTR>(SVImageDiv) );
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "MIN" ) ), static_cast<DWORD_PTR>(SVImageMin) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "MAX" ) ), static_cast<DWORD_PTR>(SVImageMax) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "PASS" ) ), static_cast<DWORD_PTR>(SVImagePass) );	
			
			// Special Image Operator... ( not defined by MIL ! )
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "AVERAGE" ) ), static_cast<DWORD_PTR>(SV_IMGOP_AVERAGE) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "DOUBLE HEIGHT" ) ), static_cast<DWORD_PTR>(SV_IMGOP_DOUBLE_HEIGHT) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "FLIP VERTICAL" ) ), static_cast<DWORD_PTR>(SV_IMGOP_FLIP_VERTICAL) );	
			operatorCtrl.SetItemData( operatorCtrl.AddString( _T( "FLIP HORIZONTAL" ) ), static_cast<DWORD_PTR>(SV_IMGOP_FLIP_HORIZONTAL) );

			// Set Default Operator...
			long lOperator = 0;
			pArithmeticOperator->GetValue( lOperator );
			for( int i = 0; i < operatorCtrl.GetCount(); ++ i )
			{
				if( lOperator == ( long ) operatorCtrl.GetItemData( i ) )
				{
					operatorCtrl.SetCurSel( i );
					OnSelchangeOperatorCombo();
					break;
				}
			}

			UpdateData( FALSE ); // set data to dialog

			return TRUE;
		}
	}

	ASSERT( 0 );

	// Not valid call...
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );

	return FALSE;
}


void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeCombo1()
{
	UpdateData( TRUE ); // get data from dialog

	int index = ( int ) firstAvailableSourceImageListBoxCtl.GetItemData( firstAvailableSourceImageListBoxCtl.GetCurSel() );

	if( index != LB_ERR && index >= 0 && index < imageList.GetSize() )
	{
		// Get new input...
		pFirstSourceImage = imageList.GetAt( index );

		pParentDialog->GetTool()->ConnectToImage( pFirstImageInputInfo, pFirstSourceImage );

		pFirstSourceImage = dynamic_cast<SVImageClass*>( pFirstImageInputInfo->GetInputObjectInfo().PObject );

		firstImageCtrl.UpdateImageInfo( pFirstSourceImage );

		refresh();
	}
}

void SVToolAdjustmentDialogTwoImagePageClass::OnSelChangeCombo2()
{
	UpdateData( TRUE ); // get data from dialog

	int index = ( int ) secondAvailableSourceImageListBoxCtl.GetItemData( secondAvailableSourceImageListBoxCtl.GetCurSel() );

	if( index != LB_ERR && index >= 0 && index < imageList.GetSize() )
	{
		// Get new input...
		pSecondSourceImage = imageList.GetAt( index );

		pParentDialog->GetTool()->ConnectToImage( pSecondImageInputInfo, pSecondSourceImage );

		pSecondSourceImage = dynamic_cast<SVImageClass*>( pSecondImageInputInfo->GetInputObjectInfo().PObject );

		secondImageCtrl.UpdateImageInfo( pSecondSourceImage );

		refresh();
	}
}

void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeOperatorCombo() 
{
	refresh();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogTwoImagePage.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 14:37:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:22:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:40:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   30 Jul 2012 13:08:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   27 Jul 2012 09:05:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   02 Jul 2012 17:50:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   19 Jun 2012 14:23:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   07 Apr 2011 16:41:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Mar 2011 13:59:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 Feb 2011 12:25:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Dec 2009 13:20:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   25 Jul 2007 07:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Jan 2006 17:34:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   06 Sep 2005 14:15:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Used new method in SVTaskObjectClass to handle the image connections properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Aug 2005 08:45:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jun 2005 08:33:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Feb 2005 14:05:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added check for SV_HIDDEN image property
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Feb 2005 07:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Reset methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Feb 2005 15:21:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   06 Jan 2004 14:01:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  391
 * SCR Title:  Add horizontal and vertical flip for Image Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed two small bugs in the flip code for the Image Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   29 Oct 2003 14:09:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  391
 * SCR Title:  Add horizontal and vertical flip for Image Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new operations for filp horizontal and vertical.  If the opertion does not need the second image pane, it will be disabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 16:46:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Jan 2003 09:02:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnInitDialog method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Nov 2002 14:00:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   theSVObjectManager
 * ValueObject Data Indexes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jan 2002 11:21:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  280
 * SCR Title:  Update image tool setup dialog to include all image sources in the tool set
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnInitDialog method to remove previous trigger image filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2001 15:39:22   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  240
 * SCR Title:  Color SVIM:  Color images available as sources in image tool.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified function, 
 * SVToolAdjustmentDialogTwoImagePageClass::OnInitDialog.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2001 15:09:48   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified functions, 
 * SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeCombo1,  
 * SVToolAdjustmentDialogTwoImagePageClass::OnSelChangeCombo2.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2000 16:00:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  145
 * SCR Title:  Half field images and rotation
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Moved SVToolAdjustDialogTwoImagePageClass from 
 * SVToolAdjustmentDialog.cpp and SVToolAdjustmentDialog.h
 * to it's own header and implementation file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
