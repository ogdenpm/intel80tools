using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Security.Cryptography.X509Certificates;
using System.Reflection;
using GitVersionInfo;

namespace disIntelLib
{
    class Program
    {

        static Omf omf { get; set; }
        static Image image { get; set; }
        static int t6seg { get; set; }

        
        static int Main(string[] args)
        {

            if (args.Length != 1)
            {
                Console.Error.WriteLine("Usage: {0} -v | -V | infile", Environment.GetCommandLineArgs()[0]);
                return 1;
            }

            if (args[0].ToLower() == "-v")
            {
                VersionInfo.showVersion(args[0] == "-V");
                return 0;
            }

                
            try
            {
                omf = new Omf(args[0]);
                string basedir = Path.ChangeExtension(args[0], null);
                if (!Directory.Exists(basedir))
                    Directory.CreateDirectory(basedir);
                decodeOMF(basedir);
                
             }
            catch (Exception e)
            {
                Console.Error.WriteLine(e.Message);
                return 1;
            }

  
          
 //          Console.ReadLine();
            return 0;
        }

        static void decodeOMF(string basedir)
        {
            int type;
            Console.OutputEncoding = Encoding.ASCII;
            TextWriter sw = Console.Out;
            int compiler = 0;
 

            while ((type = omf.nextRec()) >= 0)
            {
                switch (type) {
                case 2:
                        sw.Close();
                        string modname = omf.iname();
                        compiler = omf.ibyte();
                        sw = new StreamWriter(Path.ChangeExtension(Path.Combine(basedir, modname), compiler == 1 ? ".plm" :
                            compiler == 2 ? ".for" : ".asm"), false, Encoding.ASCII);
                        sw.WriteLine("{0}: DO;", modname);
                        dump2(sw, compiler);  break;
                case 4:
                    dump4(sw, compiler); break;
                case 6:
                    dump6(sw); break;
                case 8:
                    dump8(sw); break;
                //case 0xe:
                //    sw.WriteLine("ENDLIB:\n");
                //    break;
                case 0x12:
                    dump12_16(sw, type);
                    break;
                case 0x16:
                    dump12_16(sw, type);
                    break;
                case 0x18:
                    dump18(sw, compiler);break;
                case 0x20:
                    dump20(sw); break;
                case 0x22:
                    dump22_24(sw, type); break;
                case 0x24:
                    dump22_24(sw, type); break;
                //case 0x26:
                //    dump26(fpout); break;
                //case 0x28:
                //    dump28(fpout); break;
                //case 0x2a:
                //    dump2a(fpout); break;
                //case 0x2c:
                //    dump2c(fpout); break;
                //case 0x2e:
                //    dump2e(fpout); break;
                //default:
                //    sw.WriteLine("skip: type {0:X2}, length {1}", type, omf.getLen());
                //    // dumpUndef(fpout, reclen, 0, 0);
                //    break;
                }
            }
            sw.Close();
        }

        static void dump2(TextWriter sw, int compiler)
        {
            int clen, dlen;
            int version;

 
            switch (compiler)
            {
                case 0: sw.Write("; ASM80,"); break;
                case 1: sw.Write("/* PLM80,"); break;
                case 2: sw.Write("C FORT80,"); break;
                default: sw.Write("/* b1={0:X2},", compiler); break;
            }
            version = omf.ibyte();
            if (version != 0)
                sw.Write(" v{0}.{1},", version / 16, version % 16);
            else
                sw.Write(" b2={0:X2},", version);

            clen = dlen = 0;
            int segid;
            while ((segid = omf.ibyte()) >= 0)
            {
                int size = omf.iword();
                sw.Write(" {0}[{1:X4}H {2}]", Omf.segStr(segid), size, Omf.alignStr(omf.ibyte()));

                if (segid == 1)
                    clen = size;
                else if (segid == 2)
                    dlen = size;
            }
            if (compiler == 1)
                sw.WriteLine(" */");
            else
                sw.WriteLine("");
            image = new Image(clen, dlen);
        }

        static void dump4(TextWriter sw, int compiler)
        {
            image.dump(sw, compiler);
            sw.Write("MODEND:");
            if (omf.ibyte() == 1)
                sw.Write(" main");
            sw.Write(" {0:8}", Omf.segStr(omf.ibyte()));
            sw.WriteLine(" {0:X4}", omf.iword());
            sw.WriteLine();
        }

        static void dump6(TextWriter sw)
        {
            int segid = t6seg = omf.ibyte();
            int addr = omf.iword();
            //sw.WriteLine("REDATA: {0} {1:X4}", Omf.segStr(segid), addr);
            int b;
            while ((b = omf.ibyte()) >= 0)
                image.loadMem(segid, addr++, b);
        }

        static void dump8(TextWriter sw)
        {
            //sw.WriteLine("DEBUG:");
            int segid = omf.ibyte();
            int addr;
            while ((addr = omf.iword()) >= 0)
                image.addComment(segid, addr, string.Format("Line {0}", omf.iword()));
        }

        static void dump12_16(TextWriter sw, int type)
        {
            //sw.WriteLine(type == 0x12 ? "LOCALS:" : "PUBLICS:");
            int segid = omf.ibyte();

            int addr;
            while ((addr = omf.iword()) >= 0)
            {
                string name = omf.iname();
                image.addLabel(segid, addr, name, type == 0x16);
                //sw.WriteLine("{0}\t{1:X4} {2}", Omf.segStr(segid), addr, name);
                omf.ibyte();
            }
        }

        static void dump18(TextWriter sw, int compiler)
        {
            //sw.WriteLine("EXTERN:");
            string name;
            while ((name = omf.iname()) != null)
            {
                int symid = image.addSymbol(name);
                sw.WriteLine(compiler == 1 ? "declare {0} external;" : "\textrn\t{0}", name);
                //sw.WriteLine("{0}\t{1}", symid, name);
                omf.ibyte();    // junk byte
            }
        }

        static void dump20(TextWriter sw)
        {
            //sw.WriteLine("EXTFIX:");
            omf.ibyte();        // fixup type
            int symid, addr;
            while ((symid = omf.iword()) >= 0)
            {
                addr = omf.iword();
                image.addExternal(t6seg, addr, symid);
    
            }
        }

        static void dump22_24(TextWriter sw, int type)
        {
            //sw.WriteLine(type == 0x22 ? "FIXUP:" : "SEGFIX:");
            int segid = type == 0x22 ? t6seg : omf.ibyte();
            int fixup = omf.ibyte();
            int addr;
            while ((addr = omf.iword()) >= 0)
            {
                image.addFixup(t6seg, addr, fixup, segid);
            }

        }
    }
}
