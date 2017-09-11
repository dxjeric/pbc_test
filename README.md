# pbc_test说明
1. [pbc](https://github.com/cloudwu/pbc)：使用云风共享的pbc代码
2. [lua](http://www.lua.org)：使用的版本为5.2.0
3. [serverframe](https://github.com/Eric-Dang/CompletionPort)：以之前IOCP测试代码为基础修改完成

# 经验
1. 注意Lua版本间的区别
2. 注册pbc的Lua接口时，添加对应的lib name，使用是注意.proto文件的package name, 因为在encode和decode需要使用名字取结构。没有尝试string的encode方法。
3. pbc和google protocol buffer的数据交互，使用了gpb生成的C++代码

# 测试文件说明
  [addressbook.pb](https://github.com/cloudwu/pbc/tree/master/test): 云风pbc中的测试例子
  [test.pb](https://github.com/Eric-Dang/ProtocolBuffer/tree/master/proto): 自己根据做gpb时定义的消息

# 项目使用
  1. pbc.lib：		使用云风pbc生成的lib文件
  2. pbc-lua.c：	项目需要文件，生成lua调用的函数
  3. protobuf.lua： 云风已经封装好的lua接口，将代码中require "probotbuf.c" 修改为你注册的pbc的lib name
  4. protoc.exe：	使用[google protocol buffer](https://developers.google.com/protocol-buffers/)生成的运行程序，用于生成.pb文件
  5. 和gpb组合使用：简单测试，建议使用非string的encode和decode接口，非string的区别在C接口中如何获取消息

# 项目需求完善
  项目需求需要转发pbc消息解析后，添加部分字段然后进行转发，因此需要将pbc消息流完全解析为table，原生的pbc没有支持，因此对pbc进行少量修改支持C++全解析处理
  PS：没有看明白 在pbc—lua.c文件中函数new_array中使用lua_rawgeti，为什么不直接使用lua_getfield， 减少一个table.id的设计



# 备注
  项目还需要google protocol buffer(2版本)相应的几个lib，可以自己编译生成，或者[下载](https://github.com/Eric-Dang/ProtocolBuffer/tree/master/lib)。
