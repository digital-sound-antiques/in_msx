#include <stdio.h>
#include "kssplay.h"
#include "config.h"

static void quality_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  KSSPLAY_set_device_quality(kssplay, EDSC_PSG, CONFIG_get_int(cfg,"PSG_HQ")) ;
  KSSPLAY_set_device_quality(kssplay, EDSC_SCC, CONFIG_get_int(cfg,"SCC_HQ")) ;
  KSSPLAY_set_device_quality(kssplay, EDSC_OPLL, CONFIG_get_int(cfg,"OPLL_HQ"));
  cfg->quality_update = 0 ;
}

static void pan_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  if(kssplay->nch==2) 
  {
    CONFIG_set_int(cfg,"PSG_PAN",CONFIG_get_int(cfg,"PSG_PAN_MIXER"));
    CONFIG_set_int(cfg,"SCC_PAN",CONFIG_get_int(cfg,"SCC_PAN_MIXER"));
  	CONFIG_set_int(cfg,"OPLL_PAN",CONFIG_get_int(cfg,"OPLL_PAN_MIXER"));
    CONFIG_set_int(cfg,"OPL_PAN",CONFIG_get_int(cfg,"OPL_PAN_MIXER"));
  }
  KSSPLAY_set_device_pan(kssplay, EDSC_PSG, CONFIG_get_int(cfg,"PSG_PAN"));
  KSSPLAY_set_device_pan(kssplay, EDSC_SCC, CONFIG_get_int(cfg,"SCC_PAN"));
  KSSPLAY_set_device_pan(kssplay, EDSC_OPLL, CONFIG_get_int(cfg,"OPLL_PAN"));
  KSSPLAY_set_device_pan(kssplay, EDSC_OPL, CONFIG_get_int(cfg,"OPL_PAN"));
  cfg->pan_update = 0 ;
}

static void filter_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  KSSPLAY_set_device_lpf(kssplay, EDSC_PSG, CONFIG_get_int(cfg,"PSG_CUTOFF"));
  KSSPLAY_set_device_lpf(kssplay, EDSC_SCC, CONFIG_get_int(cfg,"SCC_CUTOFF"));
  KSSPLAY_set_device_lpf(kssplay, EDSC_OPLL, CONFIG_get_int(cfg,"OPLL_CUTOFF"));
  KSSPLAY_set_device_lpf(kssplay, EDSC_OPL, CONFIG_get_int(cfg,"OPL_CUTOFF"));

  if(CONFIG_get_int(cfg, "ENABLE_RCF")&&CONFIG_get_int(cfg,"R")>0&&CONFIG_get_int(cfg,"C")>0)
  {
    RCF_reset(kssplay->rcf[0],kssplay->rate,CONFIG_get_int(cfg,"R"),(double)CONFIG_get_int(cfg,"C")/1.0e9);
    RCF_reset(kssplay->rcf[1],kssplay->rate,CONFIG_get_int(cfg,"R"),(double)CONFIG_get_int(cfg,"C")/1.0e9);
  } 
  else
  {
    RCF_disable(kssplay->rcf[0]);
    RCF_disable(kssplay->rcf[1]);
  }

  if(CONFIG_get_int(cfg,"ENABLE_DCF")!=0)
  {
    DCF_reset(kssplay->dcf[0], kssplay->rate);
    DCF_reset(kssplay->dcf[1], kssplay->rate);
  }
  else
  {
    DCF_disable(kssplay->dcf[0]);
    DCF_disable(kssplay->dcf[1]);
  }

  cfg->filter_update = 0 ;
}

static void mask_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  int i ;

  for(i=0;i<EDSC_MAX;i++)
  {
    KSSPLAY_set_channel_mask(kssplay,i,cfg->mask[i]);
  }
  cfg->mask_update = 0 ;
}

static void volume_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  int i ;

  KSSPLAY_set_master_volume(kssplay,CONFIG_get_int(cfg,"MASTER_VOL")); 
  for(i=0;i<EDSC_MAX;i++)
  {
    KSSPLAY_set_device_volume(kssplay,i,cfg->curvol[i]);
    KSSPLAY_set_device_mute(kssplay,i,cfg->mute[i]);
  }
  cfg->volume_update = 0 ;
}

static void ch_pan_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  int pan = CONFIG_get_int(cfg,"OPLL_CH_PAN");
  int i;

  for(i=0;i<14;i++)
    KSSPLAY_set_channel_pan(kssplay, EDSC_OPLL, i, (pan>>(i*2))&3);
  cfg->ch_pan_update = 0;
}

static void mode_update(CONFIG *cfg, KSSPLAY *kssplay)
{  
  KSSPLAY_set_device_type(kssplay, EDSC_PSG, CONFIG_get_int(cfg,"PSG_MODE"));
  KSSPLAY_set_device_type(kssplay, EDSC_SCC, CONFIG_get_int(cfg,"SCC_MODE"));
  KSSPLAY_set_device_type(kssplay, EDSC_OPLL, CONFIG_get_int(cfg,"OPLL_MODE"));
  cfg->mode_update = 0 ;
}

void CONFIG_force_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  if(kssplay)
  {
    //mode_update(cfg,kssplay);
    quality_update(cfg,kssplay);
    filter_update(cfg,kssplay);
    volume_update(cfg,kssplay);
    pan_update(cfg,kssplay);
    ch_pan_update(cfg,kssplay);
    mask_update(cfg,kssplay);
  }
}

void CONFIG_apply_update(CONFIG *cfg, KSSPLAY *kssplay)
{
  if(!kssplay) return ;
  if(cfg->volume_update) volume_update(cfg, kssplay);
  if(cfg->pan_update) pan_update(cfg, kssplay);
  if(cfg->mode_update) mode_update(cfg, kssplay);
  if(cfg->ch_pan_update) ch_pan_update(cfg, kssplay);
  if(cfg->filter_update) filter_update(cfg, kssplay);
  if(cfg->quality_update) quality_update(cfg, kssplay);
  if(cfg->mask_update) mask_update(cfg, kssplay);
}