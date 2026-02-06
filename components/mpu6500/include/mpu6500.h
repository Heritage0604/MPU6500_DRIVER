#ifndef MPU6500_H
#define MPU6500_H

#include <stdint.h>
#include "esp_err.h"

typedef enum {
    MPU_GYRO_250DPS  = 0,
    MPU_GYRO_500DPS  = 1,
    MPU_GYRO_1000DPS = 2,
    MPU_GYRO_2000DPS = 3
} mpu_gyro_fs_t;

typedef enum {
    MPU_ACCEL_2G  = 0,
    MPU_ACCEL_4G  = 1,
    MPU_ACCEL_8G  = 2,
    MPU_ACCEL_16G = 3
} mpu_accel_fs_t;

typedef struct {
    int i2c_port;
    uint8_t addr;

    // store chosen full-scale settings
    mpu_gyro_fs_t  gyro_fs;
    mpu_accel_fs_t accel_fs;
} mpu6500_t;

typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    int16_t temp;
} mpu6500_raw_t;

typedef struct {
    float ax_g, ay_g, az_g;
    float gx_dps, gy_dps, gz_dps;
    float temp_c;
} mpu6500_scaled_t;

esp_err_t mpu6500_init(mpu6500_t *dev);
esp_err_t mpu6500_set_gyro_fs(mpu6500_t *dev, mpu_gyro_fs_t fs);
esp_err_t mpu6500_set_accel_fs(mpu6500_t *dev, mpu_accel_fs_t fs);

esp_err_t mpu6500_read_whoami(mpu6500_t *dev, uint8_t *whoami);
esp_err_t mpu6500_read_raw(mpu6500_t *dev, mpu6500_raw_t *out);
esp_err_t mpu6500_read_scaled(mpu6500_t *dev, mpu6500_scaled_t *out);

#endif
