//#include "bsp.h"
#include "algorithm.h"
#include "string.h"

void SortArrayExtreme(uint32_t Array[], const uint32_t ArraySize,
                      const uint32_t SortHeadSize, const uint32_t SortTailSize)
{
    uint32_t i, j;
    uint32_t temp;

    for (i = 0; i < SortTailSize; i++)
    {
        for (j = 0; j < ArraySize - i - 1; j++)
        {
            if (Array[j] > Array[j+1])
            {
                temp = Array[j];
                Array[j] = Array[j+1];
                Array[j+1] = temp;
            }
        }
				
    }

    for (i = 0; i < SortHeadSize; i++)
    {
        for (j = ArraySize - SortTailSize - 1 ; j > i; j--)
        {
            if (Array[j - 1] > Array[j])
            {
                temp = Array[j - 1];
                Array[j - 1] = Array[j];
                Array[j] = temp;
            }
        }
				
    }
}

uint32_t GetAverage(uint32_t Array[], const uint32_t ArraySize,
               const uint32_t DelHeadSize, const uint32_t DelTailSize)
{
    //uint64_t sum = 0;
    long long int sum = 0;
    if ((DelHeadSize + DelTailSize) >= ArraySize)
    {
        return 0;
    }

    for (long i = DelHeadSize; i < ArraySize - DelTailSize; i++)
    {
        sum += Array[i];
    }

    return(sum / (ArraySize - DelHeadSize - DelTailSize));
}


uint32_t GetDelExtremeAndAverage(uint32_t Array[], const uint32_t ArraySize,
                            const uint32_t SortHeadSize, const uint32_t SortTailSize)
{
    SortArrayExtreme(Array, ArraySize, SortHeadSize, SortTailSize);
    return(GetAverage(Array, ArraySize, SortHeadSize, SortTailSize));
}
														
long hextodec_fun(unsigned char *s)
{
	int i,t;			 //t记录临时加的数 
	long sum =0;
	for(i=0;s[i];i++)
	{
		if(s[i]>='0'&&s[i]<='9')
		t=s[i]-'0'; 		//当字符是0~9时保持原数不变
		if(s[i]>='a'&&s[i]<='z')
		t=s[i]-'a'+10;
		if(s[i]>='A'&&s[i]<='Z')
		t=s[i]-'A'+10;
		sum=sum*16+t;
	}
	return sum;
 } 
 void xz_dump_on(const char *data, short size)
 {
	 char *str = NULL;
	 unsigned char *p = (unsigned char *) data;
	 char buf[6] = { 0 };
	 int i = 0;
 
	 str = (char *)rt_malloc(size<<2);
	 if (!str)
		 return;
	 rt_memset(str, 0x00, size<<2);
	 for (i = 0; i < size; i++)
	 {
		 rt_sprintf(buf, "%02X", *p);
		 strcat(str, buf);
		 if(i<size-1)
			 strcat(str, ",");
		 if(rt_strlen(str)%48==0)
		 {
			 rt_kprintf(str+((rt_strlen(str)/48)-1)*48);
		 }
		 p++;
	 }
//	 rt_kprintf(str+(strlen(str)/48)*48);
	// rt_free(str);
 }

//获得数组最大或最小值元素
uint32_t Get_Min_Max(uint32_t *dBuf, uint8_t len, uint8_t mode)
{
    uint8_t i;
    uint32_t temp;
       
    if(mode)                                                                    //返回最大值
    {
        for(i = 0;i < len - 1;i++)                                              //找到最大的值
        {
            if(dBuf[i] > dBuf[i + 1])
            {
                temp = dBuf[i];
                dBuf[i] = dBuf[i + 1];
                dBuf[i + 1] = temp;
            }
        }
        return dBuf[len - 1];
    }
    else
    {
        for(i = len - 1;i > 0 ;i--)                                             //找到数组最小的值
        {
            if(dBuf[i - 1] > dBuf[i])
            {
                temp = dBuf[i - 1];
                dBuf[i - 1] = dBuf[i];
                dBuf[i] = temp;
            }
        }
        return dBuf[0];                                                         //返回最小值
    }
}




uint32_t ArrToHex(uint8_t *pData)
{
    uint32_t Data;
    Data = pData[0];
    Data <<= 8;
    Data += pData[1];
    Data <<= 8;
    Data += pData[2];
    Data <<= 8;
    Data += pData[3];

    return *(uint32_t*)&Data;
}

float HexToFloat(uint8_t *pData)
{
    uint32_t Data;
    Data = pData[0];
    Data <<= 8;
    Data += pData[1];
    Data <<= 8;
    Data += pData[2];
    Data <<= 8;
    Data += pData[3];

    return *(float*)&Data;
}

uint32_t HexToUlong(uint8_t *pData)
{
    uint32_t Data;
    Data = pData[0];
    Data <<= 8;
    Data += pData[1];
    Data <<= 8;
    Data += pData[2];
    Data <<= 8;
    Data += pData[3];

    return Data;
}


void floatToHexArray(float fSource, uint8_t *pu8Temp, long num)
{
    uint32_t u32Source = *(uint32_t *)&fSource;
    for(long i = 0; i < num; i++)
    {
        pu8Temp[i] = (u32Source >> (8 * (3 - i))) & 0xff;
    }
}



int sum(int *buf, uint8_t len)
{
    uint8_t i;
    int sum = 0;
    
    for(i = 0;i < len;i++)
    {
        sum += *(buf + i);
    }
    return sum;
}


uint32_t square_sum(int *bufx, uint8_t len)
{
    uint8_t i;
    uint32_t sum = 0;
    
    for(i = 0;i < len;i++)
    {
        sum += (*(bufx + i)) * (*(bufx + i));
    }
    return sum;    
}


int multi_sum(int *bufx, int *bufy, uint8_t len)
{
    uint8_t i;
    int sum = 0;
    
    for(i = 0;i < len;i++)
    {
        sum += (*(bufx + i)) * (*(bufy + i));
    }
    return sum; 
}


int get_divisor(int *bufx, uint8_t len)
{
    int sumx;
    int divisor;
    uint32_t square_sumx;

    sumx = sum(bufx, len);
    square_sumx = square_sum(bufx, len);
    divisor = sumx * sumx - square_sumx * len;
    return divisor;
}


int get_slop(int *bufx, int *bufy, uint8_t len)
{
    int k;
    int divisor;
    int sdivisor;
 
    int sumx;
    int sumy;
    int multi_sumxy;
    
    sumx = sum(bufx, len);
    sumy = sum(bufy, len);
    multi_sumxy = multi_sum(bufx, bufy, len);
    sdivisor = sumx * sumy - multi_sumxy * len;
    divisor = get_divisor(bufx, len);
    if(divisor != 0)                         
    {  
        k = (int)(sdivisor *100.0f / divisor);
    }

    else
    {
        k = 0;
    }
    return k;
}
