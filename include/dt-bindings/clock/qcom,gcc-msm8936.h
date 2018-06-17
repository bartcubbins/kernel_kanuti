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

#ifndef _DT_BINDINGS_CLK_MSM_GCC_8936_H
#define _DT_BINDINGS_CLK_MSM_GCC_8936_H

/* Hardware clocks */
#define GCC_XO				0
#define GCC_XO_A			1
#define WCNSS_M_CLK			2

/* PLLs */
#define GPLL0				3
#define GPLL0_CLK_SRC			4
#define GPLL1				5
#define GPLL1_CLK_SRC			6
#define GPLL0_AO			7
#define GPLL2				8
#define GPLL2_CLK_SRC			9
#define GPLL3				10
#define GPLL3_CLK_SRC			11
#define GPLL4				12
#define GPLL4_CLK_SRC			13
#define GPLL6				14
#define GPLL6_CLK_SRC			15
#define CLK_A53SS_C0_PLL		16
#define CLK_A53SS_C1_PLL		17
#define CLK_A53SS_CCI_PLL		18

/* RCGs */
#define APSS_AHB_CLK_SRC		19
#define CAMSS_AHB_CLK_SRC		20
#define CSI0_CLK_SRC			21
#define CSI1_CLK_SRC			22
#define CSI2_CLK_SRC			23
#define VFE0_CLK_SRC			24
#define MDP_CLK_SRC			25
#define GFX3D_CLK_SRC			26
#define BLSP1_QUP1_I2C_APPS_CLK		27
#define BLSP1_QUP2_I2C_APPS_CLK		28
#define BLSP1_QUP3_I2C_APPS_CLK		29
#define BLSP1_QUP4_I2C_APPS_CLK		30
#define BLSP1_QUP5_I2C_APPS_CLK		31
#define BLSP1_QUP6_I2C_APPS_CLK		32
#define BLSP1_UART1_APPS_CLK		33
#define BLSP1_UART2_APPS_CLK		34
#define CCI_CLK_SRC			35
#define CAMSS_GP0_CLK_SRC		36
#define CAMSS_GP1_CLK_SRC		37
#define JPEG0_CLK_SRC			38
#define MCLK0_CLK_SRC			39
#define MCLK1_CLK_SRC			40
#define MCLK2_CLK_SRC			41
#define CSI0PHYTIMER_CLK_SRC		42
#define CSI1PHYTIMER_CLK_SRC		43
#define CPP_CLK_SRC			44
#define GP1_CLK_SRC			45
#define GP2_CLK_SRC			46
#define GP3_CLK_SRC			47
#define ESC0_CLK_SRC			48
#define VSYNC_CLK_SRC			49
#define PDM2_CLK_SRC			50
#define SDCC1_APPS_CLK_SRC		51
#define SDCC2_APPS_CLK_SRC		52
#define USB_HS_SYSTEM_CLK_SRC		53
#define USB_FS_SYSTEM_CLK_SRC		54
#define USB_FS_IC_CLK_SRC		55
#define VCODEC0_CLK_SRC			56

/* Voteable Clocks */
#define GCC_BLSP1_AHB_CLK		57
#define GCC_BOOT_ROM_AHB_CLK		58
#define GCC_PRNG_AHB_CLK		59
#define GCC_APSS_TCU_CLK		60
#define GCC_GFX_TBU_CLK			61
#define GCC_GFX_TCU_CLK			62
#define GCC_GTCU_AHB_CLK		63
#define GCC_JPEG_TBU_CLK		64
#define GCC_MDP_TBU_CLK			65
#define GCC_SMMU_CFG_CLK		66
#define GCC_VENUS_TBU_CLK		67
#define GCC_VFE_TBU_CLK			68
#define GCC_CPP_TBU_CLK			69
#define GCC_MDP_RT_TBU_CLK		70

/* Branches */
#define GCC_BLSP1_QUP1_I2C_APPS_CLK	71
#define GCC_BLSP1_QUP2_I2C_APPS_CLK	72
#define GCC_BLSP1_QUP3_I2C_APPS_CLK	73
#define GCC_BLSP1_QUP4_I2C_APPS_CLK	74
#define GCC_BLSP1_QUP5_I2C_APPS_CLK	75
#define GCC_BLSP1_QUP6_I2C_APPS_CLK	76
#define GCC_BLSP1_UART1_APPS_CLK	77
#define GCC_BLSP1_UART2_APPS_CLK	78
#define GCC_CAMSS_CCI_AHB_CLK		79
#define GCC_CAMSS_CCI_CLK		80
#define GCC_CAMSS_CSI0_AHB_CLK		81
#define GCC_CAMSS_CSI0_CLK		82
#define GCC_CAMSS_CSI0PHY_CLK		83
#define GCC_CAMSS_CSI0PIX_CLK		84
#define GCC_CAMSS_CSI0RDI_CLK		85
#define GCC_CAMSS_CSI1_AHB_CLK		86
#define GCC_CAMSS_CSI1_CLK		87
#define GCC_CAMSS_CSI1PHY_CLK		88
#define GCC_CAMSS_CSI1PIX_CLK		89
#define GCC_CAMSS_CSI1RDI_CLK		90
#define GCC_CAMSS_CSI2_AHB_CLK		91
#define GCC_CAMSS_CSI2_CLK		92
#define GCC_CAMSS_CSI2PHY_CLK		93
#define GCC_CAMSS_CSI2PIX_CLK		94
#define GCC_CAMSS_CSI2RDI_CLK		95
#define GCC_CAMSS_CSI_VFE0_CLK		96
#define GCC_CAMSS_GP0_CLK		97
#define GCC_CAMSS_GP1_CLK		98
#define GCC_CAMSS_ISPIF_AHB_CLK		99
#define GCC_CAMSS_JPEG0_CLK		100
#define GCC_CAMSS_JPEG_AHB_CLK		101
#define GCC_CAMSS_JPEG_AXI_CLK		102
#define GCC_CAMSS_MCLK0_CLK		103
#define GCC_CAMSS_MCLK1_CLK		104
#define GCC_CAMSS_MCLK2_CLK		105
#define GCC_CAMSS_MICRO_AHB_CLK		106
#define GCC_CAMSS_CSI0PHYTIMER_CLK	107
#define GCC_CAMSS_CSI1PHYTIMER_CLK	108
#define GCC_CAMSS_AHB_CLK		109
#define GCC_CAMSS_TOP_AHB_CLK		110
#define GCC_CAMSS_CPP_AHB_CLK		111
#define GCC_CAMSS_CPP_CLK		112
#define GCC_CAMSS_VFE0_CLK		113
#define GCC_CAMSS_VFE_AHB_CLK		114
#define GCC_CAMSS_VFE_AXI_CLK		115
#define GCC_OXILI_GMEM_CLK		116
#define GCC_GP1_CLK			117
#define GCC_GP2_CLK			118
#define GCC_GP3_CLK			119
#define GCC_MDSS_AHB_CLK		120
#define GCC_MDSS_AXI_CLK		121
#define GCC_MDSS_ESC0_CLK		122
#define GCC_MDSS_MDP_CLK		123
#define GCC_MDSS_VSYNC_CLK		124
#define GCC_MSS_CFG_AHB_CLK		125
#define GCC_OXILI_AHB_CLK		126
#define GCC_OXILI_TIMER_CLK		127
#define GCC_OXILI_GFX3D_CLK		128
#define GCC_PDM2_CLK			129
#define GCC_PDM_AHB_CLK			130
#define GCC_SDCC1_AHB_CLK		131
#define GCC_SDCC1_APPS_CLK		132
#define GCC_SDCC2_AHB_CLK		133
#define GCC_SDCC2_APPS_CLK		134
#define GCC_USB2A_PHY_SLEEP_CLK		135
#define GCC_USB_HS_AHB_CLK		136
#define GCC_USB_HS_SYSTEM_CLK		137
#define GCC_USB_FS_AHB_CLK		138
#define GCC_USB_FS_IC_CLK		139
#define GCC_USB_FS_SYSTEM_CLK		140
#define GCC_VENUS0_AHB_CLK		141
#define GCC_VENUS0_AXI_CLK		142
#define GCC_VENUS0_VCODEC0_CLK		143
#define GCC_VENUS0_CORE0_VCODEC0_CLK	144
#define GCC_VENUS0_CORE1_VCODEC0_CLK	145
#define GCC_BIMC_GFX_CLK		146
#define GCC_BIMC_GPU_CLK		147

/* MDSS clocks */
#define BYTE0_CLK_SRC			148
#define PCLK0_CLK_SRC			149
#define GCC_MDSS_BYTE0_CLK		150
#define GCC_MDSS_PCLK0_CLK		151

/* Crypto clocks */
#define GCC_CRYPTO_CLK			152
#define GCC_CRYPTO_AHB_CLK		153
#define GCC_CRYPTO_AXI_CLK		154
#define CRYPTO_CLK_SRC			155

/* QoS Reference clock */
#define GCC_SNOC_QOSGEN_CLK		156

/* Reset clocks */
#define GCC_CAMSS_MICRO_BCR		0
#define GCC_USB_HS_BCR			1

#endif
