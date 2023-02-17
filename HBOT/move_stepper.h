#include <SpeedyStepper.h>


//const int LED_PIN = LEDB;

//const int LED_PIN = PWM0;
//const int LED_PIN_1 = PWM3;
//const int LED_PIN_2 = PWM6;

const int MOTOR_X_STEP_PIN = M0_INB;
const int MOTOR_X_DIRECTION_PIN = M0_INA;

const int MOTOR_Y_STEP_PIN = M1_INB;
const int MOTOR_Y_DIRECTION_PIN = M1_INA;


//const int MOTOR_Z_STEP_PIN = PWM7;
//const int MOTOR_Z_DIRECTION_PIN = PWM8;

// create two stepper motor objects, one for each motor
SpeedyStepper stepperX;
SpeedyStepper stepperY;
SpeedyStepper stepperZ;

int pre_z;
int pre_y;
int pre_x;

float data_pre_x = 0;
float data_pre_y = 0;
float data_pre_z = 0;

//gcode Commands;

/*
  float Velocity = 6400 * 10  ;
  float Acceleration = 6400 * 8;
*/


int abs_value_x(float data) {
  float new_data_x = (data - data_pre_x);
  data_pre_x = data;
  //Serial.println(data_pre_x);
  Serial.println(round(new_data_x));
  return round(new_data_x) ;
}

int abs_value_y(float data) {
  float new_data_y = (data - data_pre_y);
  data_pre_y = data;
  //Serial.println(data_pre_y);
  Serial.println(round(new_data_y));
  return round(new_data_y) ;
}

int abs_value_z(float data) {
  float new_data_z = (data - data_pre_z);
  data_pre_z = data;
  //Serial.println(data_pre_z);
  Serial.println(round(new_data_z));
  return round(new_data_z) ;
}

bool moveXYWithCoordination(long stepsX, long stepsY, long stepsZ, float speedInStepsPerSecond, float accelerationInStepsPerSecondPerSecond)
{
  float speedInStepsPerSecond_X;
  float accelerationInStepsPerSecondPerSecond_X;

  float speedInStepsPerSecond_Y;
  float accelerationInStepsPerSecondPerSecond_Y;

  float speedInStepsPerSecond_Z;
  float accelerationInStepsPerSecondPerSecond_Z;

  long absStepsX;
  long absStepsY;
  long absStepsZ;

  float scalerYX = 1;
  float scalerYZ = 1;
  float scalerXY = 1;
  float scalerXZ = 1;
  float scalerZX = 1;
  float scalerZY = 1;

  //
  // setup initial speed and acceleration values
  //

  speedInStepsPerSecond_X = speedInStepsPerSecond;
  accelerationInStepsPerSecondPerSecond_X = accelerationInStepsPerSecondPerSecond;

  speedInStepsPerSecond_Y = speedInStepsPerSecond;
  accelerationInStepsPerSecondPerSecond_Y = accelerationInStepsPerSecondPerSecond;

  speedInStepsPerSecond_Z = speedInStepsPerSecond;
  accelerationInStepsPerSecondPerSecond_Z = accelerationInStepsPerSecondPerSecond;

  //
  // determine how many steps each motor is moving
  //

  if (stepsX >= 0)
    absStepsX = stepsX;
  else
    absStepsX = -stepsX;

  if (stepsY >= 0)
    absStepsY = stepsY;
  else
    absStepsY = -stepsY;

  if (stepsZ >= 0)
    absStepsZ = stepsZ;
  else
    absStepsZ = -stepsZ;

  //
  // determine which motor is traveling the farthest, then slow down the
  // speed rates for the motor moving the shortest distance
  //

  if ((absStepsX > absStepsY) && (stepsX != 0))
  {
    scalerYX = (float)absStepsY / (float)absStepsX;
  }

  if ((absStepsZ > absStepsY) && (stepsZ != 0))
  {
    scalerYZ = (float)absStepsY / (float)absStepsZ;
  }

  if (scalerYX > scalerYZ)
  {
    speedInStepsPerSecond_Y = speedInStepsPerSecond_Y * scalerYZ;
    accelerationInStepsPerSecondPerSecond_Y = accelerationInStepsPerSecondPerSecond_Y * scalerYZ;
  }

  if (scalerYX < scalerYZ)
  {
    speedInStepsPerSecond_Y = speedInStepsPerSecond_Y * scalerYX;
    accelerationInStepsPerSecondPerSecond_Y = accelerationInStepsPerSecondPerSecond_Y * scalerYX;
  }



  if ((absStepsY > absStepsX) && (stepsY != 0))
  {
    scalerXY = (float)absStepsX / (float)absStepsY;
  }
  if ((absStepsZ > absStepsX) && (stepsZ != 0))
  {
    scalerXZ = (float)absStepsX / (float)absStepsZ;
  }

  if (scalerXY > scalerXZ)
  {
    speedInStepsPerSecond_X = speedInStepsPerSecond_X * scalerXZ;
    accelerationInStepsPerSecondPerSecond_X = accelerationInStepsPerSecondPerSecond_X * scalerXZ;
  }

  if (scalerXY < scalerXZ)
  {
    speedInStepsPerSecond_X = speedInStepsPerSecond_X * scalerXY;
    accelerationInStepsPerSecondPerSecond_X = accelerationInStepsPerSecondPerSecond_X * scalerXY;
  }


  if ((absStepsY > absStepsZ) && (stepsY != 0))
  {
    scalerZY = (float)absStepsZ / (float)absStepsY;
  }
  if ((absStepsX > absStepsZ) && (stepsX != 0))
  {
    scalerZX = (float)absStepsZ / (float)absStepsX;
  }

  if (scalerZY > scalerZX)
  {
    speedInStepsPerSecond_Z = speedInStepsPerSecond_Z * scalerZX;
    accelerationInStepsPerSecondPerSecond_Z = accelerationInStepsPerSecondPerSecond_Z * scalerZX;
  }

  if (scalerZY < scalerZX)
  {
    speedInStepsPerSecond_Z = speedInStepsPerSecond_Z * scalerZX;
    accelerationInStepsPerSecondPerSecond_Y = accelerationInStepsPerSecondPerSecond_Z * scalerZX;
  }

  /*
    if ((absStepsX > absStepsZ) && (stepsX != 0) || (absStepsY > absStepsZ) && (stepsY != 0))
    {
    float scaler;
    float scaler_2 = (float) absStepsZ / (float) absStepsY;
    float scaler_1 = (float) absStepsZ / (float) absStepsX;

    if (scaler_1 < scaler_2) {
      scaler = scaler_1 ;
    }
    if (scaler_1 > scaler_2) {
      scaler = scaler_2 ;
    } else {
      scaler = 1 ;
    }
    speedInStepsPerSecond_Z = speedInStepsPerSecond_Z * scaler;
    accelerationInStepsPerSecondPerSecond_Z = accelerationInStepsPerSecondPerSecond_Z * scaler;

    }

    if ((absStepsX > absStepsY) && (stepsX != 0) || (absStepsZ > absStepsY) && (stepsZ != 0))
    {
    float scaler;
    float scaler_2 = (float) absStepsY / (float) absStepsZ;
    float scaler_1 = (float) absStepsY / (float) absStepsX;
    if (scaler_1 < scaler_2) {
      scaler = scaler_1 ;
    }
    if (scaler_1 > scaler_2) {
      scaler = scaler_2 ;
    }
    else {
      scaler = 1 ;
    }
    speedInStepsPerSecond_Y = speedInStepsPerSecond_Y * scaler;
    accelerationInStepsPerSecondPerSecond_Y = accelerationInStepsPerSecondPerSecond_Y * scaler;
    }

    if ((absStepsY > absStepsX) && (stepsY != 0) || (absStepsZ > absStepsX) && (stepsZ != 0))
    {
    float scaler;
    float scaler_2 =  (float) absStepsX / (float) absStepsZ;
    float scaler_1 = (float) absStepsX / (float) absStepsY;

    if (scaler_1 < scaler_2) {
      scaler = scaler_1 ;
    }

    if (scaler_1 > scaler_2) {
      scaler = scaler_2 ;
    } else {
      scaler = 1 ;
    }
    speedInStepsPerSecond_X = speedInStepsPerSecond_X * scaler;
    accelerationInStepsPerSecondPerSecond_X = accelerationInStepsPerSecondPerSecond_X * scaler;

    }
  */
  //
  // setup the motion for the X motor
  //

  stepperX.setSpeedInStepsPerSecond(speedInStepsPerSecond_X);
  stepperX.setAccelerationInStepsPerSecondPerSecond(accelerationInStepsPerSecondPerSecond_X);
  stepperX.setupRelativeMoveInSteps(stepsX);

  //
  // setup the motion for the Y motor
  //

  stepperY.setSpeedInStepsPerSecond(speedInStepsPerSecond_Y);
  stepperY.setAccelerationInStepsPerSecondPerSecond(accelerationInStepsPerSecondPerSecond_Y);
  stepperY.setupRelativeMoveInSteps(stepsY);

  stepperZ.setSpeedInStepsPerSecond(speedInStepsPerSecond_Z);
  stepperZ.setAccelerationInStepsPerSecondPerSecond(accelerationInStepsPerSecondPerSecond_Z);
  stepperZ.setupRelativeMoveInSteps(stepsZ);

  //
  // now execute the moves, looping until both motors have finished
  //

  while ((!stepperX.motionComplete()) || (!stepperY.motionComplete()) || (!stepperZ.motionComplete()))
  {
    stepperX.processMovement();
    stepperY.processMovement();
    stepperZ.processMovement();
  }
  return true;
}
