using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace disIntelLib
{
    class Omf
    {
        Byte[] bytes;
        int start;
        int next;
        int cur;
        int type;

        public Omf(String path)
        {
            bytes = File.ReadAllBytes(path);
        }

        public String iname()
        {
            if (cur >= next - 1)
                return null;
            int len = bytes[cur++];
            if (cur + len >= next - 1)
                return null;
            char[] chars = new char[len];
            for (int i = 0; i < len; i++)
                chars[i] = (char)bytes[cur++];
            return new string(chars);
        }

        public int ibyte()
        {
            if (cur >= next - 1)
                return -1;
            return bytes[cur++];
        }

        public int peekIbyte()
        {
            if (cur >= next - 1)
                return -1;
            return bytes[cur];

        }
        public int iword()
        {
            if (cur + 1 >= next - 1)
                return -1;
            cur += 2;
            return  bytes[cur - 1] * 256 + bytes[cur - 2];
        }

        public int getLen() => next - start - 3;

        public int nextRec()
        {
            if (next + 3 > bytes.Length)
                return -1;
            cur = start = next;
            type = bytes[cur++];
            cur += 2;
            next = cur + bytes[cur - 1] * 256 + bytes[cur - 2];
            if (next <= bytes.Length)
                return type;
            else
                return -1;
        }

        public int rewindRec()
        {
            cur = start;
            return type;
        }

        static public string segStr(int segid)
        {
            switch (segid)
            {
                case 0: return "ABSOLUTE";
                case 1: return "CODE";
                case 2: return "DATA";
                case 3: return "STACK";
                case 4: return "MEMORY";
                case 255:
                    return "//";
            }
            return string.Format("/{0}/", segid);
        }

        static public string fixStr(int fixid)
        {
            switch (fixid)
            {
                case 1: return "LOW";
                case 2: return "HIGH";
                case 3: return "WORD";
            }
            return string.Format("fixup({0})", fixid);
        }

        static public string alignStr(int alignid)
        {
            switch (alignid)
            {
                case 0: return "abs";
                case 1: return "inpage";
                case 2: return "page";
                case 3: return "byte";
            }
            return string.Format("align({0})", alignid);
        }
    }
}
