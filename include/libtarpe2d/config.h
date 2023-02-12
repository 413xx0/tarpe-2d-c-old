#ifndef __LIBTARPE2D_INIT_H__
#define __LIBTARPE2D_INIT_H__


#include <string.h>
#include <tgmath.h>


struct tarpe_config
{
	double_t grav_const;
	double_t bh86_threshold;
};


extern struct tarpe_config _LIBTARPE2D_CONFIG;

static const struct tarpe_config _LIBTARPE2D_CONFIG_DEFAULT = {6.6743015E-11, 0.55};

static inline void tarpe_config_set_default(struct tarpe_config * cfg)
{
	memcpy(cfg, &_LIBTARPE2D_CONFIG_DEFAULT, sizeof(struct tarpe_config));
}


static inline void tarpe_init(struct tarpe_config * cfg)
{
	memcpy(&_LIBTARPE2D_CONFIG, cfg, sizeof(struct tarpe_config));
}


#endif /*__LIBTARPE2D_INIT_H__*/
