/* T2CODES */
declare
	T2$LT LIT '0',
	T2$LE LIT '1',
	T2$NE LIT '2',
	T2$EQ LIT '3',
	T2$GE LIT '4',
	T2$GT LIT '5',
	T2$ROL LIT '6',
	T2$ROR LIT '7',
	T2$SCL LIT '8',
	T2$SCR LIT '9',
	T2$SHL LIT '10',
	T2$SHR LIT '11',
	T2$JMPFALSE LIT '12',
/*	T2$13 LIT '13', */
/*	T2$14 LIT '14', */
/*	T2$15 LIT '15', */
/*	T2$16 LIT '16', */
/*	T2$17 LIT '17', */
	T2$DOUBLE LIT '18',
	T2$ADDB LIT '19',
	T2$MINUSSIGN LIT '20',
	T2$STAR LIT '21',
	T2$SLASH LIT '22',
	T2$MOD LIT '23',
	T2$AND LIT '24',
	T2$OR LIT '25',
	T2$XOR LIT '26',
	T2$BASED LIT '27',
	T2$BYTEINDEX LIT '28',
	T2$WORDINDEX LIT '29',
	T2$MEMBER LIT '30',
	T2$UNARYMINUS LIT '31',
	T2$NOT LIT '32',
	T2$LOW LIT '33',
	T2$HIGH LIT '34',
	T2$ADDRESSOF LIT '35',
	T2$PLUS LIT '36',
	T2$MINUS LIT '37',
/*	T2$38 LIT '38', */
/*	T2$39 LIT '39', */
/*	T2$40 LIT '40', */
/*	T2$41 LIT '41', */
/*	T2$42 LIT '42', */
/*	T2$43 LIT '43', */
/*	T2$44 LIT '44', */
/*	T2$45 LIT '45', */
/*	T2$46 LIT '46', */
/*	T2$47 LIT '47', */
/*	T2$48 LIT '48', */
/*	T2$49 LIT '49', */
/*	T2$50 LIT '50', */
/*	T2$51 LIT '51', */
/*	T2$52 LIT '52', */
/*	T2$53 LIT '53', */
/*	T2$54 LIT '54', */
/*	T2$55 LIT '55', */
/*	T2$56 LIT '56', */
	T2$TIME LIT '57',
	T2$STKBARG LIT '58',
	T2$STKWARG LIT '59',
	T2$DEC LIT '60',
	T2$STORE LIT '61',
	T2$OUTPUT LIT '62',
	T2$63 LIT '63',
	T2$STKARG LIT '64',
	T2$65 LIT '65', 
/*	T2$66 LIT '66', */
/*	T2$67 LIT '67', */
/*	T2$68 LIT '68', */
	T2$MOVE LIT '69',
/*	T2$70 LIT '70', */
	T2$RETURNBYTE LIT '71',
	T2$RETURNWORD LIT '72',
	T2$RETURN LIT '73',
/*	T2$74 LIT '74', */
/*	T2$75 LIT '75', */
/*	T2$76 LIT '76', */
/*	T2$77 LIT '77', */
/*	T2$78 LIT '78', */
/*	T2$79 LIT '79', */
/*	T2$80 LIT '80', */
/*	T2$81 LIT '81', */
/*	T2$82 LIT '82', */
/*	T2$83 LIT '83', */
/*	T2$84 LIT '84', */
/*	T2$85 LIT '85', */
/*	T2$86 LIT '86', */
/*	T2$87 LIT '87', */
/*	T2$88 LIT '88', */
/*	T2$89 LIT '89', */
/*	T2$90 LIT '90', */
/*	T2$91 LIT '91', */
/*	T2$92 LIT '92', */
/*	T2$93 LIT '93', */
/*	T2$94 LIT '94', */
/*	T2$95 LIT '95', */
/*	T2$96 LIT '96', */
/*	T2$97 LIT '97', */
/*	T2$98 LIT '98', */
/*	T2$99 LIT '99', */
/*	T2$100 LIT '100', */
/*	T2$101 LIT '101', */
/*	T2$102 LIT '102', */
/*	T2$103 LIT '103', */
/*	T2$104 LIT '104', */
/*	T2$105 LIT '105', */
/*	T2$106 LIT '106', */
/*	T2$107 LIT '107', */
/*	T2$108 LIT '108', */
/*	T2$109 LIT '109', */
/*	T2$110 LIT '110', */
/*	T2$111 LIT '111', */
/*	T2$112 LIT '112', */
/*	T2$113 LIT '113', */
/*	T2$114 LIT '114', */
/*	T2$115 LIT '115', */
/*	T2$116 LIT '116', */
/*	T2$117 LIT '117', */
/*	T2$118 LIT '118', */
/*	T2$119 LIT '119', */
/*	T2$120 LIT '120', */
/*	T2$121 LIT '121', */
/*	T2$122 LIT '122', */
/*	T2$123 LIT '123', */
/*	T2$124 LIT '124', */
/*	T2$125 LIT '125', */
/*	T2$126 LIT '126', */
/*	T2$127 LIT '127', */
/*	T2$128 LIT '128', */
/*	T2$129 LIT '129', */
	T2$ADDW LIT '130',
	T2$BEGMOVE LIT '131',
	T2$CALL LIT '132',
	T2$CALLVAR LIT '133',
/*	T2$134 LIT '134', */
	T2$PROCEDURE LIT '135',
	T2$LOCALLABEL LIT '136',
	T2$CASELABEL LIT '137',
	T2$LABELDEF LIT '138',
	T2$INPUT LIT '139',
	T2$GO$TO LIT '140',
	T2$JMP LIT '141',
	T2$JNC LIT '142',
	T2$JNZ LIT '143',
	T2$SIGN LIT '144',
	T2$ZERO LIT '145',
	T2$PARITY LIT '146',
	T2$CARRY LIT '147',
	T2$DISABLE LIT '148',
	T2$ENABLE LIT '149',
	T2$HALT LIT '150',
	T2$STMTCNT LIT '151',
	T2$LINEINFO LIT '152',
	T2$MODULE LIT '153',
	T2$SYNTAXERROR LIT '154',
	T2$TOKENERROR LIT '155',
	T2$EOF LIT '156',
	T2$LIST LIT '157',
	T2$NOLIST LIT '158',
	T2$CODE LIT '159',
	T2$NOCODE LIT '160',
	T2$EJECT LIT '161',
	T2$INCLUDE LIT '162',
	T2$ERROR LIT '163',
/*	T2$164 LIT '164', */
/*	T2$165 LIT '165', */
/*	T2$166 LIT '166', */
/*	T2$167 LIT '167', */
/*	T2$168 LIT '168', */
/*	T2$169 LIT '169', */
/*	T2$170 LIT '170', */
/*	T2$171 LIT '171', */
	T2$VARIABLE LIT '172',
	T2$NUMBER LIT '173',
	T2$BIGNUMBER LIT '174',
/*	T2$175 LIT '175', */
/*	T2$176 LIT '176', */
/*	T2$177 LIT '177', */
/*	T2$178 LIT '178', */
/*	T2$179 LIT '179', */
/*	T2$180 LIT '180', */
	T2$STACKPTR LIT '181',
	T2$SEMICOLON LIT '182',
	T2$OPTBACKREF LIT '183',
	T2$CASE LIT '184',
	T2$ENDCASE LIT '185',
	T2$ENDPROC LIT '186',
	T2$LENGTH LIT '187',
	T2$LAST LIT '188',
	T2$SIZE LIT '189',
	T2$BEGCALL LIT '190',
/*	T2$191 LIT '191', */
/*	T2$192 LIT '192', */
/*	T2$193 LIT '193', */
/*	T2$194 LIT '194', */
/*	T2$195 LIT '195', */
/*	T2$196 LIT '196', */
/*	T2$197 LIT '197', */
/*	T2$198 LIT '198', */
/*	T2$199 LIT '199', */
/*	T2$200 LIT '200', */
/*	T2$201 LIT '201', */
/*	T2$202 LIT '202', */
/*	T2$203 LIT '203', */
/*	T2$204 LIT '204', */
/*	T2$205 LIT '205', */
/*	T2$206 LIT '206', */
/*	T2$207 LIT '207', */
/*	T2$208 LIT '208', */
/*	T2$209 LIT '209', */
/*	T2$210 LIT '210', */
/*	T2$211 LIT '211', */
/*	T2$212 LIT '212', */
/*	T2$213 LIT '213', */
/*	T2$214 LIT '214', */
/*	T2$215 LIT '215', */
/*	T2$216 LIT '216', */
/*	T2$217 LIT '217', */
/*	T2$218 LIT '218', */
/*	T2$219 LIT '219', */
/*	T2$220 LIT '220', */
/*	T2$221 LIT '221', */
/*	T2$222 LIT '222', */
/*	T2$223 LIT '223', */
/*	T2$224 LIT '224', */
/*	T2$225 LIT '225', */
/*	T2$226 LIT '226', */
/*	T2$227 LIT '227', */
/*	T2$228 LIT '228', */
/*	T2$229 LIT '229', */
/*	T2$230 LIT '230', */
/*	T2$231 LIT '231', */
/*	T2$232 LIT '232', */
/*	T2$233 LIT '233', */
/*	T2$234 LIT '234', */
/*	T2$235 LIT '235', */
/*	T2$236 LIT '236', */
/*	T2$237 LIT '237', */
/*	T2$238 LIT '238', */
/*	T2$239 LIT '239', */
/*	T2$240 LIT '240', */
/*	T2$241 LIT '241', */
/*	T2$242 LIT '242', */
/*	T2$243 LIT '243', */
/*	T2$244 LIT '244', */
/*	T2$245 LIT '245', */
/*	T2$246 LIT '246', */
/*	T2$247 LIT '247', */
/*	T2$248 LIT '248', */
/*	T2$249 LIT '249', */
/*	T2$250 LIT '250', */
/*	T2$251 LIT '251', */
/*	T2$252 LIT '252', */
/*	T2$253 LIT '253', */
	T2$254 LIT '254';
