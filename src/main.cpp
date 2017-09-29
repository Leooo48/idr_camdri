#include "FlyCapture2.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <time.h>
#include <iostream>

#include <ros/ros.h>

using namespace FlyCapture2;
using namespace cv;

#define NODE "idr_camdri_node"

int main(int argc, char** argv)
{

	ros::init(argc, argv, NODE);
	ros::NodeHandle node;
	
	// Set up publishers  
	image_transport::ImageTransport it(node);
	image_transport::Publisher im_publisher;
	im_publisher = it.advertise("drivingimgs",1);

	Error error;
	Camera camera;
	CameraInfo camInfo;

	//Connect to the camera
	error = camera.Connect(0);

	if( error != PGRERROR_OK)
	{
		std::cout << "Failed to connect to the camera!" << std::endl;
		system("pause");
		return false;
	}
		
	//Get the camera info and print it out
	error = camera.GetCameraInfo( &camInfo);
	if( error != PGRERROR_OK)
	{ 
		std::cout << "Failed to get camera INFO!" << std::endl;
		//system("pause");
		return false;
	}

	std::cout<< camInfo.vendorName << " " 
		<< camInfo.modelName << " "
		<< camInfo.serialNumber << std::endl;
	 
	error = camera.StartCapture();
	if( error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED)
	{
		std::cout << "Bandwidth exceeded!" << std::endl;
		
		return false;
	}
	else if( error != PGRERROR_OK)
	{
		std::cout << "Failed to start image capture!" << std::endl;
		
		return false;
	}

	//ros::spin();
	while(ros::ok())
	{
		//Get image
		Image rawImage;
		Error error = camera.RetrieveBuffer( &rawImage);
		
		if( error != PGRERROR_OK)
		{
			std::cout << "Capture failed!" << std::endl;
		
			continue;
		}
		Image rgbImage;
		rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);

		//Convert to OpenCV Mat
		unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
		cv::Mat image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);
		resize(image,image,Size(1024,1024));

		//publish driving images topic 
		std_msgs::Header header;
		header.frame_id = "drivingimgs";
		header.stamp = ros::Time::now();
		cv_bridge::CvImage cv_image = cv_bridge::CvImage(header,"bgr8",image);
		im_publisher.publish(cv_image.toImageMsg());

		ros::spinOnce();
		looprate.sleep();
	}

	ros::shutdown();
	camera.Disconnect();
	system("pause");
	return 0;
}

// int main()
// {
// 	//capture loop
// 	std::string filename;
// 	time_t tt=time(NULL);
// 	tm* t=localtime(&tt);
// 	std::stringstream ss;
// 	ss << (t->tm_year+1900);
// 	ss<< "-";
// 	ss<< (t->tm_mon+1);
// 	ss<<"-"; 
// 	ss<<t->tm_mday;
// 	ss<<"-";
// 	ss<<t->tm_hour;
// 	ss<<"-";
// 	ss<<t->tm_min;
// 	ss<<"-";
// 	ss<<t->tm_sec;
// 	ss<<".avi";
// 	ss>>filename;

// 	cv::VideoWriter vw(/*"video1.avi"*/filename, CV_FOURCC('M','J','P','G'), 30.0, cv::Size(1024, 1024));
	
// 	char key = 0;
// 	int frame=0;
// 	while( (key != 'q') && (frame<(30*60*10*2)) )
// 	{
// 		frame++;
// 		if(frame%(30*60*10)==0)
// 		{
// 			vw.release();

// 			std::string filename;
// 			time_t tt=time(NULL);
// 			tm* t=localtime(&tt);
// 			std::stringstream ss;
// 			ss << (t->tm_year+1900);
// 			ss<< "-";
// 			ss<< (t->tm_mon+1);
// 			ss<<"-";
// 			ss<<t->tm_mday;
// 			ss<<"-";
// 			ss<<t->tm_hour;
// 			ss<<"-";
// 			ss<<t->tm_min;
// 			ss<<"-";
// 			ss<<t->tm_sec;
// 			ss<<".avi";
// 			ss>>filename;

// 			vw.open(/*"video1.avi"*/filename, CV_FOURCC('M','J','P','G'), 30.0, cv::Size(1024, 1024));
// 		}

// 		//Get image
// 		Image rawImage;
// 		Error error = camera.RetrieveBuffer( &rawImage);
		
// 		if( error != PGRERROR_OK)
// 		{
// 			std::cout << "Capture failed!" << std::endl;
		
// 			continue;
// 		}
// 		Image rgbImage;
// 		rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);

// 		//Convert to OpenCV Mat
// 		unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
// 		cv::Mat image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);
// 		resize(image,image,Size(1024,1024));

// 		/*save image*/
// 		cv::namedWindow("image");
// 		//cv::imshow("image",image);
// 		//imwrite("im.bmp",image);
// 		//waitKey(1);
// 		key=waitKey(1);

// 		/*save video*/		
// 		vw << image;
// 		std::cout<<"saved frame: "<<frame<<std::endl;

		
// 	}

// 	vw.release();
	
// 	return 0;
// }