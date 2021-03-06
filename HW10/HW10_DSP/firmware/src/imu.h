#ifndef IMU_H__
#define IMU_H__

#define SLAVE_ADDR_WRITE 0b11010110
#define SLAVE_ADDR_READ  0b11010111

void IMU_init(void);

void IMU_read_multiple(unsigned char* data, int length, unsigned char register_start);

void IMU_accelerations(unsigned char* data, float* xl_scaled_x, float* xl_scaled_y, float* xl_scaled_z);

void IMU_print_gyro_accel(char* data, char* msg, int index);

int IMU_xl_z(unsigned char* data);

#endif