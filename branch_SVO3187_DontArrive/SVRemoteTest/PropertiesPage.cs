//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PropertiesPage
//* .File Name       : $Workfile:   PropertiesPage.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Jul 2014 10:29:18  $
//******************************************************************************
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using SeidenaderVision;
using System.Reflection;

namespace SVRemoteTest
{
    public partial class PropertiesPage : Form
    {
        public PropertiesPage(SVRemoteTestHelper test)
        {
            m_test = test;
            InitializeComponent();
        }

        #region Refresh PropertyGrid
        public void RefreshPropertyGrid()
        {
            SVDynamicPropertyCollection properties = m_test.DynamicProperties;
            if (properties != null)
            {
                // set the property list
                ArrayList objs = new ArrayList();
                objs.Add(properties);
                inputPropertyGrid.SelectedObjects = null;
                inputPropertyGrid.SelectedObjects = objs.ToArray();
            }
            else
            {
                inputPropertyGrid.SelectedObjects = null;
                inputPropertyGrid.SelectedObject = m_test.ComPropertiesView;
            }
        }
        #endregion

        #region Clear PropertyGrid
        public void ClearPropertyGrid()
        {
            inputPropertyGrid.SelectedObjects = null;
            inputPropertyGrid.SelectedObject = null;
        }
        #endregion

        #region Show Com Object Properties
        public void ShowComProperties(Type type, object obj)
        {
            SVDynamicPropertyCollection properties = m_test.GetComProperties(type, obj);
            if (properties != null)
            {
                comObjectPropertyGrid.SelectedObject = null;

                ArrayList objs = new ArrayList();
                objs.Add(properties);
                comObjectPropertyGrid.SelectedObjects = objs.ToArray();
            }
        }
        #endregion

        #region Member Variables
        private SVRemoteTestHelper m_test = null;
        #endregion
    }
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\PropertiesPage.cs_v  $
 * 
 *    Rev 1.0   14 Jul 2014 10:29:18   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: initilized check in
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
