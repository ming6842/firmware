/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "algorithm_moveAve.h"
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : MoveAve_SMA
**功能 : Simple Moving Average
**輸入 : NewData, MoveAve_FIFO, SampleNum
**輸出 : AveData
**使用 : MoveAve_SMA(NewData, MoveAve_FIFO, SampleNum)
**=====================================================================================================*/
/*=====================================================================================================*/
s16 MoveAve_SMA(s16 NewData, s16 *MoveAve_FIFO, u8 SampleNum)
{
	u8 i = 0;
	s16 AveData = 0;
	s32 MoveAve_Sum = 0;

	for (i = 0; i < SampleNum - 1; i++)       // 陣列移動
		MoveAve_FIFO[i] = MoveAve_FIFO[i + 1];

	MoveAve_FIFO[SampleNum - 1] = NewData;    // 加入新數據

	for (i = 0; i < SampleNum; i++)           // 求和
		MoveAve_Sum += MoveAve_FIFO[i];

	AveData = (s16)(MoveAve_Sum / SampleNum); // 計算平均值

	return AveData;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : MoveAve_WMA
**功能 : Weighted Moving Average
**輸入 : NewData, MoveAve_FIFO, SampleNum
**輸出 : AveData
**使用 : MoveAve_WMA(NewData, MoveAve_FIFO, SampleNum)
**=====================================================================================================*/
/*=====================================================================================================*/
s16 MoveAve_WMA(s16 NewData, s16 *MoveAve_FIFO, u8 SampleNum)
{
	u8 i = 0;
	s16 AveData = 0;
	u16 SampleSum = 0;
	s32 MoveAve_Sum = 0;

	for (i = 0; i < SampleNum - 1; i++)         // 陣列移動
		MoveAve_FIFO[i] = MoveAve_FIFO[i + 1];

	MoveAve_FIFO[SampleNum - 1] = NewData;      // 加入新數據

	for (i = 0; i < SampleNum; i++)             // 求和 & 加權
		MoveAve_Sum += MoveAve_FIFO[i] * (i + 1);

	SampleSum = (SampleNum * (SampleNum + 1)) / 2; // 計算加權除數
	AveData = (s16)(MoveAve_Sum / SampleSum);   // 計算平均值

	return AveData;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : MoveAve_EMA
**功能 : Exponential Moving Average
**輸入 : NewData, MoveAve_FIFO, SampleNum
**輸出 : AveData
**使用 : MoveAve_EMA(NewData, MoveAve_FIFO, SampleNum)
**=====================================================================================================*/
/*=====================================================================================================*/

void ring_buf_write(int16_t new_data, int16_t *buf, uint16_t len)
{
	int16_t i = 0;
	for( i = 1; i < len; i++)
		buf[i] = buf[i-1];
	buf[0] = new_data;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
int16_t mov_avg_filter(int16_t new_data, mov_avg_filter_params *p, int16_t *ring_buf)
{
	int16_t sum, i;
	ring_buf_write(new_data, ring_buf, p->window_size);
	p->cnt += 1;

	if( p->cnt <= p->window_size ){
		/*doesn't have enough samples*/
		
		for ( i = 0 ; i < p->cnt ; i++)
			sum += ring_buf[i];

		p->avg = sum/p->cnt;

		return 1;

	} else if ( p->cnt > p->window_size){
		/*have enough samples*/
		for ( i = 0 ; i < p->window_size ; i++)
			sum += ring_buf[i];

		p->avg = sum/p->window_size;

		return 1;

	} else {

		return 0;
	}

	

	

}