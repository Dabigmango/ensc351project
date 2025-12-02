#include "hal/joystick.h"

static struct gpiod_chip* chip1;
static unsigned int offset[1] = {38};
static struct gpiod_line_config *config;
static struct gpiod_line_settings *settings;
static struct gpiod_request_config *req_cfg;
static struct gpiod_line_request *req;

static int isPressed = 0;

static int read_ch(int fd, int ch, uint32_t speed_hz) {
    // fd -> file descriptor, ch -> channel num, speed_hz -> SPI clock speed
    // tx -> request message to ADC, rx -> receive buffer

    uint8_t tx[3] = { (uint8_t)(0x06 | ((ch & 0x04) >> 2)),
    (uint8_t)((ch & 0x03) << 6),
    0x00 };

    uint8_t rx[3] = { 0 };

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = speed_hz,
        .bits_per_word = 8,
        .cs_change = 0
    };

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) return -1;

    return ((rx[1] & 0x0F) << 8) | rx[2]; // 12-bit result
}

void joystickInit() {
    chip1 = gpiod_chip_open("/dev/gpiochip1");

    config = gpiod_line_config_new();

    settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

    gpiod_line_config_add_line_settings(config, offset, 1, settings);

    req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, "joystick");

    req = gpiod_chip_request_lines(chip1, req_cfg, config);
}

void joystickClose() {
    gpiod_line_request_release(req);
    gpiod_line_settings_free(settings);   // free settings
    gpiod_line_config_free(config);       // free line config
    gpiod_request_config_free(req_cfg);   // free request config
    gpiod_chip_close(chip1); 
}


int yPos(void) {
    const char* dev = "/dev/spidev0.0";
    uint8_t mode = 0; // SPI mode 0
    uint8_t bits = 8;
    uint32_t speed = 250000;
    int fd = open(dev, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) { perror("mode"); return 1; }
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) { perror("bpw");return 1; }
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) { perror("speed"); return 1; }
    int ch1 = read_ch(fd, 1, speed);
    close(fd);
    return ch1;
}

int xPos(void) {
    const char* dev = "/dev/spidev0.0";
    uint8_t mode = 0; // SPI mode 0
    uint8_t bits = 8;
    uint32_t speed = 250000;
    int fd = open(dev, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) { perror("mode"); return 1; }
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) { perror("bpw");return 1; }
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) { perror("speed"); return 1; }
    int ch0 = read_ch(fd, 0, speed);
    close(fd);
    return ch0;
}

const char* direction(void) {
    if (xPos() < 500) {
        return "left";
    }
    else if (xPos() > 3500) {
        return "right";
    }
    else {
        if (yPos() < 500) {
            return "down";
        }
        if (yPos() > 3500) {
            return "up";
        }
    }
    return "none";
}

int joystickPressed(void) {
    if (!gpiod_line_request_get_value(req, offset[0]) && isPressed == 0) {
        isPressed = 1;
        return isPressed;
    }
    if (gpiod_line_request_get_value(req, offset[0]) && isPressed == 1) {
        isPressed = 0;
    }

    return 0;
}