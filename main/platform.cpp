#include "platform.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "gamebuino.h"
#include "esp_heap_caps.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

gb_core core;

static uint32_t NextFrameTime = 0;

void PlatformInit()
{
    core.init();
}

uint32_t GetTicks()
{
    return core.get_millis();
}

void Delay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void FramerateDelay()
{
    const uint32_t FrameMs = 1000 / 30;
    uint32_t Now = GetTicks();
    if ((NextFrameTime == 0) || (Now > NextFrameTime + FrameMs))
        NextFrameTime = Now;
    NextFrameTime += FrameMs;
    if (NextFrameTime > Now)
        Delay(NextFrameTime - Now);
    else
        Delay(1);
}

uint16_t PollKeys()
{
    // the library's poll also switches the AKA off when RUN is pressed, as it does in every game
    core.pool();
    return core.buttons.state() | core.joystick.state();
}

void* AllocLarge(size_t size)
{
    void* p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p)
        p = malloc(size);
    return p;
}

static void MakeDir(const char* Path)
{
    if ((mkdir(Path, 0777) != 0) && (errno != EEXIST))
        printf("cannot make %s\n", Path);
}

void MakeDataDirs()
{
    MakeDir(DATA_DIR);
    MakeDir(DATA_DIR "/userlevels");
    MakeDir(DATA_DIR "/music");
}

void ReturnToLoader()
{
    const esp_partition_t* Loader = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
    if (Loader)
    {
        esp_ota_set_boot_partition(Loader);
        esp_restart();
    }
    // no launcher on this AKA: there is nothing to go back to, so start the game over
    esp_restart();
}
