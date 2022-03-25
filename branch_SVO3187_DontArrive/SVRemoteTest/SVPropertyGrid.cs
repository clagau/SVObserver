//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPropertyGrid
//* .File Name       : $Workfile:   SVPropertyGrid.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   04 Nov 2014 16:11:24  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing.Design;

namespace SVRemoteTest
{
    #region Helper for invoking the UITypeEditor
    public class RuntimeServiceProvider : IServiceProvider, ITypeDescriptorContext
    {
        private object m_object = null;
        private PropertyDescriptor m_propertyDescriptor = null;

        public RuntimeServiceProvider(PropertyDescriptor propertyDescriptor, object obj)
        {
            m_object = obj;
            m_propertyDescriptor = propertyDescriptor;
        }

        #region IServiceProvider Members
        object IServiceProvider.GetService(Type serviceType)
        {
            if (serviceType == typeof(IWindowsFormsEditorService))
            {
                return new WindowsFormsEditorService();
            }
            return null;
        }

        class WindowsFormsEditorService : IWindowsFormsEditorService
        {
            #region IWindowsFormsEditorService Members
            public void DropDownControl(Control control)
            {
            }

            public void CloseDropDown()
            {
            }

            public System.Windows.Forms.DialogResult ShowDialog(Form dialog)
            {
                return dialog.ShowDialog();
            }
   
            #endregion
        }
        #endregion

        #region ITypeDescriptorContext Members
        public void OnComponentChanged()
        {
        }

        public IContainer Container
        {
            get { return null; }
        }

        public bool OnComponentChanging()
        {
            return true; // true to keep changes, otherwise false
        }

        public object Instance
        {
            get { return m_object; }
        }

        public PropertyDescriptor PropertyDescriptor
        {
            get { return m_propertyDescriptor; }
        }
        #endregion
    }
    #endregion

    public class SVPropertyGrid : PropertyGrid
    {
	    private bool expandOnTab = false;

        protected override void OnSelectedGridItemChanged(SelectedGridItemChangedEventArgs e)
        {
            if (base.ActiveControl != null && base.ActiveControl.Controls != null)
            {
                foreach (Control control in base.ActiveControl.Controls)
                {
                    Type type = control.GetType();
                    string typeName = type.ToString();
                    if (typeName == "System.Windows.Forms.PropertyGridInternal.PropertyGridView+GridViewEdit")
                    {
                        if (control is TextBox)
                        {
                            TextBox textBox = control as TextBox;
                            textBox.Focus();
                            if (!textBox.ReadOnly)
                            {
                                textBox.SelectAll();
                            }
                        }
                    }
                    /* Auto shows drop list on first select or tabbed to
                    else if (typeName == "System.Windows.Forms.PropertyGridInternal.DropDownButton")
                    {
                        Button button = control as Button;
                        if (button.Visible)
                        {
                            button.PerformClick();
                        }
                    }
                    */
                }
            }
            base.OnSelectedGridItemChanged(e);
        }

		// Special logic for Enter and Tab/Shift+Tab keys
		protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
		{
            GridItem selectedItem = SelectedGridItem;
            if (selectedItem != null)
            {
                if (Keys.Enter == keyData)
                {
                    if (!selectedItem.Expandable && selectedItem.GridItemType == GridItemType.Property)
                    {
                        // invoke the collection editor...
                        UITypeEditor editor = (UITypeEditor)selectedItem.PropertyDescriptor.GetEditor(typeof(UITypeEditor));
                        if (editor != null)
                        {
                            //base.ActiveControl.Container;
                            RuntimeServiceProvider serviceProvider = new RuntimeServiceProvider(selectedItem.PropertyDescriptor, selectedItem.Value);
                            editor.EditValue(serviceProvider, serviceProvider, selectedItem.Value);
                        }
                    }
                }
  	    		else if (Keys.Tab == keyData || (Keys.Tab | Keys.Shift) == keyData)
	    		{
				    GridItem root = selectedItem;
				    while (root.Parent != null) 
                    {
					    root = root.Parent;
				    }
				    // Find all expanded items and put them in a list.
				    List<GridItem> items = new List<GridItem>();
				    AddExpandedItems(root, items);

			        // Find selectedItem.
			        int foundIndex = items.IndexOf(selectedItem);
			        if (Keys.Shift == (keyData & Keys.Shift)) 
                    {
                        if (expandOnTab && (SelectedGridItem.GridItems.Count > 0) && SelectedGridItem.Expanded)
                        {
                            SelectedGridItem.Expanded = false;
                        }
                        else
                        {
                            foundIndex--;
                            if (foundIndex < 0)
                            {
                                // Don't loop, go to previous control
                                return base.ProcessCmdKey(ref msg, keyData);
                            }
                            SelectedGridItem = (GridItem)items[foundIndex];
                        }
			        } 
                    else 
                    {
                        if (expandOnTab && (SelectedGridItem.GridItems.Count > 0) && !SelectedGridItem.Expanded)
                        {
                            SelectedGridItem.Expanded = true;
                        }
                        else
                        {
                            foundIndex++;
                            if (foundIndex >= items.Count)
                            {
                                // Don't loop, go to previous control
                                return base.ProcessCmdKey(ref msg, keyData);
                            }
                            SelectedGridItem = (GridItem)items[foundIndex];
                        }
			        }
				    return true;
			    }
            }
  			return base.ProcessCmdKey(ref msg, keyData);
		}

		private void AddExpandedItems(GridItem parent, IList items) 
        {
			if (parent.PropertyDescriptor != null) 
            {
				items.Add(parent);
			}
			if (parent.Expanded) 
            {
				foreach (GridItem child in parent.GridItems) 
                {
					AddExpandedItems(child, items);
				}
			}
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVPropertyGrid.cs_v  $
 * 
 *    Rev 1.0   04 Nov 2014 16:11:24   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Initial cehckin
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/