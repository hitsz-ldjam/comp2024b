.\shaderc.exe -f vs_blinn_phong.sc -o vs_blinn_phong.bin -p vs_5_0 --type vertex --platform windows -O 3 -i "path_to_github/bgfx/src/"
.\shaderc.exe -f fs_blinn_phong.sc -o fs_blinn_phong.bin -p ps_5_0 --type fragment --platform windows -O 3 -i "path_to_github/bgfx/src/"
