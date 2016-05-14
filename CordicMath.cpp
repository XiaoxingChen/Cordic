#include "CordicMath.h"
uint32_t resolutionTab_32b[31] =
{
	536870912,	//5.36871e+008
	316933406,	//3.16933e+008
	167458907,	//1.67459e+008
	85004756,	//8.50048e+007
	42667331,	//4.26673e+007
	21354465,	//2.13545e+007
	10679838,	//1.06798e+007
	5340245,	//5.34025e+006
	2670163,	//2.67016e+006
	1335087,	//1.33509e+006
	667544,	//667544
	333772,	//333772
	166886,	//166886
	83443,	//83443
	41722,	//41721.5
	20861,	//20860.8
	10430,	//10430.4
	5215,	//5215.19
	2608,	//2607.59
	1304,	//1303.8
	652,	//651.899
	326,	//325.949
	163,	//162.975
	81,	//81.4873
	41,	//40.7437
	20,	//20.3718
	10,	//10.1859
	5,	//5.09296
	3,	//2.54648
	1,	//1.27324
	1	//0.63662
};

static uint16_t compensateTab(uint8_t cnt)
{
	/*compensate resolution is 16bits */
	uint16_t Tab[9] =
	{
		46341,	//0.707107
		41449,	//0.632456
		40211,	//0.613572
		39901,	//0.608834
		39823,	//0.607648
		39803,	//0.607352
		39799,	//0.607278
		39797,	//0.607259
		39797	//0.607254
	};
	if (cnt > 8) cnt = 8;
	return Tab[cnt];
}

//
//int32_t sin_T_u32(uint32_t theta)
//
bool sin_cos_0deg_90deg_u30b(uint32_t theta, int32_t* resultSin, int32_t* resultCos)
{
	if (theta & ~0x3FFFFFFF 
		|| resultSin == (void*)0
		|| resultCos == (void*)0)
		return 0;
	int32_t radAcc = theta;

	uint16_t cnt = 0;
	int32_t x = 0x3FFFFFFF;	//0x7FFFFFFF >> 1
	int32_t y = 0;
	int32_t x_prev = x;
	int32_t y_prev = y;
	while (true)
	{
		if (radAcc > 10)
		{
			radAcc -= resolutionTab_32b[cnt];
			x = x_prev - (y_prev >> cnt);
			y = (x_prev >> cnt) + y_prev;
		}
		else if (radAcc < -10)
		{
			radAcc += resolutionTab_32b[cnt];
			x = x_prev + (y_prev >> cnt);
			y = -(x_prev >> cnt) + y_prev;
		}
		else break;
		cnt++;
		x_prev = x;
		y_prev = y;
		if (cnt > 30) return 0;
	}
	/*y = y * compensateTab(cnt - 1) >> 16;*/
	x = (x >> 16)*compensateTab(cnt - 1) + (((x & 0xFFFF)* compensateTab(cnt - 1)) >> 16);
	y = (y >> 16)*compensateTab(cnt - 1) + (((y & 0xFFFF)* compensateTab(cnt - 1)) >> 16);
	if (x < 0) x = 0;
	else if (x > 0x3FFFFFFF) x = 0x3FFFFFFF;
	if (y < 0) y = 0;
	else if (y > 0x3FFFFFFF) y = 0x3FFFFFFF;
	x <<= 1;
	y <<= 1;

	*resultSin = y;
	*resultCos = x;
	return 1;
}

//
//int32_t sin_T_u32(uint32_t theta)
//
int32_t sin_T_u32(uint32_t theta)
{
	int32_t mapTheta = 0;
	if (theta > 0x7FFFFFFFul)								//180deg ~ 360deg
	{
		if (theta > 0xBFFFFFFFul)							//270deg ~ 360dg
			mapTheta = 0xFFFFFFFFul - theta;
		else mapTheta = 0x3FFFFFFFul & theta;	//180deg ~ 270deg
	}
	else //theta <= 0x7FFFFFFF							//0deg ~ 180deg
	{
		if (theta > 0x3FFFFFFFul)							//90deg ~ 180deg
			mapTheta = 0x80000000ul - theta;
		else mapTheta = theta;								//0deg ~ 90deg
	}
	int32_t result;
	int32_t resultTemp;
	sin_cos_0deg_90deg_u30b(mapTheta, &result, &resultTemp);

	if (theta & 0x80000000) //theta > 180deg
		result = -result;

	return result;
}
//end of file
