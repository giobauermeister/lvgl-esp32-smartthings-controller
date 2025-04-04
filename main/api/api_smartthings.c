#include "api_smartthings.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"

#define STR(x) #x
#define XSTR(x) STR(x)
#define API_URL "https://api.smartthings.com/v1/devices"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

typedef struct {
    char *buffer;
    int buffer_size;
    int received_len;
} http_response_ctx_t;

static const char *TAG_HTTP = "HTTP";

const char* api_token = XSTR(SMARTTHINGS_API_TOKEN);

extern const char smartthings_root_cert_pem_start[] asm("_binary_smartthings_root_cert_pem_start");
extern const char smartthings_root_cert_pem_end[]   asm("_binary_smartthings_root_cert_pem_end");

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    http_response_ctx_t *ctx = (http_response_ctx_t *)evt->user_data;

    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG_HTTP, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG_HTTP, "HEADER: %s = %s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_DATA: len = %d", evt->data_len);

            if (!esp_http_client_is_chunked_response(evt->client) && ctx && ctx->buffer) {
                int remaining = ctx->buffer_size - ctx->received_len - 1;
                int copy_len = (evt->data_len < remaining) ? evt->data_len : remaining;

                if (copy_len > 0) {
                    memcpy(ctx->buffer + ctx->received_len, evt->data, copy_len);
                    ctx->received_len += copy_len;
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_FINISH");
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_FINISH");
            if (ctx && ctx->buffer) {
                ctx->buffer[ctx->received_len] = '\0'; // Null terminate
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            ESP_LOGW(TAG_HTTP, "Unhandled event_id: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}


void smartthings_api_GET_devices(void)
{
    http_response_ctx_t ctx = {
        .buffer_size = 12 * 1024,
        .buffer = malloc(12 * 1024),
        .received_len = 0,
    };

    if (!ctx.buffer) {
        ESP_LOGE(TAG_HTTP, "Failed to allocate response buffer");
        return;
    }

    esp_http_client_config_t config = {
        .url = API_URL,
        .event_handler = _http_event_handler,
        .cert_pem = smartthings_root_cert_pem_start,
        .user_data = &ctx,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Add headers
    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", api_token);
    esp_http_client_set_header(client, "Authorization", auth_header);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // Make GET request
    ESP_LOGI(TAG_HTTP, "Sending HTTPS GET request to SmartThings API...");
    ESP_LOGI(TAG_HTTP, "Using token: %s", api_token);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200 && ctx.received_len > 0) {
            // Parse JSON from buffer
            cJSON *root = cJSON_Parse(ctx.buffer);
            if (root) {
                cJSON *items = cJSON_GetObjectItem(root, "items");
                if (items && cJSON_IsArray(items)) {
                    cJSON *device;
                    cJSON_ArrayForEach(device, items) {
                        const cJSON *id = cJSON_GetObjectItem(device, "deviceId");
                        const cJSON *label = cJSON_GetObjectItem(device, "label");
                        const cJSON *type = cJSON_GetObjectItem(device, "deviceTypeName");

                        if (id && label && type) {
                            ESP_LOGI(TAG_HTTP, "Device ID: %s", id->valuestring);
                            ESP_LOGI(TAG_HTTP, "Label:     %s", label->valuestring);
                            ESP_LOGI(TAG_HTTP, "Type:      %s", type->valuestring);
                        }
                    }
                }
                cJSON_Delete(root);
            } else {
                ESP_LOGE(TAG_HTTP, "Failed to parse JSON");
            }
        } else {
            ESP_LOGE(TAG_HTTP, "HTTP GET failed: Status = %d", status);
        }
    } else {
        ESP_LOGE(TAG_HTTP, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}