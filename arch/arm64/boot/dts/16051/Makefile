#dtb-$(CONFIG_OPPO_COMMON_SOFT) += sdm660-pm660a-mtp-17011.dtb
obj-$(CONFIG_OPPO_16051) += sdm660-pm660a-mtp-16051.dtb
obj-$(CONFIG_OPPO_16103) += sdm660-pm660a-mtp-16103.dtb
obj-$(CONFIG_OPPO_16118) += sdm660-pm660a-mtp-16118.dtb

ifeq ($(CONFIG_ARM64),y)
always          := $(dtb-y)
always          += $(dtbo-y)
subdir-y        := $(dts-dirs)
else
targets += dtbs
targets += $(addprefix ../, $(dtb-y))

$(obj)/../%.dtb: $(src)/%.dts FORCE
	$(call if_changed_dep,dtc)

dtbs: $(addprefix $(obj)/../,$(dtb-y))
endif
clean-files := *.dtbo *.dtb
