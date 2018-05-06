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

#define pr_fmt(fmt) "Kanuti_HWID: " fmt

#include <linux/platform_device.h>
#include <linux/of_gpio.h>

#include <soc/qcom/smem.h>

#include "kanuti_hwid.h"

static int hwid_band(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	int i;

	/* Parse gpio */
	for (i = 0; i < ARRAY_SIZE(HWID_GPIO); i++) {
		HWID_GPIO[i].gpio = of_get_named_gpio(np, "gpios", i);
		if (gpio < 0) {
			pr_info("Invalid gpio: %d", gpio);
			return -EINVAL;
		}
		gpio += (gpio_get_value(HWID_GPIO[i].gpio) << i);
	}

	/*
	 * If gpio response is not valid then
	 * return invalid argument
	 */
	if (!gpio) {
		pr_err("Parsing gpio failed\n");
		return -EINVAL;
	}

	switch (gpio) {
		case 0x00 ... 0x0b:
		case 0x10 ... 0x1d:
			band = "RITA";
			band_id = 0x01;
			break;
		case 0x20 ... 0x2e:
		case 0x30 ... 0x3d:
		case 0xa9 ... 0xaa:
			band = "GINA";
			band_id = 0x02;
			break;
		case 0x40 ... 0x4d:
		case 0x51:
			band = "REX";
			band_id = 0x03;
			break;
		case 0x52 ... 0x6d:
			band = "APAC";
			band_id= 0x04;
			break;
		case 0x72 ... 0x76:
		case 0x78 ... 0x7c:
			band = "VIV";
			band_id = 0x05;
			break;
		default:
			band = "UNKNOWN";
			band_id = 0xff;
			break;
	}

	return 0;
}

static void hwid_set(void)
{
	struct smem_oem_info *oem_info = smem_alloc(SMEM_ID_VENDOR0,
						sizeof(*oem_info), 0,
						SMEM_ANY_HOST_FLAG);

	if (oem_info == NULL) {
		pr_err("oem_info is NULL\n");
		return;
	}

	hwid = band_id << 16;
	oem_info->hwid = hwid;

	pr_info("Set HWID 0x%08X to smem\n", oem_info->hwid);
}

static int hwid_probe(struct platform_device *pdev)
{
	/* Call band parser function */
	hwid_band(pdev);

	pr_info("gpio response: 0x%x\n", gpio);
	pr_info("band variant: %s\n", band);
	pr_info("band id: 0x%x\n", band_id);

	/* Call set HWID function */
	hwid_set();

	pr_info("OK\n");

	return 0;
};

static int hwid_remove(struct platform_device *pdev)
{
	return 0;
}

static struct of_device_id hwid_match_table[] = {
	{ .compatible = "kanuti,hwid" },
	{},
};

static struct platform_driver hwid_driver = {
	.driver =
	{
		.name = "kanuti_hwid",
		.of_match_table = hwid_match_table,
		.owner = THIS_MODULE,
	},
	.probe = hwid_probe,
	.remove = hwid_remove,
};

static int __init hw_id_init(void)
{
	return platform_driver_register(&hwid_driver);
}

static void __exit hw_id_exit(void)
{
	pr_info("%s: \n", __func__);
	platform_driver_unregister(&hwid_driver);
	pr_info(KERN_INFO "Kanuti_HWID_exit enter\n");
}

subsys_initcall(hw_id_init);
module_exit(hw_id_exit);

MODULE_AUTHOR("Pavel Dubrova <pashadubrova@gmail.com>");
MODULE_DESCRIPTION("Kanuti hardware version detection driver");
MODULE_LICENSE("GPL v2");
