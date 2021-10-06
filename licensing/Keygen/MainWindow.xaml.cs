using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Management;
using System.IO;
using System.Collections;
using LicenseLib;
using Microsoft.Win32;
using System.Security.Cryptography;
using System.Net.NetworkInformation;

namespace Keygen
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static string privKey = "MHcwIwYKKoZIhvcNAQwBAzAVBBCB69O2J6DJSmzBrqjfahPmAgEKBFDZVmOS5l+KQJ/0sC594cvf3AKkB98mBsVnrB8keYQ+Abx16IEklY1pMNiV5AWfJuEFhvZekcqGkAKReuldFmAsfVXeIRZrfQEEURiX6xllWw==";
        ArrayList hardDriveDetails = new ArrayList();
        public MainWindow()
        {
            InitializeComponent();

            // Getting Hard disk serial number...
            ManagementObjectSearcher moSearcher = new ManagementObjectSearcher("SELECT * FROM Win32_DiskDrive");

            foreach (ManagementObject wmi_HD in moSearcher.Get())
            {
                HardDrive hd = new HardDrive();  // User Defined Class
                hd.Model = wmi_HD["Model"].ToString();  //Model Number
                hd.Type = wmi_HD["InterfaceType"].ToString();  //Interface Type
                hd.SerialNo = wmi_HD["SerialNumber"].ToString(); //Serial Number

                hardDriveDetails.Add(hd);
            }

            // Getting Windows Product Key...
            byte[] id = null;
            RegistryKey localKey;
            if (Environment.Is64BitOperatingSystem)
                localKey = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry64);
            else
                localKey = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32);

            var regKey = localKey.OpenSubKey(@"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", false);

            String strProdId = null, strWinProdKey = null;
            if (regKey != null)
            {
                id = regKey.GetValue("DigitalProductId") as byte[];
                
                strProdId = regKey.GetValue("ProductId") as String;
            }

            if (id != null)
            {
                WinProdKey pKey = new WinProdKey();
                strWinProdKey = pKey.DecodeKeyByteArray(id);
            }

            // Get CPU id..

            ManagementClass managClass = new ManagementClass("win32_processor");
            ManagementObjectCollection managCollec = managClass.GetInstances();

            string cpuInfo = null;
            foreach (ManagementObject managObj in managCollec)
            {
                cpuInfo = managObj.Properties["processorID"].Value.ToString();
                break;
            }

            // Get Computer name...

            HardDrive hd0 = (HardDrive)hardDriveDetails[0];
            dev_info_text.Text = getHashSha256("[" + hd0.SerialNo + "], [" + ((strProdId != null)? strProdId: "") + "], [" + 
                ((strWinProdKey != null)? strWinProdKey.ToString(): "") + "], [" + 
                ((cpuInfo != null) ? cpuInfo.ToString() : "") + "], [" + 
                Environment.MachineName + "], [" + getMACAddress() + "]");
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            KLicense klic = new KLicense();
            //klic.create_signature_key();
            var license_code = klic.generate_licese_code(dev_info_text.Text, privKey);
            txt_active_code.Text = license_code;
        }

        public static string getHashSha256(string text)
        {
            byte[] bytes = Encoding.Unicode.GetBytes(text);
            SHA256Managed hashstring = new SHA256Managed();
            byte[] hash = hashstring.ComputeHash(bytes);
            string hashString = string.Empty;
            foreach (byte x in hash)
            {
                hashString += String.Format("{0:x2}", x);
            }
            return hashString;
        }

        public static string getMACAddress()
        {
            var macAddr =
            (
                from nic in NetworkInterface.GetAllNetworkInterfaces()
                where nic.OperationalStatus == OperationalStatus.Up
                select nic.GetPhysicalAddress().ToString()
            ).FirstOrDefault();

            return macAddr;
        }
    }
}
