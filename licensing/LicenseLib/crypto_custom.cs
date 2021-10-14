using Elskom.Generic.Libs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Metadata.W3cXsd2001;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace LicenseLib
{
    public class crypto_custom
    {
        public static byte[] CalcCRC16(byte[] data)
        {
            ushort crc = 0x0000;
            for (int i = 0; i < data.Length; i++)
            {
                crc ^= (ushort)(data[i] << 8);
                for (int j = 0; j < 8; j++)
                {
                    if ((crc & 0x8000) > 0)
                        crc = (ushort)((crc << 1) ^ 0x8005);
                    else
                        crc <<= 1;
                }
            }
            byte[] btRet = new byte[2];
            btRet[0] = (byte)(crc >> 8);
            btRet[1] = (byte)(crc);
            return btRet;
        }
        public static byte[] GetBytesFromHexString(string strInput)
        {
            byte[] bytArOutput = new byte[] { };
            if (!string.IsNullOrEmpty(strInput) && (strInput.Length % 2) == 0)
            {
                SoapHexBinary hexBinary = null;
                try
                {
                    hexBinary = SoapHexBinary.Parse(strInput);
                    if (hexBinary != null)
                    {
                        bytArOutput = hexBinary.Value;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
            return bytArOutput;
        }

        public static byte[] GetByteFromString(string strInput)
        {
            byte[] byteArryRet = new byte[strInput.Length];

            int i;
            for (i = 0; i < strInput.Length; i++)
                byteArryRet[i] = (byte)char.GetNumericValue(strInput[i]);

            return byteArryRet;
        }

        public static byte[] SHA256Hash(byte[] btVal)
        {
            byte[] btArrRet = null;
            using (SHA256 mySHA256 = SHA256.Create())
            {
                try
                {
                    btArrRet = mySHA256.ComputeHash(btVal);
                }
                catch (UnauthorizedAccessException e)
                {
                    Console.WriteLine($"Access Exception: {e.Message}");
                }
                catch (Exception e)
                {
                    Console.WriteLine($"Access Exception: {e.Message}");
                }
            }
            return btArrRet;
        }

        public static void CreateRSA2048Keys()
        {
            var cryptoServiceProvider = new RSACryptoServiceProvider(2048);
            var privateKey = cryptoServiceProvider.ExportParameters(true);
            var publicKey = cryptoServiceProvider.ExportParameters(false);

            string publicKeyString = GetKeyString(publicKey);
            string privateKeyString = GetKeyString(privateKey);

            System.IO.File.WriteAllText("pubkey.txt", publicKeyString);
            System.IO.File.WriteAllText("prvkey.txt", privateKeyString);

            Console.WriteLine("Public Key: " + publicKeyString);
            Console.WriteLine("Private Key: " + privateKeyString);
        }

        public static void test_rsa()
        {
            try
            {
                // Create a UnicodeEncoder to convert between byte array and string.
                ASCIIEncoding ByteConverter = new ASCIIEncoding();

                string dataString = "Data to Sign";

                // Create byte arrays to hold original, encrypted, and decrypted data.
                byte[] originalData = ByteConverter.GetBytes(dataString);
                byte[] signedData;

                // Create a new instance of the RSACryptoServiceProvider class
                // and automatically create a new key-pair.
                RSACryptoServiceProvider RSAalg = new RSACryptoServiceProvider();

                // Export the key information to an RSAParameters object.
                // You must pass true to export the private key for signing.
                // However, you do not need to export the private key
                // for verification.
                RSAParameters prvKey = RSAalg.ExportParameters(true);
                RSAParameters pubKey = RSAalg.ExportParameters(false);

                // Hash and sign the data.
                string privateKeyString;
                {
                    //get a stream from the string
                    var sr = new System.IO.StringWriter();
                    //we need a deserializer
                    var xs = new System.Xml.Serialization.XmlSerializer(typeof(RSAParameters));
                    //get the object back from the stream
                    xs.Serialize(sr, prvKey);
                    privateKeyString = sr.ToString();
                }
                signedData = RSA2048Signature(originalData, privateKeyString);

                // Verify the data and display the result to the
                // console.

                string publicKeyString;
                {
                    //get a stream from the string
                    var sr = new System.IO.StringWriter();
                    //we need a deserializer
                    var xs = new System.Xml.Serialization.XmlSerializer(typeof(RSAParameters));
                    //get the object back from the stream
                    xs.Serialize(sr, pubKey);
                    publicKeyString = sr.ToString();
                }

                if (VerifyRSA2048Signature(originalData, signedData, publicKeyString))
                {
                    Console.WriteLine("The data was verified.");
                }
                else
                {
                    Console.WriteLine("The data does not match the signature.");
                }
            }
            catch (ArgumentNullException)
            {
                Console.WriteLine("The data was not signed or verified");
            }
        }
        public static byte[] RSA2048Signature(byte[] DataToSign, string keyString)
        {
            try
            {
                RSAParameters prvKey;
                {
                    //get a stream from the string
                    var sr = new System.IO.StringReader(keyString);
                    //we need a deserializer
                    var xs = new System.Xml.Serialization.XmlSerializer(typeof(RSAParameters));
                    //get the object back from the stream
                    prvKey = (RSAParameters)xs.Deserialize(sr);
                }
                // Create a new instance of RSACryptoServiceProvider using the
                // key from RSAParameters.
                RSACryptoServiceProvider RSAalg = new RSACryptoServiceProvider();

                RSAalg.ImportParameters(prvKey);

                // Hash and sign the data. Pass a new instance of SHA256
                // to specify the hashing algorithm.
                byte[] bt = RSAalg.SignData(DataToSign, SHA256.Create());
//                 System.IO.File.WriteAllText("enc.key", keyString);
//                 System.IO.File.WriteAllBytes("enc1.bin", DataToSign);
//                 System.IO.File.WriteAllBytes("enc2.bin", bt);
                return bt;
            }
            catch (CryptographicException e)
            {
                Console.WriteLine(e.Message);

                return null;
            }
        }

        public static bool VerifyRSA2048Signature(byte[] DataToVerify, byte[] SignedData, string keyString)
        {
            try
            {
                RSAParameters pubKey;
                {
                    //get a stream from the string
                    var sr = new System.IO.StringReader(keyString);
                    //we need a deserializer
                    var xs = new System.Xml.Serialization.XmlSerializer(typeof(RSAParameters));
                    //get the object back from the stream
                    pubKey = (RSAParameters)xs.Deserialize(sr);
                }
                // Create a new instance of RSACryptoServiceProvider using the
                // key from RSAParameters.
                RSACryptoServiceProvider RSAalg = new RSACryptoServiceProvider();

                RSAalg.ImportParameters(pubKey);

                // Verify the data using the signature.  Pass a new instance of SHA256
                // to specify the hashing algorithm.
//                 System.IO.File.WriteAllText("dec.key", keyString);
//                 System.IO.File.WriteAllBytes("dec1.bin", DataToVerify);
//                 System.IO.File.WriteAllBytes("dec2.bin", SignedData);
                return RSAalg.VerifyData(DataToVerify, SHA256.Create(), SignedData);
            }
            catch (CryptographicException e)
            {
                Console.WriteLine(e.Message);

                return false;
            }
        }

        public static byte[] RSA2048_Encrypt(byte[] data, string keyString)
        {
            byte[] btRet = null;
            using (var rsa = new RSACryptoServiceProvider(2048))
            {
                try
                {
                    RSAParameters prvKey;
                    {
                        //get a stream from the string
                        var sr = new System.IO.StringReader(keyString);
                        //we need a deserializer
                        var xs = new System.Xml.Serialization.XmlSerializer(typeof(RSAParameters));
                        //get the object back from the stream
                        prvKey = (RSAParameters)xs.Deserialize(sr);
                    }
                    rsa.ImportParameters(prvKey);
                    btRet = rsa.Encrypt(data, false);
                }
                finally
                {
                    rsa.PersistKeyInCsp = false;
                }
            }
            return btRet;
        }

        public static byte[] RSA2048_Decrypt(byte[] data, string keyString)
        {
            byte[] btRet = null;
            using (var rsa = new RSACryptoServiceProvider(2048))
            {
                try
                {
                    RSAParameters pubKey;
                    {
                        //get a stream from the string
                        var sr = new System.IO.StringReader(keyString);
                        //we need a deserializer
                        var xs = new System.Xml.Serialization.XmlSerializer(typeof(RSAParameters));
                        //get the object back from the stream
                        pubKey = (RSAParameters)xs.Deserialize(sr);
                    }
                    rsa.ImportParameters(pubKey);
                    btRet = rsa.Decrypt(data, false);
                }
                finally
                {
                    rsa.PersistKeyInCsp = false;
                }
            }
            return btRet;
        }

        public static string Base64Encode(byte[] data)
        {
            return Convert.ToBase64String(data);
        }

        public static byte[] Base64Decode(string str)
        {
            return Convert.FromBase64String(str);
        }

        public static byte[] BlowfishEncrypt(byte[] bt, byte[] key)
        {
            BlowFish bf = new BlowFish(key);
            bf.IV = new byte[] { 0, 1, 2, 3, 4, 5, 6, 7 };
            return bf.EncryptCBC(bt);
        }

        public static byte[] BlowfishDecrypt(byte[] bt, byte[] key)
        {
            BlowFish bf = new BlowFish(key);
            bf.IV = new byte[] { 0, 1, 2, 3, 4, 5, 6, 7 };
            return bf.DecryptCBC(bt);
        }

        public static string GetKeyString(RSAParameters tkey)
        {
            var StringWriter = new System.IO.StringWriter();
            var xmlSerializer = new System.Xml.Serialization.XmlSerializer(typeof(RSAParameters));
            xmlSerializer.Serialize(StringWriter, tkey);
            return StringWriter.ToString();
        }

        public static byte[] merge_bytes(byte[] byte1, byte[] byte2)
        {
            byte[] byteall = new byte[byte1.Length + byte2.Length];
            Array.Copy(byte1, byteall, byte1.Length);
            Array.Copy(byte2, 0, byteall, byte1.Length, byte2.Length);

            return byteall;
        }

        public static string key_generate(string devkey, string privRSA2048Key)
        {
            //crypto_custom.CreateRSA2048Keys();

            byte[] dev_bt = crypto_custom.GetByteFromString(devkey);
            byte[] ret_bt = crypto_custom.merge_bytes(dev_bt, crypto_custom.CalcCRC16(dev_bt));

            UInt32 dwSeq = 0;

            string strseq = null;
            try
            {
                strseq = System.IO.File.ReadAllText("sequence.txt");
            }
            catch (Exception ex)
            { }

            if (!string.IsNullOrEmpty(strseq))
            {
                dwSeq = (UInt32)int.Parse(strseq);
                dwSeq ++;
            }

            try
            {
                System.IO.File.WriteAllText("sequence.txt", dwSeq.ToString());
            }
            catch (Exception ex)
            { }

            byte[] bt_seq = new byte[4];
            bt_seq[0] = (byte)(dwSeq >> 24);
            bt_seq[1] = (byte)(dwSeq >> 16);
            bt_seq[2] = (byte)(dwSeq >> 8);
            bt_seq[3] = (byte)(dwSeq >> 0);

            ret_bt = crypto_custom.merge_bytes(ret_bt, bt_seq);

            UInt64 qwIssuedAt = (ulong)DateTime.UtcNow.Subtract(new DateTime(1970, 1, 1)).TotalSeconds;
            byte[] bt_issued_at = new byte[8];
            bt_issued_at[0] = (byte)(qwIssuedAt >> (24 + 32));
            bt_issued_at[1] = (byte)(qwIssuedAt >> (16 + 32));
            bt_issued_at[2] = (byte)(qwIssuedAt >> (8 + 32));
            bt_issued_at[3] = (byte)(qwIssuedAt >> (0 + 32));
            bt_issued_at[4] = (byte)(qwIssuedAt >> (24));
            bt_issued_at[5] = (byte)(qwIssuedAt >> (16));
            bt_issued_at[6] = (byte)(qwIssuedAt >> (8));
            bt_issued_at[7] = (byte)(qwIssuedAt >> (0));

            ret_bt = crypto_custom.merge_bytes(ret_bt, bt_issued_at);

            UInt32 dwPeriod = 30 * 24 * 3600;
            byte[] bt_period = new byte[4];
            bt_period[0] = (byte)(dwPeriod >> 24);
            bt_period[1] = (byte)(dwPeriod >> 16);
            bt_period[2] = (byte)(dwPeriod >> 8);
            bt_period[3] = (byte)(dwPeriod >> 0);

            ret_bt = crypto_custom.merge_bytes(ret_bt, bt_period);

            UInt32 dwFlag = 3;
            byte[] bt_flag = new byte[4];
            bt_flag[0] = (byte)(dwFlag >> 24);
            bt_flag[1] = (byte)(dwFlag >> 16);
            bt_flag[2] = (byte)(dwFlag >> 8);
            bt_flag[3] = (byte)(dwFlag >> 0);

            ret_bt = crypto_custom.merge_bytes(ret_bt, bt_flag);
            int nrem = (ret_bt.Length % 4);
            if (nrem != 0)
            {
                byte[] btzero = new byte[4 - nrem];
                int ti;
                for (ti = 0; ti < 4 - nrem; ti++)
                    btzero[ti] = 0;

                ret_bt = crypto_custom.merge_bytes(ret_bt, btzero);
            }

            byte[] rsaSignature = crypto_custom.RSA2048Signature(ret_bt, privRSA2048Key);

            ret_bt = crypto_custom.merge_bytes(ret_bt, rsaSignature);
            byte[] blf_enc = crypto_custom.BlowfishEncrypt(ret_bt, dev_bt);
            return crypto_custom.Base64Encode(blf_enc);
        }

        public static bool check_license(string devkey, string activation_text, string pubRSA2048Key)
        {
            byte[] dev_bt = crypto_custom.GetByteFromString(devkey);

            byte[] blf_enc;
            byte[] ret_bt;
            byte[] bt_body;
            byte[] bt_rsa;
            try
            {
                blf_enc = crypto_custom.Base64Decode(activation_text);
                ret_bt = crypto_custom.BlowfishDecrypt(blf_enc, dev_bt);
                bt_body = new byte[ret_bt.Length - 256]; // without rsa2048-signature
                Array.Copy(ret_bt, bt_body, bt_body.Length);
                bt_rsa = new byte[256];
                Array.Copy(ret_bt, bt_body.Length, bt_rsa, 0, 256);
            }
            catch (Exception ex)
            {
                return false;
            }
            finally
            {
            }

            if (!crypto_custom.VerifyRSA2048Signature(bt_body, bt_rsa, pubRSA2048Key))
                return false;

            int crc16_pos = dev_bt.Length;
            int seq_pos = crc16_pos + 2;
            int issued_pos = seq_pos + 4;
            int period_pos = issued_pos + 8;
            int flag_pos = period_pos + 4;

            if (crc16_pos <= 0)
                return false;

            byte[] bt_crc = new byte[2];
            Array.Copy(bt_body, crc16_pos, bt_crc, 0, 2);

            byte[] bt_crc_body = new byte[crc16_pos];
            Array.Copy(bt_body, bt_crc_body, crc16_pos);
            byte[] rt_crc = crypto_custom.CalcCRC16(bt_crc_body);

            if (bt_crc[0] != rt_crc[0] || bt_crc[1] != rt_crc[1])
                return false;

            byte[] bt_flag = new byte[4];
            Array.Copy(bt_body, flag_pos, bt_flag, 0, 4);
            flag = bt_flag[0];
            flag = (flag << 8) | bt_flag[1];
            flag = (flag << 8) | bt_flag[2];
            flag = (flag << 8) | bt_flag[3];

            byte[] bt_period = new byte[4];
            Array.Copy(bt_body, period_pos, bt_period, 0, 4);
            period = bt_period[0];
            period = (period << 8) | bt_period[1];
            period = (period << 8) | bt_period[2];
            period = (period << 8) | bt_period[3];

            byte[] bt_issued_at = new byte[8];
            Array.Copy(bt_body, issued_pos, bt_issued_at, 0, 8);
            issuedAt = bt_issued_at[0];
            issuedAt = (issuedAt << 8) | bt_issued_at[1];
            issuedAt = (issuedAt << 8) | bt_issued_at[2];
            issuedAt = (issuedAt << 8) | bt_issued_at[3];
            issuedAt = (issuedAt << 8) | bt_issued_at[4];
            issuedAt = (issuedAt << 8) | bt_issued_at[5];
            issuedAt = (issuedAt << 8) | bt_issued_at[6];
            issuedAt = (issuedAt << 8) | bt_issued_at[7];

            byte[] bt_sequence = new byte[4];
            Array.Copy(bt_body, seq_pos, bt_sequence, 0, 4);
            sequence = bt_sequence[0];
            sequence = (sequence << 8) | bt_sequence[1];
            sequence = (sequence << 8) | bt_sequence[2];
            sequence = (sequence << 8) | bt_sequence[3];

            return true;
        }

        public static UInt32 sequence = 0;
        public static UInt64 issuedAt = 0;
        public static UInt32 period = 0;
        public static UInt32 flag = 0;
    }
}
