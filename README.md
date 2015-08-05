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

# 备注
  项目还需要google protocol buffer(2版本)相应的几个lib，可以自己编译生成，或者[下载](https://github.com/Eric-Dang/ProtocolBuffer/tree/master/lib)。
