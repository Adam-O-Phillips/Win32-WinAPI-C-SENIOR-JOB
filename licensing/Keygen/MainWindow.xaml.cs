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
using LicenseLib;

namespace Keygen
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static string privKey = "MHcwIwYKKoZIhvcNAQwBAzAVBBCB69O2J6DJSmzBrqjfahPmAgEKBFDZVmOS5l+KQJ/0sC594cvf3AKkB98mBsVnrB8keYQ+Abx16IEklY1pMNiV5AWfJuEFhvZekcqGkAKReuldFmAsfVXeIRZrfQEEURiX6xllWw==";
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            KLicense klic = new KLicense();
            //klic.create_signature_key();
            var license_code = klic.generate_licese_code(dev_info_text.Text, privKey);
            txt_active_code.Text = license_code;
        }
    }
}
