#include "hal/lcd.h"

#define SAMPLE_SIZE 1200 // write cant handle too many bytes (found out hard way lol)

static struct gpiod_chip* chip2;
static unsigned int offset[2] = {15, 17};
static struct gpiod_line_config *config;
static struct gpiod_line_settings *settings;
static struct gpiod_request_config *req_cfg;
static struct gpiod_line_request *req;

// offsets (lcd doesnt start at 0, 0)
static int x0 = 40;
static int y0 = 53;

static uint8_t actualBuff[240 * 135 * 2];
static volatile int hasChanged = 0;
volatile int isDrawing = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int spi_init() {
    int fd = open("/dev/spidev0.1", O_RDWR);
    if (fd < 0) {
        perror("Failed to open SPI device");
        return -1; // or exit
    }
    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 250000; // 24 MHz typical for ST7789

    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    gpio_init();

    return fd;
}

void gpio_init() {
    chip2 = gpiod_chip_open("/dev/gpiochip2");

        // Create one line configuration
    config = gpiod_line_config_new();

    // Create settings for line 15
    settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    gpiod_line_config_add_line_settings(config, offset, 2, settings);

    // Create request config
    req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, "lcd_app");

    // Request both lines at once
    req = gpiod_chip_request_lines(chip2, req_cfg, config);
}

void gpio_write(int pin, int value) {
    int i;

    if (pin == 5) {
        i = 0;
    }
    else if (pin == 6) {
        i = 1;
    }

    gpiod_line_request_set_value(req, offset[i], value);   // line 15 high
}

void lcd_cmd(int fd, uint8_t cmd) {
    gpio_write(6, 0);     // DC=0 -> command
    write(fd, &cmd, 1);
}

void lcd_data(int fd, uint8_t data) {
    gpio_write(6, 1);     // DC=1 -> data
    write(fd, &data, 1);
}

void lcd_reset() {
    gpio_write(5, 0);
    usleep(100000);
    gpio_write(5, 1);
    usleep(100000);
}

void lcd_init(int fd) {
    lcd_reset();

    lcd_cmd(fd, 0x36); // MADCTL
    lcd_data(fd, 0x60);

    lcd_cmd(fd, 0x3A); // COLMOD
    lcd_data(fd, 0x05); // 16-bit color

    lcd_cmd(fd, 0x21); // Inversion on

    lcd_cmd(fd, 0x11); // Sleep out
    usleep(120000);

    lcd_cmd(fd, 0x29); // Display ON
}

void setSpace(int fd, int x, int y, int xLen, int yLen) {
    lcd_cmd(fd, 0x2A);        // Column address set (note: lcd takes in 2 bytes per pixel)
    lcd_data(fd, (x + x0) >> 8);
    lcd_data(fd, (x + x0) & 0xFF);
    lcd_data(fd, (x + x0 + xLen - 1) >> 8);
    lcd_data(fd, (x + x0 + xLen - 1) & 0xFF);

    lcd_cmd(fd, 0x2B);        // Row address set
    lcd_data(fd, (y + y0) >> 8);
    lcd_data(fd, (y + y0) & 0xFF);
    lcd_data(fd, (y + y0 + yLen - 1) >> 8);
    lcd_data(fd, (y + y0 + yLen - 1) & 0xFF);
}

void lcd_fill(int fd, uint16_t color) {
    for (int i = 0; i < 240 * 135; i++) {
        actualBuff[2*i] = (color >> 8);
        actualBuff[2*i + 1] = (color & 0xFF);
    }
    pthread_mutex_lock(&lock);
    hasChanged = 1;
    pthread_mutex_unlock(&lock);
}

void* sendScreen(void* arg) {
    int fd = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&lock);
        if (hasChanged && !isDrawing) {
            hasChanged = 0;  // mark it as being processed
            pthread_mutex_unlock(&lock);

            setSpace(fd, 0, 0, 240, 135);
            lcd_cmd(fd, 0x2C);
            gpio_write(6, 1);
            for (int i = 0; i < (240*135*2 / SAMPLE_SIZE); i++) {
                write(fd, actualBuff + i * SAMPLE_SIZE, SAMPLE_SIZE);
            }
        } else {
            pthread_mutex_unlock(&lock);
        }
        usleep(1000);
    }
    return NULL;
}

void lcd_draw_pixel(int fd, int x, int y, uint16_t color) {
    actualBuff[2 * (y * 240 + x)] = (color >> 8);
    actualBuff[2 * (y * 240 + x) + 1] = (color & 0xFF);
    pthread_mutex_lock(&lock);
    hasChanged = 1;
    pthread_mutex_unlock(&lock);
}

void lcd_close(int fd) {
    close(fd);
    gpiod_line_request_release(req);
    gpiod_line_settings_free(settings);   // free settings
    gpiod_line_config_free(config);       // free line config
    gpiod_request_config_free(req_cfg);   // free request config
    gpiod_chip_close(chip2); 
}

void test(int fd) {
    printf("sending data\n");
    setSpace(fd, 0, 0, 240, 135);
    lcd_cmd(fd, 0x2C);
    gpio_write(6, 1);
    for (int i = 0; i < (240*135*2 / SAMPLE_SIZE); i++) {
        write(fd, actualBuff + i * SAMPLE_SIZE, SAMPLE_SIZE);
    } 
}