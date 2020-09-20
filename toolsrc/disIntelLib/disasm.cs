using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace disIntelLib
{


    public static class disasm
    {
        static string[] instr = {
            "nop", "lxi\tb,", "stax\tb", "inx\tb", "inr\tb", "dcr\tb", "mvi\tb,", "rlc",
            "db\t08h", "dad\tb", "ldax\tb", "dcx\tb", "inr\tc", "dcr\tc", "mvi\tc,", "rrc",
            "db\t10h", "lxi\td,", "stax\td", "inx\td", "inr\td", "dcr\td", "mvi\td,", "ral",
            "db\t18h", "dad\td", "ldax\td", "dcx\td", "inr\te", "dcr\te", "mvi\te,", "rar",
            "rim", "lxi\th,", "shld\t", "inx\th", "inr\th", "dcr\th", "mvi\th,", "daa",
            "db\t28h", "dad\th", "lhld\t", "dcx\th", "inr\tl", "dcr\tl", "mvi\tl,", "cma",
            "sim", "lxi\tsp,", "sta\t", "inx\tsp", "inr\tm", "dcr\tm", "mvi\tm,", "stc",
            "db\t38h", "dad\tsp", "lda\t", "dcx\tsp", "inr\ta", "dcr\ta", "mvi\ta,", "cmc",
            "mov\tb,b", "mov\tb,c", "mov\tb,d", "mov\tb,e", "mov\tb,h", "mov\tb,l", "mov\tb,m", "mov\tb,a",
            "mov\tc,b", "mov\tc,c", "mov\tc,d", "mov\tc,e", "mov\tc,h", "mov\tc,l", "mov\tc,m", "mov\tc,a",
            "mov\td,b", "mov\td,c", "mov\td,d", "mov\td,e", "mov\td,h", "mov\td,l", "mov\td,m", "mov\td,a",
            "mov\te,b", "mov\te,c", "mov\te,d", "mov\te,e", "mov\te,h", "mov\te,l", "mov\te,m", "mov\te,a",
            "mov\th,b", "mov\th,c", "mov\th,d", "mov\th,e", "mov\th,h", "mov\th,l", "mov\th,m", "mov\th,a",
            "mov\tl,b", "mov\tl,c", "mov\tl,d", "mov\tl,e", "mov\tl,h", "mov\tl,l", "mov\tl,m", "mov\tl,a",
            "mov\tm,b", "mov\tm,c", "mov\tm,d", "mov\tm,e", "mov\tm,h", "mov\tm,l", "hlt", "mov\tm,a",
            "mov\ta,b", "mov\ta,c", "mov\ta,d", "mov\ta,e", "mov\ta,h", "mov\ta,l", "mov\ta,m", "mov\ta,a",
            "add\tb", "add\tc", "add\td", "add\te", "add\th", "add\tl", "add\tm", "add\ta",
            "adc\tb", "adc\tc", "adc\td", "adc\te", "adc\th", "adc\tl", "adc\tm", "adc\ta",
            "sub\tb", "sub\tc", "sub\td", "sub\te", "sub\th", "sub\tl", "sub\tm", "sub\ta",
            "sbb\tb", "sbb\tc", "sbb\td", "sbb\te", "sbb\th", "sbb\tl", "sbb\tm", "sbb\ta",
            "ana\tb", "ana\tc", "ana\td", "ana\te", "ana\th", "ana\tl", "ana\tm", "ana\ta",
            "xra\tb", "xra\tc", "xra\td", "xra\te", "xra\th", "xra\tl", "xra\tm", "xra\ta",
            "ora\tb", "ora\tc", "ora\td", "ora\te", "ora\th", "ora\tl", "ora\tm", "ora\ta",
            "cmp\tb", "cmp\tc", "cmp\td", "cmp\te", "cmp\th", "cmp\tl", "cmp\tm", "cmp\ta",
            "rnz", "pop\tb", "jnz\t", "jmp\t", "cnz\t", "push\tb", "adi\t", "rst\t0",
            "rz", "ret", "jz\t", "db\t0cbh", "cz\t", "call\t", "aci\t", "rst\t1",
            "rnc", "pop\td", "jnc\t", "out\t", "cnc\t", "push\td", "sui\t", "rst\t2",
            "rc", "db\t0d9h", "jc\t", "in\t", "cc\t", "db\t0ddh", "sbi\t", "rst\t3",
            "rpo", "pop\th", "jpo\t", "xthl", "cpo\t", "push\th", "ani\t", "rst\t4",
            "rpe", "pchl", "jpe\t", "xchg", "cpe\t", "db\t0edh", "xri\t", "rst\t5",
            "rp", "pop\tpsw", "jp\t", "di", "cp\t", "push\tpsw", "ori\t", "rst\t6",
            "rm", "sphl", "jm\t", "ei", "cm\t", "db\t0fdh", "cpi\t", "rst\t7"
        };

        static readonly int[] instrlen = {
        //  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
            1, 3, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 1,
            0, 3, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 1,
            0, 3, 3, 1, 1, 1, 2, 1, 0, 1, 3, 1, 1, 1, 2, 1,
            0, 3, 3, 1, 1, 1, 2, 1, 0, 1, 3, 1, 1, 1, 2, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, -1,1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 3, -3,3, 1, 2, 1, 1, -1,3, 0, 3, 3, 2, 1,
            1, 1, 3, 2, 3, 1, 2, 1, 1, 0, 3, 2, 3, 0, 2, 1,
            1, 1, 3, 1, 3, 1, 2, 1, 1, -1,3, 1, 3, 0, 2, 1,
            1, 1, 3, 1, 3, 1, 2, 1, 1, 1, 3, 1, 3, 0, 2, 1
        };

        static public int ilen(int m) => Math.Abs(instrlen[m]);

        static public string str(int m) => instr[m];

        static public bool isInvalid(int m) => instrlen[m] == 0;

        static public bool isCodeRef(int m) => m > 0x80 && ilen(m) == 3;

        static public bool isBreak(int m) => instrlen[m] < 0;
    }
}
