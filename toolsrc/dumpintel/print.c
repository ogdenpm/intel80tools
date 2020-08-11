#include "omf.h"
#include "rules.h"
#include <stdarg.h>


uint16_t curPos;
uint16_t cWidth;
uint16_t curCol;
uint16_t indent;


void printIData(FILE *fpout, uint32_t addr);


char *padStr(char *s, int width) {
    char *end = s + width;
    s = strchr(s, 0);
    while (s < end)
        *s++ = ' ';
    *s = 0;
    return s;
}


char *wStrcat(char *s, int width, char const *str) {
    s = strchr(s, 0);
    if (width <= 0) {     // left aligned or no padding
        strcpy(s, str);
        return padStr(s, -width);
    }
    strcpy(padStr(s, width - strlen(str)), str);
    return strchr(s, 0);
}


char *aSprintf(char *s, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vsprintf(strchr(s, 0), fmt, args);
    va_end(args);
    return strchr(s, 0);
}

char *awSprintf(char *s, char const *fmt, int width, ...) {
    va_list args;
    va_start(args, width);
    char tmp[256];

    vsprintf(tmp, fmt, args);
    s = wStrcat(s, width, tmp);
    va_end(args);
    return s;
}


void putch(char c, FILE *fpout) {
    putc(c, fpout);
    if (c == '\n')
        curPos = curCol = 0;
    else
        curPos++;
}

uint8_t const *printRule(FILE *fpout, uint8_t const *s) {
    uint32_t num;
    uint32_t tPos;
    uint8_t c;
    char *token;


    while (*s >= DMPDATTAG) {
        switch (c = *s++) {
        case IF0TAG: case IF1TAG:
            s = skipToTrue(s, c);
            break;
        case IF0TAG | ELSIFFLAG: case IF1TAG | ELSIFFLAG:
            s = skipToEnd(s, c);
            break;
        case IF0TAG | ENDIFFLAG: case IF1TAG | ENDIFFLAG:
            break;
        case DMPDATTAG:
            printData(fpout, parseExpr(&s));
            break;
        case DMPIDATTAG:
            printIData(fpout, parseExpr(&s));
            break;
        case '\b':
            break;
        case '\f':
            num = isdigit(*s) ? *s++ - '0' : 0;
            if (num == 0) {
                if (curPos)               // switch from indent mode
                    putch('\n', fpout);
                curCol = indent = 0;
                cWidth = MAXPOS;
            } else {
                if (indent == 0 && curPos)          // switch from non indent mode
                    putch('\n', fpout);

                indent = INDENT;
                cWidth = (MAXPOS - INDENT) / num;
                if (curPos) {
                    curCol = ((curPos - indent + cWidth + MINGAP) / cWidth);  // make sure at least MINGAP chars between cols
                    if (curCol >= num)
                        curCol = 0;
                } else
                    curCol = 0;

            }
            tPos = curCol * cWidth + indent;
            if (tPos < curPos)
                putch('\n', fpout);

            while (curPos < tPos)
                putch(' ', fpout);
            break;
        case '\t':
            tPos = curCol * cWidth + indent + parseExpr(&s);
            while (curPos < tPos) {
                putc(' ', fpout);
                curPos++;
            }
            break;
        case '$':
            switch (c = *s++) {
            case 'P':                   // position in file
                curPos += fprintf(fpout, "%04X:%02X", fpos / 128, fpos % 128);
                break;
            case 'R': fprintf(fpout, "%02X", rectype); break;
            case 'L': fprintf(fpout, "%04X", reclen); break;
            case 'C': printComment86(fpout); break;
            case 'T': printTypedef86(fpout); break;
            case '$': putch('$', fpout);
            default:
                if (isdigit(c)) {
                    token = vars[c - '0'].sval;
                    curPos += fprintf(fpout, "%*s", (*s == WIDETAG) ? strtol(s + 1, (char **)&s, 10) : 0, token);
                } else {
                    putch('$', fpout);      // treat as spurious $
                    s--;
                }
            }
            break;
        default:
            putch(c, fpout);
        }

    }
    return s;
}


void printData(FILE *fpout, uint32_t addr) {

    char chars[BytesPerLine + 1];
    char line[BytesPerLine * 3 + 2];
    uint32_t c;

    if (curPos != 0)
        putch('\n', fpout);
    // note routine will leave curPos as 0 with \n as the last char emitted
    while (!isRecEnd()) {
        memset(chars, ' ', sizeof(chars));
        memset(line, ' ', sizeof(line));
        fprintf(fpout, omfVer == OMF86 ? "%*s%06X " :"%*s%04X ", INDENT, "", addr & ~(BytesPerLine - 1));   // print address
        while (getRecNumber(1, &c)) {
            sprintf(line + addr % BytesPerLine * 3, " %02X", c);
            chars[addr % BytesPerLine] = (c < ' ' || '~' < c) ? '.' : c;
            if (++addr % BytesPerLine == 0)
                break;
        }

        for (char *s = chars + BytesPerLine; s > chars && *s == ' '; s--)  // trim the chars string
            *s = 0;
        fprintf(fpout, "%-*s | %s\n", BytesPerLine * 3, line, chars);
    }
}


bool printBlkContent(FILE *fpout, uint32_t basePos, uint16_t indent) {
    char chars[BytesPerLine + 1];
    char line[BytesPerLine * 3 + 2];
    uint32_t c;
    uint32_t addr = getRecPos() - basePos;
    uint32_t cnt;
    bool ok = true;
    if (!getRecNumber(1, &cnt))
        return false;

    while (cnt) {
        memset(chars, ' ', sizeof(chars));
        memset(line, ' ', sizeof(line));
        if (!curPos)
            curPos = fprintf(fpout, "%*s", indent, "");
        curPos += fprintf(fpout, "%04X ", addr);   // print address
        for (int i = 0; i < BytesPerLine && cnt && (ok = getRecNumber(1, &c)); i++, addr++) {
            cnt--;
            sprintf(line + i * 3, " %02X", c);
            chars[i] = (c < ' ' || '~' < c) ? '.' : c;
        }
        for (char *s = chars + BytesPerLine; s > chars && *s == ' '; s--)  // trim the chars string
            *s = 0;
        fprintf(fpout, "%-*s | %s\n", BytesPerLine * 3, line, chars);
        curPos = 0;
        if (!ok)
            return false;
    }
    return true;
}


bool printRepBlock(FILE *fpout, uint32_t basePos, uint32_t blockCnt, uint16_t indent) {
    bool ok = true;
    for (unsigned i = 1; ok && i <= blockCnt; i++) {
        if (curPos == 0)
            curPos = fprintf(fpout, "%*s", indent, "");
        if (indent != INDENT)
            curPos += fprintf(fpout, "%d) ", i);
        uint32_t repCnt;
        if (ok = getRecNumber(2, &repCnt)) {
            if (repCnt > 1)
                curPos += fprintf(fpout, "%u* ", repCnt);

            uint32_t blkCnt;
            if (ok = getRecNumber(2, &blkCnt)) {
                if (blkCnt)
                    ok = printRepBlock(fpout, basePos, blkCnt, curPos);
                else
                    ok = printBlkContent(fpout, basePos, curPos);
                curPos = 0;
            }
        }
    }
    return ok;
}


void printIData(FILE *fpout, uint32_t addr) {
    uint32_t initialPos = getRecPos();
    uint32_t indent;

    if (curPos != 0)
        putch('\n', fpout);
    curPos = indent = fprintf(fpout, "%*s", INDENT, "");

    while (!isRecEnd()) {
        if (!printRepBlock(fpout, initialPos, 1, indent)) {
            Log("Unexpected end in iterated data\n");
            break;
        }
    }
    if (curPos != 0)
        putch('\n', fpout);

}
