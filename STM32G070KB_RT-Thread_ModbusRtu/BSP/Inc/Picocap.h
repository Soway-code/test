/**@file        Picocap.h
* @details      Picocap.c的头文件,声明了PCap驱动的API函数,
* 定义了寄存器的参数
* @author       杨春林
* @date         2020-04-29
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/29  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __PICOCAP_H
#define __PICOCAP_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "VariaType.h"

extern const unsigned char SRAM_DATA[];
/** PCap固件大小 */
#define SRAM_DATA_NUM       sizeof(SRAM_DATA) / sizeof(SRAM_DATA[0])

extern const unsigned long REG[];
/** PCap配置数据大小 */
#define REG_CONF_NUM        sizeof(REG) / sizeof(REG[0])

#define WRITE_PCAPFIRM_ERR_MAX              10          /** 写PCAP固件错误最大次数 */


/******************************** 以下为21个配置寄存器的具体值 ***************************************/
/** REG0，OTP配置寄存器参数 */
/** SRAM与OTP的比较机制 */
#define REG0_MEMCOMP_OFF                    0x00        /** 关闭 */
#define REG0_MEMCOMP_5BYTE                  (1 << 18)   /** 5 Byte */
#define REG0_MEMCOMP_33BYTE                 (2 << 18)   /** 33 Byte */
#define REG0_MEMCOMP_257BYTE                (3 << 18)   /** 257 Byte */

/** OTP-内部错误检测和修复机制 */
#define REG0_ECC_MODE_DISABLE               0x00        /** 关闭(Single模式)，上限4032Byte */
#define REG0_ECC_MODE_DOUBLE                (0x0F << 8) /** Double模式，上限4032Byte */
#define REG0_ECC_MODE_QUAD                  (0xF0 << 8) /** Quad模式，上限1984Byte */

/** 自动从OTP中启动 */
#define REG0_AUTOBOOT_ENABLE                0x00        /** 单芯片工作模式，芯片将会自动从OTP中重新启动 */
#define REG0_AUTOBOOT_DISABLE               (0x0F << 4) /** 从机模式，芯片通过通讯接口被复位启动 */

/** 存储器上锁 */
#define REG0_MEM_LOCK_ENABLE                0x00        /** 启动防止存储内容读出，阻止固件程序信息被读出 */
#define REG0_MEM_LOCK_DISABLE               (0x0F << 0) /** 启动不阻止存储内容读出 */


/** REG1，配置内部振荡器频率 */
#define REG1_OLF_TUNE_10KHZ                 0x35        /** 10kHz */
#define REG1_OLF_TUNE_50KHZ                 0x22        /** 50kHz */
#define REG1_OLF_TUNE_100KHZ                0x13        /** 100kHz */
#define REG1_OLF_TUNE_200KHZ                0x04        /** 200kHz */


/** REG2，电容测量配置 */
/** CDC端口使能 */
#define REG2_CMEAS_PC0_EN                   (0x01 << 16)/** 使能PC0端口 */    
#define REG2_CMEAS_PC1_EN                   (0x02 << 16)/** 使能PC1端口 */  
#define REG2_CMEAS_PC2_EN                   (0x04 << 16)/** 使能PC2端口 */  
#define REG2_CMEAS_PC3_EN                   (0x08 << 16)/** 使能PC3端口 */  
#define REG2_CMEAS_PC4_EN                   (0x10 << 16)/** 使能PC4端口 */  
#define REG2_CMEAS_PC5_EN                   (0x20 << 16)/** 使能PC5端口 */  
#define REG2_CMEAS_PC6_EN                   (0x40 << 16)/** 使能PC6端口 */  
#define REG2_CMEAS_PC7_EN                   (0x80 << 16)/** 使能PC7端口 */  

/** 传感器连接方式 */
#define REG2_CMEAS_GROUND_SINGLE            0x00        /** 单端接地 */
#define REG2_CMEAS_GROUND_DIFFERENTIAL      0x00        /** 差分接地 */
#define REG2_CMEAS_FLOATING_SINGLE          (0x01 << 14)/** 单端浮空 */
#define REG2_CMEAS_FLOATING_DIFFERENTIAL    (0x02 << 14)/** 差分浮空 */

/** 差分电容镜面对称充放电的选择，防止机械应力 */
#define REG2_CMEAS_DUMMY_DISABLE            0x00        /** 关闭 */
#define REG2_CMEAS_DUMMY_ENABLE             (0x01 << 13)/** 使能 */

/** 外部寄生电容的补偿测量 */
#define REG2_COMPENS_EXT_DISABLE            0x00        /** 关闭 */
#define REG2_COMPENS_EXT_ENABLE             (0x01 << 12)/** 使能 */

/** 内部寄生电容的补偿测量 */
#define REG2_COMPENS_INT_DISABLE            0x00        /** 关闭 */
#define REG2_COMPENS_INT_ENABLE             (0x01 << 11)/** 使能 */

/** 选择内部放电电阻 */
#define REG2_RDCHG_INT_180K                 (0x04 << 8)/** 180K欧 */
#define REG2_RDCHG_INT_90K                  (0x05 << 8)/** 90K欧 */
#define REG2_RDCHG_INT_30K                  (0x06 << 8)/** 30K欧 */
#define REG2_RDCHG_INT_10K                  (0x07 << 8)/** 10K欧 */


/** REG3，电容测量配置 */
/** CDC循环的时钟基准 */
#define REG3_CY_CLK_20US                    0x00        /** 20 μs (不带高频时钟的应用) */
#define REG3_CY_CLK_1US                     (0x02 << 22)/** 1 μs (仅当应用外部 4 MHz振荡) */
#define REG3_CY_CLK_0_25US                  (0x03 << 22)/** 0.25 μs (仅当应用外部 4 MHz振荡) */

/** 在时钟-触发模式设置触发的周期 (寄存器4, TMEAS_TRIG_SEL = 2) */
#define REG3_SEQ_TIME_CONFIG                13          /** 配置触发周期数，范围：1~24，0 = 关闭 */
#define REG3_SEQ_TIME                       (REG3_SEQ_TIME_CONFIG << 16)

/** 设定CDC每次测量后的假测量次数，可从0到4选择 */
#define REG3_CMEAS_FAKE_NONE                0x00        /** 无假测量 */
#define REG3_CMEAS_FAKE_1_TIME              (0x01 << 13)/** 1次假测量 */
#define REG3_CMEAS_FAKE_2_TIMES             (0x02 << 13)/** 2次假测量 */
#define REG3_CMEAS_FAKE_4_TIMES             (0x03 << 13)/** 4次假测量 */

/** CDC 测量后内部平均的次数。设置为0无效，默认为1 */
#define REG3_C_AVRG_CONFIG                  512         /** 配置内部平均次数，范围：0~4095 */
#define REG3_C_AVRG                         REG3_C_AVRG_CONFIG


/** REG4，电容/温度测量配置 */
/** 选择相应的输出电容测量的脉冲触发管脚 */
#define REG4_CMEAS_STARTPIN_PG0             0x00        /** PG0 */
#define REG4_CMEAS_STARTPIN_PG1             (0x01 << 20)/** PG1 */
#define REG4_CMEAS_STARTPIN_PG2             (0x02 << 20)/** PG2 */
#define REG4_CMEAS_STARTPIN_PG3             (0x03 << 20)/** PG3 */

/** 选择电容测量的触发源 */
#define REG4_CMEAS_TRIG_SEL_SOFT            0x00        /** 仅软件触发 */
#define REG4_CMEAS_TRIG_SEL_CONTINUE        (0x01 << 18)/** 持续模式 */
#define REG4_CMEAS_TRIG_SEL_CLOCK           (0x02 << 18)/** 时钟触发模式 */
#define REG4_CMEAS_TRIG_SEL_PULSE           (0x03 << 18)/** 脉冲触发模式 */

/** 设置用于电容测量的 cycle time，CDC 周期时间 = 
(CMEAS_CYTIME+1) * Clock_Period，Clock_period=20μs / 1μs / 0.25μs 
根据 CYCLKSEL=0/2/3 */
#define REG4_CMEAS_CYTIME_CONFIG            2           /** 范围：0~1023 */
#define REG4_CMEAS_CYTIME                   (REG4_CMEAS_CYTIME_CONFIG << 8)

/** 设置用于温度测量的 cycle time. 选择33nF,1000kOhm 传感器设置为 0 */
#define REG4_TMEAS_CYTIME_140US             0x00        /** 140 μs */
#define REG4_TMEAS_CYTIME_280US             (0x01 << 4) /** 280 μs */

/** 选择用于温度测量的脉冲触发管脚 */
#define REG4_TMEAS_STARTPIN_PG0             0x00        /** PG0 */
#define REG4_TMEAS_STARTPIN_PG1             (0x01 << 2) /** PG1 */
#define REG4_TMEAS_STARTPIN_PG2             (0x02 << 2) /** PG2 */
#define REG4_TMEAS_STARTPIN_PG3             (0x03 << 2) /** PG3 */

/** 选择温度测量的触发源 */
#define REG4_TMEAS_TRIG_SEL_OFF             0x00        /** 关闭 / 操作码触发 */
#define REG4_TMEAS_TRIG_SEL_CMEAS           0x01        /** CMEAS-触发 (通过 C-测量,推荐) */


/** REG5，温度测量配置 */
/** RDC结果多少次采样进行平均 */
#define REG5_T_AVRG_ONCE                    0x00        /** 无平均 */
#define REG5_T_AVRG_4_TIMES                 (0x01 << 22)/** 4次平均 */
#define REG5_T_AVRG_8_TIMES                 (0x02 << 22)/** 8次平均 */
#define REG5_T_AVRG_16_TIMES                (0x03 << 22)/** 16次平均 */

/** 将确定相对于CDC的测量RDC测量出现的次数。
设置为0则为1次。建议在湿度计中设置为0。因为温度测量和湿度
测量的次数应该是一样的/ 在压力传感器设置 100...1k 更多，
因为压力的变化一般非常快，比温度要快很多*/
#define REG5_TMEAS_TRIG_PREDIV_CONFIG       0           /** 范围：0~4194303 */
#define REG5_TMEAS_TRIG_PREDIV              REG5_TMEAS_TRIG_PREDIV_CONFIG


/** REG6，温度测量配置 */
/** 选择假测量的次数.精度/分辨率当设置为1的时候比较好，但速度将会比较慢 */
#define REG6_TMEAS_FAKE_2_TIMES             0x00        /** 2 次假测量 */
#define REG6_TMEAS_FAKE_8_TIMES             (0x01 << 15)/** 8 次假测量 */

/** 芯片内部集成了一个温敏电阻和一个内部参考电阻.另外还提供了连接外部温度传感器和参
考电阻的端口.总而言之,这颗芯片支持11种组合.但是在这里所描述的带有标准固件的芯片,
将会仅有6种组合可能: */
/** Res11 = R(内部铝电阻)/R(内部多晶硅电阻) */
#define REG6_TMEAS_7BITS_AL_SI              (0x43 << 8)        
/** Res10 = R(外部电阻)/R(内部多晶硅电阻)，Res11 = R(内部铝电阻)/R(内部多晶硅电阻) */
#define REG6_TMEAS_7BITS_EX_SI_AL_SI        (0x4B << 8)        
/** Res10 = R(外部电阻)/R(内部多晶硅电阻) */
#define REG6_TMEAS_7BITS_EX_SI              (0x49 << 8)        
/** Res11 = R(内部铝电阻)/R(外部参考电阻) */
#define REG6_TMEAS_7BITS_AL_REF             (0x06 << 8)        
/** Res10 = R(外部电阻)/R(外部参考电阻)，Res11 = R(内部铝电阻)/R(外部参考电阻) */
#define REG6_TMEAS_7BITS_EX_REF_AL_REF      (0x0E << 8)        
/** Res10 = R(外部电阻)/R(外部参考电阻) */
#define REG6_TMEAS_7BITS_EX_REF             (0x0C << 8)       


/** REG8，DSP配置 */
/** 为单片机选择程序存储空间 */
#define REG8_DSP_OTP_SEL                    0x00        /** 存储在OTP中 */
#define REG8_DSP_SRAM_SEL                   (0x01 << 23)/** 存储在SRAM中 */

/** 单片机的开始命令; 单片机的时钟开始工作,程序器跳转到地址0然后处理器开
始运行. 当固件完成后,DSP将会停止其自身的时钟！ */
#define REG8_DSP_NOT_START                  0x00        /** PCap的DSP不启动 */
#define REG8_DSP_START                      (0x01 << 22)/** PCap的DSP启动 */

/** 这个设置确保了单片机在错误产生时可被开启 */
#define REG8_DSPSTARTONOVL_DEFAULT          0x00        /** 需要默认设置 */
#define REG8_DSPSTARTONOVL                  (0x01 << 21)/** 单片机在错误产生时可被开启 */

/** 这个设置保证了处理器在温度测量完成后开启。根据不同固件，在固件 03.01.
xx 当中温度测量是在 CDC-完成之后进行的 */
#define REG8_DSP_STARTONTEMP_DEFAULT        0x00        /** 默认设置, 在固件03.01.xx中是必须的 */
#define REG8_DSP_STARTONTEMP                (0x01 << 20)/** 处理器在温度测量完成后开启 */

/** 激活锁存触发的给出管脚 */
#define REG8_DSP_STARTPIN_PG0               (0x01 << 16)/** PG0 */
#define REG8_DSP_STARTPIN_PG1               (0x02 << 16)/** PG1 */
#define REG8_DSP_STARTPIN_PG2               (0x04 << 16)/** PG2 */
#define REG8_DSP_STARTPIN_PG3               (0x08 << 16)/** PG3 */

/** 激活锁存触发的给出管脚 */
#define REG8_DSP_FF_IN_PG0                  (0x01 << 12)/** PG0 */
#define REG8_DSP_FF_IN_PG1                  (0x02 << 12)/** PG1 */
#define REG8_DSP_FF_IN_PG2                  (0x04 << 12)/** PG2 */
#define REG8_DSP_FF_IN_PG3                  (0x08 << 12)/** PG3 */

/** 单片机的,由软件工程师来进行定义 */
#define REG8_DSP_WATCHDOG_LENGTH_CONFIG     0           /** 定时器时长, 范围：0~3 */
#define REG8_DSP_WATCHDOG_LENGTH            (REG8_DSP_WATCHDOG_LENGTH_CONFIG << 10)

/** 开启在 PG0 和 PG1线上的抗抖动滤波 */
#define REG8_DSP_MOFLO_PG0_EN               (0x01 << 8) /**  PG0开启 */
#define REG8_DSP_MOFLO_PG1_EN               (0x02 << 8) /**  PG1开启 */

/** 设置 DSP 速度 */
#define REG8_DSP_SPEED_STANDARD             (0x01 << 4)/** 标准 (快速) */
#define REG8_DSP_SPEED_LOW_POWER            (0x03 << 4)/** 低功耗(慢速) */

/** 在QFN24封装下有作用,因为这个时候没有INTN中断管脚.允许将中断信号通过
PG2端口输出 */
#define REG8_INT2PG2_INTN_ONLY              0x00        /** 只打开INTN管脚 */
#define REG8_INT2PG2_ENABLE                 (0x01 << 2) /** INTN和PG2管脚都打开 */

/** 脉冲调制码一般在端口 PG0 和 PG1输出。在I2C通信模式下，他们需要被选择
为输出在 PG2 和 PG3 */
#define REG8_PG0_X_G2_PG0                   0x00        /** 选择PG0 */
#define REG8_PG0_X_G2_PG2                   0x01        /** 选择PG2 */
#define REG8_PG1_X_G3_PG1                   0x00        /** 选择PG1 */
#define REG8_PG1_X_G3_PG3                   (0x01 << 2) /** 选择PG3 */


/** REG9，GPIO配置 */
/** 将输出切换为输入(PG3/位23 到 PG0/位20) */    
#define REG9_PG_DIR_PG0_IN                  (0x01 << 20)/** PG0输入 */
#define REG9_PG_DIR_PG1_IN                  (0x02 << 20)/** PG1输入 */
#define REG9_PG_DIR_PG2_IN                  (0x04 << 20)/** PG2输入 */
#define REG9_PG_DIR_PG3_IN                  (0x08 << 20)/** PG3输入 */

/** 在 PG0 到 PG3 线上开启上拉电阻; 应用机械切换机制 */
#define REG9_PG_PULL_PG0_UP                 (0x01 << 16)/** PG0上拉 */
#define REG9_PG_PULL_PG1_UP                 (0x02 << 16)/** PG1上拉 */
#define REG9_PG_PULL_PG2_UP                 (0x04 << 16)/** PG2上拉 */
#define REG9_PG_PULL_PG3_UP                 (0x08 << 16)/** PG3上拉 */

/** 开启脉冲密度或者脉冲宽度调制码输出。 PWM0/PDM0 可以输出在端口 PG0
或者 PG2. PWM1/PDM1 可以输出在端口PG1 或者 PG3 */
#define REG9_PI_PWM0_EN                     (0x01 << 12)/** PWM0 开启 */
#define REG9_PI_PDM0_EN                     (0x02 << 12)/** PDM0 开启 */
#define REG9_PI_PWM1_EN                     (0x04 << 12)/** PWM1 开启 */
#define REG9_PI_PDM1_EN                     (0x08 << 12)/** PDM1 开启 */

/** 对于脉冲接口的基础频率,基于内部低频振荡(OLF)或者外部高频晶振 (OHF) */
#define REG9_PCLK_SEL_2OHF                  0x04        /** OHF * 2 */
#define REG9_PCLK_SEL_OHF                   0x05        /** OHF     */
#define REG9_PCLK_SEL_OHF2                  0x06        /** OHF / 2 */
#define REG9_PCLK_SEL_OHF4                  0x07        /** OHF / 4 */
#define REG9_PCLK_SEL_2OLF                  0x08        /** OLF * 2 */
#define REG9_PCLK_SEL_OLF                   0x09        /** OLF     */
#define REG9_PCLK_SEL_OLF2                  0x0A        /** OLF / 2 */
#define REG9_PCLK_SEL_OLF4                  0x0B        /** OLF / 4 */

#define REG9_PI0_CLK_SEL_2OHF               (REG9_PCLK_SEL_2OHF << 4)
#define REG9_PI0_CLK_SEL_OHF                (REG9_PCLK_SEL_OHF << 4)
#define REG9_PI0_CLK_SEL_OHF2               (REG9_PCLK_SEL_OHF2 << 4)
#define REG9_PI0_CLK_SEL_OHF4               (REG9_PCLK_SEL_OHF4 << 4)
#define REG9_PI0_CLK_SEL_2OLF               (REG9_PCLK_SEL_2OLF << 4)
#define REG9_PI0_CLK_SEL_OLF                (REG9_PCLK_SEL_OLF << 4)
#define REG9_PI0_CLK_SEL_OLF2               (REG9_PCLK_SEL_OLF2 << 4)
#define REG9_PI0_CLK_SEL_OLF4               (REG9_PCLK_SEL_OLF4 << 4)

#define REG9_PI1_CLK_SEL_2OHF               (REG9_PCLK_SEL_2OHF << 8)
#define REG9_PI1_CLK_SEL_OHF                (REG9_PCLK_SEL_OHF << 8)
#define REG9_PI1_CLK_SEL_OHF2               (REG9_PCLK_SEL_OHF2 << 8)
#define REG9_PI1_CLK_SEL_OHF4               (REG9_PCLK_SEL_OHF4 << 8)
#define REG9_PI1_CLK_SEL_2OLF               (REG9_PCLK_SEL_2OLF << 8)
#define REG9_PI1_CLK_SEL_OLF                (REG9_PCLK_SEL_OLF << 8)
#define REG9_PI1_CLK_SEL_OLF2               (REG9_PCLK_SEL_OLF2 << 8)
#define REG9_PI1_CLK_SEL_OLF4               (REG9_PCLK_SEL_OLF4 << 8)

/** 脉冲输出的精度 */
#define REG9_PRES_7BITS                     0x00        /** 7 bit */
#define REG9_PRES_8BITS                     0x01        /** 8 bit */
#define REG9_PRES_9BITS                     0x02        /** 9 bit */
#define REG9_PRES_10BITS                    0x03        /** 10 bit */

#define REG9_PI0_RES_7BITS                  REG9_PRES_7BITS
#define REG9_PI0_RES_8BITS                  REG9_PRES_8BITS
#define REG9_PI0_RES_9BITS                  REG9_PRES_9BITS
#define REG9_PI0_RES_10BITS                 REG9_PRES_10BITS

#define REG9_PI1_RES_7BITS                  (REG9_PRES_7BITS << 2)
#define REG9_PI1_RES_8BITS                  (REG9_PRES_8BITS << 2)
#define REG9_PI1_RES_9BITS                  (REG9_PRES_9BITS << 2)
#define REG9_PI1_RES_10BITS                 (REG9_PRES_10BITS << 2)


/** REG10，控制1.8 V 核心电压调节器，低功耗的设置也可以获得好的测量精
度,但是这个与RDC的测量速度和时序有关.请参考附录当中的例子 */
#define REG10_PV_CORE_CTL_STANDARD          0x47        /** 标准 */
#define REG10_V_CORE_CTL_LOW_POWER          0x87        /** 低功耗 */


/** Param2，选择PDM/PWM输出的信号源 */
#define PARAM2_SELECT_C1_C0                 0x01        /** C1/C0 */
#define PARAM2_SELECT_R0_RREF               0x08        /** R0/R参考 */
#define PARAM2_SELECT_R2_RREF               0x09        /** R2/R参考 */

/** 单一模式 */
#define PARAM2_SELECT_C2_C0                 0x02        /** C2/C0 */
#define PARAM2_SELECT_C3_C0                 0x03        /** C3/C0 */
#define PARAM2_SELECT_C4_C0                 0x04        /** C4/C0 */
#define PARAM2_SELECT_C5_C0                 0x05        /** C5/C0 */
#define PARAM2_SELECT_C6_C0                 0x06        /** C6/C0 */
#define PARAM2_SELECT_C7_C0                 0x07        /** C7/C0 */

/** 差分模式 */
#define PARAM2_SELECT_C3_C2                 0x02        /** C3/C2 */
#define PARAM2_SELECT_C5_C4                 0x03        /** C5/C4 */
#define PARAM2_SELECT_C7_C6                 0x04        /** C7/C6 */

/** pulse0选择PDM/PWM输出的信号源 */
#define PARAM2_PULSE0_SELECT_C1_C0          PARAM2_SELECT_C1_C0     /** C1/C0 */
#define PARAM2_PULSE0_SELECT_R0_RREF        PARAM2_SELECT_R0_RREF   /** R0/R参考 */
#define PARAM2_PULSE0_SELECT_R2_RREF        PARAM2_SELECT_R2_RREF   /** R2/R参考 */

/** pulse0单一模式 */
#define PARAM2_PULSE0_SELECT_C2_C0          PARAM2_SELECT_C2_C0     /** C2/C0 */
#define PARAM2_PULSE0_SELECT_C3_C0          PARAM2_SELECT_C3_C0     /** C3/C0 */
#define PARAM2_PULSE0_SELECT_C4_C0          PARAM2_SELECT_C4_C0     /** C4/C0 */
#define PARAM2_PULSE0_SELECT_C5_C0          PARAM2_SELECT_C5_C0     /** C5/C0 */
#define PARAM2_PULSE0_SELECT_C6_C0          PARAM2_SELECT_C6_C0     /** C6/C0 */
#define PARAM2_PULSE0_SELECT_C7_C0          PARAM2_SELECT_C7_C0     /** C7/C0 */

/** pulse0差分模式 */
#define PARAM2_PULSE0_SELECT_C3_C2          PARAM2_SELECT_C3_C2     /** C3/C2 */
#define PARAM2_PULSE0_SELECT_C5_C4          PARAM2_SELECT_C5_C4     /** C5/C4 */
#define PARAM2_PULSE0_SELECT_C7_C6          PARAM2_SELECT_C7_C6     /** C7/C6 */

/** pulse1选择PDM/PWM输出的信号源 */
#define PARAM2_PULSE1_SELECT_C1_C0          (PARAM2_SELECT_C1_C0 << 4)  /** C1/C0 */
#define PARAM2_PULSE1_SELECT_R0_RREF        (PARAM2_SELECT_R0_RREF << 4)/** R0/R参考 */
#define PARAM2_PULSE1_SELECT_R2_RREF        (PARAM2_SELECT_R2_RREF << 4)/** R2/R参考 */

/** pulse1单一模式 */
#define PARAM2_PULSE1_SELECT_C2_C0          (PARAM2_SELECT_C2_C0 << 4)  /** C2/C0 */
#define PARAM2_PULSE1_SELECT_C3_C0          (PARAM2_SELECT_C3_C0 << 4)  /** C3/C0 */
#define PARAM2_PULSE1_SELECT_C4_C0          (PARAM2_SELECT_C4_C0 << 4)  /** C4/C0 */
#define PARAM2_PULSE1_SELECT_C5_C0          (PARAM2_SELECT_C5_C0 << 4)  /** C5/C0 */
#define PARAM2_PULSE1_SELECT_C6_C0          (PARAM2_SELECT_C6_C0 << 4)  /** C6/C0 */
#define PARAM2_PULSE1_SELECT_C7_C0          (PARAM2_SELECT_C7_C0 << 4)  /** C7/C0 */

/** pulse1差分模式 */
#define PARAM2_PULSE1_SELECT_C3_C2          (PARAM2_SELECT_C3_C2 << 4)  /** C3/C2 */
#define PARAM2_PULSE1_SELECT_C5_C4          (PARAM2_SELECT_C5_C4 << 4)  /** C5/C4 */
#define PARAM2_PULSE1_SELECT_C7_C6          (PARAM2_SELECT_C7_C6 << 4)  /** C7/C6 */

/** Param3，对于脉冲输出0的斜率，有符号固定点数,19位整数和4位小数 */
#define PARAM3_PULSE0_SLOPE_CONFIG          0           /** -524,288 到 +524,288 以 0.0625为最小单位 */
#define PARAM3_PULSE0_SLOPE                 PARAM3_PULSE0_SLOPE_CONFIG 


/** Param4，对于脉冲输出0的零点有符号固定点数, 22 整数, 1 位小数.
这一位小数用于数学的求近似值, 不会用于输出 */
#define PARAM4_PULSE0_OFFSET_CONFIG         0           /** -4,194,304到 + 4,194,303.5 以 0.5为最小单位 */
#define PARAM4_PULSE0_OFFSET                PARAM4_PULSE0_OFFSET_CONFIG 


/** Param5，对于脉冲输出1的斜率，有符号固定点数,19位整数和4位小数 */
#define PARAM5_PULSE1_SLOPE_CONFIG         0           /** -524,288 到 +524,288 以 0.0625为最小单位 */
#define PARAM5_PULSE1_SLOPE                 PARAM5_PULSE1_SLOPE_CONFIG 


/** Param6，对于脉冲输出1的零点有符号固定点数, 22 整数, 1 位小数.
这一位小数用于数学的求近似值, 不会用于输出 */
#define PARAM6_PULSE1_OFFSET_CONFIG         0           /** -4,194,304到 + 4,194,303.5 以 0.5为最小单位 */
#define PARAM6_PULSE1_OFFSET                PARAM6_PULSE1_OFFSET_CONFIG 


/** Param7，选择单一或者差分传感器 */
#define PARAM7_SEL_SINGLE                   0x00        /** 单一 */
#define PARAM7_SEL_DIFFERENTIAL             0x000001    /** 差分 */


/** Param8，用于内部补偿的乘法系数 无符号固定浮点数,3位整数,21位小数 */
#define PARAM8_GAIN_CORR_CONFIG             0x200000    /** 可设置范围: 0 到 7.9999 */
#define PARAM8_GAIN_CORR                    PARAM8_GAIN_CORR_CONFIG

/** REG20，设备开启寄存器 */
/** 保护整个设备,保证在配置过程中静止,强迫DSP处于静止状态 */
#define REG20_RUN_DISABLE                   0x00        
/** 开启 运行模式 */
#define REG20_RUN_ENABLE                    0x000001    



/******************************** 以下为21个配置寄存器的配置 ***************************************/
/** REG0寄存器值，其中0x400000和0x20000是固定值 */
#define REG0_OTP_CONFIG_VALUE               0x400000 \
                                            /** SRAM与OTP的比较机制 */             \
                                            /**  REG0_MEMCOMP_OFF        关闭      \
                                                REG0_MEMCOMP_5BYTE      5 Byte      \
                                                REG0_MEMCOMP_33BYTE     33 Byte     \
                                                REG0_MEMCOMP_257BYTE    257 Byte  */\
                                            | REG0_MEMCOMP_OFF  \
                                            | 0x20000 \
                                            /** OTP-内部错误检测和修复机制 */                           \
                                            /**  REG0_ECC_MODE_DISABLE   关闭(Single模式)，上限4032Byte \
                                                REG0_ECC_MODE_DOUBLE    Double模式，上限4032Byte     \
                                                REG0_ECC_MODE_QUAD      Quad模式，上限1984Byte */ \
                                            | REG0_ECC_MODE_DISABLE \
                                            /** 自动从OTP中启动 */                                                 \
                                            /**  REG0_AUTOBOOT_ENABLE    单芯片工作模式，芯片将会自动从OTP中重新启动 \
                                                REG0_AUTOBOOT_DISABLE   从机模式，芯片通过通讯接口被复位启动 */   \
                                            | REG0_AUTOBOOT_DISABLE \
                                            /** 存储器上锁 */                                                         \
                                            /**  REG0_MEM_LOCK_ENABLE    启动防止存储内容读出，阻止固件程序信息被读出  \
                                                REG0_MEM_LOCK_DISABLE   启动不阻止存储内容读出 */                      \
                                            | REG0_MEM_LOCK_DISABLE 
                                            
/** REG1寄存器值，其中0x201000是固定值 */
#define REG1_IN_OSC_CONFIG_VALUE            0x201000 \
                                            /** REG1，配置内部振荡器频率 */        \
                                            /**  REG1_OLF_TUNE_10KHZ     10kHz       \
                                                REG1_OLF_TUNE_50KHZ     50kHz       \
                                                REG1_OLF_TUNE_100KHZ    100kHz      \
                                                REG1_OLF_TUNE_200KHZ    200kHz */   \
                                            | REG1_OLF_TUNE_50KHZ

/** REG2寄存器值，其中0x0B是固定值 */
#define REG2_CAP_MEASURE_VALUE              /** CDC端口使能 */                       \
                                            /**  REG2_CMEAS_PC0_EN   使能PC0端口     \ 
                                                REG2_CMEAS_PC1_EN   使能PC1端口     \
                                                REG2_CMEAS_PC2_EN   使能PC2端口     \
                                                REG2_CMEAS_PC3_EN   使能PC3端口     \
                                                REG2_CMEAS_PC4_EN   使能PC4端口     \
                                                REG2_CMEAS_PC5_EN   使能PC5端口     \
                                                REG2_CMEAS_PC6_EN   使能PC6端口     \
                                                REG2_CMEAS_PC7_EN   使能PC7端口 */  \
                                            REG2_CMEAS_PC0_EN \
                                            | REG2_CMEAS_PC1_EN \
                                            /** 传感器连接方式 */                               \
                                            /**  REG2_CMEAS_GROUND_SINGLE            单端接地    \
                                                REG2_CMEAS_GROUND_DIFFERENTIAL      差分接地    \
                                                REG2_CMEAS_FLOATING_SINGLE          单端浮空    \
                                                REG2_CMEAS_FLOATING_DIFFERENTIAL    差分浮空 */ \
                                            | REG2_CMEAS_GROUND_SINGLE \
                                            /** 差分电容镜面对称充放电的选择，防止机械应力 */ \
                                            /**  REG2_CMEAS_DUMMY_DISABLE    关闭              \
                                                REG2_CMEAS_DUMMY_ENABLE     使能 */           \
                                            | REG2_CMEAS_DUMMY_DISABLE \
                                            /** 外部寄生电容的补偿测量 */           \
                                            /**  REG2_COMPENS_EXT_DISABLE    关闭      \
                                                REG2_COMPENS_EXT_ENABLE     使能 */   \
                                            | REG2_COMPENS_EXT_ENABLE \
                                            /** 内部寄生电容的补偿测量 */           \
                                            /**  REG2_COMPENS_INT_DISABLE    关闭      \
                                                REG2_COMPENS_INT_ENABLE     使能 */   \
                                            | REG2_COMPENS_INT_DISABLE \
                                            /** 选择内部放电电阻 */              \
                                            /**  REG2_RDCHG_INT_180K     180K欧   \
                                                REG2_RDCHG_INT_90K      90K欧    \
                                                REG2_RDCHG_INT_30K      30K欧    \
                                                REG2_RDCHG_INT_10K      10K欧 */ \
                                            | REG2_RDCHG_INT_90K \
                                            | 0x0B
                                            
/** REG3寄存器值 */
#define REG3_CAP_MEASURE_VALUE              /** CDC循环的时钟基准 */                                    \
                                            /**  REG3_CY_CLK_20US    20 μs (不带高频时钟的应用)           \
                                                REG3_CY_CLK_1US     1 μs (仅当应用外部 4 MHz振荡)       \
                                                REG3_CY_CLK_0_25US  0.25 μs (仅当应用外部 4 MHz振荡) */ \
                                            REG3_CY_CLK_20US \
                                            /** 在时钟-触发模式设置触发的周期 (寄存器4, TMEAS_TRIG_SEL = 2) */                \
                                            /**  REG3_SEQ_TIME_CONFIG    配置触发周期数，范围：1~24，0 = 关闭                  \
                                                REG3_SEQ_TIME           触发周期数,它的具体值在REG3_SEQ_TIME_CONFIG中配置 */  \
                                            | REG3_SEQ_TIME | \
                                            /** 设定CDC每次测量的后的假测量次数，可从0到4选择 */ \
                                            /**  REG3_CMEAS_FAKE_NONE        无假测量                \
                                                REG3_CMEAS_FAKE_1_TIME      1次假测量               \
                                                REG3_CMEAS_FAKE_2_TIMES     2次假测量               \
                                                REG3_CMEAS_FAKE_4_TIMES     4次假测量 */            \
                                            REG3_CMEAS_FAKE_1_TIME | \
                                            /** CDC 测量后内部平均的次数。设置为0无效，默认为1 */                                \
                                            /**  REG3_C_AVRG_CONFIG      配置内部平均次数，范围：0~4095                          \
                                                REG3_C_AVRG             内部平均次数,它的具体值在REG3_C_AVRG_CONFIG中配置  */  \
                                            REG3_C_AVRG
                                            
/** REG4寄存器值 */
#define REG4_CAP_TEMP_MEASURE_VALUE         /** 选择相应的输出电容测量的脉冲触发管脚 */    \
                                            /**  REG4_CMEAS_STARTPIN_PG0     PG0             \
                                                REG4_CMEAS_STARTPIN_PG1     PG1             \
                                                REG4_CMEAS_STARTPIN_PG2     PG2             \
                                                REG4_CMEAS_STARTPIN_PG3     PG3 */          \
                                            REG4_CMEAS_STARTPIN_PG0 | \
                                            /** 选择电容测量的触发源 */                            \
                                            /**  REG4_CMEAS_TRIG_SEL_SOFT        仅软件触发       \
                                                REG4_CMEAS_TRIG_SEL_CONTINUE    持续模式        \
                                                REG4_CMEAS_TRIG_SEL_CLOCK       时钟触发模式      \
                                                REG4_CMEAS_TRIG_SEL_PULSE       脉冲触发模式 */   \
                                            REG4_CMEAS_TRIG_SEL_SOFT | \
                                            /**  设置用于电容测量的 cycle time，CDC 周期时间 =                                 \
                                                (CMEAS_CYTIME+1) * Clock_Period，Clock_period=20μs / 1μs / 0.25μs            \
                                                根据 CYCLKSEL=0/2/3 */                                                        \
                                            /**  REG4_CMEAS_CYTIME_CONFIG        范围：0~1023                                   \
                                                REG4_CMEAS_CYTIME       周期时间,它的具体值在REG4_CMEAS_CYTIME_CONFIG中配置  */  \
                                            REG4_CMEAS_CYTIME \
                                            /** 设置用于温度测量的 cycle time. 选择33nF,1000kOhm 传感器设置为 0 */    \
                                            /**  REG4_TMEAS_CYTIME_140US     140 μs                                      \
                                                REG4_TMEAS_CYTIME_280US     280 μs */                                   \
                                            | REG4_TMEAS_CYTIME_280US \
                                            /** 选择用于温度测量的脉冲触发管脚 */   \
                                            /**  REG4_TMEAS_STARTPIN_PG0     PG0     \
                                                REG4_TMEAS_STARTPIN_PG1     PG1     \
                                                REG4_TMEAS_STARTPIN_PG2     PG2     \
                                                REG4_TMEAS_STARTPIN_PG3     PG3 */  \
                                            | REG4_TMEAS_STARTPIN_PG0 \
                                            /** 选择温度测量的触发源 */                 \
                                            /**  REG4_TMEAS_TRIG_SEL_OFF     关闭 / 操作码触发                  \
                                                REG4_TMEAS_TRIG_SEL_CMEAS   CMEAS-触发 (通过 C-测量,推荐) */    \
                                            | REG4_TMEAS_TRIG_SEL_OFF
                                            
/** REG5寄存器值 */
#define REG5_TEMP_MEASURE_VALUE             /** RDC结果多少次采样进行平均 */            \
                                            /**  REG5_T_AVRG_ONCE        无平均         \
                                                REG5_T_AVRG_4_TIMES     4次平均        \
                                                REG5_T_AVRG_8_TIMES     8次平均        \
                                                REG5_T_AVRG_16_TIMES    16次平均 */    \
                                            REG5_T_AVRG_8_TIMES | \
                                            /**  将确定相对于CDC的测量RDC测量出现的次数。                                     \
                                                设置为0则为1次。建议在湿度计中设置为0。因为温度测量和湿度                     \ 
                                                测量的次数应该是一样的/ 在压力传感器设置 100...1k 更多，                      \
                                                因为压力的变化一般非常快，比温度要快很多*/                                      \
                                            /**  REG5_TMEAS_TRIG_PREDIV_CONFIG   范围：0~4194303                                \
                                                REG5_TMEAS_TRIG_PREDIV          相对于CDC的测量RDC测量出现的次数,               \ 
                                                                            它的具体值在REG5_TMEAS_TRIG_PREDIV_CONFIG中配置 */   \
                                            REG5_TMEAS_TRIG_PREDIV
                                            
/** REG6寄存器值，其中0x40是固定值 */
#define REG6_TEMP_MEASURE_VALUE             /** 芯片内部集成了一个温敏电阻和一个内部参考电阻.另外还提供了连接外部温度传感器和参 \
                                            考电阻的端口.总而言之,这颗芯片支持11种组合.但是在这里所描述的带有标准固件的芯片,     \
                                            将会仅有6种组合可能: */                                                              \
                                            /**  Res11 = R(内部铝电阻)/R(内部多晶硅电阻)                                         \
                                                    REG6_TMEAS_7BITS_AL_SI                                                      \
                                                Res10 = R(外部电阻)/R(内部多晶硅电阻)，Res11 = R(内部铝电阻)/R(内部多晶硅电阻)  \
                                                    REG6_TMEAS_7BITS_EX_SI_AL_SI                                                \
                                                Res10 = R(外部电阻)/R(内部多晶硅电阻)                                          \
                                                    REG6_TMEAS_7BITS_EX_SI                                                      \
                                                Res11 = R(内部铝电阻)/R(外部参考电阻)                                          \
                                                    REG6_TMEAS_7BITS_AL_REF                                                     \
                                                Res10 = R(外部电阻)/R(外部参考电阻)，Res11 = R(内部铝电阻)/R(外部参考电阻)    \
                                                    REG6_TMEAS_7BITS_EX_REF_AL_REF                                              \
                                                Res10 = R(外部电阻)/R(外部参考电阻)                                           \
                                                    REG6_TMEAS_7BITS_EX_REF     */                                              \
                                            REG6_TMEAS_FAKE_2_TIMES \
                                            | 0x40
                                            
/** REG7寄存器值，固定0x1F0000 */
#define REG7_CONFIG_VALUE                   0x1F0000

/** REG8寄存器值 */
#define REG8_DSP_CONFIG_VALUE               /** 为单片机选择程序存储空间 */              \
                                            /**  REG8_DSP_OTP_SEL        存储在OTP中     \
                                                REG8_DSP_SRAM_SEL       存储在SRAM中 */ \
                                            REG8_DSP_SRAM_SEL \
                                            /** 单片机的开始命令; 单片机的时钟开始工作,程序器跳转到地址0然后处理器开 \
                                            始运行. 当固件完成后,DSP将会停止其自身的时钟！ */                       \
                                            /**  REG8_DSP_NOT_START      PCap的DSP不启动                             \
                                                REG8_DSP_START          PCap的DSP启动 */                           \
                                            | REG8_DSP_NOT_START \
                                            /** 这个设置确保了单片机在错误产生时可被开启 */                      \
                                            /**  REG8_DSPSTARTONOVL_DEFAULT      默认设置                        \
                                                REG8_DSPSTARTONOVL              单片机在错误产生时可被开启 */    \
                                            | REG8_DSPSTARTONOVL_DEFAULT \
                                            /** 这个设置保证了处理器在温度测量完成后开启。根据不同固件，在固件 03.01.   \
                                            xx 当中温度测量是在 CDC-完成之后进行的 */                                  \
                                            /**  REG8_DSP_STARTONTEMP_DEFAULT    默认设置, 在固件03.01.xx中是必须的      \
                                                REG8_DSP_STARTONTEMP            处理器在温度测量完成后开启 */            \
                                            | REG8_DSP_STARTONTEMP_DEFAULT \
                                            /** 激活锁存触发的给出管脚 */           \
                                            /**  REG8_DSP_STARTPIN_PG0       PG0     \
                                                REG8_DSP_STARTPIN_PG1       PG1     \
                                                REG8_DSP_STARTPIN_PG2       PG2     \
                                                REG8_DSP_STARTPIN_PG3       PG3 */  \
                                            | 0x00  \
                                            /** 激活锁存触发的给出管脚 */       \
                                            /**  REG8_DSP_FF_IN_PG0      PG0     \
                                                REG8_DSP_FF_IN_PG1      PG1     \
                                                REG8_DSP_FF_IN_PG2      PG2     \
                                                REG8_DSP_FF_IN_PG3      PG3 */  \
                                            | 0x00  \
                                            /** 单片机的,由软件工程师来进行定义 */                  \
                                            /**  REG8_DSP_WATCHDOG_LENGTH_CONFIG     定时器时长配置 \
                                                REG8_DSP_WATCHDOG_LENGTH            定时器时长，  \
                                                                            它的具体值在REG8_DSP_WATCHDOG_LENGTH_CONFIG中配置 */ \
                                            | REG8_DSP_WATCHDOG_LENGTH \
                                            /** 开启在 PG0 和 PG1线上的抗抖动滤波 */     \
                                            /**  REG8_DSP_MOFLO_PG0_EN       PG0开启       \
                                                REG8_DSP_MOFLO_PG1_EN       PG1开启 */    \
                                            | REG8_DSP_MOFLO_PG0_EN \
                                            | REG8_DSP_MOFLO_PG1_EN \
                                            /** 设置 DSP 速度 */                             \
                                            /**  REG8_DSP_SPEED_STANDARD     标准 (快速)     \
                                                REG8_DSP_SPEED_LOW_POWER    低功耗(慢速) */  \
                                            | REG8_DSP_SPEED_STANDARD   \
                                            /** 在QFN24封装下有作用,因为这个时候没有INTN中断管脚.允许将中断信号通过  \
                                            PG2端口输出 */                                                          \
                                            /**  REG8_INT2PG2_INTN_ONLY      只打开INTN管脚           \
                                                REG8_INT2PG2_ENABLE         INTN和PG2管脚都打开 */    \
                                            | 0x00  \
                                            /** 脉冲调制码一般在端口 PG0 和 PG1输出。在I2C通信模式下，他们需要被选择 \
                                            为输出在 PG2 和 PG3 */                   \
                                            /**  REG8_PG0_X_G2_PG0       选择PG0       \
                                                REG8_PG0_X_G2_PG2       选择PG2       \
                                                REG8_PG1_X_G3_PG1       选择PG1       \
                                                REG8_PG1_X_G3_PG3       选择PG3 */    \
                                            | 0x00

/** REG9寄存器值 */
#define REG9_GPIO_CONFIG_VALUE              /** 将输出切换为输入(PG3/位23 到 PG0/位20) */    \
                                            /**  REG9_PG_DIR_PG0_IN      PG0输入               \
                                                REG9_PG_DIR_PG1_IN      PG1输入               \
                                                REG9_PG_DIR_PG2_IN      PG2输入               \
                                                REG9_PG_DIR_PG3_IN      PG3输入 */            \
                                            REG9_PG_DIR_PG0_IN | \
                                            REG9_PG_DIR_PG1_IN | \
                                            REG9_PG_DIR_PG3_IN | \
                                            /** 在 PG0 到 PG3 线上开启上拉电阻; 应用机械切换机制 */    \
                                            /**  REG9_PG_PULL_PG0_UP     PG0上拉                       \
                                                REG9_PG_PULL_PG1_UP     PG1上拉                       \
                                                REG9_PG_PULL_PG2_UP     PG2上拉                       \
                                                REG9_PG_PULL_PG3_UP     PG3上拉 */                    \
                                            REG9_PG_PULL_PG0_UP | \
                                            REG9_PG_PULL_PG1_UP | \
                                            REG9_PG_PULL_PG2_UP | \
                                            REG9_PG_PULL_PG3_UP | \
                                            /** 开启脉冲密度或者脉冲宽度调制码输出。 PWM0/PDM0 可以输出在端口 PG0 \
                                            或者 PG2. PWM1/PDM1 可以输出在端口PG1 或者 PG3 */                      \
                                            /**  REG9_PI_PWM0_EN     PWM0 开启                                     \
                                                REG9_PI_PDM0_EN     PDM0 开启                                     \
                                                REG9_PI_PWM1_EN     PWM1 开启                                     \
                                                REG9_PI_PDM1_EN     PDM1 开启 */                                  \
                                            REG9_PI_PDM0_EN | \
                                            /** 对于脉冲接口的基础频率,基于内部低频振荡(OLF)或者外部高频晶振 (OHF) */  \
                                            /**  REG9_PCLK_SEL_2OHF      OHF * 2                                     \
                                                REG9_PCLK_SEL_OHF       OHF                                         \
                                                REG9_PCLK_SEL_OHF2      OHF / 2                                     \
                                                REG9_PCLK_SEL_OHF4      OHF / 4                                     \
                                                REG9_PCLK_SEL_2OLF      OLF * 2                                     \
                                                REG9_PCLK_SEL_OLF       OLF                                         \
                                                REG9_PCLK_SEL_OLF2      OLF / 2                                     \
                                                REG9_PCLK_SEL_OLF4      OLF / 4                                     \
                                                                                                                    \
                                                REG9_PI0_CLK_SEL_2OHF   PI0时钟选择 OHF * 2                         \
                                                REG9_PI0_CLK_SEL_OHF    PI0时钟选择 OHF                             \
                                                REG9_PI0_CLK_SEL_OHF2   PI0时钟选择 OHF / 2                         \
                                                REG9_PI0_CLK_SEL_OHF4   PI0时钟选择 OHF / 4                         \
                                                REG9_PI0_CLK_SEL_2OLF   PI0时钟选择 OLF * 2                         \
                                                REG9_PI0_CLK_SEL_OLF    PI0时钟选择 OLF                             \
                                                REG9_PI0_CLK_SEL_OLF2   PI0时钟选择 OLF / 2                         \
                                                REG9_PI0_CLK_SEL_OLF4   PI0时钟选择 OLF / 4                         \
                                                                                                                    \
                                                REG9_PI1_CLK_SEL_2OHF   PI1时钟选择 OHF * 2                         \
                                                REG9_PI1_CLK_SEL_OHF    PI1时钟选择 OHF                             \
                                                REG9_PI1_CLK_SEL_OHF2   PI1时钟选择 OHF / 2                         \
                                                REG9_PI1_CLK_SEL_OHF4   PI1时钟选择 OHF / 4                         \
                                                REG9_PI1_CLK_SEL_2OLF   PI1时钟选择 OLF * 2                         \
                                                REG9_PI1_CLK_SEL_OLF    PI1时钟选择 OLF                             \
                                                REG9_PI1_CLK_SEL_OLF2   PI1时钟选择 OLF / 2                         \
                                                REG9_PI1_CLK_SEL_OLF4   PI1时钟选择 OLF / 4 */                      \
                                            REG9_PI0_CLK_SEL_2OLF | \
                                            /** 脉冲输出的精度 */                           \
                                            /**  REG9_PRES_7BITS     7 bit                   \
                                                REG9_PRES_8BITS     8 bit                   \
                                                REG9_PRES_9BITS     9 bit                   \
                                                REG9_PRES_10BITS    10 bit                  \
                                                                                            \
                                                REG9_PI0_RES_7BITS  PI0脉冲精度 7 bit       \
                                                REG9_PI0_RES_8BITS  PI0脉冲精度 8 bit       \
                                                REG9_PI0_RES_9BITS  PI0脉冲精度 9 bit       \
                                                REG9_PI0_RES_10BITS PI0脉冲精度 10 bit      \
                                                                                            \
                                                REG9_PI1_RES_7BITS  PI1脉冲精度 7 bit       \
                                                REG9_PI1_RES_8BITS  PI1脉冲精度 8 bit       \
                                                REG9_PI1_RES_9BITS  PI1脉冲精度 9 bit       \
                                                REG9_PI1_RES_10BITS PI1脉冲精度 10 bit*/    \
                                            REG9_PI0_RES_10BITS | \
                                            REG9_PI1_RES_10BITS

/** REG10寄存器值，其中0x180000是固定值 */
#define REG10_IN_1_8V_REGULATOR_VALUE       0x180000 | \
                                            /** REG10，控制1.8 V 核心电压调节器，低功耗的设置也可以获得好的测量精   \
                                            度,但是这个与RDC的测量速度和时序有关.请参考附录当中的例子 */          \
                                            /**  REG10_PV_CORE_CTL_STANDARD      标准                              \
                                                REG10_V_CORE_CTL_LOW_POWER      低功耗 */                          \
                                            REG10_PV_CORE_CTL_STANDARD

/** PARAM0寄存器值，不应用，设为0 */
#define PARAM0_VALUE                        0x00

/** PARAM1寄存器值，不应用，设为0 */
#define PARAM1_VALUE                        0x00

/** PARAM2寄存器值 */
#define PARAM2_PULSE_SELECT_VALUE           /** Param2，选择PDM/PWM输出的信号源 */        \
                                            /**  PARAM2_SELECT_C1_C0         C1/C0       \
                                                PARAM2_SELECT_R0_RREF       R0/R参考      \
                                                PARAM2_SELECT_R2_RREF       R2/R参考 */   \
                                                                                        \
                                            /** 单一模式 */                              \
                                            /**  PARAM2_SELECT_C2_C0         C2/C0       \
                                                PARAM2_SELECT_C3_C0         C3/C0       \
                                                PARAM2_SELECT_C4_C0         C4/C0       \
                                                PARAM2_SELECT_C5_C0         C5/C0       \
                                                PARAM2_SELECT_C6_C0         C6/C0       \
                                                PARAM2_SELECT_C7_C0         C7/C0 */    \
                                                                                        \
                                            /** 差分模式 */                              \
                                            /**  PARAM2_SELECT_C3_C2         C3/C2       \
                                                PARAM2_SELECT_C5_C4         C5/C4       \
                                                PARAM2_SELECT_C7_C6         C7/C6 */    \
                                            0x00

/** PARAM3寄存器值 */
#define PARAM3_PULSE0_SLOPE_VALUE           /** Param3，对于脉冲输出0的斜率，有符号固定点数,19位整数和4位小数 */          \
                                            /**  PARAM3_PULSE0_SLOPE_CONFIG  -524,288 到 +524,288 以 0.0625为最小单位       \
                                                PARAM3_PULSE0_SLOPE         对于脉冲输出0的斜率,                         \
                                                                        它的具体值在PARAM3_PULSE0_SLOPE_CONFIG中配置 */  \
                                            PARAM3_PULSE0_SLOPE
/** PARAM4寄存器值 */
#define PARAM4_PULSE0_OFFSET_VALUE          /** Param4，对于脉冲输出0的零点有符号固定点数, 22 整数, 1 位小数.          \
                                            这一位小数用于数学的求近似值, 不会用于输出 */                               \
                                            /**  PARAM4_PULSE0_OFFSET_CONFIG -4,194,304到 + 4,194,303.5 以 0.5为最小单位    \
                                                PARAM4_PULSE0_OFFSET        对于脉冲输出0的零点有符号固定点数,          \
                                                                        它的具体值在PARAM4_PULSE0_OFFSET_CONFIG中配置 */  \
                                            PARAM4_PULSE0_OFFSET

/** PARAM5寄存器值 */
#define PARAM5_PULSE1_SLOPE_VALUE           /** Param5，对于脉冲输出1的斜率，有符号固定点数,19位整数和4位小数 */          \
                                            /**  PARAM5_PULSE1_SLOPE_CONFIG  -524,288 到 +524,288 以 0.0625为最小单位   \
                                                PARAM5_PULSE1_SLOPE         对于脉冲输出1的斜率,                         \
                                                                        它的具体值在PARAM5_PULSE1_SLOPE_CONFIG中配置 */  \
                                            PARAM5_PULSE1_SLOPE

/** PARAM6寄存器值 */
#define PARAM6_PULSE1_OFFSET_VALUE          /** Param6，对于脉冲输出1的零点有符号固定点数, 22 整数, 1 位小数.          \
                                            这一位小数用于数学的求近似值, 不会用于输出 */                               \
                                            /**  PARAM6_PULSE1_OFFSET_CONFIG -4,194,304到 + 4,194,303.5 以 0.5为最小单位  \  
                                                PARAM6_PULSE1_OFFSET        对于脉冲输出1的零点有符号固定点数,          \
                                                                        它的具体值在PARAM6_PULSE1_OFFSET_CONFIG中配置 */  \
                                            PARAM6_PULSE1_OFFSET

/** PARAM7寄存器值 */
#define PARAM7_CAP_MEASURE_CONFIG_VALUE     /** Param7，选择单一或者差分传感器 */    \
                                            /**  PARAM7_SEL_SINGLE           单一      \
                                                PARAM7_SEL_DIFFERENTIAL     差分 */   \
                                            PARAM7_SEL_SINGLE

/** PARAM8寄存器值 */
#define PARAM8_GAIN_CORR_VALUE              /** Param8，用于内部补偿的乘法系数 无符号固定浮点数,3位整数,21位小数 */    \
                                            /**  PARAM8_GAIN_CORR_CONFIG     0 到 7.9999                              \
                                                PARAM8_GAIN_CORR            用于内部补偿的乘法系数,                    \
                                                                        它的具体值在PARAM8_GAIN_CORR_CONFIG中配置 */  \
                                            PARAM8_GAIN_CORR

/** REG20寄存器值 */
#define REG20_RUNBIT_VALUE                  /** REG20，设备开启寄存器 */                                 \
                                            /** 保护整个设备,保证在配置过程中静止,强迫DSP处于静止状态    \
                                                REG20_RUN_DISABLE                                       \
                                                开启 运行模式                                         \
                                                REG20_RUN_ENABLE    */                                  \
                                            REG20_RUN_DISABLE


/****************************************** 状态寄存器值 *********************************************/
/** 表示通信在测量当中发生; 测量质量可能会因此下降; 增加sequence time可以有帮助 */
#define PCAP_CYC_ACTIVE                     (1 << 23)
/** 表示温度测量结束; 根据所设置的模式,这一位还可以表示目前设备正在等
待一个开始命令或者触发下一次开始测量的条件 */
#define PCAP_T_END_FLAG                     (1 << 22)
/** 显示在配置寄存器20中设置的RUNBIT的状态 */
#define PCAP_RUNBIT                         (1 << 20)
/** 这是所有错误条件给出一个综合 */
#define PCAP_COMBI_ERR                      (1 << 16)
/** 显示测量溢出或者CDC的其它错误 */
#define PCAP_CAP_ERR                        (1 << 13)
/** 在 CDC 端口中的PC7端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC7                    (1 << 12)
/** 在 CDC 端口中的PC6端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC6                    (1 << 11)
/** 在 CDC 端口中的PC5端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC5                    (1 << 10)
/** 在 CDC 端口中的PC4端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC4                    (1 << 9)
/** 在 CDC 端口中的PC3端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC3                    (1 << 8)
/** 在 CDC 端口中的PC2端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC2                    (1 << 7)
/** 在 CDC 端口中的PC1端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC1                    (1 << 6)
/** 在 CDC 端口中的PC0端口有错误(可能电容太大) */
#define PCAP_CAP_ERR_PC0                    (1 << 5)
/** 显示温度测量溢出或者 RDC的其它错误 */
#define PCAP_TEMP_ERR                       (1 << 3)

/** 配置寄存器地址 */
#define CONFIG_REG0_ADDR                    0
#define CONFIG_REG1_ADDR                    1
#define CONFIG_REG2_ADDR                    2
#define CONFIG_REG3_ADDR                    3
#define CONFIG_REG4_ADDR                    4
#define CONFIG_REG5_ADDR                    5
#define CONFIG_REG6_ADDR                    6
#define CONFIG_REG7_ADDR                    7
#define CONFIG_REG8_ADDR                    8
#define CONFIG_REG9_ADDR                    9
#define CONFIG_REG10_ADDR                   10
#define CONFIG_PARAM0_ADDR                  11
#define CONFIG_PARAM1_ADDR                  12
#define CONFIG_PARAM2_ADDR                  13
#define CONFIG_PARAM3_ADDR                  14
#define CONFIG_PARAM4_ADDR                  15
#define CONFIG_PARAM5_ADDR                  16
#define CONFIG_PARAM6_ADDR                  17
#define CONFIG_PARAM7_ADDR                  18
#define CONFIG_PARAM8_ADDR                  19
#define CONFIG_REG20_ADDR                   20


/** 结果寄存器地址 */
#define RESULT_REG0_ADDR                    0
#define RESULT_REG1_ADDR                    1
#define RESULT_REG2_ADDR                    2
#define RESULT_REG3_ADDR                    3
#define RESULT_REG4_ADDR                    4
#define RESULT_REG5_ADDR                    5
#define RESULT_REG6_ADDR                    6
#define RESULT_REG7_ADDR                    7
#define RESULT_STATUS_ADDR                  8
#define RESULT_REG8_ADDR                    11          //不应用
#define RESULT_REG9_ADDR                    12          //不应用
#define RESULT_REG10_ADDR                   13
#define RESULT_REG11_ADDR                   14

/** 操作码 */
#define PCAP_WRITE_SRAM_OPT                 0x90        ///< 写SRAM操作
#define PCAP_READ_SRAM_OPT                  0x10        ///< 读SRAM操作
#define PCAP_WRITE_OTP_OPT                  0xA0        ///< 写OTP操作
#define PCAP_READ_OTP_OPT                   0x20        ///< 读OTP操作
#define PCAP_WRITE_CONFIG_OPT               0xC0        ///< 写配置操作
#define PCAP_READ_RESULT_OPT                0x40        ///< 读结果操作

#define PCAP_RESET_CMD                      0x88        ///< PCap芯片复位指令
#define PCAP_PTL_RESET_CMD                  0x8A        ///< PCap芯片部分复位指令
#define PCAP_MEASURE_CMD                    0x8C        ///< PCap芯片开始一次电容测量指令
#define PCAP_TERMINATE_WRITE2OTP_CMD        0x84        ///< PCap芯片终止write-to-OTP 的进程指令
#define PCAP_START_TEMP_MEASURE_CMD         0x8E        ///< PCap芯片开始一次温度测量指令


///< SPI片选
#define PCAP_SPI_NSS_GPIO_PIN                                GPIO_PIN_15
///< SPI片选端口
#define PCAP_SPI_NSS_GPIO_PORT                               GPIOA
///< SPI片选端口时钟
#define PCAP_SPI_NSS_GPIO_CLK()                              __HAL_RCC_GPIOA_CLK_ENABLE()

///< SPI使能
#define PCAP_SPI_ENABLE                     HAL_GPIO_WritePin(PCAP_SPI_NSS_GPIO_PORT, PCAP_SPI_NSS_GPIO_PIN, GPIO_PIN_RESET)
///< SPI禁止
#define PCAP_SPI_DISABLE                    HAL_GPIO_WritePin(PCAP_SPI_NSS_GPIO_PORT, PCAP_SPI_NSS_GPIO_PIN, GPIO_PIN_SET)


#ifndef OP_SUCCESS
#define OP_SUCCESS
#endif

#ifndef OP_FAILED
#define OP_FAILED
#endif

/**@brief       PCap芯片初始化
* @return       函数执行结果
* - None
*/
void PCap_Init(void);

/**@brief       PCap芯片复位
* @return       函数执行结果
* - None
*/
void PCap_Reset(void);

/**@brief       PCap芯片部分复位
* @return       函数执行结果
* - None
*/
void PCap_PTL_Reset(void);

/**@brief       PCap芯片开始测量
* @return       函数执行结果
* - None
*/
void PCap_Measure(void);

/**@brief       PCap芯片启动
* @return       函数执行结果
* - None
*/
void PCap_Enable(void);

/**@brief       PCap芯片停止运行
* @return       函数执行结果
* - None
*/
void PCap_Disable(void);

/**@brief       向PCap芯片写入固件
* @return       函数执行结果
* - OP_SUCCESS(写入成功)
* - OP_FAILED(写入失败)
*/
uint8_t PCap_WR_SRAM(void);

/**@brief       向PCap芯片写入配置信息
* @return       函数执行结果
* - None
*/
void PCap_Reg_Config(void);

/**@brief       读取PCap芯片状态
* @return       函数执行结果
* - PCap状态寄存器值
*/
uint32_t PCap_Res_Sta(void);

/**@brief       读取PCap读寄存器数据
* @param[in]    reg_addr : 读寄存器地址
* @return       函数执行结果
* - PCap读寄存器的数据
*/
uint32_t PCap_Res_Data(uint8_t reg_addr);

/**@brief       读取PCap芯片温度数据
* @return       函数执行结果
* - PCap温度数据
*/
uint32_t PCap_Res_Temperature(void);

#ifdef __cplusplus
}
#endif
#endif

