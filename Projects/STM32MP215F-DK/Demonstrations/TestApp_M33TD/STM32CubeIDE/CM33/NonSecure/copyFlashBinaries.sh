mkdir -p "$2"/bin

cp  "$1".bin "$2"/bin/"$1".bin;
cp  "$1"_tfm_s_ns_signed.bin "$2"/bin/tfm_s_ns_signed.bin;
cp  "$3"/Middlewares/Third_Party/trusted-firmware-m/config_default/api_ns/bin/bl2.stm32   "$2"/bin/bl2.stm32 ;
cp  "$3"/Middlewares/Third_Party/trusted-firmware-m/config_default/api_ns/bin/ddr_phy_signed.bin "$2"/bin/ddr_phy_signed.bin;

rm "$1"_tfm_s_ns_signed.bin "$1"_tfm_s_ns.bin