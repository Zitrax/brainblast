/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)

Grinning Lizard Utilities. Note that software that uses the 
utility package (including Lilith3D and Kyra) have more restrictive
licences which applies to code outside of the utility package.


This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/


#include "gldebug.h"


// A table of prime numbers, from about 30 to about 300,000. About 1/10th
// of the prime numbers in this space.

const unsigned gPrime[] = 
{
	31,
	137,
	257,
	389,
	523,
	661,
	823,
	977,
	1117,
	1289,
	1453,
	1607,
	1777,
	1951,
	2113,
	2293,
	2447,
	2659,
	2797,
	2971,
	3187,
	3359,
	3539,
	3701,
	3889,
	4073,
	4253,
	4451,
	4643,
	4817,
	5009,
	5209,
	5417,
	5581,
	5783,
	5953,
	6163,
	6337,
	6553,
	6737,
	6947,
	7127,
	7333,
	7547,
	7717,
	7927,
	8147,
	8329,
	8563,
	8737,
	8933,
	9137,
	9337,
	9497,
	9721,
	9901,
	10133,
	10313,
	10529,
	10733,
	10957,
	11171,
	11393,
	11617,
	11831,
	12037,
	12241,
	12437,
	12613,
	12823,
	13009,
	13229,
	13457,
	13687,
	13877,
	14083,
	14347,
	14551,
	14747,
	14939,
	15161,
	15349,
	15559,
	15749,
	15959,
	16187,
	16421,
	16649,
	16883,
	17077,
	17321,
	17491,
	17729,
	17939,
	18133,
	18329,
	18539,
	18797,
	19079,
	19309,
	19483,
	19727,
	19949,
	20129,
	20353,
	20563,
	20807,
	21019,
	21227,
	21487,
	21649,
	21863,
	22079,
	22283,
	22541,
	22739,
	22973,
	23167,
	23399,
	23627,
	23831,
	24029,
	24223,
	24481,
	24763,
	24979,
	25229,
	25447,
	25667,
	25903,
	26113,
	26321,
	26561,
	26759,
	26981,
	27239,
	27481,
	27739,
	27919,
	28123,
	28409,
	28603,
	28789,
	29023,
	29243,
	29453,
	29723,
	29989,
	30203,
	30467,
	30697,
	30911,
	31151,
	31327,
	31583,
	31847,
	32083,
	32309,
	32497,
	32713,
	32941,
	33151,
	33377,
	33589,
	33791,
	34033,
	34283,
	34499,
	34703,
	34949,
	35171,
	35423,
	35677,
	35933,
	36151,
	36389,
	36629,
	36821,
	37021,
	37307,
	37511,
	37699,
	37991,
	38239,
	38501,
	38723,
	38953,
	39181,
	39383,
	39659,
	39863,
	40111,
	40361,
	40609,
	40853,
	41113,
	41281,
	41543,
	41761,
	41969,
	42193,
	42403,
	42611,
	42821,
	43049,
	43321,
	43607,
	43853,
	44071,
	44273,
	44543,
	44773,
	44987,
	45281,
	45523,
	45763,
	46027,
	46271,
	46507,
	46727,
	46997,
	47269,
	47497,
	47701,
	47933,
	48187,
	48449,
	48661,
	48871,
	49121,
	49363,
	49559,
	49807,
	50047,
	50263,
	50513,
	50777,
	51043,
	51283,
	51481,
	51691,
	51929,
	52181,
	52453,
	52697,
	52937,
	53149,
	53401,
	53633,
	53887,
	54133,
	54401,
	54581,
	54833,
	55079,
	55339,
	55621,
	55819,
	56041,
	56269,
	56509,
	56737,
	56941,
	57163,
	57383,
	57667,
	57859,
	58111,
	58363,
	58579,
	58897,
	59077,
	59281,
	59509,
	59743,
	60037,
	60271,
	60589,
	60773,
	61031,
	61339,
	61559,
	61781,
	62047,
	62299,
	62563,
	62819,
	63073,
	63347,
	63541,
	63727,
	63997,
	64271,
	64577,
	64817,
	65071,
	65293,
	65543,
	65719,
	65981,
	66271,
	66529,
	66763,
	67021,
	67219,
	67477,
	67709,
	67931,
	68171,
	68483,
	68729,
	68963,
	69233,
	69473,
	69809,
	70051,
	70241,
	70489,
	70753,
	70979,
	71233,
	71413,
	71671,
	71899,
	72103,
	72353,
	72647,
	72889,
	73079,
	73387,
	73637,
	73883,
	74159,
	74377,
	74609,
	74857,
	75133,
	75353,
	75583,
	75797,
	76091,
	76367,
	76603,
	76871,
	77141,
	77359,
	77569,
	77773,
	78049,
	78307,
	78577,
	78839,
	79139,
	79357,
	79613,
	79843,
	80107,
	80317,
	80611,
	80803,
	81031,
	81281,
	81547,
	81761,
	82007,
	82223,
	82487,
	82727,
	83003,
	83257,
	83477,
	83773,
	84059,
	84307,
	84509,
	84787,
	85061,
	85303,
	85571,
	85819,
	86113,
	86311,
	86533,
	86843,
	87107,
	87337,
	87583,
	87793,
	88019,
	88397,
	88681,
	88903,
	89123,
	89399,
	89611,
	89867,
	90067,
	90289,
	90547,
	90841,
	91127,
	91367,
	91583,
	91873,
	92153,
	92369,
	92593,
	92791,
	93053,
	93263,
	93503,
	93827,
	94057,
	94327,
	94547,
	94811,
	95071,
	95267,
	95507,
	95747,
	95987,
	96263,
	96497,
	96779,
	97007,
	97327,
	97571,
	97849,
	98123,
	98407,
	98641,
	98897,
	99109,
	99371,
	99623,
	99859,
	100153,
	100403,
	100669,
	100943,
	101183,
	101419,
	101653,
	101891,
	102103,
	102329,
	102587,
	102877,
	103171,
	103471,
	103723,
	103997,
	104233,
	104513,
	104723,
	104959,
	105263,
	105503,
	105751,
	106031,
	106297,
	106531,
	106751,
	106979,
	107251,
	107609,
	107867,
	108109,
	108347,
	108631,
	108883,
	109103,
	109357,
	109579,
	109843,
	110083,
	110441,
	110641,
	110921,
	111149,
	111443,
	111721,
	111949,
	112207,
	112403,
	112757,
	113017,
	113167,
	113453,
	113761,
	114013,
	114269,
	114593,
	114797,
	115079,
	115327,
	115603,
	115823,
	116041,
	116329,
	116579,
	116881,
	117119,
	117371,
	117659,
	117851,
	118093,
	118399,
	118673,
	118907,
	119179,
	119503,
	119723,
	119963,
	120199,
	120511,
	120739,
	120941,
	121189,
	121441,
	121637,
	121967,
	122203,
	122449,
	122701,
	122957,
	123239,
	123493,
	123727,
	123983,
	124249,
	124493,
	124769,
	125029,
	125287,
	125551,
	125789,
	126037,
	126311,
	126551,
	126839,
	127163,
	127423,
	127669,
	127873,
	128201,
	128413,
	128659,
	128939,
	129127,
	129419,
	129629,
	129937,
	130201,
	130439,
	130651,
	130981,
	131267,
	131507,
	131777,
	132049,
	132329,
	132589,
	132763,
	133069,
	133303,
	133571,
	133843,
	134129,
	134359,
	134677,
	134947,
	135211,
	135449,
	135649,
	135913,
	136217,
	136417,
	136621,
	136883,
	137153,
	137393,
	137659,
	137947,
	138197,
	138451,
	138679,
	138967,
	139303,
	139511,
	139787,
	140057,
	140321,
	140557,
	140779,
	141067,
	141301,
	141601,
	141793,
	142049,
	142319,
	142591,
	142873,
	143197,
	143489,
	143711,
	143999,
	144299,
	144569,
	144829,
	145091,
	145417,
	145633,
	145879,
	146099,
	146381,
	146669,
	146933,
	147209,
	147419,
	147689,
	147977,
	148249,
	148531,
	148793,
	149033,
	149251,
	149491,
	149749,
	150001,
	150223,
	150517,
	150797,
	151027,
	151289,
	151537,
	151733,
	152017,
	152267,
	152533,
	152809,
	153059,
	153343,
	153557,
	153889,
	154127,
	154373,
	154669,
	154927,
	155201,
	155453,
	155693,
	155891,
	156253,
	156589,
	156817,
	157103,
	157279,
	157559,
	157837,
	158141,
	158407,
	158657,
	158981,
	159233,
	159521,
	159763,
	160009,
	160253,
	160583,
	160781,
	161047,
	161333,
	161591,
	161869,
	162119,
	162473,
	162703,
	162917,
	163193,
	163477,
	163741,
	163993,
	164249,
	164477,
	164789,
	165103,
	165397,
	165653,
	165931,
	166247,
	166561,
	166783,
	167021,
	167221,
	167483,
	167779,
	168083,
	168409,
	168697,
	169003,
	169249,
	169523,
	169777,
	170021,
	170263,
	170503,
	170767,
	171029,
	171299,
	171583,
	171823,
	172097,
	172343,
	172597,
	172849,
	173137,
	173431,
	173699,
	173933,
	174169,
	174467,
	174737,
	175003,
	175361,
	175673,
	175919,
	176153,
	176357,
	176557,
	176807,
	177101,
	177409,
	177743,
	177979,
	178259,
	178513,
	178781,
	178973,
	179243,
	179479,
	179693,
	179947,
	180221,
	180437,
	180751,
	181123,
	181409,
	181721,
	181957,
	182167,
	182453,
	182659,
	182929,
	183263,
	183479,
	183709,
	184013,
	184273,
	184567,
	184831,
	185077,
	185327,
	185593,
	185821,
	186037,
	186271,
	186601,
	186859,
	187129,
	187367,
	187633,
	187909,
	188261,
	188483,
	188767,
	189011,
	189311,
	189517,
	189767,
	190027,
	190339,
	190639,
	190871,
	191143,
	191467,
	191693,
	191977,
	192233,
	192499,
	192749,
	192977,
	193247,
	193513,
	193789,
	194017,
	194323,
	194681,
	194911,
	195161,
	195413,
	195737,
	195973,
	196201,
	196543,
	196817,
	197089,
	197339,
	197599,
	197887,
	198109,
	198391,
	198637,
	198901,
	199193,
	199483,
	199741,
	200003,
	200273,
	200579,
	200881,
	201151,
	201451,
	201701,
	201911,
	202129,
	202471,
	202751,
	203011,
	203321,
	203579,
	203873,
	204137,
	204431,
	204707,
	204983,
	205237,
	205487,
	205763,
	206039,
	206273,
	206501,
	206827,
	207121,
	207371,
	207569,
	207847,
	208121,
	208379,
	208591,
	208907,
	209203,
	209401,
	209659,
	209927,
	210143,
	210361,
	210659,
	210923,
	211213,
	211457,
	211723,
	211969,
	212293,
	212627,
	212917,
	213203,
	213449,
	213737,
	214007,
	214243,
	214519,
	214771,
	215087,
	215381,
	215681,
	215939,
	216233,
	216569,
	216803,
	217111,
	217351,
	217579,
	217907,
	218171,
	218479,
	218719,
	218987,
	219251,
	219523,
	219757,
	219953,
	220279,
	220537,
	220807,
	221071,
	221317,
	221621,
	221873,
	222127,
	222379,
	222713,
	222977,
	223241,
	223441,
	223757,
	224057,
	224303,
	224591,
	224881,
	225143,
	225371,
	225629,
	225931,
	226189,
	226483,
	226769,
	227089,
	227363,
	227567,
	227827,
	228197,
	228427,
	228637,
	228887,
	229171,
	229409,
	229639,
	229847,
	230123,
	230357,
	230653,
	230933,
	231269,
	231481,
	231779,
	232049,
	232367,
	232633,
	232901,
	233239,
	233549,
	233861,
	234149,
	234343,
	234653,
	234893,
	235117,
	235489,
	235783,
	236069,
	236381,
	236681,
	236909,
	237179,
	237547,
	237857,
	238099,
	238321,
	238591,
	238859,
	239167,
	239429,
	239699,
	239963,
	240203,
	240509,
	240797,
	241061,
	241327,
	241567,
	241817,
	242059,
	242371,
	242621,
	242873,
	243197,
	243479,
	243709,
	244033,
	244303,
	244529,
	244753,
	245039,
	245339,
	245621,
	245881,
	246173,
	246439,
	246707,
	246929,
	247249,
	247531,
	247771,
	248041,
	248267,
	248537,
	248783,
	249059,
	249329,
	249563,
	249859,
	250091,
	250499,
	250807,
	251063,
	251263,
	251519,
	251831,
	252079,
	252341,
	252641,
	252913,
	253307,
	253543,
	253777,
	254003,
	254291,
	254663,
	254911,
	255127,
	255383,
	255649,
	255917,
	256163,
	256483,
	256771,
	257107,
	257371,
	257671,
	257953,
	258211,
	258469,
	258697,
	258977,
	259271,
	259583,
	259823,
	260137,
	260441,
	260717,
	260987,
	261301,
	261581,
	261847,
	262147,
	262411,
	262693,
	263009,
	263267,
	263521,
	263821,
	264053,
	264331,
	264637,
	264893,
	265157,
	265427,
	265717,
	266003,
	266221,
	266491,
	266801,
	267037,
	267317,
	267523,
	267719,
	267913,
	268267,
	268637,
	268861,
	269117,
	269377,
	269623,
	269947,
	270223,
	270463,
	270709,
	271003,
	271273,
	271571,
	271853,
	272171,
	272353,
	272621,
	272933,
	273181,
	273527,
	273881,
	274123,
	274403,
	274723,
	274957,
	275227,
	275491,
	275741,
	276011,
	276257,
	276527,
	276823,
	277087,
	277373,
	277663,
	277993,
	278237,
	278557,
	278807,
	279073,
	279413,
	279649,
	279977,
	280243,
	280547,
	280769,
	280997,
	281291,
	281579,
	281797,
	282019,
	282311,
	282617,
	282889,
	283139,
	283519,
	283799,
	284093,
	284341,
	284587,
	284783,
	285079,
	285377,
	285641,
	285949,
	286301,
	286543,
	286789,
	287149,
	287383,
	287783,
	288089,
	288389,
	288679,
	288973,
	289151,
	289397,
	289721,
	289987,
	290219,
	290509,
	290737,
	291037,
	291299,
	291563,
	291877,
	292181,
	292477,
	292717,
	293071,
	293329,
	293659,
	293999,
	294241,
	294467,
	294793,
	295037,
	295319,
	295699,
	295937,
	296221,
	296507,
	296741,
	296987,
	297359,
	297617,
	297907,
	298187,
	298477,
	298759,
	299087,
	299371,
	299671,
	299951,
	0
};


unsigned GlPrime( unsigned close, int relationship )
{
	static unsigned count = 0;

	if ( count == 0 )
	{
		// Get the size of the table.
		while ( gPrime[count] ) 
			++count;
	}

	unsigned low = 0;
	unsigned high = count - 1;
	unsigned pivot = ( high + low ) >> 1;

	while ( high > low )
	{
		if ( close > gPrime[ pivot ] )
			low = pivot + 1;
		else
			high = pivot - 1;

		pivot = ( high + low ) >> 1;
	}
	GLASSERT( low == high );

	if ( relationship < 0 )
	{
		while ( low > 0 && gPrime[ low ] > close )
			--low;
	}
	else if ( relationship > 0 )
	{
		while ( low < count && gPrime[ low ] < close )
			++low;
	}
	return gPrime[ low ];
}

