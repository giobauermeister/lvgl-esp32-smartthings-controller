#ifndef _API_SMARTTHINGS_H_
#define _API_SMARTTHINGS_H_

#include "cJSON.h"
#include <stdbool.h>

typedef struct {
    char *buffer;
    int buffer_size;
    int received_len;
} http_response_ctx_t;

typedef struct {
    char device_id[64];
    char label[64];
    char type[64];
} smartthings_device_t;

void smartthings_init_task(void *param);

void smartthings_api_GET_devices(void);
void smartthings_api_GET_device_status(const smartthings_device_t *device);
void smartthings_api_POST_device_status(const smartthings_device_t *device, const char *command);
bool smartthings_api_get_device_capability_status(const char *device_id, const char *capability, cJSON **json_out);
void smartthings_api_get_tv_status(const smartthings_device_t *device);
void smartthings_ac_status(const smartthings_device_t *device);

// Accessor to the list and count of devices
const smartthings_device_t *smartthings_get_devices(void);
int smartthings_get_device_count(void);

#endif // _API_SMARTTHINGS_H_