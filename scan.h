#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Windows.h>
#include <conio.h>
#include <vector>

using namespace cv;
using namespace std;

class startScan
{
private:
	vector<double> line;
	int ang;
	VideoCapture cap;
	bool pause;
	IplImage* img_g;
	
public:
	vector<vector<double>> map;
	bool finished;
	startScan()
	{
		line.resize(480,0);
		map.resize(180);
		for (int i = 0;i<180;i++)
		{
			map[i].resize(480,0);
		}
		
		ang = 0;
		namedWindow( "LK Demo", CV_WINDOW_AUTOSIZE);
		//namedWindow( "Gray",CV_WINDOW_AUTOSIZE);
		namedWindow( "Effect",CV_WINDOW_AUTOSIZE);
		pause = false;
		finished = false;
		img_g = cvCreateImage(cvSize(720,480),IPL_DEPTH_8U,1);
	}
	~startScan()
	{
		line.clear();
		map.clear();
		cap.~VideoCapture();
	}
	void openCamera()
	{
		
	    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
		cap.open(0);
		if( !cap.isOpened() )
	    {
	        cout << "摄像头打开失败！\n";
	        exit (-1);
	    }
		else
			cout<<"摄像头打开成功！\n";
	}
	int getCap()
	{
		 for(int i = -1;i < 181 && pause == false;i ++)
	    {
	        Mat frame;
	        cap >> frame;
	        if( frame.empty() )
			{
				return -1;
				exit(0);
			}
			Mat image,gray,final;

	        frame.copyTo(image);
	        cvtColor(image, gray, CV_BGR2GRAY);
			threshold(gray,final,125,255,THRESH_BINARY);	
			if(i > 0 && ang < 180)
			{
				getLine(final);
				cout<<"scaning "<<ang<<endl;
				ang++;
			}
			imshow("LK Demo", frame);
			imshow("Effect",final);
			if(waitKey(2) == 27)
			{
				break;
				return 0;
			}
		 }
		 finished = true;
		return 1;
	}
	void cameratest()
	{
		for(;;)
		{
			Mat frame;
			cap >> frame;
			if( frame.empty() )
			{
				// break;
				exit(0);
			}
			Mat image,gray,final;

			frame.copyTo(image);
			cvtColor(image, gray, CV_BGR2GRAY);

			//adaptiveThreshold(gray,final, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 7,param);
			//bilateralFilter(final, smooth, 20, 90, 40);
			//threshold(gray,final,155,255,THRESH_BINARY|THRESH_OTSU);
			threshold(gray,final,125,255,THRESH_BINARY);

			imshow("LK Demo", frame);
			//imshow("Gray",gray);
			imshow("Effect",final);

			if(waitKey(20) == 27)
			{
				break;
			}
		}
		
	}	
	void getLine (Mat& m)
	{
		for(int i = 0;i < 480;i++)
		{
			
			int start = 0, end = 0;
			uchar* data = m.ptr<uchar>(i);
			data[320] = 255;
			
			for (int j = 600;j > 330;j--)
			{
				if(data[j] == 255 && data[j+1] == 0)
				{
					end = j;
				}	
				if(data[j-1] == 0 && data[j] == 255)
				{
					start = j;
					if (2 < (end - start))
					{
						map[ang][i] = ((start + end)/2-335)*2.8; 
						data[int((start + end)/2)] = 0;
						data[int(((start + end)/2-350)*2.8)] = 255;
						break;
					}	
				}
			}
		}
	}
	void processing()
	{
		cout<<"数据处理中…\n";
		for (int i = 0;i < 180;i++)
		{
			for (int j = 1;j < 479;j++)
			{
				if(map[i][j-1] == map[i][j+1] && map[i][j] != map[i][j-1])
				{
					map[i][j] = map[i][j-1];
				}			
			}
		}
		for(int k = 0;k < 10;k++)
		{
			for (int i = 0;i < 180;i++)
			{
				for (int j = 1;j < 479;j++)
				{
					if(abs(map[i][j-1] - map[i][j+1]) < 10 && abs(map[((i-1) == -1) ? 179 : (i-1)][j] - map[((i+1) == 180) ? 0 : (i+1)][j]) < 10 )
						map[i][j] = (map[i][j-1] + map[i][j+1] + map[((i-1) == -1) ? 179 : (i-1)][j] + map[((i+1) == 180) ? 0 : (i+1)][j]) / 4;
				}
			}		
		}
	}
	void saveLine(vector <vector<double>> map,vector <double> line,int d)
	{
		map[d] = line;
	}
	void paused(bool p)
	{
		pause = p;
	}
};