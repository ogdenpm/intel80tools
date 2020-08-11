#include "omf.h"
#include "rules.h"

// macro to support enumeration lists
#define LAST(list)   (sizeof(list) / sizeof(*list))      // note last entry is default format

#define EnumEntry(key, name) {key, LAST(name) - 1, &name[0]}
typedef struct {
    uint8_t key;
    uint32_t maxEnum;
    char const **enumName;
} enumDef_t;

enumDef_t enumTable[] = {
    EnumEntry('A', enumAlign85),
    EnumEntry('F', enumFixup85),
    EnumEntry('R', enumRefType51),
    EnumEntry('S', enumScope51),
    EnumEntry('Q', enumAlign96),
    EnumEntry('\x1', enumRelType51),
    EnumEntry('\x2', enumUsage51),
    EnumEntry('\x3', enumSegType51),
    EnumEntry('M',enumModDat86),
    EnumEntry('\x4', enumAlign86),
    EnumEntry('\x5', enumCombine86),
    {0}
};


bool parseRecNumber(uint8_t const **ps, var_t *pvar) {
    uint32_t pos = getRecPos();     // needed for 'V' option
    uint8_t type = **ps;

    if (type == 'V') {
        if (!getRecNumber(1, &pvar->ival))
            return false;
        if (pvar->ival >= 128)
        type = pvar->ival;
    }
    switch (type) {
    case 0x88: case 'D': pvar->len = 4; break;
    case 0x84: case '3': pvar->len = 3; break;
    case 0x81: case 'W': pvar->len = 2; break;
    case 'V': case 'B': pvar->len = 1; break;
    default:            setRecPos(pos); return false;
    }
    if (type != 'V' && !getRecNumber((uint8_t) pvar->len, &pvar->ival)) {
        setRecPos(pos);
        return false;
    }
    sprintf(pvar->sval, "%0*X", pvar->len * 2, pvar->ival);
    ++*ps;
    return true;
}

bool parseRecIndex(uint8_t const **ps, var_t *pvar) {
    if (!getRecIndex(&pvar->ival))
        return false;
    pvar->len = pvar->ival > 255 ? 2 : 1;
    sprintf(pvar->sval, "%u", pvar->ival);
    ++*ps;
    return true;
}

bool parseRecName(uint8_t const **ps, var_t *pvar) {
    if (!getRecName(&pvar->ival, pvar->sval))
        return false;
    if (pvar->ival >= 60) {
        strcpy(pvar->sval + 56, "...");
        pvar->len = 60;
    } else
        pvar->len = pvar->ival;
    ++*ps;
    return true;
}


bool parseRecRest(uint8_t const **ps, var_t *pvar) {
    return getRecRest(&pvar->ival, pvar->sval);
}

bool parseIndexVal(uint8_t const **ps, var_t *pvar) {
    uint8_t const *s = *ps + 1;
    pvar->ival = getAllocatedIndex(*s);
    pvar->len = 2;
    if (pvar->ival != 0xffff) {
        sprintf(pvar->sval, "%u", pvar->ival);
        ++*ps;
    } else
        strcpy(pvar->sval, "****");
    *ps = s + 1;
    return true;
}

// because in principle seg names need to be qualified by class & ovl
// this code checks whether the same seg stem already occurs
// if it does it enables the full seg version by converting the marker from 0x80 to 0xA0
// the stored string format is segStem (0x80 or 0xA0) <class,ovl>
// passed in string has 0x80 which would supress the extent
void chkFullSegName(index_t *p, char *s) {         // checks if the seg stem is duplicated
    char *t = strchr(s, 0x80);                      
    if (!t)
        return;
    size_t len = t - s;
    for (unsigned i = 1; i < p->index; i++) {
        // check for a match
        t = (char *)p->names[i];             // override normal const as we may update the string
        if (t && strncmp(t, s, len) == 0 && (t[len] & 0x80)) {
            // make sure  both are SEGFULL, note previous name may already be full format
            // the test check for case where there is no class/overlay info
            if (s[len+1])  s[len] = SEGFULL;
            if (t[len+1])  t[len] = SEGFULL;
            return;
        }
    }
}

bool parseUpdateIndex(uint8_t const **ps, char *str) {
    uint8_t const *s = *ps;
    index_t *pIndex = selectIndex(*s++);
    if (!pIndex)
        return false;

    uint16_t insert = isdigit(*s) ? vars[*s++].ival : pIndex->index;

    if (**ps == 's') {                              // seg names are special
        if (omfVer == OMF85) {
            if (insert < 256 && !pIndex->names[insert]) {
                char tmp[258];
                sprintf(tmp, "/%s/", str);
                pIndex->names[insert] = xstrdup(tmp);
                *ps = s;
                return true;
            }
            Log("Error: Illegal Index Assignment /%u/ %s\n", insert, str);
            return false;
        } else if (omfVer == OMF86)
            chkFullSegName(pIndex, str);
    }
    
    if (insert < pIndex->index) {
        Log("Error: Illegal Index Assignment %s %u %s - index = %u\n", pIndex->label, insert, str, pIndex->index);
        return false;
    }
    if (pIndex->size) {                 // name storing options will have a none 0 size
        if (insert >= pIndex->size)
            extendIndex(pIndex, insert + INDEXCHUNK);

        if (insert != pIndex->index) {
            Log("Warning: Missing entries %s %u %s - index = %u\n", pIndex->label, insert, str, pIndex->index);
            while (insert < pIndex->index)
                pIndex->names[pIndex->index++] = NULL;
        }
        pIndex->names[pIndex->index] = xstrdup(str);
    }
    pIndex->index++;
    *ps = s;
    return true;
}

char const *getIndexName(uint8_t type, uint32_t val) {
    char tmp[256];
    index_t *p = selectIndex(type);

    if (!p)
        return "";

    if (val < p->index && p->names[val]) {
        if (type != 's' || !(strchr(p->names[val], SEGSHORT) || strchr(p->names[val], SEGFULL)))
            return p->names[val];
        strcpy(tmp, p->names[val]);
        char *s = strchr(tmp, SEGSHORT);
        if (!s) s = strchr(tmp, SEGFULL);
        *s &= 0x7f;
    } else if (type == 's' && omfVer == OMF85 && val < 256)
        sprintf(tmp, "/%u/", val);
    else
        sprintf(tmp, "%s(%u)", p->label, val);
    return xstrdup(tmp);
}


bool fmtEnum(uint8_t type, uint32_t val, char *s) {
    for (enumDef_t *p = enumTable; p->key; p++)
        if (p->key == type) {
            if (val < p->maxEnum)
                strcpy(s, p->enumName[val + 1]);
            else
                sprintf(s, "%s(%u)", p->enumName[0], val);
            return true;
        }
    return false;
}



char *pad(char *s, uint16_t width) {     // extend string with spaces until at least width long, always put at least 1 space
    char *t = strchr(s, 0);
    do {
        *t++ = ' ';
    } while (t - s < width);

    *t = 0;
    return t;
}


