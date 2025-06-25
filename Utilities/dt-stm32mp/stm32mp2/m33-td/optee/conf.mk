# SPDX-License-Identifier: BSD-2-Clause

# MP21 boards
flavor_dts_file-215F_DK_CM33TDCID_OSTL_SDCARD = stm32mp215f-dk-cm33tdcid-ostl-sdcard.dts
flavorlist-MP21 += $(flavor_dts_file-215F_DK_CM33TDCID_OSTL_SDCARD)
flavorlist-M33-TDCID += $(flavor_dts_file-215F_DK_CM33TDCID_OSTL_SDCARD)

flavor_dts_file-215F_DK_CM33TDCID_OSTL_SERIAL_CA35TDCID = stm32mp215f-dk-cm33tdcid-ostl-serial-ca35tdcid.dts
flavorlist-MP21 += $(flavor_dts_file-215F_DK_CM33TDCID_OSTL_SERIAL_CA35TDCID)

# MP25 boards
flavor_dts_file-257F_EV1_CM33TDCID_OSTL_EMMC = stm32mp257f-ev1-cm33tdcid-ostl-emmc.dts
flavor_dts_file-257F_EV1_CM33TDCID_OSTL_SNOR = stm32mp257f-ev1-cm33tdcid-ostl-snor.dts
flavor_dts_file-257F_EV1_CM33TDCID_OSTL_SDCARD = stm32mp257f-ev1-cm33tdcid-ostl-sdcard.dts
flavorlist-MP25 += $(flavor_dts_file-257F_EV1_CM33TDCID_OSTL_EMMC) \
		$(flavor_dts_file-257F_EV1_CM33TDCID_OSTL_SNOR) \
		$(flavor_dts_file-257F_EV1_CM33TDCID_OSTL_SDCARD)
flavorlist-M33-TDCID += $(flavor_dts_file-257F_EV1_CM33TDCID_OSTL_EMMC) \
		$(flavor_dts_file-257F_EV1_CM33TDCID_OSTL_SNOR) \
		$(flavor_dts_file-257F_EV1_CM33TDCID_OSTL_SDCARD)

flavor_dts_file-257F_EV1_CM33TDCID_SERIAL_CA35TDCID = stm32mp257f-ev1-cm33tdcid-ostl-serial-ca35tdcid.dts

flavorlist-MP25 += $(flavor_dts_file-257F_EV1_CM33TDCID_SERIAL_CA35TDCID)
