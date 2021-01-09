/*
 * servos.c
 */

#include "servos.h"

/**
 * @brief Initializes the struct Servo
 * @param Servo* servo: struct of the servo motor
 * @param TIM_HandleTypeDef* tim: struct of the tim handler
 * @param uint32_t channel: channel of the tim
 */
void initServo(Servo* servo, TIM_HandleTypeDef* tim, uint32_t channel){

	servo->tim = tim;
	servo->channel = channel;
	servo->currentAngle = 0;
	HAL_TIM_PWM_Start(servo->tim,servo->channel);
}

/*
 * @brief Sets the servo at a certain angle
 * @param Servo* servo: struct of the servo motor
 * @param uint16_t angle: angle the servo has to be set
 */
void setAngle(Servo* servo, uint16_t angle){
	if(flagSELECT == 0) {
		uint16_t pulse = (MIN_PULSE+angle*(MAX_PULSE-MIN_PULSE)/180);
			__HAL_TIM_SET_COMPARE(servo->tim, servo->channel, pulse);
			servo->currentAngle = angle;
	}
}

/**
 * @brief Sets the servo at a certain angle at a certain speed
 * @param Servo* servo: struct of the servo motor
 * @param uint16_t angle: angle the servo has to be set
 * @param float speed: speed at which the servo sets its angle
 */
void moveToAngle(Servo* servo, uint16_t angle, float speed){

	for(int i = 0; i<=angle; i+=speed){
		setAngle(servo,i);
	}
	servo->currentAngle = angle;
}

/**
 * @brief Stops the PWM of the servo motor
 */
void stopServo(Servo* servo){

	HAL_TIM_PWM_Stop(servo->tim,servo->channel);
}

/**
 * @brief Makes the belly move at a certain angle
 * @param uint16_t end_angle: the angle the belly has to be set
 */
void bellyRotation(uint16_t end_angle){
	if(s1.currentAngle<end_angle){
		for(int i=s1.currentAngle; i<=end_angle; i+=4){
			setAngle(&s1,i);
			HAL_Delay(100);
		}
	}else if(s1.currentAngle>end_angle){
		for(int i=s1.currentAngle; i>=end_angle; i-=4){
			setAngle(&s1,i);
			HAL_Delay(100);
		}
	}
}

/**
 * @brief Make both arms move with the following routine
 */
void armsMovement(void){
	  armsSmoothRotation(15,135,2,2);
	  HAL_Delay(5000);

	  armsSmoothRotation(150,45,2,2);
	  HAL_Delay(1000);

	  armsSmoothRotation(150,80,1,1);
	  HAL_Delay(2000);

	  armsSmoothRotation(80,150,2,1);
	  HAL_Delay(100);

	  armsSmoothRotation(38,180,1,1);
	  HAL_Delay(3000);

	  armsSmoothRotation(15,135,3,3);
	  HAL_Delay(1000);

}

/**
 * @brief Make both arms move to two angles at different speeds
 * @param uint16_t endAngle1: final angle of the elbows
 * @param uin16_t endAngle2: final angle of the shoulders
 * @param int vel1: speed of the elbows
 * @param float vel2: speed of the shoulders
 */
void armsSmoothRotation(uint16_t endAngle1, uint16_t endAngle2, int vel1, float vel2){
	int i = s2.currentAngle;
	int j = s3.currentAngle;

	if(s2.currentAngle<=endAngle1 && s3.currentAngle<=endAngle2){
		while(i<=endAngle1 && j<=endAngle2){

			//--Moving Left Arm--//
			setAngle(&s2,i);
			setAngle(&s3,j);

			//--Moving Right Arm--//
			setAngle(&s4,180 - i);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			i+=vel1;
			j+=vel2;
		}
		while(i<=endAngle1){
			//--Continuing Moving Elbows--//
			setAngle(&s2,i);
			setAngle(&s4,180 - i);
			HAL_Delay(100);
			i+=vel1;
		}
		while(j<=endAngle2){
			//--Continuing Moving Shoulders--//
			setAngle(&s3,j);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			j+=vel2;
		}
	}else if(s2.currentAngle<=endAngle1 && s3.currentAngle>endAngle2){
		while(i<=endAngle1 && j>endAngle2){

			//Moving Left Arm--//
			setAngle(&s2,i);
			setAngle(&s3,j);

			//--Moving Right Arm--//
			setAngle(&s4,180 - i);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			i+=vel1;
			j-=vel2;
		}
		while(i<=endAngle1){
			//--Continuing Moving Elbows--//
			setAngle(&s2,i);
			setAngle(&s4,180 - i);
			HAL_Delay(100);
			i+=vel1;
		}
		while(j>endAngle2){
			//--Continuing Moving Shoulders--//
			setAngle(&s3,j);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			j-=vel2;
		}
	}else if(s2.currentAngle>endAngle1 && s3.currentAngle<=endAngle2){
		while(i>endAngle1 && j<=endAngle2){

			//--Moving Left Arm--//
			setAngle(&s2,i);
			setAngle(&s3,j);

			//Moving Right Arm--//
			setAngle(&s4,180 - i);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			i-=vel1;
			j+=vel2;
		}
		while(i>endAngle1){
			//--Continuing Moving Elbows--//
			setAngle(&s2,i);
			setAngle(&s4,180 - i);
			HAL_Delay(100);
			i-=vel1;
		}
		while(j<=endAngle2){
			//--Continuing Moving Shoulders--//
			setAngle(&s3,j);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			j+=vel2;
		}
	}else if(s2.currentAngle>endAngle1 && s3.currentAngle>endAngle2){
		while(i>endAngle1 && j>endAngle2){

			//--Moving Left Arm--//
			setAngle(&s2,i);
			setAngle(&s3,j);

			//--Moving Right Arm--//
			setAngle(&s4,180 - i);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			i-=vel1;
			j-=vel2;
		}
		while(i>endAngle1){
			//--Continuing Moving Elbows--//
			setAngle(&s2,i);
			setAngle(&s4,180 - i);
			HAL_Delay(100);
			i-=vel1;
		}
		while(j>endAngle2){
			//--Continuing Moving Shoulders--//
			setAngle(&s3,j);
			setAngle(&s5,180 - j);
			HAL_Delay(100);
			j-=vel2;
		}
	}
}
