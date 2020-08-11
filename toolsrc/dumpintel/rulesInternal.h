#pragma once
/* stringified versions of the IFx Tags*/
#define IF0(cond)       "\x80" cond "\b"
#define IF1(cond)       "\x81" cond "\b"
#define ELSIF0(cond)    "\x84" cond "\b"
#define ELSIF1(cond)    "\x85" cond "\b"
#define ELSE0           ELSIF0("1")
#define ELSE1           ELSIF1("1")
#define END0            "\x88"
#define END1            "\x89"

/* stringified version of the rule separators*/
#define FIXTOK      "\x1"
#define FIXSTR      "\x2"
#define REPTOK      "\x3"
#define REPSTR      "\x4"
#define USRTOK      "\x5"
#define DMPDAT      "\x6"
#define DMPIDAT     "\x7"



// stringified versions of width / spacing
#define WIDE(var, width)  #var "\x90" #width "\b"
#define SPC(width)       WIDE($, width)