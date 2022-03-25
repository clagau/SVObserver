//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImage
//* .File Name       : $Workfile:   SVImage.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Nov 2014 16:07:32  $
//******************************************************************************
using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Design;

namespace SVRemoteTest
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class SVImage
    {
        #region Member Variables
        private String m_name = "";
        private Int32 m_Status;
        private Int32 m_TriggerCount;
        private Image m_Image;
        #endregion Member Variables

        #region Properties
        [Category("Input Parameters")]
        public String Name
        {
            get { return m_name; }
            set { m_name = value; }
        }

        public Int32 Status
        {
            get { return m_Status; }
            set { m_Status = value; }
        }

        public Int32 TriggerCount
        {
            get { return m_TriggerCount; }
            set { m_TriggerCount = value; }
        }

        [EditorAttribute(typeof(ImageDisplayUIEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public virtual Image Picture
        {
            get { return m_Image; }
            set { m_Image = value; }
        }
    }

    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class SVInputImage : SVImage
    {
        public override Image Picture
        {
            get { return base.Picture; }
            set { base.Picture = value; }
        }
    }
    #endregion Properties
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVImage.cs_v  $
 * 
 *    Rev 1.1   04 Nov 2014 16:07:32   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Added Category attribute tag of Input Parameters to aid in persistence.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   14 May 2013 17:29:00   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Check-In
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

