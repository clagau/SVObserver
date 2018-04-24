//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVRemoteTestHelper.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.10  $
//* .Check In Date   : $Date:   28 Jan 2015 13:32:40  $
//******************************************************************************

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using System.Xml;
using SeidenaderVision;

namespace SVRemoteTest
{
	public class SVRemoteTestHelper
	{
		#region Static Readonly strings
		public static readonly string scInputCategory = "Input Parameters";
		private static readonly string scInputParameter = "Input Parameter";
		private static readonly string scOutputsCategory = "Outputs";
		private static readonly string scOutput = "Output";
		private static readonly string scResultsCategory = "Results";
		private static readonly string scReturnName = "Return";
		private static readonly string scReturnValue = "Return Value";
		private static readonly string scFilenameEditor = "SeidenaderVision.Gui.SVFileNameEditor";
		private static readonly string scStringCollectionEditor = @"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a";
		private static readonly string scProperties = "Properties";
        #endregion

		#region Method Lookup Tables
		private Dictionary<string, ISVComObjectView> m_remoteMethodLookup = new Dictionary<string, ISVComObjectView>();
		// List of Lists
		private Dictionary<string, Dictionary<string, ISVComObjectView>> m_methodLookup = new Dictionary<string, Dictionary<string, ISVComObjectView>>();
		#endregion

		#region Member Variables
        private string m_methodName = string.Empty;
		private SVDynamicPropertyCollection m_dynaProperties = null;
		private SVDynamicPropertyCollection m_dynaComProperties = null;
		private ISVComObjectView m_propertiesView = null;
        public SVUserEnteredValues m_userEnteredValues = new SVUserEnteredValues();
        private static String m_MonitorListName; // so multiple forms share the same field
		#endregion

		#region Properties
		
        public static String MonitorListName
        {
            get { return m_MonitorListName; }
            set { m_MonitorListName = value; }
        }

		public SVDynamicPropertyCollection DynamicProperties
		{
			get { return m_dynaProperties; }
		}

		public ISVComObjectView ComPropertiesView
		{
			get { return m_propertiesView; }
		}
		
		public SVDynamicPropertyCollection ComProperties
		{
			get { return m_dynaComProperties; }
		}

		private string m_errorText = "";
		public String ErrorText
		{
			get { return m_errorText; }
		}

		private Exception m_exception = null;
		public Exception Exception
		{
			get { return m_exception; }
		}
		public bool HasException
		{
			get { return m_exception != null; }
		}
		#endregion

		#region Constructor
		public SVRemoteTestHelper()
		{
			InitMethodLookupTables();
		}
		#endregion

		#region Init Method Lookup Table
		private void InitMethodLookupTables()
		{
			m_remoteMethodLookup.Add("GetItems", new SVRemoteCtrlGetItemsView());
			m_remoteMethodLookup.Add("SetItems", new SVRemoteCtrlSetItemsView());
			m_remoteMethodLookup.Add("BeginSetItems", new SVRemoteCtrlSetItemsView());
            m_remoteMethodLookup.Add("GetDeviceConfigReport", new SVRemoteCtrlDeviceConfigReportView());
            m_remoteMethodLookup.Add("GetDataDefinitionList", new SVRemoteCtrlGetDataDefinitionView());
            m_remoteMethodLookup.Add("QueryProductList", new SVRemoteCtrlStringInputVariantStringArrayOutputView());
            m_remoteMethodLookup.Add("QueryRejectCondList", new SVRemoteCtrlStringInputVariantStringArrayOutputView());
            m_remoteMethodLookup.Add("QueryFailStatusList", new SVRemoteCtrlStringInputVariantStringArrayOutputView());
            m_remoteMethodLookup.Add("GetProduct", new SVRemoteCtrlGetProductView());
            m_remoteMethodLookup.Add("GetReject", new SVRemoteCtrlGetProductView());
            m_remoteMethodLookup.Add("QueryMonitorListNames", new SVRemoteCtrlNoInputVariantStringArrayOutputView());
            m_remoteMethodLookup.Add("GetInspectionNames", new SVRemoteCtrlNoInputVariantStringArrayOutputView());
            m_remoteMethodLookup.Add("RegisterMonitorList", new SVRemoteCtrlRegisterMonitorListOutputView());
            m_remoteMethodLookup.Add("GetFailStatus", new SVRemoteCtrlGetFailStatusView());
            m_remoteMethodLookup.Add("ActivateMonitorList", new SVRemoteCtrlActivateMonitorList()); //So all forms that have a MonitorList name property share the same value
            m_remoteMethodLookup.Add("DeactivateMonitorList", new SVRemoteCtrlDeactivateMonitorList()); //So all forms that have a MonitorList name property share the same value

			m_methodLookup.Add("AxSVRemoteControlLib.AxSVRemoteCtrl", m_remoteMethodLookup);
		}
		#endregion

		#region GetComProperties
		public SVDynamicPropertyCollection GetComProperties(Type type, object obj)
		{
			m_dynaComProperties = new SVDynamicPropertyCollection();
			PropertyInfo[] properties = type.GetProperties(BindingFlags.Instance | BindingFlags.DeclaredOnly | BindingFlags.Public);
			foreach (PropertyInfo propInfo in properties)
			{
				if (propInfo.PropertyType.FullName.Contains("SVRemoteControlLib") == false)
				{
                    if (propInfo.Name != "HWND")
                    {
                        SVDynamicProperty property = new SVDynamicProperty();
                        property.Category = scProperties;
                        property.Name = propInfo.Name;
                        property.TypeName = propInfo.PropertyType.FullName;
                        try
                        {
                            object v = propInfo.GetValue(obj, null);
                            if (v != null)
                            {
                                property.Value = propInfo.GetValue(obj, null);
                                property.DefaultValue = property.Value;
                                property.ConverterTypeName = string.Empty;
                                property.EditorTypeName = string.Empty;
                                property.Description = "property " + property.Name;
                                if (propInfo.CanWrite == false)
                                {
                                    Attribute[] attributes = new Attribute[1];
                                    attributes[0] = new ReadOnlyAttribute(true);

                                    property.Attributes = attributes;
                                }
                                m_dynaComProperties.Properties.Add(property);
                            }
                        }
                        catch (Exception)
                        {
                        }
                    }
				}
			}
			return m_dynaComProperties;
		}
		#endregion

        #region LoadUserEnteredValues
        public void LoadUserEnteredValues()
        {
            string ExeDir = AppDomain.CurrentDomain.BaseDirectory;
            string filename = ExeDir + SVUserEnteredValuesTags.UserDataFilename; 
            XmlReaderSettings settings = new XmlReaderSettings();
            settings.IgnoreComments = true;
            settings.ValidationType = ValidationType.None;
            // check if file exits
            if (File.Exists(filename))
            {
                XmlReader reader = XmlReader.Create(filename, settings);
                try
                {
                    while (reader.Read())
                    {
                        switch (reader.NodeType)
                        {
                            case XmlNodeType.Element:
                                if (reader.Name == SVUserEnteredValuesTags.UserEnteredValues)
                                {
                                    if (!reader.IsEmptyElement)
                                    {
                                        m_userEnteredValues.LoadInputProperties(reader, SVUserEnteredValuesTags.UserEnteredValues);
                                    }
                                }
                                break;
                        }
                    }
                    // Update the remote tab Views
                    foreach (String methodName in m_remoteMethodLookup.Keys)
                    {
                        SVDynamicPropertyCollection properties = m_userEnteredValues.GetRemoteInputProperties(methodName);
                        if (properties!= null)
                        {
                            UpdateViewProperties(m_remoteMethodLookup[methodName], properties);
                        }
                    }
                }
                catch (System.Xml.XmlException e)
                {
                    System.Diagnostics.Trace.WriteLine(e.Message);
                }
                finally
                {
                    reader.Close();
                }
            }
        }
        #endregion

        #region SaveUserEnteredValues
        public void SaveUserEnteredValues()
        {
            string ExeDir = AppDomain.CurrentDomain.BaseDirectory;
            string filename = ExeDir + SVUserEnteredValuesTags.UserDataFilename;
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Encoding = Encoding.UTF8;
            settings.Indent = true;

            using (XmlWriter writer = XmlWriter.Create(filename, settings))
            {
                if (writer != null)
                {
                    writer.WriteStartDocument(true);
                    writer.WriteStartElement(SVUserEnteredValuesTags.UserEnteredValues);
                    try
                    {
                        // Convert the view properties to the SVDynamicPropertyCollection
                        foreach (string key in m_remoteMethodLookup.Keys)
                        {
                            try
                            {
                                m_userEnteredValues.SaveRemoteViewProperties(key, m_remoteMethodLookup[key]);
                            }
                            catch (System.Exception e)
                            {
                                System.Diagnostics.Trace.WriteLine(e.Message);
                            }
                        }
                        // Save the Dynamic Properties
                        m_userEnteredValues.SaveInputProperties(writer);
                    }
                    catch (System.Exception e)
                    {
                        System.Diagnostics.Trace.WriteLine(e.Message);
                    }
                    finally
                    {
                        writer.WriteEndElement();
                        writer.WriteEndDocument();
                        writer.Close();
                    }
                }
            }
        }
        #endregion

        #region RememberInputValues
        public void RememberInputValues(Type objectType, String methodName)
        {
            if (m_dynaProperties != null && m_dynaProperties.Properties.Count > 0)
            {
                // Remember the user entered values
                SVDynamicPropertyCollection userValues = new SVDynamicPropertyCollection();

                // Get Data from Property Grid
                foreach (SVDynamicProperty prop in m_dynaProperties.Properties)
                {
                    if (prop.Category == scInputCategory)
                    {
                        // remember it for later
                        userValues.Properties.Add(prop);
                    }
                }
                // remember the user entered values for later
                if (userValues.Properties.Count > 0)
                {
                    m_userEnteredValues.SetInputProperties(objectType, methodName, userValues);
                }
            }
        }
        #endregion

        #region UpdateInputValues
        private void UpdateInputValues(SVDynamicPropertyCollection items, SVDynamicPropertyCollection userEnteredValues)
        {
            foreach (SVDynamicProperty prop in userEnteredValues.Properties)
            {
                if (prop.Category == scInputCategory)
				{
                    // Update it in items
                    int index = items.Properties.FindIndex(delegate(SVDynamicProperty p) { return p.Name == prop.Name; });
                    if (index >= 0)
                    {
                        if (prop.Value != null)
                        {
                            items.Properties[index].Value = prop.Value;
                        }
                    }
                }
            }
        }
        #endregion

        #region UpdateViewProperties
        private void UpdateViewProperties(ISVComObjectView view, SVDynamicPropertyCollection userEnteredValues)
        {
            if (userEnteredValues != null && userEnteredValues.Properties.Count > 0)
            {
                PropertyInfo[] properties = view.GetType().GetProperties();
                foreach (PropertyInfo propInfo in properties)
                {
                    CategoryAttribute[] attributes = (CategoryAttribute[])propInfo.GetCustomAttributes(typeof(CategoryAttribute), false);
                    if (attributes != null && attributes.Length > 0)
                    {
                        if (scInputCategory == attributes[0].Category)
                        {
                            // Find property by name
                            int index = userEnteredValues.Properties.FindIndex(delegate(SVDynamicProperty p) { return p.Name == propInfo.Name; });
                            if (index >= 0)
                            {
                                SVDynamicProperty dynamicProp = userEnteredValues.Properties[index];
                                try
                                {
                                    if (dynamicProp.Value != null)
                                    {
                                        propInfo.SetValue(view, dynamicProp.Value, null);
                                    }
                                }
                                catch (System.Exception e)
                                {
                                    System.Diagnostics.Trace.WriteLine(e.Message);
                                }
                            }
                        }
                    }
                }
            }
        }
        #endregion

        #region ClearSelectedMethod()
        public void ClearSelectedMethod()
        {
            m_errorText = string.Empty;
            m_dynaProperties = null;
            m_propertiesView = null;
            m_methodName = String.Empty;
        }
        #endregion

        #region SetSelectedMethod
        public void SetSelectedMethod(Type objectType, MethodInfo method)
		{
            if (m_dynaProperties != null)
            {
                RememberInputValues(objectType, m_methodName);
            }
			m_exception = null;
			m_errorText = string.Empty;
			m_dynaProperties = null;
			m_propertiesView = null;
            m_methodName = method.Name;

			ParameterInfo returnType = method.ReturnParameter;
			ParameterInfo[] parameters = method.GetParameters();

			bool bFound = false;
			if (m_methodLookup.ContainsKey(objectType.FullName))
			{
				Dictionary<string, ISVComObjectView> methodLookup = m_methodLookup[objectType.FullName];
				// check method name
				if (methodLookup.ContainsKey(method.Name))
				{
					m_propertiesView = methodLookup[method.Name];
					bFound = true;
				}
			}
			if (!bFound)
			{
                m_dynaProperties = new SVDynamicPropertyCollection();

                foreach (ParameterInfo parameter in parameters)
                {
                    if (parameter.IsOut == true)
                    {
                        SetOutputProperty(objectType, parameter);
                    }
                    else
                    {
                        SetInputProperty(objectType, parameter);
                    }
                }
                if (method.ReturnType != typeof(void))
                {
                    SetReturnProperty(objectType, returnType);
                }

                // find the user entered values
                SVDynamicPropertyCollection uservalues = m_userEnteredValues.GetInputProperties(objectType, method.Name);
                if (uservalues != null)
                {
                    // Update the Input values
                    UpdateInputValues(m_dynaProperties, uservalues);
                }
			}
		}
		#endregion

		#region Set Return Property
		private void SetReturnProperty(Type type, ParameterInfo returnType)
		{
			SVDynamicProperty property = new SVDynamicProperty();
			// populate property with return var - mark readonly
			property.Category = scResultsCategory;
			property.Name = scReturnName;
			property.TypeName = returnType.ParameterType.FullName;
			property.Description = scReturnValue + " [" + property.TypeName + "]";

			if (SetReturnType(type, property))
			{
				m_dynaProperties.Properties.Add(property);
			}
		}
		#endregion

		#region Set Ouput Property
		private void SetOutputProperty(Type type, ParameterInfo parameter)
		{
			SVDynamicProperty property = new SVDynamicProperty();
			// populate property with output vars - mark readonly
			property.Category = scOutputsCategory;
			property.Name = parameter.Name;
			property.TypeName = parameter.ParameterType.FullName;
			property.Description = scOutput + " [" + property.TypeName + "]";
            SetPropertyValue(type, property, parameter.ParameterType);

			m_dynaProperties.Properties.Add(property);
		}
		#endregion

		#region Set Input Property
		private void SetInputProperty(Type type, ParameterInfo parameter)
		{
			SVDynamicProperty property = new SVDynamicProperty();
			// populate property grid with input vars
			property.Category = scInputCategory;
			property.Name = parameter.Name;
			property.TypeName = parameter.ParameterType.FullName;
			property.Description = scInputParameter + " [" + property.TypeName + "]";
            bool bRetVal = SetPropertyValue(type, property, parameter.ParameterType);
			if (bRetVal)
			{
				m_dynaProperties.Properties.Add(property);
			}
		}
		#endregion

		#region Set Property Value
		private bool SetPropertyValue(Type type, SVDynamicProperty property, Type parameterType)
		{
			bool bRetVal = true;
            if (parameterType.IsEnum)
            {
                System.Array Values = Enum.GetValues(parameterType);
                property.Value = Values.GetValue( 0 );
                property.DefaultValue = property.Value;
                property.TypeName = parameterType.AssemblyQualifiedName.ToString();
                property.EditorTypeName = string.Empty;
                property.ConverterTypeName = typeof(SVComEnumConverter).FullName;
            }
            else
            {
                if (property.TypeName == typeof(String).FullName || property.TypeName.Contains(typeof(String).FullName))
                {
                    property.Value = "";
                    property.DefaultValue = property.Value;
                    if (property.Name.StartsWith("file") || property.Name.EndsWith("Path"))
                    {
                        property.EditorTypeName = scFilenameEditor;
                        property.ConverterTypeName = scFilenameEditor;
                    }
                    else
                    {
                        property.EditorTypeName = string.Empty;
                        property.ConverterTypeName = string.Empty;
                    }
                }
                else if (property.TypeName == typeof(Int32).FullName || property.TypeName.Contains(typeof(Int32).FullName))
                {
                    property.Value = 0;
                    property.DefaultValue = property.Value;
                    property.EditorTypeName = string.Empty;
                    property.ConverterTypeName = string.Empty;
                }
                else if (property.TypeName == typeof(Boolean).FullName || property.TypeName.Contains(typeof(Boolean).FullName))
                {
                    property.Value = false;
                    property.DefaultValue = property.Value;
                    property.EditorTypeName = string.Empty;
                    property.ConverterTypeName = string.Empty;
                }
                else // Unknown type
                {
                    bRetVal = false;
                }
            }
		
			return bRetVal;
		}
		#endregion

		#region Set Return Type
		private bool SetReturnType(Type type, SVDynamicProperty property)
		{
			bool bRetVal = true;

			if (property.TypeName == typeof(String).FullName)
			{
				property.Value = "";
				property.DefaultValue = property.Value;
				property.EditorTypeName = string.Empty;
				property.ConverterTypeName = string.Empty;
			}
			else if (property.TypeName == typeof(Int32).FullName)
			{
				property.Value = 0;
				property.DefaultValue = property.Value;
				property.EditorTypeName = string.Empty;
				property.ConverterTypeName = string.Empty;
			}
			else if (property.TypeName == typeof(System.Array).FullName)
			{
				property.Value = new List<String>();
				property.DefaultValue = property.Value;
				property.EditorTypeName = scStringCollectionEditor;
				property.ConverterTypeName = string.Empty;
			}
			else
			{
				bRetVal = false;
			}
			return bRetVal;
		}
		#endregion

		#region Set Return Value
		public void SetReturnValue<T>(T value)
		{
			foreach (SVDynamicProperty prop in m_dynaProperties.Properties)
			{
				if (prop.Category == scResultsCategory)
				{
					prop.Value = value;
					prop.TypeName = typeof(T).FullName;
					if (value.GetType() == typeof(List<String>))
					{
						prop.EditorTypeName = scStringCollectionEditor;
					}
				}
			}
		}
		#endregion

		#region UpdateOutputs
		public void UpdateOutputs(ParameterInfo[] paramInfos, object[] values)
		{
			// Get Outputs name and value
			Dictionary<string, object> namedValues = new Dictionary<string, object>();
			foreach (ParameterInfo info in paramInfos)
			{
				if (info.IsOut)
				{
					namedValues.Add(info.Name, values[info.Position]);
				}
			}
			if (namedValues.Count > 0)
			{
				foreach (SVDynamicProperty prop in m_dynaProperties.Properties)
				{
					if (prop.Category == scOutputsCategory)
					{
						// find by name
						prop.Value = namedValues[prop.Name];
						if (prop.Value == null)
						{
							prop.Value = prop.DefaultValue;
						}
					}
				}
			}
		}
		#endregion

		#region SetReturnValue
		private void SetReturnValue(Type objectType, String methodName, Type returnType, object value)
		{
			// show results ?
			if (returnType == typeof(Int32))
			{
				// Update Property Grid ?
				SetReturnValue<Int32>((Int32)value);
			}
			else if (returnType == typeof(String))
			{
                if (m_methodLookup.ContainsKey(objectType.FullName))
                {
                    Dictionary<string, ISVComObjectView> methodReturnLookup = m_methodLookup[objectType.FullName];
                    if (methodReturnLookup.ContainsKey(methodName))
                    {
                        ISVComObjectView view = methodReturnLookup[methodName];
                        view.SetResult(value);
                    }
                }
                else
                {
                    SetReturnValue<String>((String)value);
                }
			}
			else if (returnType == typeof(System.Array))
			{
				List<String> stringList = new List<String>();
				System.Array a = value as System.Array;
				foreach (object s in a)
				{
					if (s != null)
					{
						stringList.Add(s.ToString());
					}
				}
				SetReturnValue<List<String>>(stringList);
			}
			else 
			{
				if (m_methodLookup.ContainsKey(objectType.FullName))
				{
					Dictionary<string, ISVComObjectView> methodReturnLookup = m_methodLookup[objectType.FullName];
					if (methodReturnLookup.ContainsKey(methodName))
					{
						ISVComObjectView view = methodReturnLookup[methodName];
						view.SetResult(value);
					}
				}
			}
		}
		#endregion

        #region CheckErrorList
        bool CheckErrorList(SVRemoteControlLib.ISVErrorObjectList errorList)
        {
            return (errorList != null && errorList.Count > 0);
        }
        #endregion

        #region CheckFaults
        bool CheckFaults(object returnValue)
        {
            bool bRetVal = false;
            
            Type type = returnValue.GetType();
            if (type == typeof(SVRemoteControlLib.SVProductItemsClass))
            {
                SVRemoteControlLib.ISVProductItems product = returnValue as SVRemoteControlLib.ISVProductItems;
                if (product != null)
                {
                    bRetVal = CheckErrorList(product.Errors);
                }
            }
            else if (type == typeof(SVRemoteControlLib.SVErrorObjectListClass))
            {
                bRetVal = CheckErrorList(returnValue as SVRemoteControlLib.ISVErrorObjectList);
            }
            return bRetVal;
        }
        #endregion

        #region Call Method
        private void CallMethod(Type objectType, object obj, MethodInfo method, object[] parameters)
		{
			// Get Method to Invoke
			//MethodInfo method = objectType.GetMethod(methodName, BindingFlags.Instance | BindingFlags.Public | BindingFlags.DeclaredOnly | BindingFlags.InvokeMethod);

			try
			{
                m_errorText = string.Empty;
				// need to determine return type...
				ParameterInfo returnType = method.ReturnParameter;
				if (method.ReturnType != typeof(void))
				{
					// call the method
					object x = method.Invoke(obj, parameters);

					SetReturnValue(objectType, method.Name, method.ReturnType, x);
                    m_errorText = CheckFaults(x) ? "Completed with Errors" : "Success";
				}
				else
				{
					// call the method
					method.Invoke(obj, parameters);
                    m_errorText = "Success";
				}
				// Update Outputs
				UpdateOutputs(method.GetParameters(), parameters);

				m_exception = null;
			}
			catch (Exception e)
			{
				// Update Status with exception info
				m_exception = e;
				m_errorText = GetExceptionMessage();
			}
		}
		#endregion

		#region Exec Command
		public void ExecCommand(Type objectType, object obj, MethodInfo method)
		{
			m_exception = null;
			m_errorText = "";

			List<object> items = new List<object>();

			if (m_dynaProperties != null)
			{
                // Get Data from Property Grid
				foreach (SVDynamicProperty prop in m_dynaProperties.Properties)
				{
					if (prop.Category == scInputCategory)
					{
                		// add to array
						if (prop.TypeName.Contains("System.Collections.Generic.List"))
						{
							List<string> list = prop.Value as List<string>;
							items.Add(list.ToArray());
						}
						else if (prop.TypeName == typeof(Int32).FullName)
						{
							Int32Converter converter = new Int32Converter();
							Int32 iVal = (Int32)converter.ConvertFromString(prop.Value.ToString());
							items.Add(iVal);
						}
                        else if (prop.ConverterTypeName == typeof(SVComEnumConverter).FullName)
                        {
                            Type EnumType = Type.GetType(prop.TypeName);
                            SVComEnumConverter converter = new SVComEnumConverter(EnumType);
                            object eVal = converter.ConvertFromString(prop.Value.ToString());
                            items.Add(eVal);
                        }
                        else
                        {
							items.Add(prop.Value);
						}
					}
				}
                // Populate the Outputs category
				foreach (SVDynamicProperty prop in m_dynaProperties.Properties)
				{
					if (prop.Category == scOutputsCategory)
					{
						// add to array
						items.Add(prop.DefaultValue);
					}
				}
			}
			else
			{
                try
                {
                    m_propertiesView.GetPropertyValues(items);
                }
                catch (Exception e)
                {
                    m_errorText = e.Message;
                    return;
                }
			}
			//
			CallMethod(objectType, obj, method, items.ToArray());
		}
		#endregion

		#region TestComObjectReflection
		private void TestComObjectReflection<T>(T x)
		{
			Type type = typeof(T);
			PropertyInfo[] props = type.GetProperties();
			foreach (PropertyInfo prop in props)
			{
				System.Diagnostics.Trace.WriteLine(prop.Name);
			}
		}
		#endregion

		#region Get ExceptionMessage
		public String GetExceptionMessage()
		{
			String msg = m_exception.Message;
			if (m_exception.InnerException != null && m_exception.InnerException.Message != null)
			{
				msg += "\r\n";
				msg += m_exception.InnerException.Message;
			}
			return msg;
		}
		#endregion

        #region Predicate ByName
        static Predicate<SVDynamicProperty> ByName(string name)
        {
            return delegate(SVDynamicProperty prop)
            {
                return prop.Name.ToLower() == name.ToLower();
            };
        }
        #endregion

        #region GetListName
        public string GetListName()
        {
            string name = String.Empty;
            
            if (m_dynaProperties != null)
            {
                SVDynamicProperty prop = m_dynaProperties.Properties.Find(ByName("ListName"));
                if (prop != null)
                {
                   name = prop.Value.ToString();
                }
            }
            else
            {
                try
                {
                    List<object> items = new List<object>();
                    m_propertiesView.GetPropertyValues(items);
                    if (items.Count > 0)
                    {
                        name = items[0].ToString();
                    }
                }
                catch (Exception e)
                {
                    System.Diagnostics.Trace.WriteLine(e.Message);
                }
            }
            return name;
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteTestHelper.cs_v  $
 * 
 *    Rev 1.10   28 Jan 2015 13:32:40   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Added m_failStatusMethodLookup
 * Revised InitMethodLookupTables method
 * Revised LoadUserEnteredValues method
 * Revised SaveUserEnteredValues method
 * Added ClearSelectedMethod method
 * Revised ExecCommand method
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.9   09 Dec 2014 07:11:20   ryoho
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 23
 * SCR Title: Fix the exception that happens with SetItems.(SVO-545)
 * Checked in by: rYoho; Rob Yoho
 * Change Description: changed the error text when to Completed with errors instead of having the message say that an exception had been thrown.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.8   06 Nov 2014 17:02:54   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised Load and Save to use the directory where the executable was launched.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.7   28 Oct 2014 11:56:16   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to track, restore, load, and save user entered parameters
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.6   08 Sep 2014 11:30:00   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Removed commented out methodLookup entry for GetFailStatus
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.5   27 Aug 2014 10:45:42   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: add command "RegisterMonitorList and GetFailStatus to LookupTable
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.4   14 Jul 2014 10:35:16   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: use SVRemoteCtrlStringInputVariantStringArrayOutputView and SVRemoteCtrlNoInputVariantStringArrayOutputView instead of SVRemoteCtrlVariantStringArrayView
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.3   24 Apr 2014 14:28:38   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Revised InitMethodLookupTables to add views for GetProduct, GetReject, QueryProductList, QueryRejectCondList, and QueryFailStatusList.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   07 Nov 2013 19:25:42   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 9
 * SCR Title: Add GetDataDefinitionList Command to SVRemoteControl
 * Checked in by: bWalter; Ben Walter
 * Change Description: Added GetDataDefinitionList to lookup table.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   14 May 2013 17:37:06   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:40   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/