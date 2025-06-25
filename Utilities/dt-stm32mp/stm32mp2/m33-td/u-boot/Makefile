# SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause)

dtb-$(CONFIG_STM32MP13X) += \


dtb-$(CONFIG_STM32MP15X) += \


dtb-$(CONFIG_STM32MP21X) += \
	stm32mp215f-dk-cm33tdcid-ostl-sdcard.dtb \
	stm32mp215f-dk-cm33tdcid-ostl-serial-ca35tdcid.dtb


dtb-$(CONFIG_STM32MP25X) += \
	stm32mp257f-ev1-cm33tdcid-ostl-emmc.dtb \
	stm32mp257f-ev1-cm33tdcid-ostl-snor.dtb \
	stm32mp257f-ev1-cm33tdcid-ostl-sdcard.dtb \
	stm32mp257f-ev1-cm33tdcid-ostl-serial-ca35tdcid.dtb

#include $(srctree)/scripts/Makefile.dts

targets += $(dtb-y)

# Add any required device tree compiler flags here
DTC_FLAGS += -a 0x8

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb *.dtbo *_HS
