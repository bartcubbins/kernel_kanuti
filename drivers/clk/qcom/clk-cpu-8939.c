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

DEFINE_VDD_REGS_INIT(vdd_cpu_bc, 1);
DEFINE_VDD_REGS_INIT(vdd_cpu_lc, 1);
DEFINE_VDD_REGS_INIT(vdd_cpu_cci, 1);

enum {
	A53SS_MUX_BC,
	A53SS_MUX_LC,
	A53SS_MUX_CCI,
	A53SS_MUX_NUM,
};

static const char const *mux_names[] = {"c0", "c1", "cci"};

static DEFINE_VDD_REGS_INIT(vdd_cpu_a53_lc, 1);
static DEFINE_VDD_REGS_INIT(vdd_cpu_a53_bc, 1);
static DEFINE_VDD_REGS_INIT(vdd_cpu_a53_cci, 1);

#define CPU_LATENCY_NO_L2_PC_US (250)

#define to_clk_regmap_mux_div(_hw) \
	container_of(to_clk_regmap(_hw), struct clk_regmap_mux_div, clkr)

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

struct cpu_desc_8939 {
	cpumask_t cpumask;
	struct pm_qos_request req;
};

static struct cpu_desc_8939 a53_bc_clk_desc;
static struct cpu_desc_8939 a53_lc_clk_desc;
static void do_nothing(void *unused) { }

static int cpu_clk_8939_set_rate(struct clk_hw *hw, unsigned long rate,
					unsigned long parent_rate)
{
	struct clk_regmap_mux_div *cpuclk = to_clk_regmap_mux_div(hw);

	return __mux_div_set_src_div(cpuclk, cpuclk->src, cpuclk->div);
}

static int cpu_clk_8939_set_rate_and_parent(struct clk_hw *hw, unsigned long rate,
						unsigned long prate, u8 index)
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

static int cpu_clk_8939_set_rate_big(struct clk_hw *hw, unsigned long rate,
					unsigned long parent_rate)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53_bc_clk_desc);

	rc = cpu_clk_8939_set_rate(hw, rate, parent_rate);

	pm_qos_remove_request(&a53_bc_clk_desc.req);

	return rc;
}

static int cpu_clk_8939_set_rate_and_parent_big(struct clk_hw *hw, unsigned long rate,
							unsigned long prate, u8 index)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53_bc_clk_desc);

	rc = cpu_clk_8939_set_rate_and_parent(hw, rate, prate, index);

	pm_qos_remove_request(&a53_bc_clk_desc.req);

	return rc;
}

static int cpu_clk_8939_set_rate_little(struct clk_hw *hw, unsigned long rate,
						unsigned long parent_rate)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53_lc_clk_desc);

	rc = cpu_clk_8939_set_rate(hw, rate, parent_rate);

	pm_qos_remove_request(&a53_lc_clk_desc.req);

	return rc;
}

static int cpu_clk_8939_set_rate_and_parent_little(struct clk_hw *hw, unsigned long rate,
							unsigned long prate, u8 index)
{
	int rc;

	cpu_clk_8939_pm_qos_add_req(&a53_lc_clk_desc);

	rc = cpu_clk_8939_set_rate_and_parent(hw, rate, prate, index);

	pm_qos_remove_request(&a53_lc_clk_desc.req);

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




static const struct clk_ops clk_ops_cpu_big = {
	.enable = cpu_clk_8939_enable,
	.disable = cpu_clk_8939_disable,
	.get_parent = cpu_clk_8939_get_parent,
	.set_rate = cpu_clk_8939_set_rate_big,
	.set_rate_and_parent = cpu_clk_8939_set_rate_and_parent_big,
	.set_parent = cpu_clk_8939_set_parent,
	.recalc_rate = cpu_clk_8939_recalc_rate,
	//.determine_rate = cpu_clk_8939_determine_rate,
	.debug_init = clk_debug_measure_add,
};

static const struct clk_ops clk_ops_cpu_little = {
	.enable = cpu_clk_8939_enable,
	.disable = cpu_clk_8939_disable,
	.get_parent = cpu_clk_8939_get_parent,
	.set_rate = cpu_clk_8939_set_rate_little,
	.set_rate_and_parent = cpu_clk_8939_set_rate_and_parent_little,
	.set_parent = cpu_clk_8939_set_parent,
	.recalc_rate = cpu_clk_8939_recalc_rate,
	//.determine_rate = cpu_clk_8939_determine_rate,
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
	//.determine_rate = cpu_clk_8939_determine_rate_cci,
	.debug_init = clk_debug_measure_add,
};


static struct clk_init_data cpu_8939_clk_init_data[A53SS_MUX_NUM] = {
	[A53SS_MUX_LC] = {
		.name = "a53ssmux_lc",
		//.parent_names = cpuss_parent_names_a53_lc,
		//.num_parents = ARRAY_SIZE(cpuss_parent_map_a53_lc),
		.ops = &clk_ops_cpu_little,
		.vdd_class = &vdd_cpu_a53_lc,
		.flags = CLK_SET_RATE_PARENT,
	},
	[A53SS_MUX_BC] = {
		.name = "a53ssmux_bc",
		//.parent_names = cpuss_parent_names_a53_bc,
		//.num_parents = ARRAY_SIZE(cpuss_parent_names_a53_bc),
		.ops = &clk_ops_cpu_big,
		.vdd_class = &vdd_cpu_a53_bc,
		.flags = CLK_SET_RATE_PARENT,
	},
	[A53SS_MUX_CCI] = {
		.name = "a53ssmux_cci",
		//.parent_names = cpuss_parent_names_a53_cci,
		//.num_parents = ARRAY_SIZE(cpuss_parent_names_a53_cci),
		.ops = &clk_ops_cpu_cci,
		.vdd_class = &vdd_cpu_a53_cci,
		.flags = CLK_SET_RATE_PARENT,
	},
};

static struct clk_regmap_mux_div a53ssmux_lc = {
	.reg_offset = 0x0,
	.hid_width = 5,
	.hid_shift = 0,
	.src_width = 3,
	.src_shift = 8,
	.safe_src = 4,
	.safe_div = 2,
	.safe_freq = 400000000,
	//.parent_map = cpuss_parent_map_a53_lc,
	//.clk_nb.notifier_call = cpu_clk_8939_notifier_cb,
	.clkr.hw.init = &cpu_8939_clk_init_data[A53SS_MUX_LC],
};

static struct clk_regmap_mux_div a53ssmux_bc = {
	.reg_offset = 0x0,
	.hid_width = 5,
	.hid_shift = 0,
	.src_width = 3,
	.src_shift = 8,
	.safe_src = 4,
	.safe_div = 2,
	.safe_freq = 400000000,
	//.parent_map = cpuss_parent_map_a53_bc,
	//.clk_nb.notifier_call = cpu_clk_8939_notifier_cb,
	.clkr.hw.init = &cpu_8939_clk_init_data[A53SS_MUX_BC],
};

static struct clk_regmap_mux_div a53ssmux_cci = {
	.reg_offset = 0x0,
	.hid_width = 5,
	.hid_shift = 0,
	.src_width = 3,
	.src_shift = 8,
	.safe_src = 4,
	.safe_div = 4,
	.safe_freq = 200000000,
	//.parent_map = cpuss_parent_map_a53_cci,
	//.clk_nb.notifier_call = cpu_clk_8939_cci_notifier_cb,
	.clkr.hw.init = &cpu_8939_clk_init_data[A53SS_MUX_CCI],
};


static struct clk_hw *clk_cpu_8939_hw[] = {
	[P_A53_LC_CLK]	= &a53ssmux_lc.clkr.hw,
	[P_A53_BC_CLK]	= &a53ssmux_bc.clkr.hw,
	[P_A53_CCI_CLK]	= &a53ssmux_cci.clkr.hw,
};

static int find_vdd_level(struct clk_init_data *clk_data, unsigned long rate)
{
	int level;

	for (level = 0; level < clk_data->num_rate_max; level++)
		if (rate <= clk_data->rate_max[level])
			break;

	if (level == clk_data->num_rate_max) {
		pr_err("Rate %lu for %s is greater than highest Fmax\n", rate,
				clk_data->name);
		return -EINVAL;
	}

	return level;
}

static int add_opp(struct clk_hw *hw, struct device *cpudev, struct device *vregdev,
			unsigned long max_rate)
{
	struct clk_init_data *clk_data = NULL;
	struct clk_vdd_class *voltspec = NULL;
	unsigned long rate = 0;
	long ret, uv;
	int level, j = 1;

	if (IS_ERR_OR_NULL(cpudev)) {
		pr_err("%s: Invalid parameters\n", __func__);
		return -EINVAL;
	}

	clk_data = (struct clk_init_data *)hw->init;
	voltspec = clk_data->vdd_class;

	while (1) {
		rate = clk_data->rate_max[j++];
		level = find_vdd_level(clk_data, rate);
		if (level <= 0) {
			pr_warn("clock-cpu: no corner for %lu.\n", rate);
			return -EINVAL;
		}

		uv = voltspec->vdd_uv[level];
		if (uv < 0) {
			pr_warn("clock-cpu: no uv for %lu.\n", rate);
			return -EINVAL;
		}

		ret = dev_pm_opp_add(cpudev, rate, uv);
		if (ret) {
			pr_warn("clock-cpu: failed to add OPP for %lu\n", rate);
			return rate;
		}

		if (rate >= max_rate)
			break;
	}

	return 0;
}

static void print_opp_table(int a53_c0_cpu, int a53_c1_cpu)
{
	struct dev_pm_opp *oppfmax, *oppfmin;
	unsigned int apc0_rate_max = a53ssmux_lc.clkr.hw.init->num_rate_max - 1;
	unsigned int apc1_rate_max = a53ssmux_bc.clkr.hw.init->num_rate_max - 1;
	unsigned long apc0_fmax = a53ssmux_lc.clkr.hw.init->rate_max[apc0_rate_max];
	unsigned long apc1_fmax = a53ssmux_bc.clkr.hw.init->rate_max[apc1_rate_max];
	unsigned long apc0_fmin = a53ssmux_lc.clkr.hw.init->rate_max[1];
	unsigned long apc1_fmin = a53ssmux_bc.clkr.hw.init->rate_max[1];

	rcu_read_lock();

	oppfmax = dev_pm_opp_find_freq_exact(get_cpu_device(a53_c0_cpu),
					apc0_fmax, true);
	oppfmin = dev_pm_opp_find_freq_exact(get_cpu_device(a53_c0_cpu),
					apc0_fmin, true);

	/*
	 * One time information during boot. Important to know that this
	 * looks sane since it can eventually make its way to the scheduler.
	 */
	pr_info("clock_cpu: a53_c0: OPP voltage for %lu: %ld\n",
		apc0_fmin, dev_pm_opp_get_voltage(oppfmin));
	pr_info("clock_cpu: a53_c0: OPP voltage for %lu: %ld\n",
		apc0_fmax, dev_pm_opp_get_voltage(oppfmax));

	oppfmax = dev_pm_opp_find_freq_exact(get_cpu_device(a53_c1_cpu),
						apc1_fmax, true);
	oppfmin = dev_pm_opp_find_freq_exact(get_cpu_device(a53_c1_cpu),
						apc1_fmin, true);
	pr_info("clock_cpu: a53_c1: OPP voltage for %lu: %lu\n", apc1_fmin,
		dev_pm_opp_get_voltage(oppfmin));
	pr_info("clock_cpu: a53_c1: OPP voltage for %lu: %lu\n", apc1_fmax,
		dev_pm_opp_get_voltage(oppfmax));

	rcu_read_unlock();
}

static void populate_opp_table(struct platform_device *pdev)
{
	struct platform_device *apc0_dev, *apc1_dev;
	struct device_node *apc0_node = NULL, *apc1_node = NULL;
	unsigned long apc0_fmax, apc1_fmax;
	unsigned int apc0_rate_max = 0, apc1_rate_max = 0;
	int cpu, a53_c0_cpu = 0, a53_c1_cpu = 0;

	apc0_node = of_parse_phandle(pdev->dev.of_node,	"vdd-c0-supply", 0);
	if (!apc0_node) {
		pr_err("Can't find the apc0 dt node.\n");
		return;
	}

	apc1_node = of_parse_phandle(pdev->dev.of_node, "vdd-c1-supply", 0);
	if (!apc1_node) {
		pr_err("Can't find the apc1 dt node.\n");
		return;
	}

	apc0_dev = of_find_device_by_node(apc0_node);
	if (!apc0_dev) {
		pr_err("Can't find the apc0 device node.\n");
		return;
	}

	apc1_dev = of_find_device_by_node(apc1_node);
	if (!apc1_dev) {
		pr_err("Can't find the apc1 device node.\n");
		return;
	}

	apc0_rate_max = a53ssmux_lc.clkr.hw.init->num_rate_max - 1;
	apc1_rate_max = a53ssmux_bc.clkr.hw.init->num_rate_max - 1;
	apc0_fmax = a53ssmux_lc.clkr.hw.init->rate_max[apc0_rate_max];
	apc1_fmax = a53ssmux_bc.clkr.hw.init->rate_max[apc1_rate_max];

	for_each_possible_cpu(cpu) {
		pr_debug("the CPU number is : %d\n", cpu);
		if (cpu/4 == 0) {
			a53_c1_cpu = cpu;
			WARN(add_opp(&a53ssmux_bc.clkr.hw, get_cpu_device(cpu),
				     &apc1_dev->dev, apc1_fmax),
				     "Failed to add OPP levels for A53 big cluster\n");
		} else if (cpu/4 == 1) {
			a53_c0_cpu = cpu;
			WARN(add_opp(&a53ssmux_lc.clkr.hw, get_cpu_device(cpu),
				     &apc0_dev->dev, apc0_fmax),
				     "Failed to add OPP levels for A53 little cluster\n");
		}
	}

	/* One time print during bootup */
	pr_info("clock-cpu-8939: OPP tables populated (cpu %d and %d)",
		a53_c0_cpu, a53_c1_cpu);

	print_opp_table(a53_c0_cpu, a53_c1_cpu);

}

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

static int cpu_parse_devicetree(struct platform_device *pdev, int mux_id)
{
	struct resource *res;
	int rc;
	char rcg_name[] = "apcs-xxx-rcg-base";
	char vdd_name[] = "vdd-xxx";
	struct regulator *regulator;

	snprintf(rcg_name, ARRAY_SIZE(rcg_name), "apcs-%s-rcg-base",
						mux_names[mux_id]);
	res = platform_get_resource_byname(pdev,
					IORESOURCE_MEM, rcg_name);
	if (!res) {
		dev_err(&pdev->dev, "missing %s\n", rcg_name);
		return -EINVAL;
	}
	a53ssmux[mux_id]->base = devm_ioremap(&pdev->dev, res->start,
							resource_size(res));
	if (!a53ssmux[mux_id]->base) {
		dev_err(&pdev->dev, "ioremap failed for %s\n", rcg_name);
		return -ENOMEM;
	}

	snprintf(vdd_name, ARRAY_SIZE(vdd_name), "vdd-%s", mux_names[mux_id]);
	regulator = devm_regulator_get(&pdev->dev, vdd_name);
	if (IS_ERR(regulator)) {
		if (PTR_ERR(regulator) != -EPROBE_DEFER)
			dev_err(&pdev->dev, "unable to get regulator\n");
		return PTR_ERR(regulator);
	}
	cpuclk[mux_id]->c.vdd_class->regulator[0] = regulator;

	//rc = of_get_clk_src(pdev, a53ssmux[mux_id]->parents, mux_id);
	//if (IS_ERR_VALUE(rc))
	//	return rc;

	a53ssmux[mux_id]->num_parents = rc;

	return 0;
}

static void config_pll(int mux_id)
{
	unsigned long rate, aux_rate;
	struct clk *aux_clk, *main_pll;

	aux_clk = a53ssmux[mux_id]->parents[0].src;
	main_pll = a53ssmux[mux_id]->parents[1].src;

	aux_rate = clk_get_rate(aux_clk);
	rate = clk_get_rate(&a53ssmux[mux_id]->c);
	clk_set_rate(&a53ssmux[mux_id]->c, aux_rate);
	clk_set_rate(main_pll, clk_round_rate(main_pll, 1));
	clk_set_rate(&a53ssmux[mux_id]->c, rate);

	return;
}

static int clock_8939_pm_event(struct notifier_block *this,
				unsigned long event, void *ptr)
{
	switch (event) {
	case PM_POST_HIBERNATION:
	case PM_POST_SUSPEND:
		clk_unprepare(&a53_lc_clk.c);
		clk_unprepare(&a53_bc_clk.c);
		clk_unprepare(&cci_clk.c);
		break;
	case PM_HIBERNATION_PREPARE:
	case PM_SUSPEND_PREPARE:
		clk_prepare(&a53_lc_clk.c);
		clk_prepare(&a53_bc_clk.c);
		clk_prepare(&cci_clk.c);
		break;
	default:
		break;
	}
	return NOTIFY_DONE;
}

static struct notifier_block clock_8939_pm_notifier = {
	.notifier_call = clock_8939_pm_event,
};

/**
 * clock_panic_callback() - panic notification callback function.
 *		This function is invoked when a kernel panic occurs.
 * @nfb:	Notifier block pointer
 * @event:	Value passed unmodified to notifier function
 * @data:	Pointer passed unmodified to notifier function
 *
 * Return: NOTIFY_OK
 */
static int clock_panic_callback(struct notifier_block *nfb,
					unsigned long event, void *data)
{
	unsigned long rate;

	rate  = clk_hw_is_enabled(&a53_bc_clk.clkr.hw) ? clk_hw_get_rate(&a53_bc_clk.clkr.hw) : 0;
	pr_err("%s frequency: %10lu Hz\n", clk_hw_get_name(&a53_bc_clk.clkr.hw), rate);

	rate  = clk_hw_is_enabled(&a53_lc_clk.clkr.hw) ? clk_hw_get_rate(%a53_lc_clk.clkr.hw) : 0;
	pr_err("%s frequency: %10lu Hz\n", clk_hw_get_name(%a53_lc_clk.clkr.hw), rate);

	return NOTIFY_OK;
}

static struct notifier_block clock_panic_notifier = {
	.notifier_call = clock_panic_callback,
	.priority = 1,
};

static int clock_a53_probe(struct platform_device *pdev)
{
	int speed_bin, version, rc, cpu, mux_id, rate;
	char prop_name[] = "qcom,speedX-bin-vX-XXX";
	int mux_num;

	get_speed_bin(pdev, &speed_bin, &version);

	mux_num = A53SS_MUX_NUM;

	for (mux_id = 0; mux_id < mux_num; mux_id++) {
		rc = cpu_parse_devicetree(pdev, mux_id);
		if (rc)
			return rc;

		snprintf(prop_name, ARRAY_SIZE(prop_name),
					"qcom,speed%d-bin-v%d-%s",
					speed_bin, version, mux_names[mux_id]);

		rc = of_get_fmax_vdd_class(pdev, &cpuclk[mux_id]->c,
								prop_name);
		if (rc) {
			/* Fall back to most conservative PVS table */
			dev_err(&pdev->dev, "Unable to load voltage plan %s!\n",
								prop_name);

			snprintf(prop_name, ARRAY_SIZE(prop_name),
				"qcom,speed0-bin-v0-%s", mux_names[mux_id]);
			rc = of_get_fmax_vdd_class(pdev, &cpuclk[mux_id]->c,
								prop_name);
			if (rc) {
				dev_err(&pdev->dev,
					"Unable to load safe voltage plan\n");
				return rc;
			}
			dev_info(&pdev->dev, "Safe voltage plan loaded.\n");
		}
	}

	rc = of_msm_clock_register(pdev->dev.of_node,
			cpu_clocks_8939, ARRAY_SIZE(cpu_clocks_8939));
	if (rc) {
		dev_err(&pdev->dev, "msm_clock_register failed\n");
		return rc;
	}

	rate = clk_get_rate(&cci_clk.c);
	clk_set_rate(&cci_clk.c, rate);

 	 for (mux_id = 0; mux_id < A53SS_MUX_CCI; mux_id++) {
 	 	 /* Force a PLL reconfiguration */
 	 	 config_pll(mux_id);
 	 }

	/*
	 * We don't want the CPU clocks to be turned off at late init
	 * if CPUFREQ or HOTPLUG configs are disabled. So, bump up the
	 * refcount of these clocks. Any cpufreq/hotplug manager can assume
	 * that the clocks have already been prepared and enabled by the time
	 * they take over.
	 */
	get_online_cpus();
	for_each_online_cpu(cpu) {
		WARN(clk_prepare_enable(&cpuclk[cpu/4]->c),
				"Unable to turn on CPU clock");
		clk_prepare_enable(&cci_clk.c);
	}
	put_online_cpus();

	for_each_possible_cpu(cpu) {
		if (logical_cpu_to_clk(cpu) == &a53_bc_clk.c)
			cpumask_set_cpu(cpu, &a53_bc_clk.cpumask);
		if (logical_cpu_to_clk(cpu) == &a53_lc_clk.c)
			cpumask_set_cpu(cpu, &a53_lc_clk.cpumask);
	}

	a53_lc_clk.hw_low_power_ctrl = true;
	a53_bc_clk.hw_low_power_ctrl = true;

	register_pm_notifier(&clock_8939_pm_notifier);

	populate_opp_table(pdev);

	atomic_notifier_chain_register(&panic_notifier_list,
						&clock_panic_notifier);

	return 0;
}

static struct of_device_id clock_a53_match_table[] = {
	{.compatible = "qcom,cpu-clock-8939"},
	{}
};

static struct platform_driver clock_a53_driver = {
	.probe = clock_a53_probe,
	.driver = {
		.name = "cpu-clock-8939",
		.of_match_table = clock_a53_match_table,
		.owner = THIS_MODULE,
	},
};

static int __init clock_a53_init(void)
{
	return platform_driver_register(&clock_a53_driver);
}
arch_initcall(clock_a53_init);

#define APCS_C0_PLL			0xb116000
#define C0_PLL_MODE			0x0
#define C0_PLL_L_VAL			0x4
#define C0_PLL_M_VAL			0x8
#define C0_PLL_N_VAL			0xC
#define C0_PLL_USER_CTL			0x10
#define C0_PLL_CONFIG_CTL		0x14

#define APCS_ALIAS0_CMD_RCGR		0xb111050
#define APCS_ALIAS0_CFG_OFF		0x4
#define APCS_ALIAS0_CORE_CBCR_OFF	0x8
#define SRC_SEL				0x4
#define SRC_DIV				0x3

static void __init configure_enable_sr2_pll(void __iomem *base)
{
	/* Disable Mode */
	writel_relaxed(0x0, base + C0_PLL_MODE);

	/* Configure L/M/N values */
	writel_relaxed(0x34, base + C0_PLL_L_VAL);
	writel_relaxed(0x0,  base + C0_PLL_M_VAL);
	writel_relaxed(0x1,  base + C0_PLL_N_VAL);

	/* Configure USER_CTL and CONFIG_CTL value */
	writel_relaxed(0x0100000f, base + C0_PLL_USER_CTL);
	writel_relaxed(0x4c015765, base + C0_PLL_CONFIG_CTL);

	/* Enable PLL now */
	writel_relaxed(0x2, base + C0_PLL_MODE);
	udelay(2);
	writel_relaxed(0x6, base + C0_PLL_MODE);
	udelay(50);
	writel_relaxed(0x7, base + C0_PLL_MODE);
	mb();
}

static int __init cpu_clock_a53_init_little(void)
{
	void __iomem  *base;
	int regval = 0, count;
	struct device_node *ofnode = of_find_compatible_node(NULL, NULL,
							"qcom,cpu-clock-8939");
	if (!ofnode)
		return 0;

	base = ioremap_nocache(APCS_C0_PLL, SZ_32);
	configure_enable_sr2_pll(base);
	iounmap(base);

	base = ioremap_nocache(APCS_ALIAS0_CMD_RCGR, SZ_8);
	regval = readl_relaxed(base);
	/* Source GPLL0 and 1/2 the rate of GPLL0 */
	regval = (SRC_SEL << 8) | SRC_DIV; /* 0x403 */
	writel_relaxed(regval, base + APCS_ALIAS0_CFG_OFF);
	mb();

	/* update bit */
	regval = readl_relaxed(base);
	regval |= BIT(0);
	writel_relaxed(regval, base);

	/* Wait for update to take effect */
	for (count = 500; count > 0; count--) {
		if (!(readl_relaxed(base)) & BIT(0))
			break;
		udelay(1);
	}

	/* Enable the branch */
	regval =  readl_relaxed(base + APCS_ALIAS0_CORE_CBCR_OFF);
	regval |= BIT(0);
	writel_relaxed(regval, base + APCS_ALIAS0_CORE_CBCR_OFF);
	mb();
	iounmap(base);

	pr_info("A53 Power clocks configured\n");

	return 0;
}
early_initcall(cpu_clock_a53_init_little);
