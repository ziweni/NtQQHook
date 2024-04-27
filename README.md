# NtQQHook

> 支持的功能如下

* 部分 Tea
* 部分 AES
* Ecdh密钥(不含PrivateKey)

> 使用的库如下

* frida-gumjs
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



**如何使用?**

方式一、通过导入表修改工具，编译的文件`Lingc.dll`添加到`wrapper.node`的导入项目中，并且移动`Lingc.dll`到`wrapper.node`所在目录(所依赖Dll也需要，如已存在请忽略)

方式二、通过劫持文件，如`version.dll`来实现注入。同样需要移动文件以及其依赖，但需注意并非所有NTQQ的进程都加载`wrapper.node`，需要自行编写代码进行判断。具体可[参考](https://github.com/LiteLoaderQQNT/QQNTFileVerifyPatch)
