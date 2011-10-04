//////////////////////////////////////////////////////////////////////////////////////////////////////
// Kinect_OpenCV.cpp : A basic application for interfacing with the Kinect via OpenCV and the       //
//                     NUI Platform API                                                             //
//                                                                                                  //
// Make sure under the Project settings that the proper directories dependencies are added          //
// Make sure under the Project settings that the required library files are referenced              //
// NOTE: The exact location of the Include and Lib folders for OpenCV and CL NUI API will           //
//       vary depending on where you installed them, make sure they match your install location     //
//                                                                                                  //
// Make sure the CLNUIDevice.dll is in the same directory as the exe or the program will crash      //
//                                                                                                  //
// Under Project->Properties->Config Properties->C/C++->General add to Addtional Dependencies:      //
//   - C:\OpenCV2.0\include\opencv;C:\OpenCV2.0\lib;C:\NUI Kinect\Include;C:\NUI Kinect\Lib         //
//                                                                                                  //
// Under Project->Properties->Config Properties->Linker->General add to Addtional Dependencies:     //
//   - C:\OpenCV2.0\include\opencv;C:\OpenCV2.0\lib;C:\NUI Kinect\Include;C:\NUI Kinect\Lib         //
//                                                                                                  //
// Under Project->Properties->Config Properties->Linker->Input add to Addtional Dependencies:       //
//   - cv200.lib;cxcore200.lib;highgui200.lib;cvaux200.lib;opencv_ffmpeg200.lib;cv200d.lib;         //
//     cxcore200d.lib;highgui200d.lib;cvaux200d.lib;opencv_ffmpeg200d.lib;winmm.lib;CLNUIDevice.lib //
//                                                                                                  //
// Lastly, paste these includes into either stdafx.h or into the main cpp file:                     //
//   - #include "cv.h"                                                                              //
//   - #include "highgui.h"                                                                         //
//   - #include "cxcore.h"                                                                          //
//   - #include "cvaux.h"                                                                           //
//   - #include "CLNUIDevice.h"                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////

// See stdafx header for the rest of the includes
#include "stdafx.h"
using namespace std;

bool collision = false;
int xx = -1;
int yy = -1;
int color = -1;

int _tmain(int argc, _TCHAR* argv[])
{
	Collision *obj = new Collision();
	obj->initialize();

	while(1)
	{
		color = -1;

		obj->pollColorFrame();
		obj->pollDepthFrame(obj->SPECTRUM);
		collision = obj->detect(5, &xx, &yy, &color);

		if(collision == true)
		{
			cout << "Detection: " << xx << ", " << yy;

			if(color == 0) cout << ", red" << endl;
			else if(color == 1) cout << ", yellow" << endl;
			else if(color == 2) cout << ", blue" << endl;
			else cout << endl;

			collision = false;

			cvShowImage("Depth", obj->depthFrame);
			cvMoveWindow("Depth", 0, 0);
			cvShowImage("Color", obj->colorFrame);
			cvMoveWindow("Color", 700, 0);
		}

		cvWaitKey(1);
	}

	system("PAUSE");
	return 0;
}
