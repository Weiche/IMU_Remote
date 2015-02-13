#include <stdint.h>
#include <math.h>

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu_hal_lpc81x.h"

struct AHRP_Data{
	float pitch;
	float roll;
	float yaw;
	int statu;
}IMU_out;
#define MPU60xx_I2C_ADDR (0xD0)
#define DEFAULT_MPU_HZ (10)
int Quaternion_getPitch(float *quat)
{
  return atan2f(2*(quat[1]*quat[2] + quat[3]*quat[0]), quat[3]*quat[3] - quat[0]*quat[0] - quat[1]*quat[1] + quat[2]*quat[2])*180.0/3.14159;
}

int Quaternion_getRoll(float *quat)
{
  return asinf(-2*(quat[0]*quat[2] - quat[3]*quat[1]))*180.0/3.14159;
}

int Quaternion_getYaw(float *quat)
{
  return atan2f(2*(quat[0]*quat[1] + quat[3]*quat[2]), quat[3]*quat[3] + quat[0]*quat[0] - quat[1]*quat[1] - quat[2]*quat[2])*180.0/3.14159;
}


struct int_param_s int_param;

short gyro[3], accel[3], sensors;
unsigned char more;
long quat[4];

struct hal_s {
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned short report;
    unsigned short dmp_features;
    unsigned char motion_int_mode;
}hal;
uint16_t gyro_rate, gyro_fsr;
uint8_t accel_fsr;
struct {
	uint32_t x_down;
	uint32_t x_up;
	uint32_t y_down;
	uint32_t y_up;
	uint32_t z_down;
	uint32_t z_up;
}gesture_stat;

static void Dataready(void){
}
static void tap_cb(unsigned char direction, unsigned char count ){
	switch( direction ){
		case TAP_X_DOWN:
			gesture_stat.x_down += 1;
			break;
		case TAP_X_UP:
			gesture_stat.x_up += 1;
			break;
		case TAP_Y_DOWN:
			gesture_stat.y_down += 1;
			break;
		case TAP_Y_UP:
			gesture_stat.y_up += 1;
			break;
		case TAP_Z_DOWN:
			gesture_stat.z_down += 1;
			break;
		case TAP_Z_UP:
			gesture_stat.z_up += 1;
			break;
	}
}
void MPU6050_Init(void){
	volatile int result = 0;
	    /* Set up gyro.
     * Every function preceded by mpu_ is a driver function and can be found
     * in inv_mpu.h.
     */
    int_param.cb = Dataready;
    int_param.pin = 2;
	
    result |= mpu_init(&int_param);
	/* Get/set hardware configuration. Start gyro. */
    /* Wake up all sensors. */
    result |= mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    /* Push both gyro and accel data into the FIFO. */
    result |= mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    result |= mpu_set_sample_rate(DEFAULT_MPU_HZ);
    /* Read back configuration in case it was set improperly. */
    result |= mpu_get_sample_rate(&gyro_rate);
    result |= mpu_get_gyro_fsr(&gyro_fsr);
    result |= mpu_get_accel_fsr(&accel_fsr);

    /* Initialize HAL state variables. */
    hal.sensors = 0x11;
    hal.report = 0;
	
    result |= dmp_load_motion_driver_firmware();
    //dmp_set_orientation(
    //inv_orientation_matrix_to_scalar(gyro_orientation));
    dmp_register_tap_cb(tap_cb);
    //dmp_register_android_orient_cb(android_orient_cb);
    hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_TAP;
    result |= dmp_enable_feature(hal.dmp_features);
	
    result |= dmp_set_fifo_rate(DEFAULT_MPU_HZ);
	result |= dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);
    result |= mpu_set_dmp_state(1);
	result |= dmp_set_tap_axes(TAP_XYZ);
	result |= dmp_set_tap_count(1);
	result |= dmp_set_tap_thresh(TAP_XYZ,40);
    hal.dmp_on = 1;	
	
	dmp_register_tap_cb(tap_cb);
}

int MPU6050_DataProcess(void){
	int result = 0;
	unsigned long sensor_timestamp;
	float fquat[4];
	result = dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more);
	if(result==0){
		
		/* Heavy soft float point calculation */
		fquat[0] = quat[1] / 1073741823.0;
		fquat[1] = quat[2] / 1073741823.0;
		fquat[2] = quat[3] / 1073741823.0;
		fquat[3] = quat[0] / 1073741823.0;		
		
		IMU_out.pitch = Quaternion_getPitch(fquat);
		IMU_out.roll = Quaternion_getRoll(fquat);
		IMU_out.yaw = Quaternion_getYaw(fquat);
		IMU_out.statu = 0;

		return 0;
	}else{
		IMU_out.pitch = 0.0F;
		IMU_out.roll = 0.0F;
		IMU_out.yaw = 0.0F;
		IMU_out.statu = -1;
		return -1;
	}

}
