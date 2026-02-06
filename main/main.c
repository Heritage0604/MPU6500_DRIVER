#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_err.h"      // for ESP_ERROR_CHECK and esp_err_t
#include "mpu6500.h"

#define I2C_PORT I2C_NUM_0
#define I2C_SDA  21
#define I2C_SCL  22
#define I2C_FREQ 400000

static void i2c_init(void)
{
    i2c_config_t cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &cfg));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, cfg.mode, 0, 0, 0));
}

void app_main(void)
{
    i2c_init();

    mpu6500_t imu = {
        .i2c_port = I2C_PORT,
        .addr = 0x68, // change to 0x69 if AD0 is high
        // if your struct stores defaults:
        // .gyro_fs = MPU_GYRO_250DPS,
        // .accel_fs = MPU_ACCEL_2G,
    };

    // Init sensor (wake + default config in your driver)
    esp_err_t e = mpu6500_init(&imu);
    if (e != ESP_OK) {
        printf("MPU init failed: %s\n", esp_err_to_name(e));
    }

    // Set desired full-scale ranges
    ESP_ERROR_CHECK(mpu6500_set_gyro_fs(&imu, MPU_GYRO_500DPS));
    ESP_ERROR_CHECK(mpu6500_set_accel_fs(&imu, MPU_ACCEL_8G));

    // Read WHO_AM_I
    uint8_t who = 0;
    e = mpu6500_read_whoami(&imu, &who);
    if (e == ESP_OK) {
        printf("WHO_AM_I = 0x%02X\n", who);
    } else {
        printf("WHO_AM_I read failed: %s\n", esp_err_to_name(e));
    }

    while (1) {
        mpu6500_scaled_t s;
        e = mpu6500_read_scaled(&imu, &s);
        if (e == ESP_OK) {
            printf("A[g] %.3f %.3f %.3f | G[dps] %.2f %.2f %.2f | T[C] %.2f\n",
                   s.ax_g, s.ay_g, s.az_g,
                   s.gx_dps, s.gy_dps, s.gz_dps,
                   s.temp_c);
        } else {
            printf("Read failed: %s\n", esp_err_to_name(e));
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}


