#include "bsp_tim_pwm.h"
#include "bsp_rc.h"
#include "math_extend.h"
#include "stdbool.h"
#include "bsp_gpio.h"
#include "AHRS.h"
#include "PID_Attitude.h"
#include "main.h"

uint16_t Rc_PWMCount[CHANNEL_MAX];
uint8_t CAPTURE_STA;  								// 8位PWM捕获状态。 1：有新数据，0：没有数据
MOTOR motor = {.enable = false};
/*****************************************
* 	函 数 名: PWM_Out_Init		  				 *
* 	功能说明: 初始化PWM输出     				 *
*   形    参：频率											 *
* 	返 回 值: 频率是否越界							 *
*****************************************/
bool PWM_Out_Init()
{
	// 此部分需手动修改IO口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t PrescalerValue = 0;
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  //TIM模块设为默认值
	TIM_OCStructInit(&TIM_OCInitStructure);					 //TIM通道设为默认值
	
	uint32_t hz_set = ACCURACY * TIM_PWM_OUT_FREQ;
	
	hz_set = LIMIT(hz_set, ACCURACY, TIM_PWM_MAX_FREQ * ACCURACY); // 频率限幅

  #if TIM1_ENABLE	
	/*******	TIM1  CH1_E9	CH2_E11 CH3_E13 CH4_E14			*******/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // TIM1时钟使能  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // 使能PORTE时钟	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14; // 对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        // 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // 上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);              // 初始化Pin_E口
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1); // GPIOE9复用为定时器1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1); // GPIOE11复用为定时器1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1); // GPIOE13复用为定时器1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1); // GPIOE14复用为定时器1
	
	// Compute the prescaler value
	PrescalerValue = (uint16_t)((84000000 * 2) / hz_set) - 1; // 分频系数（当分频系数大于1时，APB2时钟频率会放大2倍）
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;  //定时器分频
	TIM_TimeBaseStructure.TIM_Period = ACCURACY;   //自动重装载值
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频 不分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//重复计数器寄存器(TIM1和TIM8中才设置)
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//初始化定时器1
	
	// 初始化TIM1 Channel PWM模式	  
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;		
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
	TIM_OCInitStructure.TIM_Pulse = INIT_DUTY;  
	
	// 使能通道1
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  // 根据T指定的参数初始化外设TIM1 OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  // 使能TIM1在CCR1上的预装载寄存器
	
	// 使能通道2
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  // 根据T指定的参数初始化外设TIM1 OC2
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  // 使能TIM1在CCR1上的预装载寄存器
	
	// 使能通道3
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);  // 根据T指定的参数初始化外设TIM1 OC3
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  // 使能TIM1在CCR1上的预装载寄存器
	
	// 使能通道4
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  // 根据T指定的参数初始化外设TIM1 OC4
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  // 使能TIM1在CCR1上的预装载寄存器
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE); // TIM1主输出使能（TIM1与TIM8专有寄存器）
	TIM_ARRPreloadConfig(TIM1, ENABLE);// ARPE使能（自动重载预装载使能）
	TIM_Cmd(TIM1, ENABLE);  // 使能TIM1
	
  #endif	

  #if TIM5_ENABLE
	/*******	TIM5  CH1_A0	CH2_A1 CH3_A2 CH4_A3					*******/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM5时钟使能  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 	//使能PORTA时钟	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //GPIOA0复用为定时器5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5); //GPIOA1复用为定时器5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM5); //GPIOA2复用为定时器5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM5); //GPIOA3复用为定时器5
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //初始化PA口
	
	// Compute the prescaler value
	PrescalerValue = (uint16_t)((42000000 * 2) / hz_set) - 1; // 分频系数（当分频系数大于1时，APB2时钟频率会放大2倍）
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue; //PrescalerValue;  //定时器分频
	TIM_TimeBaseStructure.TIM_Period = ACCURACY;   //自动重装载值
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频 不分频
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);//初始化定时器5
	
	// 初始化TIM5 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = INIT_DUTY;
	
	// 使能通道1
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 1OC1
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1上的预装载寄存器
	
	// 使能通道2
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 1OC1
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1上的预装载寄存器
	
	// 使能通道3
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 1OC1
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1上的预装载寄存器
	
	// 使能通道4
	TIM_OC4Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 1OC1
	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1上的预装载寄存器
	
	TIM_ARRPreloadConfig(TIM5,ENABLE);//ARPE使能 
	TIM_Cmd(TIM5, ENABLE);  //使能TIM5
#endif
	return hz_set <ACCURACY || hz_set > TIM_PWM_MAX_FREQ * ACCURACY;
} 
float action_partial[MOTORS_MAX];
void MOTOR_Ctrl(float throttle)
{
	float out_pitch = ctrl_rate.out.pitch;	// 向上抬头，out_pitch为正值
	float out_roll = ctrl_rate.out.roll; 		// 向右横滚，out_roll为正值
	float out_yaw = ctrl_rate.out.yaw;			// 向左逆时针旋转，out_yaw为正值
	
  if(mav.system_status == MAV_STATE_ACTIVE) // 解锁
	{
		
		LED1_ON;
		out_yaw = LIMIT(out_yaw, -MAX_ADJUST, MAX_ADJUST);
		action_partial[0] = LIMIT((- out_roll + out_pitch - out_yaw), -MAX_ADJUST, MAX_ADJUST);
		action_partial[1] = LIMIT((+ out_roll - out_pitch - out_yaw), -MAX_ADJUST, MAX_ADJUST);
		action_partial[2] = LIMIT((+ out_roll + out_pitch + out_yaw), -MAX_ADJUST, MAX_ADJUST);
		action_partial[3] = LIMIT((- out_roll - out_pitch + out_yaw), -MAX_ADJUST, MAX_ADJUST);

		float tilt_compensation = __cos(status.attitude.roll) * __cos(status.attitude.pitch);
		
		motor.motor_raw.motor_1 = LIMIT((action_partial[0] + throttle) / tilt_compensation, (READY_SPEED), (MAX_PWM));
		motor.motor_raw.motor_2 = LIMIT((action_partial[1] + throttle) / tilt_compensation, (READY_SPEED), (MAX_PWM));
		motor.motor_raw.motor_3 = LIMIT((action_partial[2] + throttle) / tilt_compensation, (READY_SPEED), (MAX_PWM));
		motor.motor_raw.motor_4 = LIMIT((action_partial[3] + throttle) / tilt_compensation, (READY_SPEED), (MAX_PWM));
	}
	else
	{
		LED1_OFF;
		motor.motor_raw.motor_1 = motor.motor_raw.motor_2 = motor.motor_raw.motor_3 = motor.motor_raw.motor_4 = 0;
	}
	motor.motor_pwm.motor_1 = PWM_RADIO * (motor.motor_raw.motor_1 + INIT_DUTY);
	motor.motor_pwm.motor_2 = PWM_RADIO * (motor.motor_raw.motor_2 + INIT_DUTY);
	motor.motor_pwm.motor_3 = PWM_RADIO * (motor.motor_raw.motor_3 + INIT_DUTY);
	motor.motor_pwm.motor_4 = PWM_RADIO * (motor.motor_raw.motor_4 + INIT_DUTY);
	
	if(motor.enable)
	{
		TIM_SetCompare1(TIM1, motor.motor_pwm.motor_4);	// 修改比较值，修改占空比
		TIM_SetCompare2(TIM1, motor.motor_pwm.motor_3); // 修改比较值，修改占空比
		TIM_SetCompare3(TIM1, motor.motor_pwm.motor_2);	// 修改比较值，修改占空比
		TIM_SetCompare4(TIM1, motor.motor_pwm.motor_1);	// 修改比较值，修改占空比
	}
	else
	{
		TIM_SetCompare1(TIM1, PWM_RADIO * INIT_DUTY);	// 修改比较值，修改占空比
		TIM_SetCompare2(TIM1, PWM_RADIO * INIT_DUTY); // 修改比较值，修改占空比
		TIM_SetCompare3(TIM1, PWM_RADIO * INIT_DUTY);	// 修改比较值，修改占空比
		TIM_SetCompare4(TIM1, PWM_RADIO * INIT_DUTY);	// 修改比较值，修改占空比
	}
}

/*****************************************************
* 	函 数 名: TIM_Cap_Init		  				             *
* 	功能说明: 初始化PWM捕获     				             *
*   形    参：void											             *
* 	返 回 值: void											             *
*   其    它：使用TIM3、TIM4进行捕获                 *
*   					TIM3：CH1_B4.CH2_B5.CH3_B0.CH4_B1      *
*   					TIM4	CH1_D12.CH2_D13.CH3_D14.CH4_D15  *
*****************************************************/
void TIM_Cap_Init()
{
	#if CHANNEL_MAX > 0
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_Cmd(TIM3, DISABLE); 
		
	/*****  TIM3	CH1_B4.CH2_B5.CH3_B0.CH4_B1	*****/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // TIM3时钟使能  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // 使能PORTB时钟	

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10; // 抢占优先级10
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	// 根据指定的参数初始化VIC寄存器
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5; //GPIOB0.1.4.5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // 下拉

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3); // PB0复用位定时器3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3); // PB1复用位定时器3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3); // PB4复用位定时器3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3); // PB5复用位定时器3
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB0

	TIM_TimeBaseStructure.TIM_Prescaler = TIM_CAP_IN_PRESC; // 定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // 自动重装载值（16位计数器）
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	#endif
	
	#if CHANNEL_MAX > 0
	// 初始化TIM3_CH1输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; // CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 // 配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F; // IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); //允许更新中断 ,允许CC1IE捕获中断	
	#endif
	
	#if CHANNEL_MAX > 1
	// 初始化TIM3_CH2输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; // CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 // 配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F;// IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE); //允许更新中断 ,允许CC2IE捕获中断	
	#endif
	
	#if CHANNEL_MAX > 2	
	// 初始化TIM3_CH3输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3; // CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 // 配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F;// IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE); //允许更新中断 ,允许CC3IE捕获中断	
	#endif
	
	#if CHANNEL_MAX > 3
	// 初始化TIM3_CH4输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4; // CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 // 配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F; // IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE); //允许更新中断 ,允许CC4IE捕获中断	
	#endif

	#if CHANNEL_MAX > 0
	TIM_Cmd(TIM3, ENABLE); 	// 使能定时器3
	#endif
	
	#if	CHANNEL_MAX > 4
	/*****  TIM4	CH4_D15  CH3_D14  CH2_D13  CH1_D12	*****/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM4时钟使能  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //使能PORTD时钟	

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; //GPIOB0.1.4.5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4); //PD12复用位定时器4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4); //PD13复用位定时器4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_TIM4); //PD14复用位定时器4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4); //PD15复用位定时器4
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD

	TIM_TimeBaseStructure.TIM_Prescaler = TIM_CAP_IN_PRESC;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;   // 自动重装载值（16位计数器）
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);

	#endif
	
	#if	CHANNEL_MAX > 4
		//初始化TIM4_CH4输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F; // IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ITConfig(TIM4, TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断
  #endif
	
	#if	CHANNEL_MAX > 5
	//初始化TIM4_CH3输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F; // IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
  TIM_ITConfig(TIM4, TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC3IE捕获中断
	#endif
	
	#if	CHANNEL_MAX > 6
	//初始化TIM4_CH2输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F; // IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ITConfig(TIM4, TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC2IE捕获中断	
	#endif
	
	#if	CHANNEL_MAX > 7
//初始化TIM4_CH1输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	// 双边沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x0F; // IC1F=000F 配置输入滤波器
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
  TIM_ITConfig(TIM4, TIM_IT_CC1,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
  #endif
	
	#if	CHANNEL_MAX > 4
	TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4
	#endif
}

/*********************************  遥控器输入捕获  **********************************/
static uint16_t LAST_CAPTURE_RISING_VAL[CHANNEL_MAX]; // 最后一次捕获到上升沿时的计数值
static uint16_t LAST_CAPTURE_FALLING_VAL[CHANNEL_MAX]; // 最后一次捕获到下降沿时的计数值
//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		
	#if CHANNEL_MAX > 0
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)  // 捕获1发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); //清除1通道的中断标志位
		if(GPIOB->IDR & GPIO_Pin_4) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[0] = TIM_GetCapture1(TIM3); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[0] = TIM_GetCapture1(TIM3); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[0] < LAST_CAPTURE_RISING_VAL[0]) // 计时器是否溢出
			{
				Rc_PWMCount[0] = LAST_CAPTURE_FALLING_VAL[0] - LAST_CAPTURE_RISING_VAL[0] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[0] = LAST_CAPTURE_FALLING_VAL[0] - LAST_CAPTURE_RISING_VAL[0];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<0;
		}
	}
	#endif
		
	#if CHANNEL_MAX > 1
	if(TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)  // 捕获2发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2); //清除2通道的中断标志位
		if(GPIOB->IDR & GPIO_Pin_5) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[1] = TIM_GetCapture2(TIM3); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[1] = TIM_GetCapture2(TIM3); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[1] < LAST_CAPTURE_RISING_VAL[1]) // 计时器是否溢出
			{
				Rc_PWMCount[1] = LAST_CAPTURE_FALLING_VAL[1] - LAST_CAPTURE_RISING_VAL[1] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[1] = LAST_CAPTURE_FALLING_VAL[1] - LAST_CAPTURE_RISING_VAL[1];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<1;
		}
	}
	#endif
		
	#if CHANNEL_MAX > 2
	if(TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)  // 捕获3发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3); //清除3通道的中断标志位
		if(GPIOB->IDR & GPIO_Pin_0) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[2] = TIM_GetCapture3(TIM3); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[2] = TIM_GetCapture3(TIM3); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[2] < LAST_CAPTURE_RISING_VAL[2]) // 计时器是否溢出
			{
				Rc_PWMCount[2] = LAST_CAPTURE_FALLING_VAL[2] - LAST_CAPTURE_RISING_VAL[2] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[2] = LAST_CAPTURE_FALLING_VAL[2] - LAST_CAPTURE_RISING_VAL[2];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<2;
		}
	}
	#endif
		
	#if CHANNEL_MAX > 3
	if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)  // 捕获4发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC4); //清除4通道的中断标志位
		if(GPIOB->IDR & GPIO_Pin_1) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[3] = TIM_GetCapture4(TIM3); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[3] = TIM_GetCapture4(TIM3); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[3] < LAST_CAPTURE_RISING_VAL[3]) // 计时器是否溢出
			{
				Rc_PWMCount[3] = LAST_CAPTURE_FALLING_VAL[3] - LAST_CAPTURE_RISING_VAL[3] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[3] = LAST_CAPTURE_FALLING_VAL[3] - LAST_CAPTURE_RISING_VAL[3];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<3;
		}
	}
	#endif
}

//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		
	#if CHANNEL_MAX > 4
	if(TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)  // 捕获4发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC4); //清除4通道的中断标志位
		if(GPIOD->IDR & GPIO_Pin_15) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[4] = TIM_GetCapture4(TIM4); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[4] = TIM_GetCapture4(TIM4); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[4] < LAST_CAPTURE_RISING_VAL[4]) // 计时器是否溢出
			{
				Rc_PWMCount[4] = LAST_CAPTURE_FALLING_VAL[4] - LAST_CAPTURE_RISING_VAL[4] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[4] = LAST_CAPTURE_FALLING_VAL[4] - LAST_CAPTURE_RISING_VAL[4];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<4;
		}
	}
	#endif
		
	#if CHANNEL_MAX > 5
	if(TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)  // 捕获3发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC3); //清除3通道的中断标志位
		if(GPIOD->IDR & GPIO_Pin_14) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[5] = TIM_GetCapture3(TIM4); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[5] = TIM_GetCapture3(TIM4); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[5] < LAST_CAPTURE_RISING_VAL[5]) // 计时器是否溢出
			{
				Rc_PWMCount[5] = LAST_CAPTURE_FALLING_VAL[5] - LAST_CAPTURE_RISING_VAL[5] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[5] = LAST_CAPTURE_FALLING_VAL[5] - LAST_CAPTURE_RISING_VAL[5];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<5;
		}
	}
  #endif
		
	#if CHANNEL_MAX > 6
	if(TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)  // 捕获2发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2); //清除2通道的中断标志位
		if(GPIOD->IDR & GPIO_Pin_13) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[6] = TIM_GetCapture2(TIM4); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[6] = TIM_GetCapture2(TIM4); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[6] < LAST_CAPTURE_RISING_VAL[6]) // 计时器是否溢出
			{
				Rc_PWMCount[6] = LAST_CAPTURE_FALLING_VAL[6] - LAST_CAPTURE_RISING_VAL[6] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[6] = LAST_CAPTURE_FALLING_VAL[6] - LAST_CAPTURE_RISING_VAL[6];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<6;
		}
	}
	#endif
		
	#if CHANNEL_MAX > 7
	if(TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)  // 捕获1发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1); //清除1通道的中断标志位
		if(GPIOD->IDR & GPIO_Pin_12) // 标记捕获到了上升沿
		{
			LAST_CAPTURE_RISING_VAL[7] = TIM_GetCapture1(TIM4); // 记录最后一次上升沿计数
		}
		else
		{
			LAST_CAPTURE_FALLING_VAL[7] = TIM_GetCapture1(TIM4); // 记录最后一次下降沿计数
			if(LAST_CAPTURE_FALLING_VAL[7] < LAST_CAPTURE_RISING_VAL[6]) // 计时器是否溢出
			{
				Rc_PWMCount[7] = LAST_CAPTURE_FALLING_VAL[7] - LAST_CAPTURE_RISING_VAL[7] + 0xFFFF +1 ; // 得到总的高电平时间
			} 
			else 
			{
				Rc_PWMCount[7] = LAST_CAPTURE_FALLING_VAL[7] - LAST_CAPTURE_RISING_VAL[7];  // 得到总的高电平时间
			}
			CAPTURE_STA |= 1<<7;
		}
	}
  #endif
}

/*********************************** 飞航科技 (END OF FILE) ***************************************/

