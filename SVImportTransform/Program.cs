//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Program
//* .File Name       : $Workfile:   Program.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:34:14  $
//******************************************************************************
using System;
using System.Collections.Generic;
using System.Text;

namespace SVImportTransform
{
    class Program
    {
        static void Main(string[] args)
        {
            string infile = "";
            string outfile = "";
            string inspectionName = "";

            char[] separator = { '=' };

            foreach (string arg in args)
            {
                string[] parts = arg.Split(separator);
                if (parts.Length == 2)
                {
                    if (parts[0] == "/in")
                    {
                        infile = parts[1];
                    }
                    else if (parts[0] == "/out")
                    {
                        outfile = parts[1];
                    }
                    else if (parts[0] == "/name")
                    {
                        inspectionName = parts[1];
                    }
                }
            }
            if (infile.Length > 0 && outfile.Length > 0 && inspectionName.Length > 0)
            {
                SVInspectionTransform.Transform(infile, outfile, inspectionName);
            }
        }
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImportTransform\Program.cs_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:34:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 16:59:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/