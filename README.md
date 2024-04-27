# NtQQHook

> 支持的功能如下

* 部分 Tea
* 部分 AES
* Ecdh密钥(不含PrivateKey)

> 使用的库如下

* [frida-gumjs](https://github.com/frida/frida/releases/tag/16.2.1)
* jsoncpp
* curl

**使用的库除`Firda-Gumjs`以外可通过 [vcpkg](https://github.com/microsoft/vcpkg) 自动化引用，通过以下指令安装**

```bash
vcpkg install jsoncpp:x86-windows jsoncpp:x64-windows curl:x86-windows curl:x64-windows
```

*Tips: 如不需要编译32位的，可以去除对应的安装指令*
