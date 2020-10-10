#include <stdint.h>
/*
 * this is an expanded version of the original tables in ml80
 * to show the match / shift / reduce elements
 * it compiles to the same binary
 */
#define M(s)	(0x1000 + s)
#define S(s)	(0x2000 + s)
#define R(s)	(0x3000 + s)


#define YERROR 0x100
#define IDENTIFIER	0x101
#define NUMBER	0x102
#define STRING	0x103
#define ASL	0x104
#define TT_ADC	0x105
#define TT_SBB	0x106
#define TT_SWAP	0x107
#define TT_RRC	0x108
#define TT_RLC	0x109
#define TT_CMP	0x10a
#define BSLASH	0x5c
#define TT_XOR	0x10b
#define TT_EOF	0x10c
#define TT_DECLARE 0x10d	
#define TT_INITIAL	0x10e
#define TT_END	0x10f
#define TT_IF	0x110
#define TT_THEN	0x111
#define TT_ELSE	0x112
#define TT_ZERO	0x113
#define TT_CY	0x114
#define TT_PY	0x115
#define TT_ODD	0x116
#define TT_EVEN	0x117
#define TT_PLUS	0x118
#define TT_MINUS	0x119
#define TT_DATA	0x11a
#define TT_BYTE	0x11b
#define TT_LABEL	0x11c
#define TT_EXTERNAL	0x11d
#define TT_COMMON	0x11e
#define TT_DO	0x11f
#define TT_BY	0x120
#define TT_WHILE	0x121
#define TT_CASE	0x122
#define	TT_PROCEDURE 	0x123
#define TT_RETURN	0x124
#define TT_CALL	0x125
#define TT_GOTO	0x126
#define TT_HL	0x127
#define TT_UNTIL	0x128
#define TT_REPEAT	0x129
#define TT_HALT	0x12a
#define TT_NOP	0x12b
#define TT_DISABLE	0x12c
#define TT_ENABLE	0x12d
#define TT_BC	0x12e
#define TT_DE	0x12f
#define TT_SP	0x130
#define TT_STACK	0x131
#define TT_PSW	0x132
#define TT_IN	0x133
#define TT_OUT	0x134


uint16_t lract[] = {
    0,
/*  0*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_DECLARE),	S(20),		M(TT_IF),	S(24),
    M(TT_CY),	S(50),		M(TT_DO),	S(52),		M(TT_RETURN),	S(23),
    M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),		M('M'),		S(49),
    M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),		M(TT_HALT),	S(29),
    M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),		M(TT_ENABLE),	S(32),
    M(TT_BC),	S(43),		M(TT_DE),	S(44),		M(TT_SP),	S(46),
    M(TT_STACK),	S(47),		M(TT_PSW),	S(48),		M(TT_IN),	S(53),
    M(TT_OUT),	S(54),		0,
/*  1*/	M(0),		0x4000,		0,
/*  2*/	M(';'),		S(55),		0,
/*  3*/	R(2),
/*  4*/	R(4),
/*  5*/	R(5),
/*  6*/	R(6),
/*  7*/	M(','),		S(56),		R(7),
/*  8*/	R(8),
/*  9*/	R(9),
/* 10*/	R(10),
/* 11*/	R(11),
/* 12*/	R(12),
/* 13*/	R(13),
/* 14*/	R(14),
/* 15*/	R(15),
/* 16*/	R(16),
/* 17*/	R(17),
/* 18*/	M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),		M(ASL),	S(42),
    M(TT_DECLARE),	S(20),		M(TT_IF),	S(24),		M(TT_CY),	S(50),
    M(TT_DO),	S(52),		M(TT_PROCEDURE),S(59),		M(TT_RETURN),	S(23),
    M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),		M('M'),		S(49),
    M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),		M(TT_HALT),	S(29),
    M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),		M(TT_ENABLE),	S(32),
    M(TT_BC),	S(43),		M(TT_DE),	S(44),		M(TT_SP),	S(46),
    M(TT_STACK),	S(47),		M(TT_PSW),	S(48),		M(TT_IN),	S(53),
    M(TT_OUT),	S(54),		0,
/* 19*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_DECLARE),	S(20),		M(TT_IF),	S(24),
    M(TT_CY),	S(50),		M(TT_DO),	S(52),		M(TT_RETURN),	S(23),
    M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),		M('M'),		S(49),
    M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),		M(TT_HALT),	S(29),
    M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),		M(TT_ENABLE),	S(32),
    M(TT_BC),	S(43),		M(TT_DE),	S(44),		M(TT_SP),	S(46),
    M(TT_STACK),	S(47),		M(TT_PSW),	S(48),		M(TT_IN),	S(53),
    M(TT_OUT),	S(54),		0,
/* 20*/	
/* 56*/	M(IDENTIFIER),	S(66),		M('('),		S(69),		0,
/* 21*/	M(';'),		S(70),		0,
/* 22*/	
/* 28*/	
/* 76*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_DECLARE),	S(20),		M(TT_IF),	S(24),
    M(TT_CY),	S(50),		M(TT_DO),	S(52),		M(TT_RETURN),	S(23),
    M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),		M('M'),		S(49),
    M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),		M(TT_HALT),	S(29),
    M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),		M(TT_ENABLE),	S(32),
    M(TT_BC),	S(43),		M(TT_DE),	S(44),		M(TT_SP),	S(46),
    M(TT_STACK),	S(47),		M(TT_PSW),	S(48),		M(TT_IN),	S(53),
    M(TT_OUT),	S(54),		0,
/* 23*/	R(90),
/* 24*/	M('('),		S(76),		M('!'),		S(78),		M(TT_ZERO),	S(79),
    M(TT_CY),	S(80),		M(TT_PY),	S(81),		M(TT_PLUS),	S(82),
    M(TT_MINUS),	S(83),		0,
/* 25*/	M(IDENTIFIER),	S(84),		M(NUMBER),	S(86),		0,
/* 26*/	M(IDENTIFIER),	S(88),		M(NUMBER),	S(89),		0,
/* 27*/	M('M'),		S(90),		R(103),
/* 29*/	R(107),
/* 30*/	R(108),
/* 31*/	R(109),
/* 32*/	R(110),
/* 33*/	M(TT_SWAP),	S(93),		M(TT_CMP),	S(92),		M('='),		S(94),
    0,
/* 34*/	R(28),
/* 35*/	M(','),		S(95),		R(29),
/* 36*/	M('('),		S(97),		M(':'),		S(96),		R(154),
/* 37*/	M(':'),		S(98),		0,
/* 38*/	M(IDENTIFIER),	S(105),		M(ASL),	S(42),		M(';'),		R(73),
    M(TT_CY),	S(50),		M(TT_CASE),	S(102),		M('M'),		S(49),
    M(TT_HL),	S(45),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		R(26),
/* 39*/	M(';'),		S(106),		M('('),		S(109),		0,
/* 40*/	R(98),
/* 41*/	M(';'),		S(110),		0,
/* 42*/	R(130),
/* 43*/	R(131),
/* 44*/	R(132),
/* 45*/	R(133),
/* 46*/	R(134),
/* 47*/	R(135),
/* 48*/	R(136),
/* 49*/	M('('),		S(111),		0,
/* 50*/	R(138),
/* 51*/	M('='),		S(112),		0,
/* 52*/	R(77),
/* 53*/	M('('),		S(113),		0,
/* 54*/	M('('),		S(114),		0,
/* 55*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_EOF),	S(115),		M(TT_DECLARE),	S(20),
    M(TT_IF),	S(24),		M(TT_CY),	S(50),		M(TT_DO),	S(52),
    M(TT_RETURN),	S(23),		M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),
    M('M'),		S(49),		M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),
    M(TT_HALT),	S(29),		M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),
    M(TT_ENABLE),	S(32),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		0,
/* 57*/	R(18),
/* 58*/	R(19),
/* 59*/	R(86),
/* 60*/	R(32),
/* 61*/	
/*212*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_DECLARE),	S(20),		M(TT_IF),	S(24),
    M(TT_CY),	S(50),		M(TT_DO),	S(52),		M(TT_RETURN),	S(23),
    M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),		M('M'),		S(49),
    M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),		M(TT_HALT),	S(29),
    M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),		M(TT_ENABLE),	S(32),
    M(TT_BC),	S(43),		M(TT_DE),	S(44),		M(TT_SP),	S(46),
    M(TT_STACK),	S(47),		M(TT_PSW),	S(48),		M(TT_IN),	S(53),
    M(TT_OUT),	S(54),		0,
/* 62*/	M(TT_ELSE),	S(119),		R(4),
/* 63*/	R(20),
/* 64*/	M(TT_INITIAL),	S(122),		R(53),
/* 65*/	M('('),		S(125),		M(TT_BYTE),	S(124),		M(TT_LABEL),	S(126),
    M(TT_EXTERNAL),	S(127),		M(TT_COMMON),	S(128),		0,
/* 66*/	M(TT_DATA),	S(131),		R(65),
/* 67*/	M(NUMBER),	S(132),		0,
/* 68*/	M(IDENTIFIER),	S(133),		0,
/* 69*/	R(67),
/* 70*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_DECLARE),	S(20),		M(TT_END),	S(136),
    M(TT_IF),	S(24),		M(TT_CY),	S(50),		M(TT_DO),	S(52),
    M(TT_RETURN),	S(23),		M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),
    M('M'),		S(49),		M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),
    M(TT_HALT),	S(29),		M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),
    M(TT_ENABLE),	S(32),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		0,
/* 71*/	M(';'),		S(138),		0,
/* 72*/	M(TT_THEN),	S(139),		0,
/* 73*/	M('&'),		S(140),		M('\\'),		S(141),		M(TT_RETURN),	S(142),
    M(TT_CALL),	S(143),		M(TT_GOTO),	S(144),		R(38),
/* 74*/	
/* 75*/	M('('),		S(76),		M('!'),		S(78),		M(TT_ZERO),	S(79),
    M(TT_CY),	S(80),		M(TT_PY),	S(81),		M(TT_PLUS),	S(82),
    M(TT_MINUS),	S(83),		0,
/* 77*/	R(44),
/* 78*/	M(TT_ZERO),	S(148),		M(TT_CY),	S(149),		0,
/* 79*/	R(46),
/* 80*/	R(48),
/* 81*/	M(TT_ODD),	S(150),		M(TT_EVEN),	S(151),		0,
/* 82*/	R(51),
/* 83*/	R(52),
/* 84*/	M('('),		S(152),		R(92),
/* 85*/	R(93),
/* 86*/	R(94),
/* 87*/	
/*121*/	
/*130*/	M(NUMBER),	S(155),		M(STRING),	S(154),		M('-'),		S(156),
    M('.'),		S(157),		0,
/* 88*/	R(100),
/* 89*/	R(101),
/* 90*/	M('('),		S(158),		0,
/* 91*/	M(';'),		S(159),		0,
/* 92*/	
/*177*/	
/*223*/	M(NUMBER),	S(155),		M(STRING),	S(154),		M(ASL),	S(42),
    M('('),		S(164),		M(TT_CY),	S(50),		M('M'),		S(165),
    M(TT_HL),	S(45),		M('-'),		S(156),		M('.'),		S(157),
    M(TT_BC),	S(43),		M(TT_DE),	S(44),		M(TT_SP),	S(46),
    M(TT_STACK),	S(47),		M(TT_PSW),	S(48),		0,
/* 93*/	
/*112*/	M(ASL),	S(42),		M('('),		S(164),		M(TT_CY),	S(50),
    M('M'),		S(165),		M(TT_HL),	S(45),		M(TT_BC),	S(43),
    M(TT_DE),	S(44),		M(TT_SP),	S(46),		M(TT_STACK),	S(47),
    M(TT_PSW),	S(48),		0,
/* 94*/	M(IDENTIFIER),	S(105),		M(NUMBER),	S(155),		M(STRING),	S(154),
    M(ASL),	S(42),		M(TT_RRC),	S(175),		M(TT_RLC),	S(176),
    M('('),		S(164),		M('!'),		S(173),		M(TT_CY),	S(50),
    M('M'),		S(49),		M(TT_HL),	S(45),		M('-'),		S(156),
    M('.'),		S(157),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M('<'),		S(171),		M('>'),		S(172),		M('#'),		S(174),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		0,
/* 95*/	M(TT_ADC),	S(183),		M(TT_SBB),	S(184),		M(TT_XOR),	S(182),
    M('&'),		S(180),		M('\\'),		S(181),		M('-'),		S(179),
    M('+'),		S(178),		0,
/* 96*/	R(30),
/* 97*/	M(NUMBER),	S(185),		0,
/* 98*/	R(31),
/* 99*/	R(74),
/*100*/	R(75),
/*101*/	M(TT_BY),	S(188),		M(TT_WHILE),	S(189),		0,
/*102*/	M(ASL),	S(42),		M(TT_CY),	S(50),		M('M'),		S(165),
    M(TT_HL),	S(45),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    0,
/*103*/	R(27),
/*104*/	M('='),		S(94),		0,
/*105*/	M('('),		S(97),		R(154),
/*106*/	R(84),
/*107*/	M(';'),		S(191),		0,
/*108*/	M(IDENTIFIER),	S(192),		0,
/*109*/	R(88),
/*110*/	R(106),
/*111*/	M(NUMBER),	S(155),		M(STRING),	S(154),		M(TT_HL),	S(193),
    M('-'),		S(156),		M('.'),		S(157),		M(TT_BC),	S(194),
    M(TT_DE),	S(195),		0,
/*113*/	M(NUMBER),	S(198),		0,
/*114*/	M(NUMBER),	S(199),		0,
/*115*/	R(1),
/*116*/	R(3),
/*117*/	R(21),
/*118*/	R(33),
/*119*/	R(35),
/*120*/	R(61),
/*122*/	M('('),		S(201),		0,
/*123*/	R(54),
/*124*/	R(59),
/*125*/	R(69),
/*126*/	R(62),
/*127*/	R(63),
/*128*/	R(64),
/*129*/	R(55),
/*131*/	M('('),		S(203),		0,
/*132*/	M(')'),		S(204),		0,
/*133*/	M(','),		S(206),		M(')'),		S(205),		0,
/*134*/	R(72),
/*135*/	R(76),
/*136*/	M(IDENTIFIER),	S(207),		R(23),
/*137*/	M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),		M(ASL),	S(42),
    M(TT_DECLARE),	S(20),		M(TT_END),	S(136),		M(TT_IF),	S(24),
    M(TT_CY),	S(50),		M(TT_DO),	S(52),		M(TT_PROCEDURE),	S(59),
    M(TT_RETURN),	S(23),		M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),
    M('M'),		S(49),		M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),
    M(TT_HALT),	S(29),		M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),
    M(TT_ENABLE),	S(32),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		0,
/*138*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_DECLARE),	S(20),		M(TT_END),	S(136),
    M(TT_IF),	S(24),		M(TT_CY),	S(50),		M(TT_DO),	S(52),
    M(TT_RETURN),	S(23),		M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),
    M('M'),		S(49),		M(TT_HL),	S(45),		M(TT_REPEAT),	S(41),
    M(TT_HALT),	S(29),		M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),
    M(TT_ENABLE),	S(32),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		0,
/*139*/	R(34),
/*140*/	R(39),
/*141*/	R(41),
/*142*/	R(91),
/*143*/	R(99),
/*144*/	R(104),
/*145*/	M('&'),		S(210),		R(36),
/*146*/	M('\\'),		S(211),		R(37),
/*147*/	M(';'),		S(212),		M(')'),		S(213),		0,
/*148*/	R(45),
/*149*/	R(47),
/*150*/	R(49),
/*151*/	R(50),
/*152*/	R(96),
/*153*/	M(','),		S(215),		M(')'),		S(214),		0,
/*154*/	R(124),
/*155*/	R(125),
/*156*/	M(NUMBER),	S(216),		0,
/*157*/	M(IDENTIFIER),	S(217),		M(STRING),	S(218),		0,
/*158*/	M(TT_HL),	S(219),		0,
/*159*/	M(YERROR),	S(6),		M(IDENTIFIER),	S(36),		M(NUMBER),	S(37),
    M(ASL),	S(42),		M(TT_DECLARE),	S(20),		M(TT_IF),	S(24),
    M(TT_CY),	S(50),		M(TT_DO),	S(52),		M(TT_RETURN),	S(23),
    M(TT_CALL),	S(40),		M(TT_GOTO),	S(27),		M('M'),		S(49),
    M(TT_HL),	S(45),		M(TT_UNTIL),	S(220),		M(TT_REPEAT),	S(41),
    M(TT_HALT),	S(29),		M(TT_NOP),	S(30),		M(TT_DISABLE),	S(31),
    M(TT_ENABLE),	S(32),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		0,
/*160*/	R(111),
/*161*/	R(122),
/*162*/	R(123),
/*163*/	R(114),
/*164*/	M(ASL),	S(42),		M(TT_CY),	S(50),		M('M'),		S(165),
    M(TT_HL),	S(45),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    0,
/*165*/	M('('),		S(222),		0,
/*166*/	R(112),
/*167*/	M(TT_ADC),	S(183),		M(TT_SBB),	S(184),		M(TT_XOR),	S(182),
    M('&'),		S(180),		M('\\'),		S(181),		M('-'),		S(179),
    M('+'),		S(178),		R(118),
/*168*/	M(IDENTIFIER),	S(105),		M(NUMBER),	S(155),		M(STRING),	S(154),
    M(ASL),	S(42),		M('('),		S(164),		M(TT_CY),	S(50),
    M('M'),		S(49),		M(TT_HL),	S(45),		M('-'),		S(156),
    M('.'),		S(157),		M(TT_BC),	S(43),		M(TT_DE),	S(44),
    M(TT_SP),	S(46),		M(TT_STACK),	S(47),		M(TT_PSW),	S(48),
    M(TT_IN),	S(53),		M(TT_OUT),	S(54),		0,
/*169*/	R(120),
/*170*/	R(121),
/*171*/	R(146),
/*172*/	R(147),
/*173*/	R(148),
/*174*/	R(149),
/*175*/	R(150),
/*176*/	R(151),
/*178*/	R(139),
/*179*/	R(140),
/*180*/	R(141),
/*181*/	R(142),
/*182*/	R(143),
/*183*/	R(144),
/*184*/	R(145),
/*185*/	M(')'),		S(226),		0,
/*186*/	M(IDENTIFIER),	S(105),		M(ASL),	S(42),		M(TT_CY),	S(50),
    M('M'),		S(49),		M(TT_HL),	S(45),		M(TT_BC),	S(43),
    M(TT_DE),	S(44),		M(TT_SP),	S(46),		M(TT_STACK),	S(47),
    M(TT_PSW),	S(48),		M(TT_IN),	S(53),		M(TT_OUT),	S(54),
    0,
/*187*/	
/*220*/	
/*249*/	M('('),		S(76),		M('!'),		S(78),		M(TT_ZERO),	S(79),
    M(TT_CY),	S(80),		M(TT_PY),	S(81),		M(TT_PLUS),	S(82),
    M(TT_MINUS),	S(83),		0,
/*188*/	R(80),
/*189*/	R(81),
/*190*/	R(82),
/*191*/	R(85),
/*192*/	M(','),		S(231),		M(')'),		S(230),		0,
/*193*/	M(')'),		S(232),		0,
/*194*/	M(')'),		S(233),		0,
/*195*/	M(')'),		S(234),		0,
/*196*/	M(')'),		S(235),		0,
/*197*/	R(113),
/*198*/	M(')'),		S(236),		0,
/*199*/	M(')'),		S(237),		0,
/*200*/	M(','),		S(239),		M(')'),		S(238),		0,
/*201*/	R(22),
/*202*/	M(','),		S(241),		M(')'),		S(240),		0,
/*203*/	R(57),
/*204*/	M(TT_BYTE),	S(242),		0,
/*205*/	R(66),
/*206*/	R(68),
/*207*/	R(24),
/*208*/	R(25),
/*209*/	R(83),
/*210*/	R(40),
/*211*/	R(42),
/*213*/	M('!'),		S(78),		M(TT_ZERO),	S(79),		M(TT_CY),	S(80),
    M(TT_PY),	S(81),		M(TT_PLUS),	S(82),		M(TT_MINUS),	S(83),
    0,
/*214*/	R(95),
/*215*/	R(97),
/*216*/	R(126),
/*217*/	M('('),		S(244),		R(127),
/*218*/	R(129),
/*219*/	M(')'),		S(245),		0,
/*221*/	M(','),		S(95),		M(')'),		S(247),		0,
/*222*/	M(TT_HL),	S(193),		0,
/*224*/	R(117),
/*225*/	R(119),
/*226*/	R(155),
/*227*/	M(TT_WHILE),	S(189),		0,
/*228*/	R(79),
/*229*/	M('&'),		S(140),		M('\\'),		S(141),		R(38),
/*230*/	R(87),
/*231*/	R(89),
/*232*/	R(137),
/*233*/	R(152),
/*234*/	R(153),
/*235*/	R(158),
/*236*/	R(156),
/*237*/	R(157),
/*238*/	R(70),
/*239*/	R(71),
/*240*/	R(56),
/*241*/	R(58),
/*242*/	R(60),
/*243*/	R(43),
/*244*/	M(NUMBER),	S(250),		0,
/*245*/	R(102),
/*246*/	R(105),
/*247*/	R(115),
/*248*/	R(116),
/*250*/	M(')'),		S(252),		0,
/*251*/	R(78),
/*252*/	R(128),
        0};


uint16_t lrpact[] = {
       0,      1,   0x34,   0x37,   0x3A,   0x3B,   0x3C,   0x3D,	/* 0 */
    0x3E,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,	/* 8 */
    0x48,   0x49,   0x4A,   0x4B,   0x7E,  0x0B1,  0x0B6,  0x0B9,	/* 16 */
   0x0EC,  0x0ED,  0x0FC,  0x101,  0x106,  0x0B9,  0x109,  0x10A,	/* 24 */
   0x10B,  0x10C,  0x10D,  0x114,  0x115,  0x118,  0x11D,  0x120,	/* 32 */
   0x13D,  0x142,  0x143,  0x146,  0x147,  0x148,  0x149,  0x14A,	/* 40 */
   0x14B,  0x14C,  0x14D,  0x150,  0x151,  0x154,  0x155,  0x158,	/* 48 */
   0x15B,  0x0B1,  0x190,  0x191,  0x192,  0x193,  0x194,  0x1C7,	/* 56 */
   0x1CA,  0x1CB,  0x1CE,  0x1D9,  0x1DC,  0x1DF,  0x1E2,  0x1E3,	/* 64 */
   0x218,  0x21B,  0x21E,  0x229,  0x229,  0x0B9,  0x238,  0x239,	/* 72 */
   0x23E,  0x23F,  0x240,  0x245,  0x246,  0x247,  0x24A,  0x24B,	/* 80 */
   0x24C,  0x255,  0x256,  0x257,  0x25A,  0x25D,  0x27A,  0x28F,	/* 88 */
   0x2BE,  0x2CD,  0x2CE,  0x2D1,  0x2D2,  0x2D3,  0x2D4,  0x2D9,	/* 96 */
   0x2EC,  0x2ED,  0x2F0,  0x2F3,  0x2F4,  0x2F7,  0x2FA,  0x2FB,	/* 104 */
   0x2FC,  0x27A,  0x30B,  0x30E,  0x311,  0x312,  0x313,  0x314,	/* 112 */
   0x315,  0x316,  0x24C,  0x317,  0x31A,  0x31B,  0x31C,  0x31D,	/* 120 */
   0x31E,  0x31F,  0x320,  0x24C,  0x321,  0x324,  0x327,  0x32C,	/* 128 */
   0x32D,  0x32E,  0x331,  0x366,  0x39B,  0x39C,  0x39D,  0x39E,	/* 136 */
   0x39F,  0x3A0,  0x3A1,  0x3A4,  0x3A7,  0x3AC,  0x3AD,  0x3AE,	/* 144 */
   0x3AF,  0x3B0,  0x3B1,  0x3B6,  0x3B7,  0x3B8,  0x3BB,  0x3C0,	/* 152 */
   0x3C3,  0x3F8,  0x3F9,  0x3FA,  0x3FB,  0x3FC,  0x40F,  0x412,	/* 160 */
   0x413,  0x422,  0x445,  0x446,  0x447,  0x448,  0x449,  0x44A,	/* 168 */
   0x44B,  0x44C,  0x25D,  0x44D,  0x44E,  0x44F,  0x450,  0x451,	/* 176 */
   0x452,  0x453,  0x454,  0x457,  0x470,  0x47F,  0x480,  0x481,	/* 184 */
   0x482,  0x483,  0x488,  0x48B,  0x48E,  0x491,  0x494,  0x495,	/* 192 */
   0x498,  0x49B,  0x4A0,  0x4A1,  0x4A6,  0x4A7,  0x4AA,  0x4AB,	/* 200 */
   0x4AC,  0x4AD,  0x4AE,  0x4AF,  0x4B0,  0x194,  0x4B1,  0x4BE,	/* 208 */
   0x4BF,  0x4C0,  0x4C1,  0x4C4,  0x4C5,  0x470,  0x4C8,  0x4CD,	/* 216 */
   0x25D,  0x4D0,  0x4D1,  0x4D2,  0x4D3,  0x4D6,  0x4D7,  0x4DC,	/* 224 */
   0x4DD,  0x4DE,  0x4DF,  0x4E0,  0x4E1,  0x4E2,  0x4E3,  0x4E4,	/* 232 */
   0x4E5,  0x4E6,  0x4E7,  0x4E8,  0x4E9,  0x4EA,  0x4ED,  0x4EE,	/* 240 */
   0x4EF,  0x4F0,  0x470,  0x4F1,  0x4F4,  0x4F5,      0};		/* 248 */


uint8_t llr1[160] = {
        0,	1,	2,	2,	3,	3,	3,	4,	4,	4,	/* 0 */
        4,	4,	4,	4,	4,	4,	4,	4,	4,	5,	/* 10 */
        6,	6,	19,	20,	20,	20,	21,	21,	16,	16,	/* 20 */
        17,	17,	5,	5,	24,	25,	26,	26,	26,	27,	/* 30 */
        27,	29,	29,	28,	28,	30,	30,	30,	30,	30,	/* 40 */
        30,	30,	30,	18,	18,	18,	34,	35,	35,	31,	/* 50 */
        31,	31,	33,	33,	33,	32,	32,	39,	39,	37,	/* 60 */
        38,	19,	7,	40,	40,	40,	40,	41,	42,	42,	/* 70 */
        44,	45,	43,	8,	47,	47,	48,	49,	50,	50,	/* 80 */
        9,	9,	10,	10,	10,	52,	53,	53,	51,	51,	/* 90 */
        11,	11,	11,	54,	54,	12,	55,	13,	13,	13,	/* 100 */
        13,	14,	15,	22,	57,	57,	23,	23,	23,	23,	/* 110 */
        59,	59,	56,	56,	36,	36,	36,	36,	36,	36,	/* 120 */
        46,	46,	46,	46,	46,	46,	46,	46,	46,	60,	/* 130 */
        60,	60,	60,	60,	60,	60,	61,	61,	61,	61,	/* 140 */
        61,	61,	58,	58,	58,	58,	58,	58,	58,	0};	/* 150 */


uint8_t llr2[160] = {
        0,	3,	1,	3,	1,	1,	1,	1,	1,	1,	/* 0 */
        1,	1,	1,	1,	1,	1,	1,	1,	2,	2,	/* 10 */
        2,	3,	2,	1,	2,	2,	0,	1,	1,	1,	/* 20 */
        2,	2,	2,	3,	3,	2,	2,	2,	1,	2,	/* 30 */
        3,	2,	3,	4,	1,	2,	1,	2,	1,	2,	/* 40 */
        2,	1,	1,	1,	2,	2,	3,	2,	3,	2,	/* 50 */
        4,	2,	1,	1,	1,	1,	3,	1,	3,	2,	/* 60 */
        3,	3,	3,	1,	2,	2,	3,	1,	5,	3,	/* 70 */
        1,	1,	2,	4,	2,	3,	2,	3,	1,	3,	/* 80 */
        1,	3,	2,	2,	2,	3,	2,	3,	1,	3,	/* 90 */
        2,	2,	5,	1,	3,	5,	2,	1,	1,	1,	/* 100 */
        1,	3,	3,	3,	1,	3,	5,	4,	3,	4,	/* 110 */
        1,	1,	1,	1,	1,	1,	2,	2,	5,	2,	/* 120 */
        1,	1,	1,	1,	1,	1,	1,	4,	1,	1,	/* 130 */
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	/* 140 */
        1,	1,	4,	4,	1,	4,	4,	4,	4,	0};	/* 150 */

uint8_t lrpgo[63] = {
      0,     1,     3,  0x0B,  0x1B,  0x23,  0x29,  0x2B,	/* 0 */
   0x2D,  0x2F,  0x31,  0x33,  0x35,  0x37,  0x39,  0x3B,	/* 8 */
   0x3D,  0x43,  0x4B,  0x4F,  0x51,  0x57,  0x59,  0x5B,	/* 16 */
   0x5F,  0x61,  0x63,  0x6B,  0x6D,  0x75,  0x77,  0x7B,	/* 24 */
   0x7D,  0x7F,  0x81,  0x83,  0x85,  0x8F,  0x91,  0x93,	/* 32 */
   0x95,  0x97,  0x99,  0x9B,  0x9D,  0x9F, 0x0A3, 0x0BB,	/* 40 */
  0x0BD, 0x0BF, 0x0C1, 0x0C3, 0x0C5, 0x0C7, 0x0C9, 0x0CB,	/* 48 */
  0x0CD, 0x0D5, 0x0DB, 0x0E1, 0x0E5, 0x0E9,     0};			/* 56 */

uint8_t lrgo[236] = {
/*  0*/	0,
/*  1*/	0,	1,
/*  2*/	22,	71,	28,	91,	76,	147,	0,	2,
/*  3*/	19,	60,	55,	116,	61,	118,	70,	135,
        138,	116,	159,	116,	212,	116,	0,	3,
/*  4*/	18,	57,	19,	62,	137,	57,	0,	4,
/*  5*/	18,	58,	137,	58,	0,	5,
/*  6*/	0,	7,
/*  7*/	0,	8,
/*  8*/	0,	9,
/*  9*/	0,	10,
/* 10*/	0,	11,
/* 11*/	0,	12,
/* 12*/	0,	13,
/* 13*/	0,	14,
/* 14*/	0,	15,
/* 15*/	0,	16,
/* 16*/	38,	103,	186,	227,	0,	17,
/* 17*/	70,	137,	137,	137,	138,	137,	0,	18,
/* 18*/	56,	117,	0,	63,
/* 19*/	0,	121,
/* 20*/	137,	208,	138,	209,	0,	134,
/* 21*/	0,	101,
/* 22*/	0,	34,
/* 23*/	164,	221,	0,	35,
/* 24*/	0,	19,
/* 25*/	0,	61,
/* 26*/	187,	228,	220,	246,	249,	251,	0,	72,
/* 27*/	0,	74,
/* 28*/	24,	73,	74,	145,	75,	146,	0,	229,
/* 29*/	0,	75,
/* 30*/	213,	243,	0,	77,
/* 31*/	0,	64,
/* 32*/	0,	65,
/* 33*/	0,	123,
/* 34*/	0,	129,
/* 35*/	0,	130,
/* 36*/	87,	153,	111,	196,	121,	200,	130,	202,
        0,	162,
/* 37*/	0,	67,
/* 38*/	0,	120,
/* 39*/	0,	68,
/* 40*/	0,	21,
/* 41*/	0,	38,
/* 42*/	0,	99,
/* 43*/	0,	100,
/* 44*/	0,	186,
/* 45*/	227,	249,	0,	187,
/* 46*/	38,	104,	92,	163,	93,	163,	94,	163,
        102,	190,	112,	163,	164,	104,	168,	163,
        177,	163,	186,	104,	223,	163,	0,	33,
/* 47*/	0,	22,
/* 48*/	0,	39,
/* 49*/	0,	107,
/* 50*/	0,	108,
/* 51*/	0,	25,
/* 52*/	0,	85,
/* 53*/	0,	87,
/* 54*/	0,	26,
/* 55*/	0,	28,
/* 56*/	92,	160,	177,	225,	223,	248,	0,	170,
/* 57*/	93,	166,	112,	197,	0,	161,
/* 58*/	94,	169,	168,	169,	0,	51,
/* 59*/	168,	224,	0,	167,
/* 60*/	167,	223,	0,	177,
/* 61*/	0,	168,
        0};

    
