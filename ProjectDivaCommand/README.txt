如果需要计算文件md5，在需要计算md5的地方include md5file.h，然后调用getFilemd5，传入路径即可
如果需要解析json,在当前位置include json\json.h，即可按Test中代码规格解析
如果需要操作sqlite3数据库，当前位置Include sqlite3.h


sqlite3步骤，先去官网下dll, def, h回来，用lib.exe编译def出lib出来，再把.dll,.h,.lib扔到工程目录，在属性，连接器，输入，附加依赖项那里添加sqlite3.lib就可以了。


jsoncpp步骤，去官网下jsoncpp回来，makefiles下打开sln，lib_json工程属性，C/C++，代码生成，多线程调试，然后配置文件设成Debug,X86平台，直接按生成，生成完了之后去build vs71那里找整个lib_json,把里面的lib复制到工程目录，把src include json整个也放到工程目录，再同sqlite3那样去附加依赖项那里添加一下json_vc71_libmtd.lib，保存之后就可以正常使用了。