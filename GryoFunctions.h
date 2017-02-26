#ifndef __GYRO__
#define __GYRO__

// Gyro angle values (left/right aka roll, up/down aka pitch, spin left/right aka yaw)
struct gyroVectors {
	int gyro_x;
	int gyro_y;
	int gyro_z;
};

// Gyro offsets (at rest values subtracted from our readings)
struct gyroOffsets {
	long gyro_x_cal;
	long gyro_y_cal;
	long gyro_z_cal;
};

gyroVectors gV;
gyroOffsets gO;

boolean set_gyro_angles;

long acc_x, acc_y, acc_z, acc_total_vector;
float angle_roll_acc, angle_pitch_acc;

float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
float angle_pitch_output, angle_roll_output;

int temperature;


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void configureMPU6050() {
	//Activate the MPU-6050
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
	Wire.write(0x6B);                                                    //Send the requested starting register
	Wire.write(0x00);                                                    //Set the requested starting register
	Wire.endTransmission();
	//Configure the accelerometer (+/-8g)
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
	Wire.write(0x1C);                                                    //Send the requested starting register
	Wire.write(0x10);                                                    //Set the requested starting register
	Wire.endTransmission();
	//Configure the gyro (500dps full scale)
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
	Wire.write(0x1B);                                                    //Send the requested starting register
	Wire.write(0x08);                                                    //Set the requested starting register
	Wire.endTransmission();
}

//-----------------------------------------------------------------------------------
// Subroutine for reading the raw gyro and accelerometer data
//-----------------------------------------------------------------------------------
void read_mpu_6050_data() {
	Wire.beginTransmission(0x68);	//Start communicating with the MPU-6050
	Wire.write(0x3B);               //Send the requested starting register
	Wire.endTransmission();         //End the transmission
	Wire.requestFrom(0x68, 14);     //Request 14 bytes from the MPU-6050
	while (Wire.available() < 14);  //Wait until all the bytes are received
	acc_x = Wire.read() << 8 | Wire.read();
	acc_y = Wire.read() << 8 | Wire.read();
	acc_z = Wire.read() << 8 | Wire.read();
	temperature = Wire.read() << 8 | Wire.read();
	gV.gyro_x = Wire.read() << 8 | Wire.read();
	gV.gyro_y = Wire.read() << 8 | Wire.read();
	gV.gyro_z = Wire.read() << 8 | Wire.read();
}

//-----------------------------------------------------------------------------------
// This is the nitty-gritty angle calculation from the MPU6060
//-----------------------------------------------------------------------------------
float calcGyroAngles () {
	read_mpu_6050_data();
	//Subtract the offset values from the raw gyro values
	gV.gyro_x -= gO.gyro_x_cal;
	gV.gyro_y -= gO.gyro_y_cal;
	gV.gyro_z -= gO.gyro_z_cal;

	//Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5)
	angle_pitch += gV.gyro_x * 0.0000611;     //Calculate the traveled pitch angle and add this to the angle_pitch variable
	angle_roll += gV.gyro_y * 0.0000611;        //Calculate the traveled roll angle and add this to the angle_roll variable

	//0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
	angle_pitch += angle_roll * sin(gV.gyro_z * 0.000001066); //If the IMU has yawed transfer the roll angle to the pitch angel
	angle_roll -= angle_pitch * sin(gV.gyro_z * 0.000001066); //If the IMU has yawed transfer the pitch angle to the roll angel

	//Accelerometer angle calculations
	acc_total_vector = sqrt((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z)); //Calculate the total accelerometer vector
	//57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
	angle_pitch_acc = asin((float) acc_y / acc_total_vector) * 57.296;       //Calculate the pitch angle
	angle_roll_acc = asin((float) acc_x / acc_total_vector) * -57.296;       //Calculate the roll angle

	//Serial.print("Pitch:");Serial.print(angle_pitch_acc);
	//Serial.print("\tRoll:");Serial.println(angle_roll_acc);
	//delay(1000);

	angle_pitch_acc -= -0.62;                                         //Accelerometer calibration value for pitch
	angle_roll_acc -= -1.77;                                          //Accelerometer calibration value for roll

	if (set_gyro_angles) {                                                 //If the IMU is already started
		angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004; //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
		angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004; //Correct the drift of the gyro roll angle with the accelerometer roll angle
	}
	else {                                                                //At first start
		angle_pitch = angle_pitch_acc;                 //Set the gyro pitch angle equal to the accelerometer pitch angle
		angle_roll = angle_roll_acc;                     //Set the gyro roll angle equal to the accelerometer roll angle
		set_gyro_angles = true;                                            //Set the IMU started flag
	}

	//To dampen the pitch and roll angles a complementary filter is used
	angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1; //Take 90% of the output pitch value and add 10% of the raw pitch value
	angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1; //Take 90% of the output roll value and add 10% of the raw roll value
	return angle_pitch_output;
}

//-----------------------------------------------------------------------------------
// One-off reading of angles at rest so we can correctly adjust future measurements
//-----------------------------------------------------------------------------------
void calcGyroOffsets() {
	Serial.print(F("\nCalibrating"));

	for (int cal_int = 0; cal_int < 2000; cal_int++) { //Read the raw acc and gyro data from the MPU-6050 for 1000 times
		read_mpu_6050_data();
		gO.gyro_x_cal += gV.gyro_x;                                         //Add the gyro x offset to the gyro_x_cal variable
		gO.gyro_y_cal += gV.gyro_y;                                         //Add the gyro y offset to the gyro_y_cal variable
		gO.gyro_z_cal += gV.gyro_z;                                         //Add the gyro z offset to the gyro_z_cal variable
		delay(3);                                                          //Delay 3us to have 250Hz for-loop
		if (cal_int % 100 == 0) Serial.print(".");
	}
	Serial.println("");

	// Get average
	gO.gyro_x_cal /= 2000;
	gO.gyro_y_cal /= 2000;
	gO.gyro_z_cal /= 2000;

	// Offsets
	Serial.print("X Offset: ");
	Serial.println(gO.gyro_x_cal);
	Serial.print("Y Offset: ");
	Serial.println(gO.gyro_y_cal);
	Serial.print("Z Offset: ");
	Serial.println(gO.gyro_z_cal);

	// Give time to read this
	delay(1000);
}

#endif
