/* See LICENSE file for copyright and license details. */
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>
#include "../util.h"

const char* vol_perc(const char* card) {
  snd_mixer_t* handle;
  snd_mixer_elem_t* elem;
  snd_mixer_selem_id_t* sid;

  static const char* mix_name = "Master";
  //static const char* card = "default";
  static int mix_index = 0;

  int ret = 0;
  long minv, maxv, out_vol;

  snd_mixer_selem_id_alloca(&sid);

  // sets simple-mixer index and name
  snd_mixer_selem_id_set_index(sid, mix_index);
  snd_mixer_selem_id_set_name(sid, mix_name);

  if ((snd_mixer_open(&handle, 0)) < 0) return NULL;
  if ((snd_mixer_attach(handle, card)) < 0) {
    snd_mixer_close(handle);
    return NULL;
  }
  if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
    snd_mixer_close(handle);
    return NULL;
  }
  ret = snd_mixer_load(handle);
  if (ret < 0) {
    snd_mixer_close(handle);
    return NULL;
  }
  elem = snd_mixer_find_selem(handle, sid);
  if (!elem) {
    snd_mixer_close(handle);
    return NULL;
  }

  snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv); 
  if (snd_mixer_selem_get_playback_volume(elem, 0, &out_vol) < 0) {
    snd_mixer_close(handle);
    return NULL;
  }

  snd_mixer_close(handle);
  return bprintf("%d", out_vol & 0xff);
}
