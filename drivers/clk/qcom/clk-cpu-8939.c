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

#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/cpu.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/pm_opp.h>
#include <linux/pm_qos.h>
#include <linux/regulator/consumer.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/regmap.h>
#include <linux/suspend.h>
#include <linux/regulator/rpm-smd-regulator.h>

#include <dt-bindings/clock/qcom,cpu-8939.h>

#include "clk-debug.h"
#include "common.h"
#include "clk-pll.h"
#include "clk-hfpll.h"
#include "clk-rcg.h"
#include "clk-regmap-mux-div.h"

static const char *mux_names[] = {"c0", "c1", "cci"};

#define to_clk_regmap_mux_div(_hw) \
	container_of(to_clk_regmap(_hw), struct clk_regmap_mux_div, clkr)

#define F_APCS_PLL(f, l, m, n) { (f), (l), (m), (n), 0 }

#define CPU_LATENCY_NO_L2_PC_US (250)

DEFINE_VDD_REGS_INIT(vdd_c0, 1);
DEFINE_VDD_REGS_INIT(vdd_c1, 1);
DEFINE_VDD_REGS_INIT(vdd_cci, 1);

enum {
	APCS_C0_PLL_BASE,
	APCS_C1_PLL_BASE,
	APCS_CCI_PLL_BASE,
	N_BASES,
};

enum {
	A53SS_MUX_C0,
	A53SS_MUX_C1,
	A53SS_MUX_CCI,
	A53SS_MUX_NUM,
};

static void __iomem *virt_bases[N_BASES];

enum {
	P_GPLL0,
	P_A53SSPLL,
};

struct cpu_desc_8939 {
	cpumask_t cpumask;
	struct pm_qos_request req;
};
static struct cpu_desc_8939 a53ssmux_lc_desc;
static struct cpu_desc_8939 a53ssmux_bc_desc;
static void do_nothing(void *unused) { }

static const struct parent_map gpll0_a53sspll_map[] = {
	{ P_GPLL0, 4 },
	{ P_A53SSPLL, 5 },
};

static const char * const gpll0_a53sspll[] = {
	"gpll0_vote",
	"a53ss_c1_pll",
};

static const struct pll_freq_tbl apcs_c0_pll_freq[] = {
	F_APCS_PLL(998400000,	52, 0x0, 0x1),
	F_APCS_PLL(1113600000,	58, 0x0, 0x1),
	F_APCS_PLL(1209600000,	63, 0x0, 0x1),
};

static struct clk_pll a53ss_c0_pll = {
	.l_reg = 0x00004,
	.m_reg = 0x00008,
	.n_reg = 0x0000c,
	.config_reg = 0x00010,
	.mode_reg = 0x00000,
	.status_reg = 0x0001c,
	.status_bit = 17,
	.freq_tbl = apcs_c0_pll_freq,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "a53ss_c0_pll",
		.parent_names = (const char *[]){ "xo_a" },
		.num_parents = 1,
		.ops = &clk_pll_sr2_ops,
	},
};

static const struct pll_freq_tbl apcs_c1_pll_freq[] = {
	F_APCS_PLL(652800000,	34, 0x0, 0x1),
	F_APCS_PLL(691200000,	36, 0x0, 0x1),
	F_APCS_PLL(729600000,	38, 0x0, 0x1),
	F_APCS_PLL(806400000,	42, 0x0, 0x1),
	F_APCS_PLL(844800000,	44, 0x0, 0x1),
	F_APCS_PLL(883200000,	46, 0x0, 0x1),
	F_APCS_PLL(960000000,	50, 0x0, 0x1),
	F_APCS_PLL(998400000,	52, 0x0, 0x1),
	F_APCS_PLL(1036800000,	54, 0x0, 0x1),
	F_APCS_PLL(1113600000,	58, 0x0, 0x1),
	F_APCS_PLL(1209600000,	63, 0x0, 0x1),
	F_APCS_PLL(1190400000,	62, 0x0, 0x1),
	F_APCS_PLL(1267200000,	66, 0x0, 0x1),
	F_APCS_PLL(1344000000,	70, 0x0, 0x1),
	F_APCS_PLL(1363200000,	71, 0x0, 0x1),
	F_APCS_PLL(1420800000,	74, 0x0, 0x1),
	F_APCS_PLL(1459200000,	76, 0x0, 0x1),
	F_APCS_PLL(1497600000,	78, 0x0, 0x1),
	F_APCS_PLL(1536000000,	80, 0x0, 0x1),
	F_APCS_PLL(1574400000,	82, 0x0, 0x1),
	F_APCS_PLL(1612800000,	84, 0x0, 0x1),
	F_APCS_PLL(1632000000,	85, 0x0, 0x1),
	F_APCS_PLL(1651200000,	86, 0x0, 0x1),
	F_APCS_PLL(1689600000,	88, 0x0, 0x1),
	F_APCS_PLL(1708800000,	89, 0x0, 0x1),
};

static struct clk_pll a53ss_c1_pll = {
	.l_reg = 0x00004,
	.m_reg = 0x00008,
	.n_reg = 0x0000c,
	.config_reg = 0x00010,
	.mode_reg = 0x00000,
	.status_reg = 0x0001c,
	.status_bit = 17,
	.freq_tbl = apcs_c1_pll_freq,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "a53ss_c1_pll",
		.parent_names = (const char *[]){ "xo_a" },
		.num_parents = 1,
		.ops = &clk_pll_sr2_ops,
	},
};

static const struct pll_freq_tbl apcs_cci_pll_freq[] = {
	F_APCS_PLL(403200000, 21, 0x0, 0x1),
	F_APCS_PLL(595200000, 31, 0x0, 0x1),
};

static struct clk_pll a53ss_cci_pll = {
	.l_reg = 0x00004,
	.m_reg = 0x00008,
	.n_reg = 0x0000c,
	.config_reg = 0x00010,
	.mode_reg = 0x00000,
	.status_reg = 0x0001c,
	.status_bit = 17,
	.freq_tbl = apcs_cci_pll_freq,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "a53ss_cci_pll",
		.parent_names = (const char *[]){ "xo_a" },
		.num_parents = 1,
		.ops = &clk_pll_sr2_ops,
	},
};

/*
 * We use the notifier function for switching to a temporary safe configuration
 * (mux and divider), while the A53 PLL is reconfigured.
 */
static int cpu_clk_8939_notifier_cb(struct notifier_block *nb,
				unsigned long evt, void *data)
{
	int ret = 0;
	struct clk_regmap_mux_div *cpuclk = container_of(nb,
					struct clk_regmap_mux_div, clk_nb);

	if (evt == PRE_RATE_CHANGE)
		ret = __mux_div_set_src_div(cpuclk, 4, 1);

	if (evt == ABORT_RATE_CHANGE)
		pr_err("Error in configuring PLL - stay at safe src only\n");

	return notifier_from_errno(ret);
}

static int cpu_clk_8939_cci_notifier_cb(struct notifier_block *nb,
				unsigned long evt, void *data)
{
	int ret = 0;
	struct clk_regmap_mux_div *cpuclk = container_of(nb,
					struct clk_regmap_mux_div, clk_nb);

	if (evt == PRE_RATE_CHANGE)
		ret = __mux_div_set_src_div(cpuclk, 4, 4);

	if (evt == ABORT_RATE_CHANGE)
		pr_err("Error in configuring PLL - stay at safe src only\n");

	return notifier_from_errno(ret);
}

static int cpu_clk_8939_set_rate(struct clk_hw *hw, unsigned long rate,
			unsigned long parent_rate)
{
	struct clk_regmap_mux_div *cpuclk = to_clk_regmap_mux_div(hw);

	return __mux_div_set_src_div(cpuclk, cpuclk->src, cpuclk->div);
}

static int cpu_clk_8939_set_rate_and_parent(struct clk_hw *hw,
						unsigned long rate,
						unsigned long prate,
						u8 index)
{
	struct clk_regmap_mux_div *cpuclk = to_clk_regmap_mux_div(hw);

	return __mux_div_set_src_div(cpuclk, cpuclk->parent_map[index].cfg,
					cpuclk->div);
}

static void cpu_clk_8939_pm_qos_add_req(struct cpu_desc_8939* cluster_desc)
{
	memset(&cluster_desc->req, 0, sizeof(cluster_desc->req));
	cpumask_copy(&(cluster_desc->req.cpus_affine),
			(const struct cpumask *)&cluster_desc->cpumask);
	cluster_desc->req.type = PM_QOS_REQ_AFFINE_CORES;
	pm_qos_add_request(&cluster_desc->req, PM_QOS_CPU_DMA_LATENCY,
			CPU_LATENCY_NO_L2_PC_US);
	smp_call_function_any(&cluster_desc->cpumask, do_nothing,
				NULL, 1);
}

static int cpu_clk_8939_set_rate_little(struct clk_hw *hw, unsigned long rate,
			unsigned long parent_rate)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53ssmux_lc_desc);

	rc = cpu_clk_8939_set_rate(hw, rate, parent_rate);

	pm_qos_remove_request(&a53ssmux_lc_desc.req);

	return rc;
}

static int cpu_clk_8939_set_rate_and_parent_little(struct clk_hw *hw,
						unsigned long rate,
						unsigned long prate,
						u8 index)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53ssmux_lc_desc);

	rc = cpu_clk_8939_set_rate_and_parent(hw, rate, prate, index);

	pm_qos_remove_request(&a53ssmux_lc_desc.req);

	return rc;
}


static int cpu_clk_8939_set_rate_big(struct clk_hw *hw, unsigned long rate,
			unsigned long parent_rate)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53ssmux_bc_desc);

	rc = cpu_clk_8939_set_rate(hw, rate, parent_rate);

	pm_qos_remove_request(&a53ssmux_bc_desc.req);

	return rc;
}

static int cpu_clk_8939_set_rate_and_parent_big(struct clk_hw *hw,
						unsigned long rate,
						unsigned long prate,
						u8 index)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53ssmux_bc_desc);

	rc = cpu_clk_8939_set_rate_and_parent(hw, rate, prate, index);

	pm_qos_remove_request(&a53ssmux_bc_desc.req);

	return rc;
}

static int cpu_clk_8939_enable(struct clk_hw *hw)
{
	return clk_regmap_mux_div_ops.enable(hw);
}

static void cpu_clk_8939_disable(struct clk_hw *hw)
{
	clk_regmap_mux_div_ops.disable(hw);
}

static u8 cpu_clk_8939_get_parent(struct clk_hw *hw)
{
	return clk_regmap_mux_div_ops.get_parent(hw);
}

static int cpu_clk_8939_set_parent(struct clk_hw *hw, u8 index)
{
	/*
	 * We have defined set_rate_and_parent and this function
	 * will never get called in this driver's whole life,
	 * however, we need to define it to make the clk API to
	 * be happy.
	 */
	return 0;
}

static unsigned long cpu_clk_8939_recalc_rate(struct clk_hw *hw,
					unsigned long prate)
{
	struct clk_regmap_mux_div *cpuclk = to_clk_regmap_mux_div(hw);
	struct clk_hw *parent;
	unsigned long parent_rate;
	u32 i, div, num_parents, src = 0;
	int ret = 0;

	ret = mux_div_get_src_div(cpuclk, &src, &div);
	if (ret)
		return ret;

	num_parents = clk_hw_get_num_parents(hw);
	for (i = 0; i < num_parents; i++) {
		if (src == cpuclk->parent_map[i].cfg) {
			parent = clk_hw_get_parent_by_index(hw, i);
			parent_rate = clk_hw_get_rate(parent);

			parent_rate *= 2;
			parent_rate /= div + 1;
			return parent_rate;
		}
	}
	pr_err("%s: Can't find parent %d\n", clk_hw_get_name(hw), src);
	return ret;
}

static int cpu_clk_8939_determine_rate(struct clk_hw *hw,
					struct clk_rate_request *req)
{
	int ret;
	u32 div = 1;
	struct clk_hw *cpu_pll_main_hw;
	unsigned long rate = req->rate;
	struct clk_rate_request parent_req = { };
	struct clk_regmap_mux_div *cpuclk = to_clk_regmap_mux_div(hw);
	int pll_clk_index = P_A53SSPLL;

	cpu_pll_main_hw = clk_hw_get_parent_by_index(hw, P_A53SSPLL);

	parent_req.rate = rate;
	parent_req.best_parent_hw = cpu_pll_main_hw;

	req->best_parent_hw = cpu_pll_main_hw;
	ret = __clk_determine_rate(req->best_parent_hw, &parent_req);

	cpuclk->src = cpuclk->parent_map[pll_clk_index].cfg;
	req->best_parent_rate = parent_req.rate;

	cpuclk->div = div;

	return 0;
}

static int cpu_clk_8939_determine_rate_cci(struct clk_hw *hw,
					struct clk_rate_request *req)
{
	int ret;
	u32 div = 1;
	struct clk_hw *cpu_pll_main_hw;
	unsigned long rate = req->rate;
	struct clk_rate_request parent_req = { };
	struct clk_regmap_mux_div *cpuclk = to_clk_regmap_mux_div(hw);
	int pll_clk_index = P_GPLL0;

	cpu_pll_main_hw = clk_hw_get_parent_by_index(hw, P_GPLL0);

	parent_req.rate = rate;
	parent_req.best_parent_hw = cpu_pll_main_hw;

	req->best_parent_hw = cpu_pll_main_hw;
	ret = __clk_determine_rate(req->best_parent_hw, &parent_req);

	cpuclk->src = cpuclk->parent_map[pll_clk_index].cfg;
	req->best_parent_rate = parent_req.rate;

	cpuclk->div = div;

	return 0;
}


static const struct clk_ops clk_ops_cpu_little = {
	.enable = cpu_clk_8939_enable,
	.disable = cpu_clk_8939_disable,
	.get_parent = cpu_clk_8939_get_parent,
	.set_rate = cpu_clk_8939_set_rate_little,
	.set_rate_and_parent = cpu_clk_8939_set_rate_and_parent_little,
	.set_parent = cpu_clk_8939_set_parent,
	.recalc_rate = cpu_clk_8939_recalc_rate,
	.determine_rate = cpu_clk_8939_determine_rate,
	.debug_init = clk_debug_measure_add,
};

static const struct clk_ops clk_ops_cpu_big = {
	.enable = cpu_clk_8939_enable,
	.disable = cpu_clk_8939_disable,
	.get_parent = cpu_clk_8939_get_parent,
	.set_rate = cpu_clk_8939_set_rate_big,
	.set_rate_and_parent = cpu_clk_8939_set_rate_and_parent_big,
	.set_parent = cpu_clk_8939_set_parent,
	.recalc_rate = cpu_clk_8939_recalc_rate,
	.determine_rate = cpu_clk_8939_determine_rate,
	.debug_init = clk_debug_measure_add,
};

static const struct clk_ops clk_ops_cpu_cci = {
	.enable = cpu_clk_8939_enable,
	.disable = cpu_clk_8939_disable,
	.get_parent = cpu_clk_8939_get_parent,
	.set_rate = cpu_clk_8939_set_rate,
	.set_rate_and_parent = cpu_clk_8939_set_rate_and_parent,
	.set_parent = cpu_clk_8939_set_parent,
	.recalc_rate = cpu_clk_8939_recalc_rate,
	.determine_rate = cpu_clk_8939_determine_rate_cci,
	.debug_init = clk_debug_measure_add,
};

static struct clk_init_data cpu_8939_clk_init_data[A53SS_MUX_NUM] = {
	[A53SS_MUX_C0] = {
		.name = "a53ssmux_lc",
		.parent_names = gpll0_a53sspll,
		.num_parents = ARRAY_SIZE(gpll0_a53sspll_map),
		.vdd_class = &vdd_c0,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_ops_cpu_little,
	},
	[A53SS_MUX_C1] = {
		.name = "a53ssmux_bc",
		.parent_names = gpll0_a53sspll,
		.num_parents = ARRAY_SIZE(gpll0_a53sspll_map),
		.vdd_class = &vdd_c1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_ops_cpu_big,
	},
	[A53SS_MUX_CCI] = {
		.name = "a53ssmux_cci",
		.parent_names = gpll0_a53sspll,
		.num_parents = ARRAY_SIZE(gpll0_a53sspll_map),
		.vdd_class = &vdd_cci,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_ops_cpu_cci,
	},
};

static struct clk_regmap_mux_div a53ssmux_lc = {
	.reg_offset = 0x50,
	.hid_width = 5,
	.hid_shift = 0,
	.src_width = 3,
	.src_shift = 8,
	.safe_src = 4,
	.safe_freq = 200000000,
	.parent_map = gpll0_a53sspll_map,
	.clk_nb.notifier_call = cpu_clk_8939_notifier_cb,
	.clkr.hw.init = &cpu_8939_clk_init_data[A53SS_MUX_C0],
};

static struct clk_regmap_mux_div a53ssmux_bc = {
	.reg_offset = 0x50,
	.hid_width = 5,
	.hid_shift = 0,
	.src_width = 3,
	.src_shift = 8,
	.safe_src = 4,
	.safe_freq = 400000000,
	.parent_map = gpll0_a53sspll_map,
	.clk_nb.notifier_call = cpu_clk_8939_notifier_cb,
	.clkr.hw.init = &cpu_8939_clk_init_data[A53SS_MUX_C1],
};

static struct clk_regmap_mux_div a53ssmux_cci = {
	.reg_offset = 0x50,
	.hid_width = 5,
	.hid_shift = 0,
	.src_width = 3,
	.src_shift = 8,
	.safe_src = 4,
	.safe_freq = 200000000,
	.parent_map = gpll0_a53sspll_map,
	.clk_nb.notifier_call = cpu_clk_8939_cci_notifier_cb,
	.clkr.hw.init = &cpu_8939_clk_init_data[A53SS_MUX_CCI],
};

static const struct regmap_config pll_blocks_regmap_config = {
	.reg_bits       = 32,
	.reg_stride     = 4,
	.val_bits       = 32,
	.max_register	= 0x40,
	.fast_io        = true,
};

static void get_speed_bin(struct platform_device *pdev, int *bin,
							int *version)
{
	struct resource *res;
	void __iomem *base, *base1, *base2;
	u32 pte_efuse, pte_efuse1, pte_efuse2;

	*bin = 0;
	*version = 0;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "efuse");
	if (!res) {
		dev_info(&pdev->dev,
			 "No speed/PVS binning available. Defaulting to 0!\n");
		return;
	}

	base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!base) {
		dev_warn(&pdev->dev,
			 "Unable to read efuse data. Defaulting to 0!\n");
		return;
	}

	pte_efuse = readl_relaxed(base);
	devm_iounmap(&pdev->dev, base);

	*bin = (pte_efuse >> 2) & 0x7;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "efuse1");
	if (!res) {
		dev_info(&pdev->dev,
			 "No PVS version available. Defaulting to 0!\n");
		goto out;
	}

	base1 = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!base1) {
		dev_warn(&pdev->dev,
			 "Unable to read efuse1 data. Defaulting to 0!\n");
		goto out;
	}

	pte_efuse1 = readl_relaxed(base1);
	devm_iounmap(&pdev->dev, base1);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "efuse2");
	if (!res) {
		dev_info(&pdev->dev,
			 "No PVS version available. Defaulting to 0!\n");
		goto out;
	}

	base2 = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!base2) {
		dev_warn(&pdev->dev,
			 "Unable to read efuse2 data. Defaulting to 0!\n");
		goto out;
	}

	pte_efuse2 = readl_relaxed(base2);
	devm_iounmap(&pdev->dev, base2);

	*version = ((pte_efuse1 >> 29 & 0x1) | ((pte_efuse2 >> 18 & 0x3) << 1));

out:
	dev_info(&pdev->dev, "Speed bin: %d PVS Version: %d\n", *bin,
								*version);
}

static int cpu_8939_map_pll(struct platform_device *pdev,
				struct clk_regmap *clkr,
				int virt_id, char* res_name)
{
	struct resource *res;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, res_name);
	if (!res) {
		dev_err(&pdev->dev, "Cannot get cluster %s PLL\n", res_name);
		return -EINVAL;
	}

	virt_bases[virt_id] = devm_ioremap(&pdev->dev, res->start,
						resource_size(res));
	if (!virt_bases[APCS_C0_PLL_BASE]) {
		dev_err(&pdev->dev, "Cannot remap cluster %s PLL\n", res_name);
		return -EINVAL;
	}

	clkr->regmap = devm_regmap_init_mmio(&pdev->dev, virt_bases[virt_id],
				&pll_blocks_regmap_config);
	if (IS_ERR(clkr->regmap)) {
		dev_err(&pdev->dev, "Cannot init regmap MMIO for %s PLL\n",
				res_name);
		return PTR_ERR(clkr->regmap);
	};

	return 0;
}

static int cpu_parse_devicetree(struct platform_device *pdev)
{
	int rc;

	rc = cpu_8939_map_pll(pdev, &a53ss_c0_pll.clkr, APCS_C0_PLL_BASE, "apcs-c0-rcg-base");
	if (rc) {
		pr_info("------------ERROR apcs-c0-rcg-base!!!!-----------\n");
		return rc;
	};
pr_info("------------DONE apcs-c0-rcg-base!!!!-----------\n");

	rc = cpu_8939_map_pll(pdev, &a53ss_c1_pll.clkr, APCS_C1_PLL_BASE, "apcs-c1-rcg-base");
	if (rc) {
		pr_info("------------ERROR apcs-c1-rcg-base!!!!-----------\n");
		return rc;
	};
pr_info("------------DONE apcs-c0-rcg-base!!!!-----------\n");

	rc = cpu_8939_map_pll(pdev, &a53ss_cci_pll.clkr, APCS_CCI_PLL_BASE, "apcs-cci-rcg-base");
	if (rc) {
		pr_info("------------ERROR apcs-cci-rcg-base!!!!-----------\n");
		return rc;
	};
pr_info("------------DONE apcs-cci-rcg-base!!!!-----------\n");

	vdd_c0.regulator[0] = devm_regulator_get(&pdev->dev, "vdd-c0");
	if (IS_ERR(vdd_c0.regulator[0])) {
		pr_err("vdd-c0 vreg err %ld\n", PTR_ERR(vdd_c0.regulator[0]));
		if (PTR_ERR(vdd_c0.regulator[0]) != -EPROBE_DEFER)
			dev_err(&pdev->dev, "Unable to get vdd_c0 regulator\n");
		return PTR_ERR(vdd_c0.regulator[0]);
	}
	vdd_c0.use_max_uV = true;
pr_info("------------DONE vdd-c0!!!!-----------\n");

	vdd_c1.regulator[0] = devm_regulator_get(&pdev->dev, "vdd-c1");
	if (IS_ERR(vdd_c1.regulator[0])) {
		pr_err("vdd-c1 vreg err %ld\n", PTR_ERR(vdd_c1.regulator[0]));
		if (PTR_ERR(vdd_c1.regulator[0]) != -EPROBE_DEFER)
			dev_err(&pdev->dev, "Unable to get vdd_c1 regulator\n");
		return PTR_ERR(vdd_c1.regulator[0]);
	}
	vdd_c1.use_max_uV = true;
pr_info("------------DONE vdd-c1!!!!-----------\n");

	vdd_cci.regulator[0] = devm_regulator_get(&pdev->dev, "vdd-cci");
	if (IS_ERR(vdd_cci.regulator[0])) {
		pr_err("vdd-cci vreg err %ld\n", PTR_ERR(vdd_cci.regulator[0]));
		if (PTR_ERR(vdd_cci.regulator[0]) != -EPROBE_DEFER)
			dev_err(&pdev->dev, "Unable to get vdd_cci regulator\n");
		return PTR_ERR(vdd_cci.regulator[0]);
	}
	vdd_cci.use_max_uV = true;
pr_info("------------DONE vdd-cci!!!!-----------\n");


	return 0;
};

static struct clk_hw *clk_cpu_8939_hws[] = {
	[P_A53_C0]	= &a53ssmux_lc.clkr.hw,
	[P_A53_C1]	= &a53ssmux_bc.clkr.hw,
	[P_A53_CCI]	= &a53ssmux_cci.clkr.hw,
};

static int of_get_fmax_vdd_class(struct platform_device *pdev,
				 int mux_id, char *prop_name)
{
	struct device_node *of = pdev->dev.of_node;
	int prop_len, i, j;
	struct clk_vdd_class *vdd = NULL;
	int nreg = 2;
	u32 *array;

	if (!of_find_property(of, prop_name, &prop_len)) {
		dev_err(&pdev->dev, "missing %s\n", prop_name);
		return -EINVAL;
	}

	prop_len /= sizeof(u32);
	if (prop_len % nreg) {
		dev_err(&pdev->dev, "bad length %d\n", prop_len);
		return -EINVAL;
	}

	prop_len /= nreg;

	vdd = cpu_8939_clk_init_data[mux_id].vdd_class;

	vdd->level_votes = devm_kzalloc(&pdev->dev,
				prop_len * sizeof(*vdd->level_votes),
					GFP_KERNEL);
	if (!vdd->level_votes) {
		pr_err("Cannot allocate memory for level_votes\n");
		return -ENOMEM;
	}

	vdd->vdd_uv = devm_kzalloc(&pdev->dev,
		prop_len * sizeof(int) * (nreg - 1), GFP_KERNEL);
	if (!vdd->vdd_uv) {
		pr_err("Cannot allocate memory for vdd_uv\n");
		return -ENOMEM;
	}

	cpu_8939_clk_init_data[mux_id].rate_max = devm_kzalloc(&pdev->dev,
					prop_len * sizeof(unsigned long),
					GFP_KERNEL);
	if (!cpu_8939_clk_init_data[mux_id].rate_max) {
		pr_err("Cannot allocate memory for rate_max\n");
		return -ENOMEM;
	}

	array = devm_kzalloc(&pdev->dev,
			prop_len * sizeof(u32) * nreg, GFP_KERNEL);
	if (!array)
		return -ENOMEM;

pr_info("%s: ---11111111----\n", __func__);

	of_property_read_u32_array(of, prop_name, array, prop_len * nreg);
	for (i = 0; i < prop_len; i++) {
		cpu_8939_clk_init_data[mux_id].rate_max[i] = array[nreg * i];
		for (j = 1; j < nreg; j++)
			vdd->vdd_uv[(nreg - 1) * i + (j - 1)] =
					array[nreg * i + j];
	}

	devm_kfree(&pdev->dev, array);
	vdd->num_levels = prop_len;
	vdd->cur_level = prop_len;
	vdd->use_max_uV = true;
	cpu_8939_clk_init_data[mux_id].num_rate_max = prop_len;

	return 0;
}

static int clock_a53_probe(struct platform_device *pdev)
{
	char prop_name[] = "qcom,speedX-bin-vX-XXX";
	int mux_id, rc, speed_bin, version, clks_sz, i;
	struct clk *clk_tmp = NULL;
	struct clk_onecell_data *clk_onecell = NULL;

	get_speed_bin(pdev, &speed_bin, &version);

	rc = cpu_parse_devicetree(pdev);
	if (rc)
		return rc;


	clk_onecell = devm_kzalloc(&pdev->dev, sizeof(struct clk_onecell_data),
					GFP_KERNEL);
	if (!clk_onecell)
		return -ENOMEM;

	clks_sz = ARRAY_SIZE(clk_cpu_8939_hws) * sizeof(struct clk *);
	clk_onecell->clks = devm_kzalloc(&pdev->dev, clks_sz, GFP_KERNEL);
	if (!clk_onecell->clks) {
		devm_kfree(&pdev->dev, clk_onecell);
		return -ENOMEM;
	}


pr_info("--------1111!!!!!!!-----\n");

	for (mux_id = 0; mux_id < A53SS_MUX_NUM; mux_id++) {
		snprintf(prop_name, ARRAY_SIZE(prop_name),
					"qcom,speed%d-bin-v%d-%s",
					speed_bin, version, mux_names[mux_id]);
pr_info("qcom,speed%d-bin-v%d-%s\n", speed_bin, version, mux_names[mux_id]);

		rc = of_get_fmax_vdd_class(pdev, mux_id, prop_name);
pr_info("---------222222!!!!------\n");
		if (rc) {
			/* Fall back to most conservative PVS table */
			dev_err(&pdev->dev, "Unable to load voltage plan %s!\n",
								prop_name);
			pr_err("Error %d\n", rc);
			snprintf(prop_name, ARRAY_SIZE(prop_name),
				"qcom,speed0-bin-v0-%s", mux_names[mux_id]);
			rc = of_get_fmax_vdd_class(pdev, mux_id, prop_name);
			if (rc) {
				dev_err(&pdev->dev,
					"Unable to load safe voltage plan\n");
				return rc;
			}
pr_info("-------------44444!!!!!-----\n");
			dev_info(&pdev->dev, "Safe voltage plan loaded.\n");
		}
	}

	for (i = 0; i < ARRAY_SIZE(clk_cpu_8939_hws); i++) {
		clk_tmp = devm_clk_register(&pdev->dev, clk_cpu_8939_hws[i]);
		if (IS_ERR(clk_tmp)) {
			dev_err(&pdev->dev,
				"Cannot register HW clock at position %d\n",i);
			return PTR_ERR(clk_tmp);
		}
		clk_onecell->clks[i] = clk_tmp;
	}

	return 0;
};

static struct of_device_id clock_a53_match_table[] = {
	{ .compatible = "qcom,clk-cpu-msm8939" },
	{ }
};

static struct platform_driver clock_a53_driver = {
	.probe = clock_a53_probe,
	.driver = {
		.name = "clk-cpu-msm8939",
		.of_match_table = clock_a53_match_table,
		.owner = THIS_MODULE,
	},
};

static int __init clock_a53_init(void)
{
	return platform_driver_register(&clock_a53_driver);
}
arch_initcall(clock_a53_init);
