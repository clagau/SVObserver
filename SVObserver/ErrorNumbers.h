//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ErrorNumbers
//* .File Name       : $Workfile:   ErrorNumbers.h  $
//* .Description     : PLEASE ENTER A DESCRIPTION
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   12 Feb 2015 02:57:36  $
//******************************************************************************

#pragma once


//Marc Ziegler error base
static const int Maz_ErrorBase = 2000;
//SVGuiExtentUpdater
static const int Err_SetImageExtent_InvalidParameter_2001 = Maz_ErrorBase + 1;
static const int Err_SetImageExtentToParent_InvalidParameter_2002 = Maz_ErrorBase + 2;
static const int Err_SetImageExtentToFit_InvalidParameter_2003 = Maz_ErrorBase + 3;
//SVCommandInspectionExtentUpdater
static const int Err_SVCommandInspectionExtentUpdater_InvalidMode_2004 = Maz_ErrorBase + 4;
static const int Err_SVCommandInspectionExtentUpdater_ResetAllObjects_2005 = Maz_ErrorBase + 5;
static const int Err_SVCommandInspectionExtentUpdater_RunOnce_2006 = Maz_ErrorBase + 6;
static const int Err_SVCommandInspectionExtentUpdater_InvalidParameter_2007 = Maz_ErrorBase + 7;

//Marcus Eichengrün error base
static const int Mec_ErrorBase = 16000;
//RangeHelper
static const int Err_16001 = Mec_ErrorBase + 1;
static const int Err_16002 = Mec_ErrorBase + 2;
static const int Err_16003 = Mec_ErrorBase + 3;
static const int Err_16004 = Mec_ErrorBase + 4;
static const int Err_16005 = Mec_ErrorBase + 5;
static const int Err_16006 = Mec_ErrorBase + 6;
static const int Err_16007 = Mec_ErrorBase + 7;
static const int Err_16008 = Mec_ErrorBase + 8;
static const int Err_16009 = Mec_ErrorBase + 9;
static const int Err_16010 = Mec_ErrorBase + 10;
static const int Err_16011 = Mec_ErrorBase + 11;
static const int Err_16012 = Mec_ErrorBase + 12;
static const int Err_16013 = Mec_ErrorBase + 13;
static const int Err_16014 = Mec_ErrorBase + 14;
static const int Err_16015 = Mec_ErrorBase + 15;
static const int Err_16016 = Mec_ErrorBase + 16;
static const int Err_16017 = Mec_ErrorBase + 17;
static const int Err_16018 = Mec_ErrorBase + 18;
static const int Err_16019 = Mec_ErrorBase + 19;
static const int Err_16020 = Mec_ErrorBase + 20;
static const int Err_16021 = Mec_ErrorBase + 21;
static const int Err_16022 = Mec_ErrorBase + 22;
static const int Err_16023 = Mec_ErrorBase + 23;
static const int Err_16024 = Mec_ErrorBase + 24;
static const UINT Err_16025 = Mec_ErrorBase + 25;
static const UINT Err_16026 = Mec_ErrorBase + 26;

//Guido Ramseier error base
static const int Gra_ErrorBase = 25000;
static const int Err_25000 = Gra_ErrorBase + 0;
static const int Err_25001 = Gra_ErrorBase + 1;
static const int Err_25002 = Gra_ErrorBase + 2;
static const int Err_25003 = Gra_ErrorBase + 3;
static const int Err_25004 = Gra_ErrorBase + 4;
static const int Err_25005 = Gra_ErrorBase + 5;
static const int Err_25006 = Gra_ErrorBase + 6;
static const int Err_25007 = Gra_ErrorBase + 7;
static const int Err_25008 = Gra_ErrorBase + 8;
static const int Err_25009 = Gra_ErrorBase + 9;
static const int Err_25010 = Gra_ErrorBase + 10;
static const int Err_25011 = Gra_ErrorBase + 11;
static const int Err_25012 = Gra_ErrorBase + 12;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ErrorNumbers.h_v  $
 * 
 *    Rev 1.4   12 Feb 2015 02:57:36   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  983
 * SCR Title:  Linear Tool - Crash while continuous resizing tool SVO-392
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add error numbers from MZA
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Jan 2015 08:23:34   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Add new Errornumbers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jan 2015 16:40:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed type to int to avoid "unsigned" compiler warnings.
 *   Added new Mec error numbers to make return values unique.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2015 10:34:12   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Combined unique error numbers
 * changed from define to static const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2014 13:53:40   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check In for SCR 979 Indirect Range Objects 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */