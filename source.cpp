#include "cv.h"
#include "highgui.h"
#include <windows.h> 
#include <string>
#include <vector>
#include <io.h>
#include <iostream>

using namespace std;

void GetAllFiles(char *videoStorePath, vector<string>& files);   //得到视频文件路径

void rollBack(char * videoStorePath, int videoRollbackInterval);   //回滚函数

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)  
{  
	const int VIDEO_STATUS_SAVE = 1;      //视频保存开启状态   

	int videoRollbackInterval;     //回滚时间间隔
	int videoStoreInterval;		   //视频存储分割时间
	char * videoStorePath = (char *)malloc(128);          //保存路径
	int captureWidth;              //窗口坐标Y
	int captureHeight;             //窗口坐标X
	int videoWidth;                //视频分辨率Y
	int videoHeight;               //视频分辨率X
	int status;                    //视频保存状态
	status = GetPrivateProfileInt("VideoStatus" ,"Status",0,"C:\\CameraConfiguration\\conf.ini");
	videoRollbackInterval = GetPrivateProfileInt("Time" ,"VideoRollbackInterval",0,"C:\\CameraConfiguration\\conf.ini");
	videoStoreInterval = GetPrivateProfileInt("Time" ,"VideoStoreInterval",0, "C:\\CameraConfiguration\\conf.ini");
	GetPrivateProfileString("Path" ,"VideoStorePath", "",videoStorePath, 128, "C:\\CameraConfiguration\\conf.ini");
	captureWidth = GetPrivateProfileInt("Size" ,"CaptureWidth",0,"C:\\CameraConfiguration\\conf.ini");
	captureHeight = GetPrivateProfileInt("Size" ,"CaptureHeight",0,"C:\\CameraConfiguration\\conf.ini");
	videoWidth = GetPrivateProfileInt("Size" ,"VideoWidth",0,"C:\\CameraConfiguration\\conf.ini");
	videoHeight = GetPrivateProfileInt("Size" ,"VideoHeight",0,"C:\\CameraConfiguration\\conf.ini");

	rollBack(videoStorePath, videoRollbackInterval);

    CvCapture* capture = cvCreateCameraCapture(0);
	//Sleep(30000);

    IplImage* frame = cvQueryFrame(capture);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, videoWidth);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, videoHeight);
	
	
	time_t startTime = time(0);
	time_t t = time(0); 
	char timetmp[32]; 
    strftime( timetmp, sizeof(timetmp), "%Y_%m_%d_%H_%M_%S",localtime(&t) ); 
	char tmp[20];
    //char* rootPath = "D:\\";
	char * endPath = ".mpg";
	char * path = (char *) malloc(128);
	for(int i =0; i < 19; i ++)
	{
		tmp[i] = timetmp[i];
	}
	tmp[19] = '\0';
	sprintf (path, "%s%s.mpg", videoStorePath, tmp);
	
	cvNamedWindow("camera");
	cvMoveWindow("camera", captureHeight, captureWidth);
	if(status == VIDEO_STATUS_SAVE)
	{
		CvVideoWriter* writer = cvCreateVideoWriter(
        path,
        CV_FOURCC('P','I','M','1'),
		24,
        cvGetSize(frame)	
		 );
		while(1) 
		{
			time_t nowTime = time(0);
			if(difftime(nowTime,startTime) >= double(videoStoreInterval))
			{
				cvReleaseVideoWriter(&writer);
				time_t t = time(0); 
				startTime = t;
				char timetmp[40]; 
				 strftime( timetmp, sizeof(timetmp), "%Y_%m_%d_%H_%M_%S",localtime(&t) ); 
				char tmp[20];
				char * endPath = ".mpg";
				char * path = (char *) malloc(128);
				for(int i =0; i < 19; i ++)
				{
					tmp[i] = timetmp[i];
				}
				tmp[19] = '\0';
				sprintf (path, "%s%s.mpg"
					, videoStorePath
                  , tmp);
	
				writer = cvCreateVideoWriter(
				  path,
				  CV_FOURCC('P','I','M','1'),
				  24,
				   cvGetSize(frame)	
				  );
				continue;
			}
			frame = cvQueryFrame(capture);
			if(!frame) break;
		
			cvWriteFrame(writer, frame);
			cvShowImage("camera", frame);
			char c = cvWaitKey(32);
			if(c==27) 
			{
				cvReleaseVideoWriter(&writer);
				break;
			}
		} 
		cvReleaseCapture(&capture);
	}
	else
	{
		while(1)
		{
			frame = cvQueryFrame(capture);
			if(!frame) break;
			cvShowImage("camera", frame);
			char c = cvWaitKey(32);
			if(c==27) 
			{
				break;
			}
		}
		cvReleaseCapture(&capture);
	}
    cvDestroyWindow("camera");
    return 0;
}    

void getAllFiles(char * videoStorePath,  vector<string>& files)
{
	string path = videoStorePath;
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("*.ini").c_str(),&fileinfo)) !=  -1)  
	{  
		do  
		{	
		files.push_back(p.assign(path).append("\\").append(fileinfo.name));  //保存视频文件路径
		}while(_findnext(hFile, &fileinfo)  == 0);  
		_findclose(hFile); 
	}
}

void rollBack(char * videoStorePath, int videoRollbackInterval)
{
	time_t t = time(0);
	int days = localtime(&t)->tm_yday;
	int year = localtime(&t)->tm_year;
	int buff[12]= {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
	if(year%4 == 0)
	{
		for(int i = 1; i < 12; i ++)
		{
			buff[i] ++;
		}
	}

	vector<string> files;  
	getAllFiles(videoStorePath, files);
	int size = files.size(); 
	for(int i = 0; i < size; i ++)
	{
		WIN32_FIND_DATA f;
		SYSTEMTIME st;
		FindClose(FindFirstFile(files[i].c_str(), &f));
		FileTimeToSystemTime(&(f.ftCreationTime),&st); 
		int interval =days - (buff[st.wMonth - 1] + st.wDay - 1);
		if(interval >= videoRollbackInterval)
		{
			remove(files[i].c_str());
		}
	}
}