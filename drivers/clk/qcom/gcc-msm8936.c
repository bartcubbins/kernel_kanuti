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

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/clk-provider.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>

#include <dt-bindings/clock/qcom,gcc-msm8936.h>

#include "common.h"
#include "clk-regmap.h"
#include <linux/clk.h>
#include "clk-pll.h"
#include "clk-rcg.h"
#include "clk-branch.h"
#include "reset.h"
#include "gdsc.h"
#include "vdd-level-8936.h"

enum {
	P_XO,
	P_GPLL0,
	P_GPLL0_AUX,
	P_GPLL1,
	P_GPLL2,
	P_GPLL2_AUX,
	P_DSI0_PHYPLL_DSI,
};

static const struct parent_map gcc_xo_gpll0_map[] = {
	{ P_XO, 0 },
	{ P_GPLL0, 1 },
};

static const char * const gcc_xo_gpll0[] = {
	"xo",
	"gpll0_vote",
};

static const struct parent_map gcc_xo_gpll0_gpll2_map[] = {
	{ P_XO, 0 },
	{ P_GPLL0, 1 },
	{ P_GPLL2, 2 },
};

static const char * const gcc_xo_gpll0_gpll2[] = {
	"xo",
	"gpll0_vote",
	"gpll2_vote",
};


static const struct parent_map gcc_xo_gpll0_dsiphy_map[] = {
	{ P_XO, 0 },
	{ P_GPLL0, 1 },
	{ P_DSI0_PHYPLL_DSI, 2 },
};

static const char * const gcc_xo_gpll0_dsiphy[] = {
	"xo",
	"gpll0_vote",
	"dsi0pll",
};

static const struct parent_map gcc_xo_gpll0a_gpll1_gpll2a_map[] = {
	{ P_XO, 0 },
	{ P_GPLL0_AUX, 3 },
	{ P_GPLL1, 1 },
	{ P_GPLL2_AUX, 2 },
};

static const char * const gcc_xo_gpll0a_gpll1_gpll2a[] = {
	"xo",
	"gpll0_vote",
	"gpll1_vote",
	"gpll2_vote",
};

#define F(f, s, h, m, n) { (f), (s), (2 * (h) - 1), (m), (n) }

static struct clk_pll gpll0 = {
	.l_reg = 0x21004,
	.m_reg = 0x21008,
	.n_reg = 0x2100c,
	.config_reg = 0x21014,
	.mode_reg = 0x21000,
	.status_reg = 0x2101c,
	.status_bit = 17,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll0",
		.parent_names = (const char *[]){ "xo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_pll gpll1 = {
	.l_reg = 0x20004,
	.m_reg = 0x20008,
	.n_reg = 0x2000c,
	.config_reg = 0x20014,
	.mode_reg = 0x20000,
	.status_reg = 0x2001c,
	.status_bit = 17,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll1",
		.parent_names = (const char *[]){ "xo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_pll gpll2 = {
	.l_reg = 0x4a004,
	.m_reg = 0x4a008,
	.n_reg = 0x4a00c,
	.config_reg = 0x4a014,
	.mode_reg = 0x4a000,
	.status_reg = 0x4a01c,
	.status_bit = 17,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll2",
		.parent_names = (const char *[]){ "xo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_pll gpll3 = {
	.l_reg		= 0x22004,
	.m_reg		= 0x22008,
	.n_reg		= 0x2200c,
	.config_reg	= 0x22010,
	.mode_reg	= 0x22000,
	.status_reg	= 0x22024,
	.status_bit	= 17,
	.clkr.hw.init = &(struct clk_init_data) {
		.name = "gpll3",
		.parent_names = (const char*[]) { "xo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_pll gpll4 = {
	.l_reg = 0x24004,
	.m_reg = 0x24008,
	.n_reg = 0x2400c,
	.config_reg = 0x24018,
	.mode_reg = 0x24000,
	.status_reg = 0x24024,
	.status_bit = 17,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll4",
		.parent_names = (const char *[]){ "xo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_pll gpll6 = {
	.mode_reg = 0x37000,
	.l_reg = 0x37004,
	.m_reg = 0x37008,
	.n_reg = 0x3700c,
	.config_reg = 0x37014,
	.status_reg = 0x3701c,
	.status_bit = 17,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll6",
		.parent_names = (const char *[]){ "xo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static const struct freq_tbl ftbl_apss_ahb_clk[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(50000000, P_GPLL0, 16, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	{ }
};

static struct clk_rcg2 apss_ahb_clk_src = {
	.cmd_rcgr = 0x46000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_apss_ahb_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "apss_ahb_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_camss_ahb_clk[] = {
	F(40000000, P_GPLL0, 10, 1, 2),
	F(80000000, P_GPLL0, 10, 0, 0),
	{ }
};

static struct clk_rcg2 camss_ahb_clk_src = {
	.cmd_rcgr = 0x5a000,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_camss_ahb_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "camss_ahb_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_camss_csi0_1_2_clk[] = {
	F(100000000, P_GPLL0, 8, 0,	0),
	F(200000000, P_GPLL0, 4, 0,	0),
	{ }
};

static struct clk_rcg2 csi0_clk_src = {
	.cmd_rcgr = 0x4e020,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_camss_csi0_1_2_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "csi0_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 csi1_clk_src = {
	.cmd_rcgr = 0x4f020,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_camss_csi0_1_2_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "csi1_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 csi2_clk_src = {
        .cmd_rcgr = 0x3c020,
        .hid_width = 5,
        .parent_map = gcc_xo_gpll0_map,
        .freq_tbl = ftbl_gcc_camss_csi0_1_2_clk,
        .clkr.hw.init = &(struct clk_init_data){
                .name = "csi2_clk_src",
                .parent_names = gcc_xo_gpll0,
                .num_parents = 2,
                .ops = &clk_rcg2_ops,
        },
};

static const struct freq_tbl ftbl_gcc_camss_vfe0_clk[] = {
	F(50000000, P_GPLL0, 16, 0, 0),
	F(80000000, P_GPLL0, 10, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(160000000, P_GPLL0, 5, 0, 0),
	F(177780000, P_GPLL0, 4.5, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(320000000, P_GPLL0, 2.5, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	F(480000000, P_GPLL0, 2.5, 0, 0),
	F(600000000, P_GPLL0, 2, 0,0),
	{ }
};

static struct clk_rcg2 vfe0_clk_src = {
	.cmd_rcgr = 0x58000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_gpll2_map,
	.freq_tbl = ftbl_gcc_camss_vfe0_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "vfe0_clk_src",
		.parent_names = gcc_xo_gpll0_gpll2,
		.num_parents = 3,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_mdss_mdp_clk[] = {
	F(50000000, P_GPLL0, 16, 0, 0),
	F(80000000, P_GPLL0, 10, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(145500000, P_GPLL0, 5.5, 0, 0),
	F(153600000, P_GPLL0, 4, 0, 0),
	F(160000000, P_GPLL0, 5, 0, 0),
	F(177780000, P_GPLL0, 4.5, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(307200000, P_GPLL0, 2, 0, 0),
	F(366670000, P_GPLL0, 3, 0, 0),
	{ }
};

static struct clk_rcg2 mdp_clk_src = {
	.cmd_rcgr = 0x4d014,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_dsiphy_map,
	.freq_tbl = ftbl_gcc_mdss_mdp_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "mdp_clk_src",
		.parent_names = gcc_xo_gpll0_dsiphy,
		.num_parents = 3,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_oxili_gfx3d_clk[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(50000000, P_GPLL0_AUX, 16, 0, 0),
	F(80000000, P_GPLL0_AUX, 10, 0, 0),
	F(100000000, P_GPLL0_AUX, 8, 0, 0),
	F(160000000, P_GPLL0_AUX, 5, 0, 0),
	F(200000000, P_GPLL0_AUX, 4, 0, 0),
	F(220000000, P_GPLL0_AUX,   5,	  0,	0),
	F(266670000, P_GPLL0_AUX, 3, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(400000000, P_GPLL0_AUX, 2, 0, 0),
	F(465000000, P_GPLL0_AUX, 2, 0, 0),
	F(550000000, P_GPLL0_AUX, 2, 0, 0),
	{ }
};

static struct clk_rcg2 gfx3d_clk_src = {
	.cmd_rcgr = 0x59000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0a_gpll1_gpll2a_map,
	.freq_tbl = ftbl_gcc_oxili_gfx3d_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gfx3d_clk_src",
		.parent_names = gcc_xo_gpll0a_gpll1_gpll2a,
		.num_parents = 4,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_blsp1_qup1_6_i2c_apps_clk[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(50000000, P_GPLL0, 16, 0, 0),
	{ }
};

static struct clk_rcg2 blsp1_qup1_i2c_apps_clk_src = {
	.cmd_rcgr = 0x0200c,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_qup1_6_i2c_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_qup1_i2c_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 blsp1_qup2_i2c_apps_clk_src = {
	.cmd_rcgr = 0x03000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_qup1_6_i2c_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_qup2_i2c_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 blsp1_qup3_i2c_apps_clk_src = {
	.cmd_rcgr = 0x04000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_qup1_6_i2c_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_qup3_i2c_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 blsp1_qup4_i2c_apps_clk_src = {
	.cmd_rcgr = 0x05000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_qup1_6_i2c_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_qup4_i2c_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 blsp1_qup5_i2c_apps_clk_src = {
	.cmd_rcgr = 0x06000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_qup1_6_i2c_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_qup5_i2c_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 blsp1_qup6_i2c_apps_clk_src = {
	.cmd_rcgr = 0x07000,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_qup1_6_i2c_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_qup6_i2c_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_blsp1_uart1_6_apps_clk[] = {
	F(3686400, P_GPLL0, 1, 72, 15625),
	F(7372800, P_GPLL0, 1, 144, 15625),
	F(14745600, P_GPLL0, 1, 288, 15625),
	F(16000000, P_GPLL0, 10, 1, 5),
	F(19200000, P_XO, 1, 0, 0),
	F(24000000, P_GPLL0, 1, 3, 100),
	F(25000000, P_GPLL0, 16, 1, 2),
	F(32000000, P_GPLL0, 1, 1, 25),
	F(40000000, P_GPLL0, 1, 1, 20),
	F(46400000, P_GPLL0, 1, 29, 500),
	F(48000000, P_GPLL0, 1, 3, 50),
	F(51200000, P_GPLL0, 1, 8, 125),
	F(56000000, P_GPLL0, 1, 7, 100),
	F(58982400, P_GPLL0, 1, 1152, 15625),
	F(60000000, P_GPLL0, 1, 3, 40),
	{ }
};

static struct clk_rcg2 blsp1_uart1_apps_clk_src = {
	.cmd_rcgr = 0x02044,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_uart1_6_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_uart1_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 blsp1_uart2_apps_clk_src = {
	.cmd_rcgr = 0x03034,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = gcc_xo_gpll0_map,
	.freq_tbl = ftbl_gcc_blsp1_uart1_6_apps_clk,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "blsp1_uart2_apps_clk_src",
		.parent_names = gcc_xo_gpll0,
		.num_parents = 2,
		.ops = &clk_rcg2_ops,
	},
};

static DEFINE_VDD_REGULATORS(vdd_dig, VDD_DIG_NUM, 1, vdd_corner);
static DEFINE_VDD_REGULATORS(vdd_sr2_pll, VDD_SR2_PLL_NUM, 2, vdd_sr2_levels);
static DEFINE_VDD_REGULATORS(vdd_hf_pll, VDD_HF_PLL_NUM, 2, vdd_hf_levels);

static const struct regmap_config gcc_msm8936_regmap_config = {
	.reg_bits	= 32,
	.reg_stride	= 4,
	.val_bits	= 32,
	.max_register	= 0x80000,
	.fast_io	= true,
};

static struct clk_regmap *gcc_msm8936_clocks[] = {
	[GPLL0] = &gpll0.clkr,
	[GPLL1] = &gpll1.clkr,
	[GPLL2] = &gpll2.clkr,
	[GPLL3] = &gpll3.clkr,
	[GPLL4] = &gpll4.clkr,
	[GPLL6] = &gpll6.clkr,

	[APSS_AHB_CLK_SRC] = &apss_ahb_clk_src.clkr,
	[CAMSS_AHB_CLK_SRC] = &camss_ahb_clk_src.clkr,
	[CSI0_CLK_SRC] = &csi0_clk_src.clkr,
	[CSI1_CLK_SRC] = &csi1_clk_src.clkr,
	[CSI2_CLK_SRC] = &csi2_clk_src.clkr,
	[VFE0_CLK_SRC] = &vfe0_clk_src.clkr,
	[MDP_CLK_SRC] = &mdp_clk_src.clkr,
	[GFX3D_CLK_SRC] = &gfx3d_clk_src.clkr,

	[BLSP1_QUP1_I2C_APPS_CLK] = &blsp1_qup1_i2c_apps_clk_src.clkr,
	[BLSP1_QUP2_I2C_APPS_CLK] = &blsp1_qup2_i2c_apps_clk_src.clkr,
	[BLSP1_QUP3_I2C_APPS_CLK] = &blsp1_qup3_i2c_apps_clk_src.clkr,
	[BLSP1_QUP4_I2C_APPS_CLK] = &blsp1_qup4_i2c_apps_clk_src.clkr,
	[BLSP1_QUP5_I2C_APPS_CLK] = &blsp1_qup5_i2c_apps_clk_src.clkr,
	[BLSP1_QUP6_I2C_APPS_CLK] = &blsp1_qup6_i2c_apps_clk_src.clkr,
	[BLSP1_UART1_APPS_CLK] = &blsp1_uart1_apps_clk_src.clkr,
	[BLSP1_UART2_APPS_CLK] = &blsp1_uart2_apps_clk_src.clkr,
};

static const struct qcom_cc_desc gcc_msm8936_desc = {
	.config		= &gcc_msm8936_regmap_config,
	.clks		= gcc_msm8936_clocks,
//	.num_clks	= ARRAY_SIZE(gcc_msm8936_clocks),
//	.resets         = gcc_msm8936_resets,
//	.num_resets     = ARRAY_SIZE(gcc_msm8936_resets),
//	.gdscs = gcc_msm8936_gdscs,
//	.num_gdscs = ARRAY_SIZE(gcc_msm8936_gdscs),
};

static const struct of_device_id gcc_msm8936_match_table[] = {
	{ .compatible = "qcom,gcc-msm8936" },
	{},
};
MODULE_DEVICE_TABLE(of, gcc_msm8936_match_table);

#define GCC_REG_BASE 0x1800000
static int gcc_msm8936_probe(struct platform_device *pdev)
{
	struct regmap *regmap;
	//struct clk *tmp_clk;
	int ret;

	regmap = qcom_cc_map(pdev, &gcc_msm8936_desc);
	if (IS_ERR(regmap))
		return -EPROBE_DEFER; //PTR_ERR(regmap);

	vdd_dig.regulator[0] = devm_regulator_get(&pdev->dev, "vdd_dig");
	if (IS_ERR(vdd_dig.regulator[0])) {
		if (!(PTR_ERR(vdd_dig.regulator[0]) == -EPROBE_DEFER))
			dev_err(&pdev->dev,
				"Unable to get vdd_dig regulator!!!\n");
		return -EPROBE_DEFER; //PTR_ERR(vdd_dig.regulator[0]);
	}

	vdd_sr2_pll.regulator[0] = devm_regulator_get(&pdev->dev, "vdd_sr2_pll");
	if (IS_ERR(vdd_sr2_pll.regulator[0])) {
		if (!(PTR_ERR(vdd_sr2_pll.regulator[0]) == -EPROBE_DEFER))
			dev_err(&pdev->dev,
				"Unable to get vdd_sr2_pll regulator!!!\n");
		return -EPROBE_DEFER; //PTR_ERR(vdd_sr2_pll.regulator[0]);
	}

	vdd_sr2_pll.regulator[1] = devm_regulator_get(&pdev->dev, "vdd_sr2_dig");
	if (IS_ERR(vdd_sr2_pll.regulator[1])) {
		if (!(PTR_ERR(vdd_sr2_pll.regulator[1]) == -EPROBE_DEFER))
			dev_err(&pdev->dev,
				"Unable to get vdd_sr2_dig regulator!!!\n");
		return -EPROBE_DEFER; //PTR_ERR(vdd_sr2_pll.regulator[1]);
	}

	vdd_hf_pll.regulator[0] = devm_regulator_get(&pdev->dev, "vdd_hf_pll");
	if (IS_ERR(vdd_hf_pll.regulator[0])) {
		if (!(PTR_ERR(vdd_hf_pll.regulator[0]) == -EPROBE_DEFER))
			dev_err(&pdev->dev,
				"Unable to get vdd_sr2_pll regulator!!!\n");
		return PTR_ERR(vdd_hf_pll.regulator[0]);
	}

	vdd_hf_pll.regulator[1] = devm_regulator_get(&pdev->dev, "vdd_hf_dig");
	if (IS_ERR(vdd_hf_pll.regulator[1])) {
		if (!(PTR_ERR(vdd_hf_pll.regulator[1]) == -EPROBE_DEFER))
			dev_err(&pdev->dev,
				"Unable to get vdd_hf_dig regulator!!!\n");
		return PTR_ERR(vdd_hf_pll.regulator[1]);
	}

	/* Vote for GPLL0 to turn on. Needed by acpuclock. */
	regmap_update_bits(regmap, 0x45000, BIT(0), BIT(0));

	ret = qcom_cc_really_probe(pdev, &gcc_msm8936_desc, regmap);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register GCC clocks\n");
		return ret;
	}

	/*
	 * TODO: UNCOMMENT ME WHEN CLOCK TABLE BE DONE
	 * clk_set_rate(&apss_ahb_clk_src.c, 19200000);
	 * clk_prepare_enable(&apss_ahb_clk_src.c);
	 */

	ret = of_platform_populate(pdev->dev.of_node, NULL, NULL, &pdev->dev);
	if (ret)
		return ret;

	dev_info(&pdev->dev, "Registered GCC clocks\n");

	return 0;
}


static struct platform_driver gcc_msm8936_driver = {
	.probe		= gcc_msm8936_probe,
	.driver		= {
		.name	= "gcc-msm8936",
		.of_match_table = gcc_msm8936_match_table,
	},
};

static int __init gcc_msm8936_init(void)
{
	return platform_driver_register(&gcc_msm8936_driver);
}
core_initcall(gcc_msm8936_init);

static void __exit gcc_msm8936_exit(void)
{
	platform_driver_unregister(&gcc_msm8936_driver);
}
module_exit(gcc_msm8936_exit);

MODULE_DESCRIPTION("Qualcomm GCC MSM8936 Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:gcc-msm8936");
