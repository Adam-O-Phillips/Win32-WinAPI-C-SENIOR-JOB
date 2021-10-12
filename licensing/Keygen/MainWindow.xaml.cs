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

namespace Keygen
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static string privKey = "MHcwIwYKKoZIhvcNAQwBAzAVBBCB69O2J6DJSmzBrqjfahPmAgEKBFDZVmOS5l+KQJ/0sC594cvf3AKkB98mBsVnrB8keYQ+Abx16IEklY1pMNiV5AWfJuEFhvZekcqGkAKReuldFmAsfVXeIRZrfQEEURiX6xllWw==";

        static string privRSA2048Key =
            "<?xml version=\"1.0\" encoding=\"utf-16\"?>"+
            "<RSAParameters xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"+
              "<Exponent>AQAB</Exponent>"+
              "<Modulus>uVirzS7vy49V8EjBGBy1+zL9CD7+fjaH1Xn3COwtOUi1x5HJ1UJGlfVLafJUNPQAZeysq0GPgoYtv/uSzs73ypV4ndizGbDR1gi/qaq/CMEVh3YrOB0GFUD9JX/J3TA+TvH0oFGLSxXh/9Etg7TOZwVrjBj+K00wu+agJJseqdELYqJFn/el7pSdpVz3tu9s12m34rzyntVBLRvBqTE/aZ2+oOj8rBQgLQF7sM1Wn8u4aG7KgegLtzwWs4Uh1bJLVLCTH2ZSrSKixSE6jbSc/2Nn/OlshmGkO4XdLuMAzllFsmT14uajdmcNOiC3HtnpcZsyEjgZUQ9yXIzLNw9T/Q==</Modulus>"+
              "<P>yKew0qhSbWmZcR6/4PZblHctpAP/HofrY47ZMrz5vXvFDfojziFVKcz/9Uh4LkBCsgTVNAEyPInJf+XA87CAWRM4Dc3nQkFuwMz/ggsZqn+U40dM4DZmyhk6WbQHpwOlKGUfGiZdblvWjObT4c3RQ39rYAVJcvacyKbQ2dPBl1M=</P>"+
              "<Q>7HgH5QabtFLkBZEE17jnvdfovrEXBQd8OsMiDdYZfTjdeF0TwTk5d8sL8DdeLLpzKz53f3QaEZLzpxyX5FyEo6BLtTTSNHWq5/xq1dXlsnTzNSEPa+0d0vNfGq8YW+IUByqgkLs7mslnQauVncQo6sWS0WfDt9JV5m4N1shijW8=</Q>"+
              "<DP>AuJWKlD9DlGMQRH+FA+2dUXZRBhCnH+7pGyzNoV82Xpj9LV5ukbEYJtvumjWQWPvEXv8TIkP3crlOI9m3JnW3nDmR1yETHGAVAkeSxI5xjebPkESG87sX09JhWyPB5mH0DLT5Z5TjXiIdvICbaZ1sZCtTG4HBj4BNuke62Kdz70=</DP>"+
              "<DQ>XaK+BYuTVllIX4foMf/PDBaV3ATFx8EKCT7BmlcAKty8ne/FZcriHbRyWsGh8fY2qpYGvZpcxD67ap3g65VTFHPh5rQu1IUPkuMZ7ujPFtZGg/A+ag8vsxuZujhBXgjsw1W21Pi/BnDgCVoHWbyWMzjfBTqgfoa0axWDN1yL4dM=</DQ>"+
              "<InverseQ>CUL9wy0RpsynQEC5kNkhQvHgBE9bsB5FJ3S1MNDQkljwQNEFShPyo5guVXyspLlOWMdRiT+yIP87LKa/AnFMwKIqKwvDX9jpSSxjIFUAKWrRwoU7Y1r1JQAtOShJVvfsHjefejuRCYm/6Up+hsYTBYjkIp16Q1gIrBwdEmoc3TM=</InverseQ>"+
              "<D>t9D88c0u/I73CArfle5DfRGgaOEwvDOQm2MOYMb6QumdfoNTzDCimG5hkH1Ijgu/qVYEGQQ4M5anINN7UadpkHyU1tbnKP932/FpQ/eW5jO4ClRjoI8ITWPaq6oEpV0HLnw/xA+bOahGLtNhdFGIshtqAdN3n7I6NuAOyr8ctgRu5IWExBxQAu17Me7/87f3yGP9Af2BNSfib7fYzfqPaH4N4OgaJtNiKpE1lvIbuCno4PbiLRr3+5UQWoI12tQmOyqaaxX0QBm+8qj6PZa0IRBH6GfeSq4yRZMoi4yjDGZYDZwouu4fsYZaITNP4GKy2xgKX8QoNM1xzlEZCbNdcQ==</D>"+
            "</RSAParameters>";

        static string pubRSA2048Key =
            "<?xml version=\"1.0\" encoding=\"utf-16\"?>" +
            "<RSAParameters xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">" +
              "<Exponent>AQAB</Exponent>" +
              "<Modulus>uVirzS7vy49V8EjBGBy1+zL9CD7+fjaH1Xn3COwtOUi1x5HJ1UJGlfVLafJUNPQAZeysq0GPgoYtv/uSzs73ypV4ndizGbDR1gi/qaq/CMEVh3YrOB0GFUD9JX/J3TA+TvH0oFGLSxXh/9Etg7TOZwVrjBj+K00wu+agJJseqdELYqJFn/el7pSdpVz3tu9s12m34rzyntVBLRvBqTE/aZ2+oOj8rBQgLQF7sM1Wn8u4aG7KgegLtzwWs4Uh1bJLVLCTH2ZSrSKixSE6jbSc/2Nn/OlshmGkO4XdLuMAzllFsmT14uajdmcNOiC3HtnpcZsyEjgZUQ9yXIzLNw9T/Q==</Modulus>" +
            "</RSAParameters>";

        ArrayList hardDriveDetails = new ArrayList();
        DeviceInfo deviceInfo = null;
        public MainWindow()
        {
            InitializeComponent();

            deviceInfo = new DeviceInfo();
            dev_info_text.Text = deviceInfo.deviceHash;
            //crypto_custom.test_rsa();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            //KLicense klic = new KLicense();
            //klic.create_signature_key();
            
//             RSATest ht = new RSATest();
//             ht.doTest();
            txt_active_code.Text = crypto_custom.key_generate(dev_info_text.Text, privRSA2048Key);
//             var license_code = klic.generate_licese_code(dev_info_text.Text, privKey);
//             txt_active_code.Text = license_code;
        }
    }
}
