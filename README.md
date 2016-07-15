* Camera Controler

基于[opencv][]的Win32程序，运行环境为VS2012，程序会调用摄像头并实时显示到窗口，可以通过配置文件设置是否开启保存视频的功能和设置视频文件回滚周期，极少数摄像头可能会因为开启后没有立刻得到数据而程序保存，可以自行设置延时解决问题。
配置文件请放在C:\\CameraConfiguration下面
---
[opencv][]采用版本为248,x86环境。

[opencv]: http://opencv.org/ "opencv"