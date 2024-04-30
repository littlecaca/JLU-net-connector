### 项目功能
以windows服务的方式自动运行校园网认证程序，每次开机之后自动连接，没有弹窗打扰。此外，临时的断网或者系统休眠后也会自动重连。

### 如何使用
#### 1. 设置用户名和密码
打开项目主目录下的dogcom.conf文件，将前两行信息填写完整：
```txt
username = '' 	// 用户名
password = ''	// 密码
```
记得保存关闭。
#### 2. 安装jlu_network服务
在项目主目录下，打开控制台（powershell或者cmd），执行以下命令安装windows服务：
```shell
.\winsw.exe install .\config.xml
```
如果安装成功，打印如下信息：
```shell
Installing service 'JLU Net (jlu_network)'...
Service 'JLU Net (jlu_network)' was installed successfully.
```
#### 2.开启服务
执行以下指令
```shell
.\winsw.exe start .\config.xml
```
或者管理员身份打开命令行，执行
```shell
net start jlu_network
```

#### 3.（可选）关闭服务
如果暂时不再使用校园网，可以关闭服务，有两种方式：
- 使用管理员身份打开power shell或者cmd，执行```net stop jlu_network```
- ```.\winsw.exe stop config.xml```
#### 4. (可选）卸载服务
执行以下命令卸载服务
```shell
.\winsw.exe uninstall config.xml
```
或者管理员身份下使用window自带的服务管理工具:
```shell
sc delete jlu_network
```

