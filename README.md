# NtQQHook

> 支持的功能如下

* 部分 Tea
* 部分 AES
* Ecdh密钥(不含PrivateKey)

> 使用的库如下

* [frida-gumjs](https://github.com/frida/frida/releases/tag/16.2.1)
* jsoncpp
* curl



**如何编译？**

1. 建立`frida-gumjs/libs/x64`、`frida-gumjs/libs/Win32`文件夹
2. 前往 [Frida](https://github.com/frida/frida/releases/tag/16.2.1) 下载名称为 `frida-gumjs-devkit-X.X.X-windows-X.tar.xz` 的压缩包
3. 根据所下载的架构类型将`frida-gumjs.lib`解压放入所对应的文件夹
4. 使用 [vcpkg](https://github.com/microsoft/vcpkg) 自动化编译安装其他库, 在项目目录内使用命令`vcpkg install`安装/打开VS编译时会附带一同进行编译，手动下载并编译可通过下面的命令

```bash
vcpkg install jsoncpp:x86-windows jsoncpp:x64-windows curl:x86-windows curl:x64-windows
```

*Tips: 如不需要编译32位的，可以去除对应的安装指令*
