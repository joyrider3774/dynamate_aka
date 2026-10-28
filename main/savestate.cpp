#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "savestate.h"
#include "commonvars.h"
#include "defines.h"
#include "platform.h"
#include "sound.h"

void LoadHighScores()
{
    FILE *File;
    int Teller;
    File = fopen(DATA_DIR "/high.dat", "r");
    if (File)
    {
        for (Teller = 0; Teller < MAXSTATICLEVELS; Teller++)
            if (fscanf(File, "%d\n", &HighScores[Teller]) != 1)
                HighScores[Teller] = 0;
        fclose(File);
    }
    else
    {
        for (Teller = 0; Teller < MAXSTATICLEVELS; Teller++)
            HighScores[Teller] = 0;
    }
}

void SaveHighScores()
{
    FILE *File;
    int Teller;
    File = fopen(DATA_DIR "/high.dat", "w");
    if (File)
    {
        for (Teller = 0; Teller < MAXSTATICLEVELS; Teller++)
            fprintf(File, "%d\n", HighScores[Teller]);
        fclose(File);
    }
}

void LoadSettings()
{
    FILE *SettingsFile;
    SettingsFile = fopen(DATA_DIR "/settings.dat", "r");
    if (SettingsFile)
    {
        if ((fscanf(SettingsFile, "Volume=%d\n", &Volume) != 1) || (Volume < 0) || (Volume > 128))
            Volume = 128;
        fclose(SettingsFile);
    }
    else
        Volume = 128;
    SetVolume(Volume);
}

void SaveSettings()
{
    FILE *SettingsFile;
    SettingsFile = fopen(DATA_DIR "/settings.dat", "w");
    if (SettingsFile)
    {
        fprintf(SettingsFile, "Volume=%d\n", Volume);
        fclose(SettingsFile);
    }
}

void LoadUnlockData(void)
{
    FILE *f;
    f = fopen(DATA_DIR "/dynamate.dat", "rb");
    if (f && (fread(&UnlockData, sizeof(UnlockData), 1, f) == 1))
        fclose(f);
    else
    {
        if (f)
            fclose(f);
        for (int teller = 0; teller < MAXSTATICLEVELS; teller++)
            UnlockData[teller] = '0';
        UnlockData[0] = '1';
    }
}

void SaveUnlockData(void)
{
    FILE *f;
    f = fopen(DATA_DIR "/dynamate.dat", "wb");
    if (f)
    {
        fwrite(&UnlockData, sizeof(UnlockData), 1, f);
        fclose(f);
    }
}

void LoadLevel(const char *FileName)
{
    FILE *f;
    f = fopen(FileName, "rb");
    if (f)
    {
        if (fread(&UserLevel, sizeof(dm_user_lev), 1, f) != 1)
            memset(&UserLevel, 0, sizeof(UserLevel));
        fclose(f);
        dm_init_level(UserLevel.field);
    }
}

void SaveLevel(const char *FileName)
{
    dm_user_lev tmp;
    FILE *f;
    memset(&tmp, 0, sizeof(tmp));
    memcpy(&tmp.field, dm_field(), 256);
    snprintf(tmp.name, sizeof(tmp.name), "%s", UserLevel.name);
    f = fopen(FileName, "wb");
    if (f)
    {
        fwrite(&tmp, sizeof(dm_user_lev), 1, f);
        fclose(f);
    }
}

static int CompareNames(const void *a, const void *b)
{
    return strcasecmp((const char *)a, (const char *)b);
}

void SearchForLevels()
{
    struct dirent *Entry;
    DIR *Directory;
    int Teller = 0;
    Directory = opendir(DATA_DIR "/userlevels");
    if (Directory)
    {
        while ((Entry = readdir(Directory)) && (Teller < MAXUSERLEVELS))
        {
            size_t Len = strlen(Entry->d_name);
            if ((Entry->d_type == DT_DIR) || (Entry->d_name[0] == '.') || (Len < 5) ||
                (strcasecmp(Entry->d_name + Len - 4, ".lev") != 0))
                continue;
            if (snprintf(UserLevelsFilenames[Teller], LEVELPATHMAX, DATA_DIR "/userlevels/%s", Entry->d_name) >= LEVELPATHMAX)
                continue;
            Teller++;
        }
        closedir(Directory);
    }
    UserLevelCount = Teller;
    qsort(UserLevelsFilenames, UserLevelCount, LEVELPATHMAX, CompareNames);
}

void UpdateUserLevelFilenames(const char *filename)
{
    int teller;
    bool found = false;
    for (teller = 0; teller < UserLevelCount; teller++)
    {
        if (strcmp(filename, UserLevelsFilenames[teller]) == 0)
        {
            found = true;
            break;
        }
    }
    if (!found && (UserLevelCount < MAXUSERLEVELS))
    {
        UserLevelCount++;
        snprintf(UserLevelsFilenames[UserLevelCount - 1], LEVELPATHMAX, "%s", filename);
        lvl = UserLevelCount;
    }
}
