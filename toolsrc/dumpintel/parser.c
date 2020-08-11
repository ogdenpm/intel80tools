#include "omf.h"
#include "rules.h"
#include "rulesInternal.h"



var_t vars[10];      // upto 10 vars
uint8_t varIdx;




uint8_t const **rules;
uint8_t minRecType;
uint8_t maxRecType;

bool resetOnHeader = true;


uint32_t parseExpr(uint8_t const **ps) {
    uint8_t const *s = *ps;
    uint32_t rhs = 0;
    uint32_t lhs = 0;
    uint8_t c;
    char op = 0;

    for (;;) {
        switch (c = *s) {
        case '$':
            rhs = isdigit(*++s) ? vars[*s++ - '0'].ival : 0;
            break;
        default:
            rhs = isdigit(*s) ? strtoul(s, (char **)&s, 0) : 0;
            break;
        }
        switch (op) {
        case 0:    lhs = rhs;  break;
        case '+':  lhs += rhs; break;
        case '-':  lhs -= rhs; break;
        case '*':  lhs *= rhs; break;
        case '&':  lhs &= rhs; break;
        case '|':  lhs |= rhs; break;
        case '/':  lhs = (rhs == 0) ? 0 : lhs / rhs; break;
        case '%':  lhs = (rhs == 0) ? 0 : lhs % rhs; break;
        }
        if (*s && strchr("+-*/%|&", *s))
            op = *s++;
        else {
            *ps = s;
            return lhs;
        }
    }
}


bool parseCond(uint8_t const **ps) {
    uint32_t lhs = parseExpr(ps);
    char op;

    if (strchr("=<>!", op = **ps)) {
        ++*ps;
        uint32_t rhs = parseExpr(ps);
        switch (op) {
        case '=':   return lhs == rhs;
        case '<':   return lhs < rhs;
        case '>':   return lhs > rhs;
        case '!':   return lhs != rhs;
        }
    }
    return lhs != 0;
}


uint8_t const *skipToTrue(uint8_t const *s, uint8_t ifType) {
    uint8_t endVal = ifType | ENDIFFLAG;
    uint8_t elsifVal = ifType | ELSIFFLAG;

    while (*s) {
        if (parseCond(&s))
            return s;

        while (*s && *s != endVal && *s != elsifVal)       // allow if to group tok & str sections
            s++;
        if (!*s || *s == endVal)
            break;
        s++;
    }
    return s;
}

uint8_t const *skipToEnd(uint8_t const *s, uint8_t ifType) {
    uint8_t endVal = (ifType & ~ELSIFFLAG)| ENDIFFLAG;
    while (*s && *s != endVal)
        s++;
    return s;
}

bool format(char const **ps, var_t *pvar) {
    uint8_t c;
    int num;
    char const *s = *ps + 1;

    if (*s <= DMPDATTAG)        // no format
        return false;

    switch (c = *s++) {
    case 'r':                                       // refType
    default:                                        // upper case, digits - note \x1 ... used for specials
        if (!fmtEnum(c, pvar->ival, pvar->sval))
            return false;
        break;
    case 's': case 'b': case 'e': case 'l': case 'g': case 'o':
        strcpy(pvar->sval, getIndexName(c, pvar->ival));
        break;
    case 'p':
        sprintf(pvar->sval, "%04X:%02X", pvar->ival / 128, pvar->ival % 128);
        break;
    case 'u':           // unsigned optional num to specific 0 padding
    case 'x':           // hex variant
        num = isdigit(*s) ? strtol(s, (char **)&s, 10) : 0;
        if (num)
            sprintf(pvar->sval, c == 'u' ? "%0*u" : "%0*X", num, pvar->ival);
        else
            sprintf(pvar->sval, c == 'u' ? "%u" : "%X", pvar->ival);
        break;
    case 'v':
        sprintf(pvar->sval, "%u.%u", (pvar->ival >> 4) & 0xf, pvar->ival & 0xf);
        break;
    case 't':           // trn
        if (isTrn85(pvar->ival)) fmtTrn85(pvar);
        else if (isTrn51(pvar->ival)) fmtTrn51(pvar);
        else if(isTrn96(pvar->ival)) fmtTrn96(pvar);
        else
            sprintf(pvar->sval, "Translator(%u)", pvar->ival);
        break;
    case 'q':
        fmtSegId96(pvar);
        break;
    case 'm':           // reg mask 8051
        fmtRegMask51(pvar);
        break;
    case 'i':           // segInfo 8051
        fmtSegInfo51(pvar);
        break;
    case 'y':           // symInfo 8051
        fmtSymInfo51(pvar);
        break;
    case 'z':
        if (!pvar->sval[0])
            strcpy(pvar->sval, "==end==");
        break;
    }
    *ps = s;
    return true;
}



uint8_t const *parseUsrTok(uint8_t rectype, uint8_t const *s) {
    return s;
}




char const *parseTokens(uint8_t const *s) {
    bool ok = true;
    while (ok && *s > DMPDATTAG) {
        switch (*s) {
        case IF0TAG: case IF1TAG:
            s = skipToTrue(s + 1, *s);
            break;
        case IF0TAG | ELSIFFLAG: case IF1TAG | ELSIFFLAG:
            s = skipToEnd(s + 1, *s);
            break;
        case IF0TAG | ENDIFFLAG: case IF1TAG | ENDIFFLAG:
        case ' ': case '\b':
            s++;
            break;
        case 'I':
            ok = parseRecIndex(&s, &vars[varIdx++]);
            break;
        case 'D': case '3': case 'W' :  case 'B': case 'V':
            ok = parseRecNumber(&s, &vars[varIdx++]);
            break;
        case 'F':
            ok = parseFixup86(&s, &vars[varIdx++]);
            break;
        case 'Q':
            ok = getRecRest(&vars[varIdx].ival, vars[varIdx].sval);
            s++;
            varIdx++;
            break;
        case 'N':
            ok = parseRecName(&s, &vars[varIdx++]);
            break;
        case '#':
            ok = parseIndexVal(&s, &vars[varIdx++]);
            break;
        case 'G':
            ok = parseGroupComponent86(&s, &vars[varIdx++]);
            break;
        case 'L':
            ok = parseBase86(&s, &vars[varIdx++]);
            break;
        case 'S':
            ok = parseSegDef86(&s, &vars[varIdx++]);
            break;
        case 'e': case 's': case 'b':                   // add previous var in tables optional hex digit to select token to use vs. auto next
        case 'l': case 'g': case 'o': case 't':
            ok = parseUpdateIndex(&s, vars[varIdx - 1].sval);
            break;
        case '%':
            ok = format(&s, &vars[varIdx - 1]);
            break;
        default:
            return s;
        }
    }
    return s;
}

bool parseRule(FILE *fpout, uint8_t rectype) {
    uint8_t const *p;
    int repVarIdx = 0;

    if (resetOnHeader) {
        if (rectype == 2) {         // autodetect OMF type
            uint32_t trn;
            uint32_t pos = getRecPos();
            if (getRecName(NULL, NULL) && getRecNumber(1, &trn)) {
                setRecPos(pos);

                if (isTrn51(trn))
                    init51();
                else if (isTrn85(trn))
                    init85();
                else if (isTrn96(trn))
                    init96();
                else
                    init85();
                resetOnHeader = false;
            } else {
                setRecPos(pos);
                return false;
            }
        } else if (rectype == 0x6E || rectype == 0x80 || rectype == 0x82) {
            init86();
            resetOnHeader = false;
        }
    }
    if (!rules)
        if (rectype >= 0x6e)
            init86();
        else
            init85();

    if (rectype == 0x8A || rectype == 0x4)
        resetOnHeader = true;

    if (rectype % 2 || rectype < minRecType || rectype > maxRecType || !(p = rules[(rectype - minRecType) / 2 + 1])) {
        Log("Invalid Record Type %d\n", rectype);
        p = rules[0];
    }

    varIdx = 0;
    memset(vars, 0, sizeof(vars));
    p = printRule(fpout, p);

     char const *rep = NULL;
    for (;;) {
        switch (*p++) {
        case FIXTOKTAG:
            p = parseTokens(p);
            break;
        case DMPDATTAG:             // make sure print notices the tag
        case DMPIDATTAG:
            --p;
        case FIXSTRTAG:
        case REPSTRTAG:
            p = printRule(fpout, p);
            break;
        case REPTOKTAG:
            rep = p;    
            repVarIdx = varIdx;     // fall through to end of record test (note will repeat from after the tag)
        case 0:
            if (isRecEnd())
                return true;
            else if (rep == NULL)
                return false;
            varIdx = repVarIdx;
            memset(vars + varIdx, 0, sizeof(vars) - sizeof(vars[0]) * varIdx);
            p = parseTokens(rep);
            break;
        case USRTOKTAG:
            p = parseUsrTok(rectype, p);
            break;
        default:
            return false;
        }
    }
}




