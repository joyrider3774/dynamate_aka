// What the game used SDL_mixer for: the sound effects (built into the firmware) on their own
// tracks, and the music, WAV files streamed from the music folder on the SD card.
#ifndef SOUND_H
#define SOUND_H

#define MAXMUSICFILES 25

extern int MusicCount;

void SoundInit();
// 0 to 128, the range Mix_Volume had
void SetVolume(int VolumeIn);
// Mix_PlayChannel(-1, Sounds[Snd], 0)
void PlaySound(int Snd);
// music 0 is title.wav, the others are every other WAV file in the music folder
void PlayMusic(int Nr);
bool MusicPlaying();
void HaltMusic();
// Mix_FadeOutMusic followed by the wait for it the game always did
void FadeOutMusic(int Ms);

#endif
