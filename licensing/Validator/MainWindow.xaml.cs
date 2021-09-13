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

namespace Validator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static string pubKey = "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEmHdk/3FDWYzsbWIb+Bk7JS7WIEd2GRCklF7O/VE9fU0MBi4ZXapzcbJt2+HS5uA9kG/QFNeesyfyp4MKwtj9VA==";
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var lic = new KLicense();

            txtResult.Text = lic.validate_license(txtActivCode.Text, pubKey);
        }
    }
}
