/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __USBAUDIO_H
#define __USBAUDIO_H
/*
 *   (Tentative) USB Audio Driver for ALSA
 *
 *   Copyright (c) 2002 by Takashi Iwai <tiwai@suse.de>
 */

/* handling of USB vendor/product ID pairs as 32-bit numbers */
#define USB_ID(vendor, product) (((vendor) << 16) | (product))
#define USB_ID_VENDOR(id) ((id) >> 16)
#define USB_ID_PRODUCT(id) ((u16)(id))

/*
 *
 */

struct media_device;
struct media_intf_devnode;

#define MAX_CARD_INTERFACES	16

struct snd_usb_audio {
	int index;
	struct usb_device *dev;
	struct snd_card *card;
	struct usb_interface *intf[MAX_CARD_INTERFACES];
	u32 usb_id;
	struct mutex mutex;
	unsigned int system_suspend;
	atomic_t active;
	atomic_t shutdown;
	atomic_t usage_count;
	wait_queue_head_t shutdown_wait;
	unsigned int txfr_quirk:1; /* Subframe boundaries on transfers */
	unsigned int tx_length_quirk:1; /* Put length specifier in transfers */
	unsigned int setup_fmt_after_resume_quirk:1; /* setup the format to interface after resume */
	unsigned int need_delayed_register:1; /* warn for delayed registration */
	int num_interfaces;
	int num_suspended_intf;
	int sample_rate_read_error;

	int badd_profile;		/* UAC3 BADD profile */

	struct list_head pcm_list;	/* list of pcm streams */
	struct list_head ep_list;	/* list of audio-related endpoints */
	int pcm_devs;

	struct list_head midi_list;	/* list of midi interfaces */

	struct list_head mixer_list;	/* list of mixer interfaces */

	int setup;			/* from the 'device_setup' module param */
	bool autoclock;			/* from the 'autoclock' module param */
	bool keep_iface;		/* keep interface/altset after closing
					 * or parameter change
					 */

	struct usb_host_interface *ctrl_intf;	/* the audio control interface */
	struct media_device *media_dev;
	struct media_intf_devnode *ctl_intf_media_devnode;
	struct mutex dev_lock;  /* to protect any race with disconnect */
	int card_num;	/* cache pcm card number to use upon disconnect */
	void (*disconnect_cb)(struct snd_usb_audio *chip);
};

#define usb_audio_err(chip, fmt, args...) \
	dev_err(&(chip)->dev->dev, fmt, ##args)
#define usb_audio_warn(chip, fmt, args...) \
	dev_warn(&(chip)->dev->dev, fmt, ##args)
#define usb_audio_info(chip, fmt, args...) \
	dev_info(&(chip)->dev->dev, fmt, ##args)
#define usb_audio_dbg(chip, fmt, args...) \
	dev_dbg(&(chip)->dev->dev, fmt, ##args)

/*
 * Information about devices with broken descriptors
 */

/* special values for .ifnum */
#define QUIRK_NO_INTERFACE		-2
#define QUIRK_ANY_INTERFACE		-1

enum quirk_type {
	QUIRK_IGNORE_INTERFACE,
	QUIRK_COMPOSITE,
	QUIRK_AUTODETECT,
	QUIRK_MIDI_STANDARD_INTERFACE,
	QUIRK_MIDI_FIXED_ENDPOINT,
	QUIRK_MIDI_YAMAHA,
	QUIRK_MIDI_ROLAND,
	QUIRK_MIDI_MIDIMAN,
	QUIRK_MIDI_NOVATION,
	QUIRK_MIDI_RAW_BYTES,
	QUIRK_MIDI_EMAGIC,
	QUIRK_MIDI_CME,
	QUIRK_MIDI_AKAI,
	QUIRK_MIDI_US122L,
	QUIRK_MIDI_FTDI,
	QUIRK_MIDI_CH345,
	QUIRK_AUDIO_STANDARD_INTERFACE,
	QUIRK_AUDIO_FIXED_ENDPOINT,
	QUIRK_AUDIO_EDIROL_UAXX,
	QUIRK_AUDIO_ALIGN_TRANSFER,
	QUIRK_AUDIO_STANDARD_MIXER,
	QUIRK_SETUP_FMT_AFTER_RESUME,

	QUIRK_TYPE_COUNT
};

struct snd_usb_audio_quirk {
	const char *vendor_name;
	const char *product_name;
	const char *profile_name;	/* override the card->longname */
	int16_t ifnum;
	uint16_t type;
	bool shares_media_device;
	const void *data;
};

#define combine_word(s)    ((*(s)) | ((unsigned int)(s)[1] << 8))
#define combine_triple(s)  (combine_word(s) | ((unsigned int)(s)[2] << 16))
#define combine_quad(s)    (combine_triple(s) | ((unsigned int)(s)[3] << 24))

int snd_usb_lock_shutdown(struct snd_usb_audio *chip);
void snd_usb_unlock_shutdown(struct snd_usb_audio *chip);

extern bool snd_usb_use_vmalloc;
extern bool snd_usb_skip_validation;

struct audioformat;

enum snd_vendor_pcm_open_close {
	SOUND_PCM_CLOSE = 0,
	SOUND_PCM_OPEN,
};

/**
 * struct snd_usb_audio_vendor_ops - function callbacks for USB audio accelerators
 * @connect: called when a new interface is found
 * @disconnect: called when an interface is removed
 * @set_interface: called when an interface is initialized
 * @set_rate: called when the rate is set
 * @set_pcm_buf: called when the pcm buffer is set
 * @set_pcm_intf: called when the pcm interface is set
 * @set_pcm_connection: called when pcm is opened/closed
 * @set_pcm_binterval: called when the pcm binterval is set
 * @usb_add_ctls: called when USB controls are added
 *
 * Set of callbacks for some accelerated USB audio streaming hardware.
 *
 * TODO: make this USB host-controller specific, right now this only works for
 * one USB controller in the system at a time, which is only realistic for
 * self-contained systems like phones.
 */
struct snd_usb_audio_vendor_ops {
	int (*connect)(struct usb_interface *intf);
	void (*disconnect)(struct usb_interface *intf);

	int (*set_interface)(struct usb_device *udev,
			     struct usb_host_interface *alts,
			     int iface, int alt);
	int (*set_rate)(struct usb_interface *intf, int iface, int rate,
			int alt);
	int (*set_pcm_buf)(struct usb_device *udev, int iface);
	int (*set_pcm_intf)(struct usb_interface *intf, int iface, int alt,
			    int direction);
	int (*set_pcm_connection)(struct usb_device *udev,
				  enum snd_vendor_pcm_open_close onoff,
				  int direction);
	int (*set_pcm_binterval)(struct audioformat *fp,
				 struct audioformat *found,
				 int *cur_attr, int *attr);
	int (*usb_add_ctls)(struct snd_usb_audio *chip);
};

#endif /* __USBAUDIO_H */
