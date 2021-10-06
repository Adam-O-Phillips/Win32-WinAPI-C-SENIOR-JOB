using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Keygen
{
    class WinProdKey
    {
        //first byte offset
        const int start = 52;
        //last byte offset
        const int end = start + 15;
        //decoded key length
        const int length = 29;
        //decoded key in byte form
        const int decoded = 15;

        //char[] for holding the decoded product key
        char[] decodedKey = new char[length];

        //List<byte> to hold the key bytes
        List<byte> keyHex = new List<byte>();

        //list to hold possible alpha-numeric characters
        //that may be in the product key
        List<char> charsInKey = new List<char>()
                    {
                        'B', 'C', 'D', 'F', 'G', 'H',
                        'J', 'K', 'M', 'P', 'Q', 'R',
                        'T', 'V', 'W', 'X', 'Y', '2',
                        '3', '4', '6', '7', '8', '9'
                    };
        
        public String DecodeKeyByteArray(byte[] id)
        {
            //add all bytes to our list
            for (var i = start; i <= end; i++)
                keyHex.Add(id[i]);

            //now the decoding starts
            for (var i = length - 1; i >= 0; i--)
            {
                switch ((i + 1) % 6)
                {
                    //if the calculation is 0 (zero) then add the seperator
                    case 0:
                        decodedKey[i] = '-';
                        break;
                    default:
                        var idx = 0;

                        for (var j = decoded - 1; j >= 0; j--)
                        {
                            var @value = (idx << 8) | keyHex[j];
                            keyHex[j] = (byte)(@value / 24);
                            idx = @value % 24;
                            decodedKey[i] = charsInKey[idx];
                        }
                        break;
                }
            }
            return new string(decodedKey);
        }
    }
}
