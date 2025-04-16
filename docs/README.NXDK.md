# Original Xbox Build Guide

To build XBMC, we will need an open-source toolchain known as **NXDK**. The official NXDK is missing some of the libraries required by XBMC, so we will use NXDK from [my own repository](https://github.com/antonic901/nxdk-xbmc). This version is the same as the official one but includes additional libraries such as `fstrcmp`, `libpcre`, `sqlite3`, `tinyxml`, etc., which are needed by XBMC.

## Table of Contents
1. [Document Conventions](#1-document-conventions)
2. [Set Up NXDK Toolchain](#2-set-up-nxdk-toolchain)
3. [Build XBMC](#3-build-xbmc)

## 1. Document Conventions
This guide assumes you are familiar with `Linux`, the command line (`CLI`), and `Git`.  
Commands should be run in the terminal, one at a time and in the order provided.  
If you are on Windows, you can use WSL (Windows Subsystem for Linux).

## 2. Set Up NXDK Toolchain
To set up NXDK, head to the official [documentation](https://github.com/XboxDev/nxdk/wiki/Getting-Started) and follow the steps.  
When cloning NXDK, make sure to use **my fork** of NXDK instead of the official one:


Once NXDK is configured, add a system environment variable called `NXDK_DIR` that points to the root of the NXDK repository and switch to `dev` branch which contains needed libraries:
```bash
git checkout dev
```

## 3. Build XBMC
Clone the XBMC repository:
```bash
git clone https://github.com/antonic901/xbmc4xbox-nxdk
```
Navigate into the XBMC directory:
```bash
cd xbmc4xbox-nxdk
```
Run the build script to compile XBMC:
```bash
./build.sh
```

After a successful build, the BUILD folder will contain XBMC ready to be installed on your Xbox.
You can transfer the files using FTP or any other method.

### **Note:** XBMC cannot be run from a DVD (aka as ISO) in Xemu!
