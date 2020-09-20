using System;
using System.Collections.Generic;
using System.IO;

namespace disIntelLib
{
    [FlagsAttribute]
    enum Usage { Used = 1, Word = 2, Code = 4, Data = 8};

    class loc
    {
        public Usage usage;
        public int mem;
        public string comment;
        public int symbolId;
        public int ftype;
        public int fseg;
        public string label;

    }



    class Image
    {

        loc[] code;
        loc[] data;
        List<string> symbols = new List<string>();
        List<string> equ = new List<string>();
        Queue<int> codept = new Queue<int>();

        loc[] getLoc(int segid)
        {
            if (segid == 1)
                return code;
            if (segid == 2)
                return data;
            return null;
        }

        public Image(int csize, int dsize)
        {
            code = new loc [csize];
            for(int i = 0; i < csize; i++) {
                code[i] = new loc();
            }

            data = new loc[dsize];
            for (int i = 0; i < dsize; i++)
            {
                data[i] = new loc();
            }

        }

        public int addSymbol(string sym)
        {
            symbols.Add(sym);
            return symbols.Count - 1;
        }

        public void addExternal(int segid, int addr, int symid)
        {
            loc[] cloc = getLoc(segid);
            if (cloc == null)
                return;
            cloc[addr].symbolId = symid + 1;
            cloc[addr].usage |= Usage.Word;
        }



        public string extSymbol(int symbolId) => symbols[symbolId];

        public void loadMem(int segid, int addr, int b)
        {
            loc[] cloc = getLoc(segid);
            if (cloc == null)
                return;

            cloc[addr].mem = b;
            cloc[addr].usage |= Usage.Used;
        }

        public void addComment(int segid, int addr, string com)
        {
            loc[] cloc = getLoc(segid);
            if (cloc == null || addr >= cloc.Length)
                return;
            if (cloc[addr].comment == null)
                cloc[addr].comment = com;
            else
                cloc[addr].comment += "\r\n\t\t;" + com;
        }

        public void addLabel(int segid, int addr, string name, bool isPublic)
        {
            loc[] cloc = getLoc(segid);
            if (cloc == null || addr >= cloc.Length)
                equ.Add(string.Format("{0}\tequ\t{1}\t;{2}{3}", name, hexify(addr), Omf.segStr(segid), isPublic ? " public" : ""));
            else if (cloc[addr].label == null || cloc[addr].label[1] == '$')    /* new or overwrite of local L$ D$ label */
                cloc[addr].label = name + (isPublic ? ":\t; public": ":");
            else
                cloc[addr].label += "\r\n" + name + (isPublic ? ":\t; public" : ":");
            if (cloc == code)
                codept.Enqueue(addr);       // add as a code point            
        }

        public void addRefLabel(int segid, int addr)
        {
            loc[] cloc = getLoc(segid);
            if (cloc == null || addr >= cloc.Length || cloc[addr].label != null)
                return;
            cloc[addr].label = string.Format("{0}{1:X4}:", segid == 1 ? "L$" : "D$", addr);
        }

        public void addFixup(int segid, int addr, int fixupType, int fixupSeg)
        {
            loc[] cloc = getLoc(segid);
            if (cloc == null)
                return;
            cloc[addr].ftype = fixupType;
            cloc[addr].fseg = fixupSeg;
            if (fixupType == 3)
            {
                cloc[addr].usage |= Usage.Word;
                if (fixupSeg == 1 || fixupSeg == 2)
                {
                    int target = cloc[addr].mem + cloc[addr + 1].mem * 256;
                    addRefLabel(fixupSeg, target);
                }
            }
        }

        string getLabel(int segid, int addr)
        {
            loc[] cloc = getLoc(segid);
            if (cloc == null || addr >= cloc.Length)
                return string.Format("{1} + {0}", hexify(addr), Omf.segStr(segid));
            return cloc[addr].label.Substring(0, cloc[addr].label.IndexOf(':'));
        }


        string byteref(loc[] cloc, int addr)
        {
            if ((cloc[addr].usage & Usage.Used) == 0)
                return "?";

            int mem = cloc[addr].mem;
            int ftype = cloc[addr].ftype;
            int fseg = cloc[addr].fseg;
            if (ftype != 0)
            {
                if (mem == 0)
                    return string.Format("{0}({1})", Omf.fixStr(ftype), Omf.segStr(fseg));
                else
                    return string.Format("{0}({1}) + {2}", Omf.fixStr(ftype), Omf.segStr(fseg), hexify(mem));
            }
            else
            {
                return hexify(mem);
            }
        }

        string wordref(loc[] cloc, int addr)
        {
            if ((cloc[addr].usage & Usage.Used) == 0)
                return "?";
            int mem = cloc[addr].mem + cloc[addr + 1].mem * 256;
            int fseg = cloc[addr].fseg;
            string target = "";
            if (cloc[addr].symbolId > 0)
                target = extSymbol(cloc[addr].symbolId - 1);
            else if (fseg == 1 || fseg == 2)
                return getLabel(fseg, mem);
            else if (fseg == 3 || fseg == 4)
                target = Omf.segStr(fseg);

            if (target != "")
                if (mem == 0)
                    return target;
                else
                    return string.Format("{0}.o{1}", target, mem);
            else
                return hexify(mem);
        }

        void dumploc(TextWriter sw, loc[] cloc, int compiler)
        {
            string db = "";
            int dscnt = 0;
            bool inString = false;
            int dbcnt = 0;
            int dwVal = 0;

            for (int i = 0; i < cloc.Length; i++)
            {
                if (cloc[i].label != null)
                {
                    if (dbcnt != 0)
                    {
                        if (dbcnt == 2 && !inString)
                            sw.WriteLine(compiler == 1 ? "\tADDRESS INITIAL({0})" : "\tdw {0}", dwVal);
                        else
                            sw.WriteLine(compiler == 1 ? "\tBYTE(*) INITIAL({0}{1})" : "\tdb {0}{1}", db, inString ? "'" : "");
                        
                        db = "";
                        inString = false;
                        dbcnt = 0;
                    }
                    if (dscnt != 0)
                    {
                        if (dscnt == 2)
                            sw.WriteLine(compiler == 1 ? "\tADDRESS" : "\tds 2");
                        else
                            sw.WriteLine(compiler == 1 ? "\tBYTE({0})" : "\tds {0}", dscnt);
                        dscnt = 0;
                    }

                    sw.WriteLine(cloc[i].label);
                }
                Usage usage = cloc[i].usage;
                if ((usage & (Usage.Code | Usage.Word)) == 0)    // simple db
                {
                    if ((usage & Usage.Used) == 0)
                    {    // simple db
                        if (db != "")
                        {
                            if (dbcnt == 2 && !inString)
                                sw.WriteLine(compiler == 1 ? "\tADDRESS INITIAL({0})" : "\tdw {0}", dwVal);
                            else
                                sw.WriteLine(compiler == 1 ? "\tBYTE(*) INITIAL({0}{1})" : "\tdb {0}{1}", db, inString ? "'" : "");
                            db = "";
                            inString = false;
                            dbcnt = 0;
                        }
                        dscnt++;
                    }
                    else
                    {
                        if (dscnt != 0)
                        {
                            if (dscnt == 2)
                                sw.WriteLine(compiler == 1 ?"\tADDRESS" : "\tds 2");
                            else
                                sw.WriteLine(compiler == 1 ? "\tBYTE({0})" : "\tds {0}", dscnt);
                            dscnt = 0;
                        }
                        int mem = cloc[i].mem;
                        if (0x20 <= mem && mem <= 0x7f && mem != 0x27 && cloc[i].ftype == 0)
                        {
                            if (!inString)
                                db += (db != "") ? ", '" : "'";
                            inString = true;
                            db += (char)mem;
                        }
                        else
                        {
                            if (inString)
                            {
                                db += "', ";
                                inString = false;
                            }
                            else if (db != "")
                                db += ", ";
                            db += byteref(cloc, i);
                        }
                        if (!inString)
                            if (dbcnt == 0)
                                dwVal = mem;
                            else if (dbcnt == 1)
                                dwVal += mem * 256;
                        if (++dbcnt == 16)
                        {
                            sw.WriteLine(compiler == 1 ? "\tBYTE(*) INITIAL({0}{1})" : "\tdb {0}{1}", db, inString ? "'" : "");
                            db = "";
                            inString = false;
                            dbcnt = 0;
                        }
                    }
                    continue;
                }
                else
                {
                    if (db != "")
                    {
                        if (dbcnt == 2 && !inString)
                            sw.WriteLine(compiler == 1 ? "\tADDRESS INITIAL({0})" : "\tdw {0}", dwVal);
                        else
                            sw.WriteLine(compiler == 1 ? "\tBYTE(*) INITIAL({0}{1})" : "\tdb {0}{1}", db, inString ? "'" : "");
                        db = "";
                        inString = false;
                        dbcnt = 0;
                    }
                    if (dscnt != 0)
                    {
                        if (dscnt == 2)
                            sw.WriteLine(compiler == 1 ? "\tADDRESS" : "\tds 2");
                        else
                            sw.WriteLine(compiler == 1 ? "\tBYTE({0})" :"\tds {0}", dscnt);
                        dscnt = 0;
                    }
                    if ((usage & Usage.Code) == 0)                   // must be a word
                    {
                        if ((usage & Usage.Used) == 0)
                            sw.Write(compiler == 1 ? "\tADDRESS" : "\tdw");
                        else
                        {
                            string target = wordref(cloc, i);
                            if ('0' <= target[0] && target[0] <= '9')
                                sw.Write(compiler == 1 ? "\tADDRESS INITIAL({0})" : "\tdw {0}", target);
                            else
                                sw.Write(compiler == 1 ? "\tADDRESS INITIAL(.{0})" : "\tdw {0}", target);
                        }
                        i++;
                    }
                    else// must be code
                    {
                        int mem = cloc[i].mem;

                        int len = Math.Abs(disasm.ilen(mem));
                        sw.Write("\t");
                        sw.Write(disasm.str(mem));
                        if (len == 2)
                            sw.Write(byteref(cloc, ++i));
                        else if (len == 3)
                        {
                            sw.Write(wordref(cloc, ++i));
                            i++;
                        }
                    }
                }
                sw.WriteLine((cloc[i].comment != null) ? "\t;" + cloc[i].comment : "");
            }
            if (db != "")
            {
                if (dbcnt == 2 && !inString)
                    sw.WriteLine(compiler == 1 ? "\tADDRESS INITIAL({0})" : "\tdw {0}", dwVal);
                else
                    sw.WriteLine(compiler == 1 ? "\tBYTE(*) INITIAL({0}{1})" : "\tdb {0}{1}", db, inString ? "'" : "");
            }
            if (dscnt != 0)
            {
                if (dscnt == 2)
                    sw.WriteLine(compiler == 1 ? "\tADDRESS" : "\tds 2");
                else
                    sw.WriteLine(compiler == 1 ? "\tBYTE({0})" : "\tds {0}", dscnt);
            }

        }


        public void dump(TextWriter sw, int compiler)
        {
            markCode();
            foreach (string s in equ)
                sw.WriteLine(s);

            sw.WriteLine(".DATA");
            dumploc(sw, data, compiler);
            sw.WriteLine(".CODE");
            dumploc(sw, code, compiler);
        }

        bool chkValidInstr(int addr)
        {
            if (addr >= code.Length                             // ran off end
                || (code[addr].usage & Usage.Used) == 0         // no code
                || disasm.isInvalid(code[addr].mem))            // illegal instruction
                return false;

            int len = disasm.ilen(code[addr].mem);
            if (addr + len > code.Length)
                return false;
            if (len == 1 && (code[addr].usage & Usage.Word) != 0
                || (len > 1 && (code[addr + 1].usage & Usage.Used) == 0)
                || (len > 2 && (code[addr + 2].usage & Usage.Used) == 0)
                || (len == 2 && (code[addr + 1].usage & Usage.Word) != 0)
                || (len == 3 && (code[addr + 2].usage & Usage.Word) != 0))
                return false;
            return true;

        }


        public void markCode()
        {
            while(codept.Count > 0)
            {
                int start = codept.Dequeue();
                if (start >= code.Length)           // safety check
                    continue;
                if ((code[start].usage & Usage.Data) != 0)  // already rejected
                    continue;
                int addr = start;
                bool more = true;
                var coderefs = new List<int>();

                while (more)
                {
                    if (!chkValidInstr(addr))       // hit a problem
                    {
                        while (--addr >= start)     // remove code markers
                            code[addr].usage &= ~Usage.Code;
                        coderefs.Clear();
                        code[start].usage |= Usage.Data;
                        break;

                    }
                    int mem = code[addr].mem;

                    int len = disasm.ilen(mem);
                    more = ! disasm.isBreak(mem);    // terminating instruction
                    if (disasm.isCodeRef(mem) && code[addr+1].ftype == 3 && code[addr + 1].fseg == 1)
                        coderefs.Add(code[addr+1].mem + code[addr+2].mem * 256);
                    for (int i = 0; i < len; i++, addr++)
                    {
                        if ((code[addr].usage & Usage.Code) != 0)
                        {   // ran into existing code
                            more = false;
                            break;
                        }
                        else
                            code[addr].usage |= Usage.Code;
                    }
                    foreach (int m in coderefs)
                        codept.Enqueue(m);
                }
            }

        }
        string hexify(int n)
        {
            if (n <= 9)
                return n.ToString();
            if (0x10 <= n && n <= 0x9f || 0x100 <= n && n <= 0x9ff || 0x1000 <= n && n <= 0x9fff)
                return string.Format("{0:X}H", n);
            else
                return string.Format("0{0:X}H", n);
        }

    }

 

}
