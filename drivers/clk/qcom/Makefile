obj-y				+= clock.o
obj-y				+= clock-dummy.o
obj-y				+= clock-generic.o
obj-y				+= clock-local2.o
obj-y				+= clock-pll.o
obj-y				+= clock-alpha-pll.o
obj-y				+= clock-rpm.o
obj-y				+= clock-voter.o
obj-y				+= clock-pm.o

obj-$(CONFIG_MSM_CLK_CONTROLLER_V2)	+= msm-clock-controller.o

obj-$(CONFIG_DEBUG_FS)		+= clock-debug.o

ifndef CONFIG_ARCH_SONY_KANUTI
# MSM8916
obj-$(CONFIG_ARCH_MSM8916)	+= clock-rpm-8916.o
obj-$(CONFIG_ARCH_MSM8916)	+= clock-gcc-8916.o
endif
obj-$(CONFIG_ARCH_MSM8909)	+= clock-rpm-8909.o
obj-$(CONFIG_ARCH_MSM8909)	+= clock-gcc-8909.o

# MSM8994
obj-$(CONFIG_ARCH_MSM8994)	+= clock-rpm-8994.o
obj-$(CONFIG_ARCH_MSM8994)	+= clock-gcc-8994.o
obj-$(CONFIG_ARCH_MSM8994)	+= clock-mmss-8994.o
obj-$(CONFIG_ARCH_MSM8994)	+= clock-cpu-8994.o

# MSM8992
obj-$(CONFIG_ARCH_MSM8992)	+= clock-rpm-8992.o
obj-$(CONFIG_ARCH_MSM8992)	+= clock-gcc-8992.o
obj-$(CONFIG_ARCH_MSM8992)	+= clock-mmss-8992.o
obj-$(CONFIG_ARCH_MSM8992)	+= clock-cpu-8994.o

# MSM8936
obj-$(CONFIG_ARCH_MSM8916)	+= clock-rpm-8936.o
obj-$(CONFIG_ARCH_MSM8916)	+= clock-gcc-8936.o

ifndef CONFIG_ARCH_SONY_KANUTI
# MSM8952
obj-$(CONFIG_ARCH_MSM8916)	+=clock-gcc-8952.o

# MSM8976
obj-$(CONFIG_ARCH_MSM8916)	+=clock-gcc-8976.o
obj-$(CONFIG_ARCH_MSM8916)	+=clock-rcgwr.o
obj-$(CONFIG_ARCH_MSM8916)	+=clock-cpu-8976.o
endif

# FSM9010
obj-$(CONFIG_ARCH_FSM9010)	+= clock-gcc-fsm9010.o

# ACPU clock
obj-$(CONFIG_ARCH_MSM8610)	+= clock-a7.o
obj-$(CONFIG_ARCH_MSM8226)	+= clock-a7.o
obj-$(CONFIG_ARCH_MDM9630)	+= clock-a7.o
ifndef CONFIG_ARCH_SONY_KANUTI
obj-$(CONFIG_ARCH_MSM8916)	+= clock-a7.o
endif
obj-$(CONFIG_ARCH_MDM9640)	+= clock-a7.o
obj-$(CONFIG_ARCH_MSM8916)	+= clock-cpu-8939.o
obj-$(CONFIG_ARCH_MSM8916)	+= clock-cpu-8936.o
obj-$(CONFIG_ARCH_MSM8909)	+= clock-a7.o
obj-$(CONFIG_ARCH_FSM9010)	+= clock-a7.o

obj-$(CONFIG_ARCH_MSM_KRAIT)	+= clock-krait.o
obj-$(CONFIG_ARCH_FSM9900)	+= clock-krait-8974.o
obj-$(CONFIG_ARCH_APQ8084)	+= clock-krait-8974.o
obj-$(CONFIG_ARCH_MSM8974)	+= clock-krait-8974.o

obj-y				+= gdsc.o
obj-y				+= mdss/
