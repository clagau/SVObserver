//******************************************************************************
//* COPYRIGHT (c) 2014, 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : ErrorNumbers
//* .File Name       : $Workfile:   ErrorNumbers.h  $
//* .Description	    : PLEASE 
//*                    ENTER A DESCRIPTION
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Jan 2015 10:34:12  $
//******************************************************************************
#pragma once

//Marcus Eichengrün error base
static const UINT Mec_ErrorBase = 16000;
//RangeHelper
static const UINT Err_16001 = Mec_ErrorBase + 1;
static const UINT Err_16002 = Mec_ErrorBase + 2;
static const UINT Err_16003 = Mec_ErrorBase + 3;
static const UINT Err_16004 = Mec_ErrorBase + 4;
static const UINT Err_16005 = Mec_ErrorBase + 5;
static const UINT Err_16006 = Mec_ErrorBase + 6;
static const UINT Err_16007 = Mec_ErrorBase + 7;
static const UINT Err_16008 = Mec_ErrorBase + 8;
static const UINT Err_16009 = Mec_ErrorBase + 9;
static const UINT Err_16010 = Mec_ErrorBase + 10;
static const UINT Err_16011 = Mec_ErrorBase + 11;
static const UINT Err_16012 = Mec_ErrorBase + 12;
static const UINT Err_16013 = Mec_ErrorBase + 13;
static const UINT Err_16014 = Mec_ErrorBase + 14;
static const UINT Err_16015 = Mec_ErrorBase + 15;
static const UINT Err_16016 = Mec_ErrorBase + 16;
static const UINT Err_16017 = Mec_ErrorBase + 17;
static const UINT Err_16018 = Mec_ErrorBase + 18;
static const UINT Err_16019 = Mec_ErrorBase + 19;
static const UINT Err_16020 = Mec_ErrorBase + 20;
static const UINT Err_16021 = Mec_ErrorBase + 21;

//Guido Ramseier error base
static const UINT Gra_ErrorBase = 25000;
static const UINT Err_25000 = Gra_ErrorBase + 0;
static const UINT Err_25001 = Gra_ErrorBase + 1;
static const UINT Err_25002 = Gra_ErrorBase + 2;
static const UINT Err_25003 = Gra_ErrorBase + 3;
static const UINT Err_25004 = Gra_ErrorBase + 4;
static const UINT Err_25005 = Gra_ErrorBase + 5;
static const UINT Err_25006 = Gra_ErrorBase + 6;
static const UINT Err_25007 = Gra_ErrorBase + 7;
static const UINT Err_25008 = Gra_ErrorBase + 8;
static const UINT Err_25009 = Gra_ErrorBase + 9;
static const UINT Err_25010 = Gra_ErrorBase + 10;
static const UINT Err_25011 = Gra_ErrorBase + 11;
static const UINT Err_25012 = Gra_ErrorBase + 12;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ErrorNumbers.h_v  $
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