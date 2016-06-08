#ifndef __LCM_SHOW_H__
#define __LCM_SHOW_H__

#define STRING_LENTH                32
#define CMD_LENGTH                  128
#define SHOW_LENGTH                 128

typedef struct lcm_tare{
    char iface[STRING_LENTH];
    double rx_bytes;
    double tx_bytes;
    double time;
}lcm_tare_t;

typedef struct lcm_rate_show{
    char iface[STRING_LENTH];
    char rx_speed[STRING_LENTH];
    char tx_speed[STRING_LENTH];
}lcm_rate_show_t;

#endif
