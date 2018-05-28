#ifndef IMU_H__
#define IMU_H__

#define SLAVE_ADDR_WRITE 0b11010110
#define SLAVE_ADDR_READ  0b11010111

void initIMU(void);

void IMU_read_multiple(unsigned char* data, int length, unsigned char register_start);

void IMU_accelerations(unsigned char* data, float* xl_scaled_x, float* xl_scaled_y, float* xl_scaled_z);

int IMU_mouse_x(unsigned char* data, int scale);

int IMU_mouse_y(unsigned char* data, int scale);

#endif