function PrintLog (value) {
    const PrintLogPtr = Module.findExportByName('Lingc.dll', 'PrintLog')
    if(PrintLogPtr) {
        const callFunction = new NativeFunction(PrintLogPtr, 'int', ['pointer', 'pointer'])
        callFunction(Memory.allocAnsiString("http://127.0.0.1:8888/"), Memory.allocAnsiString(JSON.stringify(value)))
    }
}
const wrapper = Module.findBaseAddress("wrapper.node")

function bytesToHex(arrayBuffer) {
    var bytes = new Uint8Array(arrayBuffer)
    for (var hex = [], i = 0; i < bytes.length; i++) {
        hex.push((bytes[i] >>> 4).toString(16));
        hex.push((bytes[i] & 0xF).toString(16));
    }
    return hex.join("");
}

// ecc公钥导出
Interceptor.attach(wrapper.add(0x1FACE50), {
    onEnter: function (args) {
        this.outPublic = args[1]
        this.outPublicLength = args[2]
    },
    onLeave: function(retval) {
        var pubKey = null
        if (this.outPublicLength.toInt32() > retval.toInt32()) {
            var pubKey = this.outPublic.readByteArray(retval.toInt32())
        } else {
            var pubKey = this.outPublic.readByteArray(this.outPublicLength.toInt32())
        }
        PrintLog({"publicKey": bytesToHex(pubKey)})
    }
})

// ecdh密钥协商
Interceptor.attach(wrapper.add(0x1FBD820), {
    onEnter: function (args) {
        this.outShare = this.context.ecx
    },
    onLeave: function(retval) {
        var shaKey = this.outShare.readByteArray(retval.toInt32())
        PrintLog({"shareKey": bytesToHex(shaKey)})
    }
})

// B1F6D0
// AES加密算法
Interceptor.attach(wrapper.add(0xC5F4E0), {
    onEnter: function (args) {
        //console.log("AES_encrypt START======================")
        this.out1 = args[1]
        this.out2 = args[2]
        var dataSize = this.context.ecx.add(0x4).readPointer().sub(this.context.ecx.readPointer())
        this.iv = args[0].readPointer().readByteArray(0xc)
        this.key = this.context.edx.readPointer().readByteArray(0x20)
        this.data = this.context.ecx.readPointer().readByteArray(dataSize.toInt32())
        //console.log("AES_encrypt => size:", dataSize, "key:", bytesToHex(key), "iv:", bytesToHex(iv), "data:", bytesToHex(data))

    },
    onLeave: function(retval) {
        var resultSize = this.out2.add(0x4).readPointer().sub(this.out2.readPointer())
        // console.log("tag", bytesToHex(this.out1.readPointer().readByteArray(0x10)))
        // console.log("data", bytesToHex(this.out2.readPointer().readByteArray(resultSize.toInt32())))
        // console.log("AES_encrypt END========================")
        PrintLog({
            "AES_encrypt": bytesToHex(this.data),
            "result": bytesToHex(this.out2.readPointer().readByteArray(resultSize.toInt32())),
            "key": bytesToHex(this.key),
            "iv:": bytesToHex(this.iv),
            "tag": bytesToHex(this.out1.readPointer().readByteArray(0x10))
        })
    }
})

// B1FB30
// AES解密算法
Interceptor.attach(wrapper.add(0xC5F900), {
    onEnter: function (args) {
        //console.log("AES_decrypt START======================")
        this.out1 = args[1]
        this.out2 = args[2]
        var dataSize = this.context.ecx.add(0x4).readPointer().sub(this.context.ecx.readPointer())
        this.iv = args[0].readPointer().readByteArray(0xc)
        this.key = this.context.edx.readPointer().readByteArray(0x20)
        this.data = this.context.ecx.readPointer().readByteArray(dataSize.toInt32())
        // console.log("AES_decrypt => size:", dataSize, "key:", bytesToHex(key), "iv:", bytesToHex(iv), "data:", bytesToHex(data))
    },
    onLeave: function(retval) {
        var resultSize = this.out2.add(0x4).readPointer().sub(this.out2.readPointer())
        // console.log("tag", bytesToHex(this.out1.readPointer().readByteArray(0x10)))
        // console.log("data", bytesToHex(this.out2.readPointer().readByteArray(resultSize.toInt32())))
        // console.log("AES_decrypt END========================")
        PrintLog({
            "AES_decrypt": bytesToHex(this.data),
            "result": bytesToHex(this.out2.readPointer().readByteArray(resultSize.toInt32())),
            "key": bytesToHex(this.key),
            "iv:": bytesToHex(this.iv),
            "tag": bytesToHex(this.out1.readPointer().readByteArray(0x10))
        })
    }
})


// TEA加密（业务部分）
Interceptor.attach(wrapper.add(0x1F4FC00), {
    onEnter: function (args) {
        this.data = this.context.ecx.readByteArray(this.context.edx.toInt32())
        this.key = args[0].readByteArray(0x10)
        this.out1 = args[2]
        this.out2 = args[3]
        //PrintLog({"key":"encode","size:": this.context.edx, "key:": bytesToHex(key), "data:": bytesToHex(data)})
    },
    onLeave: function(retval) {
        if (retval == 0) {
            PrintLog({
                "Tea_encrypt": bytesToHex(this.data),
                "result": "失败",
                "key": bytesToHex(this.key)
            })
        } else {
            var resultSize = this.out2.readU32()
            PrintLog({
                "Tea_encrypt": bytesToHex(this.data),
                "result": bytesToHex(this.out1.readByteArray(resultSize)),
                "key": bytesToHex(this.key)
            })
        }
    }
})
// TEA解密（业务部分）
Interceptor.attach(wrapper.add(0x1F4FEE0), {
    onEnter: function (args) {
        this.data = this.context.ecx.readByteArray(this.context.edx.toInt32())
        this.key = args[0].readByteArray(0x10)
        this.out1 = args[2]
        this.out2 = args[3]
        //PrintLog({"key":"decode","size:": this.context.edx, "key:": bytesToHex(key), "data:": bytesToHex(data)})
    },
    onLeave: function(retval) {
        if (retval == 0) {
            PrintLog({
                "Tea_encrypt": bytesToHex(this.data),
                "result": "失败",
                "key": bytesToHex(this.key)
            })
        } else {
            var resultSize = this.out2.readU32()
            PrintLog({
                "Tea_encrypt": bytesToHex(this.data),
                "result": bytesToHex(this.out1.readByteArray(resultSize)),
                "key": bytesToHex(this.key)
            })
        }
    }
})
// TEA加密（扫码登录部分）
Interceptor.attach(wrapper.add(0xC5DC60), {
    onEnter: function (args) {
        this.data = this.context.ecx.readByteArray(this.context.edx.toInt32())
        this.key = args[0].readByteArray(0x10)
        this.out1 = args[1]
        this.out2 = args[2]
        // PrintLog({"key":"encode","size:": this.context.edx, "key:": bytesToHex(key), "data:": bytesToHex(data)})
    },
    onLeave: function(retval) {
        if (retval == 0) {
            PrintLog({
                "Tea_encrypt": bytesToHex(this.data),
                "result": "失败",
                "key": bytesToHex(this.key)
            })
        } else {
            var resultSize = this.out2.readU32()
            PrintLog({
                "Tea_encrypt": bytesToHex(this.data),
                "result": bytesToHex(this.out1.readByteArray(resultSize)),
                "key": bytesToHex(this.key)
            })
        }
    }
})
// TEA解密（扫码登录部分）
Interceptor.attach(wrapper.add(0xC5DFD0), {
    onEnter: function (args) {
        this.data = this.context.ecx.readByteArray(this.context.edx.toInt32())
        this.key = args[0].readByteArray(0x10)
        //PrintLog({"key":"decode","size:": this.context.edx, "key:": bytesToHex(key), "data:": bytesToHex(data)})
        this.out1 = args[1]
        this.out2 = args[2]
    },
    onLeave: function(retval) {
        if (retval == 0) {
            PrintLog({
                "Tea_decrypt": bytesToHex(this.data),
                "result": "失败",
                "key": bytesToHex(this.key)
            })
        } else {
            var resultSize = this.out2.readU32()
            PrintLog({
                "Tea_decrypt": bytesToHex(this.data),
                "result": bytesToHex(this.out1.readByteArray(resultSize)),
                "key": bytesToHex(this.key)
            })
        }
    }
})