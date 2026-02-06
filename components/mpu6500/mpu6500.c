#include "mpu6500.h"
#include "driver/i2c.h"

// Registers (MPU6500 / MPU6xxx family)
#define MPU6500_REG_WHO_AM_I     0x75
#define MPU6500_REG_PWR_MGMT_1   0x6B
#define MPU6500_REG_ACCEL_XOUT_H 0x3B
#define MPU6500_REG_GYRO_CONFIG   0x1B
#define MPU6500_REG_ACCEL_CONFIG  0x1C


static esp_err_t mpu_write_reg(mpu6500_t *dev, uint8_t reg, uint8_t val)
{
    uint8_t data[2] = { reg, val };
    return i2c_master_write_to_device(dev->i2c_port, dev->addr, data, sizeof(data), pdMS_TO_TICKS(100));
}

static esp_err_t mpu_read_regs(mpu6500_t *dev, uint8_t start_reg, uint8_t *buf, size_t len)
{
    // Write register address, then read bytes
    return i2c_master_write_read_device(dev->i2c_port, dev->addr, &start_reg, 1, buf, len, pdMS_TO_TICKS(100));
}

esp_err_t mpu6500_read_whoami(mpu6500_t *dev, uint8_t *whoami)
{
    return mpu_read_regs(dev, MPU6500_REG_WHO_AM_I, whoami, 1);
}

esp_err_t mpu6500_init(mpu6500_t *dev)
{
    esp_err_t err = mpu_write_reg(dev, MPU6500_REG_PWR_MGMT_1, 0x01);
    if (err != ESP_OK) return err;

    // defaults
    err = mpu6500_set_gyro_fs(dev, MPU_GYRO_250DPS);
    if (err != ESP_OK) return err;

    err = mpu6500_set_accel_fs(dev, MPU_ACCEL_2G);
    if (err != ESP_OK) return err;

    return ESP_OK;
}


esp_err_t mpu6500_set_gyro_fs(mpu6500_t *dev, mpu_gyro_fs_t fs)
{
    // FS_SEL bits are [4:3]
    uint8_t val = (uint8_t)(fs << 3);
    esp_err_t err = mpu_write_reg(dev, MPU6500_REG_GYRO_CONFIG, val);
    if (err == ESP_OK) dev->gyro_fs = fs;
    return err;
}

esp_err_t mpu6500_set_accel_fs(mpu6500_t *dev, mpu_accel_fs_t fs)
{
    // AFS_SEL bits are [4:3]
    uint8_t val = (uint8_t)(fs << 3);
    esp_err_t err = mpu_write_reg(dev, MPU6500_REG_ACCEL_CONFIG, val);
    if (err == ESP_OK) dev->accel_fs = fs;
    return err;
}


esp_err_t mpu6500_read_raw(mpu6500_t *dev, mpu6500_raw_t *out)
{
    uint8_t b[14];
    esp_err_t err = mpu_read_regs(dev, MPU6500_REG_ACCEL_XOUT_H, b, sizeof(b));
    if (err != ESP_OK) return err;

    out->ax = (int16_t)((b[0] << 8) | b[1]);
    out->ay = (int16_t)((b[2] << 8) | b[3]);
    out->az = (int16_t)((b[4] << 8) | b[5]);
    out->temp = (int16_t)((b[6] << 8) | b[7]);
    out->gx = (int16_t)((b[8] << 8) | b[9]);
    out->gy = (int16_t)((b[10] << 8) | b[11]);
    out->gz = (int16_t)((b[12] << 8) | b[13]);

    return ESP_OK;
}


static float gyro_lsb_per_dps(mpu_gyro_fs_t fs)
{
    switch (fs) {
        case MPU_GYRO_250DPS:  return 131.0f;
        case MPU_GYRO_500DPS:  return 65.5f;
        case MPU_GYRO_1000DPS: return 32.8f;
        case MPU_GYRO_2000DPS: return 16.4f;
        default: return 131.0f;
    }
}

static float accel_lsb_per_g(mpu_accel_fs_t fs)
{
    switch (fs) {
        case MPU_ACCEL_2G:  return 16384.0f;
        case MPU_ACCEL_4G:  return 8192.0f;
        case MPU_ACCEL_8G:  return 4096.0f;
        case MPU_ACCEL_16G: return 2048.0f;
        default: return 16384.0f;
    }
}

esp_err_t mpu6500_read_scaled(mpu6500_t *dev, mpu6500_scaled_t *out)
{
    mpu6500_raw_t r;
    esp_err_t err = mpu6500_read_raw(dev, &r);
    if (err != ESP_OK) return err;

    float gyro_lsb = gyro_lsb_per_dps(dev->gyro_fs);
    float accel_lsb = accel_lsb_per_g(dev->accel_fs);

    out->ax_g = (float)r.ax / accel_lsb;
    out->ay_g = (float)r.ay / accel_lsb;
    out->az_g = (float)r.az / accel_lsb;
    out->gx_dps = (float)r.gx / gyro_lsb;
    out->gy_dps = (float)r.gy / gyro_lsb;
    out->gz_dps = (float)r.gz / gyro_lsb;
    out->temp_c = ((float)r.temp / 333.87f) + 21.0f;;

    return ESP_OK;
}
