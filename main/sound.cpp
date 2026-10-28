#include "sound.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "gamebuino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "assets.h"
#include "defines.h"
#include "platform.h"

extern const uint8_t dm_assets[];

// the player mixes 4 tracks: the music and 3 for sound effects, so effects can overlap
#define SFXTRACKS 3

static gb_audio_player Player;
static gb_audio_track_wav MusicTrack;
static gb_audio_track_wav SfxTracks[SFXTRACKS];
static int NextSfxTrack = 0;
static SemaphoreHandle_t Lock = NULL;

static char MusicFiles[MAXMUSICFILES][64];
int MusicCount = 0;

// the player has to be fed all the time, also while the game waits or loads
static void AudioTask(void*)
{
    for (;;)
    {
        xSemaphoreTake(Lock, portMAX_DELAY);
        Player.pool();
        xSemaphoreGive(Lock);
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

static int CompareNames(const void* a, const void* b)
{
    return strcasecmp((const char*)a, (const char*)b);
}

// SearchForMusic: title.wav first, then the other WAV files of the music folder
static void SearchForMusic()
{
    snprintf(MusicFiles[0], sizeof(MusicFiles[0]), DATA_DIR "/music/title.wav");
    // without the title music there is no music at all, as there was without title.mod
    FILE* Title = fopen(MusicFiles[0], "rb");
    if (!Title)
    {
        MusicCount = 0;
        return;
    }
    fclose(Title);
    int Teller = 1;
    DIR* Directory = opendir(DATA_DIR "/music");
    if (Directory)
    {
        struct dirent* Entry;
        while ((Entry = readdir(Directory)) && (Teller < MAXMUSICFILES))
        {
            size_t Len = strlen(Entry->d_name);
            if ((Entry->d_name[0] == '.') || (Len < 5) || (strcasecmp(Entry->d_name + Len - 4, ".wav") != 0) ||
                (strcasecmp(Entry->d_name, "title.wav") == 0) || (Len + 20 >= sizeof(MusicFiles[0])))
                continue;
            snprintf(MusicFiles[Teller], sizeof(MusicFiles[0]), DATA_DIR "/music/%s", Entry->d_name);
            Teller++;
        }
        closedir(Directory);
        qsort(MusicFiles[1], Teller - 1, sizeof(MusicFiles[0]), CompareNames);
    }
    MusicCount = Teller;
}

void SoundInit()
{
    Lock = xSemaphoreCreateMutex();
    Player.add_track(&MusicTrack);
    for (int Teller = 0; Teller < SFXTRACKS; Teller++)
        Player.add_track(&SfxTracks[Teller]);
    SearchForMusic();
    // the game runs on core 0, the music keeps streaming on the other one
    xTaskCreatePinnedToCore(AudioTask, "audio", 4096, NULL, 5, NULL, 1);
}

void SetVolume(int VolumeIn)
{
    int Master = VolumeIn * 2;
    if (Master > 255)
        Master = 255;
    if (Master < 0)
        Master = 0;
    xSemaphoreTake(Lock, portMAX_DELAY);
    Player.set_master_volume((uint8_t)Master);
    xSemaphoreGive(Lock);
}

void PlaySound(int Snd)
{
    if ((Snd < 0) || (Snd >= NROFSOUNDS))
        return;
    const AssetSound& Sound = ASSET_SOUNDS[Snd];
    xSemaphoreTake(Lock, portMAX_DELAY);
    // a free track if there is one, the one that started longest ago otherwise
    int Track = -1;
    for (int Teller = 0; Teller < SFXTRACKS; Teller++)
        if (!SfxTracks[(NextSfxTrack + Teller) % SFXTRACKS].is_playing())
        {
            Track = (NextSfxTrack + Teller) % SFXTRACKS;
            break;
        }
    if (Track < 0)
        Track = NextSfxTrack;
    NextSfxTrack = (Track + 1) % SFXTRACKS;
    SfxTracks[Track].play_raw((const int16_t*)(dm_assets + Sound.samples), Sound.count);
    xSemaphoreGive(Lock);
}

void PlayMusic(int Nr)
{
    if ((Nr < 0) || (Nr >= MusicCount))
        return;
    xSemaphoreTake(Lock, portMAX_DELAY);
    MusicTrack.set_track_volume(1.0f);
    MusicTrack.play_wav(MusicFiles[Nr]);
    xSemaphoreGive(Lock);
}

bool MusicPlaying()
{
    xSemaphoreTake(Lock, portMAX_DELAY);
    bool Playing = MusicTrack.is_playing();
    xSemaphoreGive(Lock);
    return Playing;
}

void HaltMusic()
{
    xSemaphoreTake(Lock, portMAX_DELAY);
    MusicTrack.stop_playing();
    xSemaphoreGive(Lock);
}

void FadeOutMusic(int Ms)
{
    uint32_t Start = GetTicks();
    while (MusicPlaying())
    {
        uint32_t Elapsed = GetTicks() - Start;
        if (Elapsed >= (uint32_t)Ms)
            break;
        xSemaphoreTake(Lock, portMAX_DELAY);
        MusicTrack.set_track_volume(1.0f - (float)Elapsed / Ms);
        xSemaphoreGive(Lock);
        Delay(10);
    }
    HaltMusic();
    xSemaphoreTake(Lock, portMAX_DELAY);
    MusicTrack.set_track_volume(1.0f);
    xSemaphoreGive(Lock);
}
