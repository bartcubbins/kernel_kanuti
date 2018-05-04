/*
 * Copyright (C) 2018, Pavel Dubrova <pashadubrova@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

static int gpio = 0;
static int band_id = 0;
static int hwid = 0;
static char *band;

struct smem_oem_info {
	int hwid;
};

struct gpio HWID_GPIO[7] = {
	{ -1, GPIOF_DIR_IN, NULL }
};

typedef struct {
	int BAND_ID;
	int GPIO_LOGIC;
	char *NAME;
} hw_id;

static hw_id HW_ID[] = {
	{ 0x01, 0x00, "RITA" }, // BAND_RITA
	{ 0x01, 0x01, "RITA" }, // BAND_RITA
	{ 0x01, 0x02, "RITA" }, // BAND_RITA
	{ 0x01, 0x03, "RITA" }, // BAND_RITA
	{ 0x01, 0x04, "RITA" }, // BAND_RITA
	{ 0x01, 0x05, "RITA" }, // BAND_RITA
	{ 0x01, 0x06, "RITA" }, // BAND_RITA
	{ 0x01, 0x07, "RITA" }, // BAND_RITA
	{ 0x01, 0x08, "RITA" }, // BAND_RITA
	{ 0x01, 0x09, "RITA" }, // BAND_RITA
	{ 0x01, 0x0a, "RITA" }, // BAND_RITA
	{ 0x01, 0x0b, "RITA" }, // BAND_RITA
	{ 0x01, 0x10, "RITA" }, // BAND_RITA
	{ 0x01, 0x11, "RITA" }, // BAND_RITA
	{ 0x01, 0x12, "RITA" }, // BAND_RITA
	{ 0x01, 0x13, "RITA" }, // BAND_RITA
	{ 0x01, 0x14, "RITA" }, // BAND_RITA
	{ 0x01, 0x15, "RITA" }, // BAND_RITA
	{ 0x01, 0x16, "RITA" }, // BAND_RITA
	{ 0x01, 0x17, "RITA" }, // BAND_RITA
	{ 0x01, 0x18, "RITA" }, // BAND_RITA
	{ 0x01, 0x19, "RITA" }, // BAND_RITA
	{ 0x01, 0x1a, "RITA" }, // BAND_RITA
	{ 0x01, 0x1b, "RITA" }, // BAND_RITA
	{ 0x01, 0x1c, "RITA" }, // BAND_RITA
	{ 0x01, 0x1d, "RITA" }, // BAND_RITA

	{ 0x02, 0x20, "GINA" }, // BAND_GINA
	{ 0x02, 0x21, "GINA" }, // BAND_GINA
	{ 0x02, 0x22, "GINA" }, // BAND_GINA
	{ 0x02 ,0x23, "GINA" }, // BAND_GINA
	{ 0x02, 0x24, "GINA" }, // BAND_GINA
	{ 0x02, 0x25, "GINA" }, // BAND_GINA
	{ 0x02, 0x26, "GINA" }, // BAND_GINA
	{ 0x02, 0x27, "GINA" }, // BAND_GINA
	{ 0x02, 0x28, "GINA" }, // BAND_GINA
	{ 0x02, 0x29, "GINA" }, // BAND_GINA
	{ 0x02, 0xa9, "GINA" }, // BAND_GINA
	{ 0x02, 0x2a, "GINA" }, // BAND_GINA
	{ 0x02, 0xaa, "GINA" }, // BAND_GINA
	{ 0x02, 0x2b, "GINA" }, // BAND_GINA
	{ 0x02, 0x2c, "GINA" }, // BAND_GINA
	{ 0x02, 0x2d, "GINA" }, // BAND_GINA
	{ 0x02, 0x2e, "GINA" }, // BAND_GINA
	{ 0x02, 0x30, "GINA" }, // BAND_GINA
	{ 0x02, 0x31, "GINA" }, // BAND_GINA
	{ 0x02, 0x32, "GINA" }, // BAND_GINA
	{ 0x02, 0x33, "GINA" }, // BAND_GINA
	{ 0x02, 0x34, "GINA" }, // BAND_GINA
	{ 0x02, 0x35, "GINA" }, // BAND_GINA
	{ 0x02, 0x36, "GINA" }, // BAND_GINA
	{ 0x02, 0x37, "GINA" }, // BAND_GINA
	{ 0x02, 0x38, "GINA" }, // BAND_GINA
	{ 0x02, 0x39, "GINA" }, // BAND_GINA
	{ 0x02, 0x3a, "GINA" }, // BAND_GINA
	{ 0x02, 0x3b, "GINA" }, // BAND_GINA
	{ 0x02, 0x3c, "GINA" }, // BAND_GINA
	{ 0x02, 0x3d, "GINA" }, // BAND_GINA

	{ 0x03, 0x40, "REX" },  // BAND_REX
	{ 0x03, 0x41, "REX" },  // BAND_REX
	{ 0x03, 0x42, "REX" },  // BAND_REX
	{ 0x03, 0x43, "REX" },  // BAND_REX
	{ 0x03, 0x44, "REX" },  // BAND_REX
	{ 0x03, 0x45, "REX" },  // BAND_REX
	{ 0x03, 0x46, "REX" },  // BAND_REX
	{ 0x03, 0x47, "REX" },  // BAND_REX
	{ 0x03, 0x48, "REX" },  // BAND_REX
	{ 0x03, 0x49, "REX" },  // BAND_REX
	{ 0x03, 0x4a, "REX" },  // BAND_REX
	{ 0x03, 0x4b, "REX" },  // BAND_REX
	{ 0x03, 0x4c, "REX" },  // BAND_REX
	{ 0x03, 0x4d, "REX" },  // BAND_REX
	{ 0x03, 0x51, "REX" },  // BAND_REX

	{ 0x04, 0x52, "APAC" }, // BAND_APAC
	{ 0x04, 0x53, "APAC" }, // BAND_APAC
	{ 0x04, 0x54, "APAC" }, // BAND_APAC
	{ 0x04, 0x55, "APAC" }, // BAND_APAC
	{ 0x04, 0x56, "APAC" }, // BAND_APAC
	{ 0x04, 0x57, "APAC" }, // BAND_APAC
	{ 0x04, 0x58, "APAC" }, // BAND_APAC
	{ 0x04, 0x59, "APAC" }, // BAND_APAC
	{ 0x04, 0x5a, "APAC" }, // BAND_APAC
	{ 0x04, 0x5b, "APAC" }, // BAND_APAC
	{ 0x04, 0x5c, "APAC" }, // BAND_APAC
	{ 0x04, 0x5d, "APAC" }, // BAND_APAC
	{ 0x04, 0x5e, "APAC" }, // BAND_APAC
	{ 0x04, 0x5f, "APAC" }, // BAND_APAC
	{ 0x04, 0x60, "APAC" }, // BAND_APAC
	{ 0x04, 0x61, "APAC" }, // BAND_APAC
	{ 0x04, 0x62, "APAC" }, // BAND_APAC
	{ 0x04, 0x63, "APAC" }, // BAND_APAC
	{ 0x04, 0x64, "APAC" }, // BAND_APAC
	{ 0x04, 0x65, "APAC" }, // BAND_APAC
	{ 0x04, 0x66, "APAC" }, // BAND_APAC
	{ 0x04, 0x67, "APAC" }, // BAND_APAC
	{ 0x04, 0x68, "APAC" }, // BAND_APAC
	{ 0x04, 0x69, "APAC" }, // BAND_APAC
	{ 0x04, 0x6a, "APAC" }, // BAND_APAC
	{ 0x04, 0x6b, "APAC" }, // BAND_APAC
	{ 0x04, 0x6c, "APAC" }, // BAND_APAC
	{ 0x04, 0x6d, "APAC" }, // BAND_APAC

	{ 0x05, 0x72, "VIV" },  // BAND_VIV
	{ 0x05, 0x73, "VIV" },  // BAND_VIV
	{ 0x05, 0x74, "VIV" },  // BAND_VIV
	{ 0x05, 0x75, "VIV" },  // BAND_VIV
	{ 0x05, 0x76, "VIV" },  // BAND_VIV
	{ 0x05, 0x78, "VIV" },  // BAND_VIV
	{ 0x05, 0x79, "VIV" },  // BAND_VIV
	{ 0x05, 0x7a, "VIV" },  // BAND_VIV
	{ 0x05, 0x7b, "VIV" },  // BAND_VIV
	{ 0x05, 0x7c, "VIV" },  // BAND_VIV
	{ 0xff, 0xff, "UNKNOWN" },  // BAND_UNKNOWN

};
