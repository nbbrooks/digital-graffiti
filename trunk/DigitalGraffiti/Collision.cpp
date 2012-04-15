//////////////////////////////////////////////////////////////////////////////////////////////////////
// Kinect Collision Detection                                                                       //
// Writen by Andrew Sarratori                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <fstream>
using namespace std;

void Collision::on_mouse(int click_event, int xx, int yy, int flags, void *param)
{
	if(click_event == CV_EVENT_LBUTTONDOWN)
	{
		(*(int *)param) = yy * WIDTH + xx;
	}
}

void Collision::depthToRGBSpectrum(short *depthValues, unsigned char *rgbs, int numlen, int depthMin, int depthMax)
{
#pragma omp parallel for
	for(int ii = 0; ii < numlen; ii++)
	{
		float wavelength	= 0.0f;
		float red			= 0.0f;
		float green			= 0.0f;
		float blue			= 0.0f;
		float factor		= 0.0f;
		float gamma			= 1.0f;
		// Add in a tolerance region
		depthMin -= 5;
		depthMax -= 5;

		if(depthValues[ii] < depthMin)			wavelength = 0.0f;
		else if(depthValues[ii] <= depthMax)	wavelength = ((float)depthValues[ii] - (float)depthMin) / ((float)depthMax - (float)depthMin) * (750.0f - 350.0f) + 350.0f;
		else							wavelength = 0.0f;

		if(wavelength == 0.0f)
		{
			red		= 0.0f;
			green	= 0.0f;
			blue	= 0.0f;
		}
		else if(wavelength < 440.0f)
		{
			red		= -(wavelength - 440.0f) / (440.0f - 350.0f);
			green	= 0.0f;
			blue	= 1.0f;
		}
		else if(wavelength < 490.0f)
		{
			red		= 0.0f;
			green	= (wavelength - 440.0f) / (490.0f - 440.0f);
			blue	= 1.0f;
		}
		else if(wavelength < 510.0f)
		{
			red		= 0.0f;
			green	= 1.0f;
			blue	= -(wavelength - 510.0f) / (510.0f - 490.0f);
		}
		else if(wavelength < 580.0f)
		{ 
			red		= (wavelength - 510.0f) / (580.0f - 510.0f);
			green	= 1.0f;
			blue	= 0.0f;
		}
		else if(wavelength < 645)
		{
			red		= 1.0f;
			green	= -(wavelength - 645.0f) / (645.0f - 580.0f);
			blue	= 0.0f;
		}
		else
		{
			red		= 1.0f;
			green	= 0.0f;
			blue	= 0.0f;
		}

		if(wavelength == 0.0f)		factor = 0.0f;
		else if(wavelength < 420)	factor = 0.3f + 0.7f * (wavelength - 350.0f) / (420.0f - 350.0f);
		else if(wavelength < 680)	factor = 1.0f;
		else						factor = 0.3f + 0.7f * (750.0f - wavelength) / (750.0f - 680.0f);

		rgbs[3 * ii] = (unsigned char)floor(255.0f * pow(blue * factor, gamma));
		rgbs[3 * ii + 1] = (unsigned char)floor(255.0f * pow(green * factor, gamma));
		rgbs[3 * ii + 2] = (unsigned char)floor(255.0f * pow(red * factor, gamma));
	}
}

void Collision::depthToRGBThresh(short *depthValues, unsigned char *rgbs, int numlen, int minDepth, int maxDepth)
{
#pragma omp parallel for
	for(int ii = 0; ii < numlen; ii++)
	{
		if(depthValues[ii] >= maxDepth - 4)
		{
			rgbs[3 * ii]		= 0;
			rgbs[3 * ii + 1]	= 0;
			rgbs[3 * ii + 2]	= 255;
		}
		else if(depthValues[ii] <= minDepth + 4)
		{
			rgbs[3 * ii]		= 255;
			rgbs[3 * ii + 1]	= 0;
			rgbs[3 * ii + 2]	= 0;
		}
		else
		{
			rgbs[3 * ii]		= 0;
			rgbs[3 * ii + 1]	= 255;
			rgbs[3 * ii + 2]	= 0;
		}
	}
}

void Collision::translate_XY(int src_xx, int src_yy, int *dst_xx, int *dst_yy, bool select)
{
	int data[3] = {src_xx, src_yy, 1};

	CvMat *src_Mat;
	CvMat *dst_Mat;

	src_Mat = &cvMat(3, 1, CV_32FC1, data);
	dst_Mat = cvCreateMat(2, 1, CV_32FC1);

	if(select == true)	cvMatMul(this->c2d_map, src_Mat, dst_Mat);
	else				cvMatMul(this->d2c_map, src_Mat, dst_Mat);

	*dst_xx = dst_Mat->data.i[0];
	*dst_yy = dst_Mat->data.i[1];

	//cvReleaseMat(&src_Mat);
	//cvReleaseMat(&dst_Mat);
}

void Collision::pollDepthFrame(int select)
{
	GetNUICameraDepthFrameRAW(this->kinect, (PUSHORT)this->rawDepthData);

	if(select == SPECTRUM) depthToRGBSpectrum(this->rawDepthData, this->rgbDepthData, AREA, this->minDepth, this->maxDepth);
	else if(select == THRESHOLD) depthToRGBThresh(this->rawDepthData, this->rgbDepthData, AREA, this->minDepth, this->maxDepth);

	cvSetData(this->depthFrame, this->rgbDepthData, this->depthFrame->widthStep);
	//cvRectangle(this->depthFrame, this->depth_tl, this->depth_br, cvScalar(255, 255, 255));
	//cvRectangle(this->depthFrame, this->color_tl, this->color_br, cvScalar(255, 0, 0));
}

void Collision::pollColorFrame()
{
	GetNUICameraColorFrameRGB24(this->kinect, (PBYTE)this->rawColorData);
	cvSetData(this->colorFrame, this->rawColorData, this->colorFrame->widthStep);
}

bool Collision::initialize()
{
	bool SKIP = true;

	this->minDepth	= 2047;
	this->maxDepth	= 0;
	this->state		= 0;
	this->point		= -1;

	cvNamedWindow("Setup");
	cvSetMouseCallback("Setup", &on_mouse, &(this->point));
	cvInitFont(&this->myFont, CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0f, 1.0f, 0, 1);

	if(!SKIP) {
		// Thresholds
		this->pollDepthFrame(SPECTRUM);

		for(int ii = 1; ii < AREA; ii++)
		{
			if(this->rawDepthData[ii] > this->maxDepth && this->rawDepthData[ii] < 2047) this->maxDepth = this->rawDepthData[ii];
		}
		if(this->minDepth > this->maxDepth - 10) this->minDepth = this->maxDepth / 2; 

		while(state < 3)
		{
			if(point != -1)
			{
				if(this->state == 0) this->maxDepth = this->rawDepthData[point];
				else if(this->state == 1)	this->minDepth = this->rawDepthData[point];
				else if(this->state == 2)	this->target = this->rawDepthData[point];

				this->state++;
				this->point = -1;
			}

			this->pollDepthFrame(SPECTRUM);

			if(state == 0) cvPutText(this->depthFrame, "Selct Background Reference Point", cvPoint(10, HEIGHT - 15), &this->myFont, cvScalar(0,0,0));
			if(state == 1) cvPutText(this->depthFrame, "Selct Foreground Reference Point", cvPoint(10, HEIGHT - 15), &this->myFont, cvScalar(0,0,0));
			if(state == 2) cvPutText(this->depthFrame, "Selct The Target Reference Point", cvPoint(10, HEIGHT - 15), &this->myFont, cvScalar(255,255,255));
			cvShowImage("Setup", this->depthFrame);
			cvWaitKey(1);
		}
		// Boundaries
		while(state < 5)
		{
			if(point != -1)
			{
				if(this->state == 3)
				{
					this->color_tl.x = this->point % this->WIDTH;
					this->color_tl.y = this->point / this->WIDTH;
				}
				else if(this->state == 4)
				{
					this->color_br.x = this->point % this->WIDTH;
					this->color_br.y = this->point / this->WIDTH;
				}
				this->state++;
				this->point = -1;
			}

			this->pollColorFrame();

			if(state == 3) cvPutText(this->colorFrame, "Selct Upper Left Exclusion Point" , cvPoint(10, HEIGHT - 15), &this->myFont, cvScalar(0,0,0));
			if(state == 4) cvPutText(this->colorFrame, "Selct Lower Right Exclusion Point", cvPoint(10, HEIGHT - 15), &this->myFont, cvScalar(0,0,0));
			cvShowImage("Setup", this->colorFrame);
			cvWaitKey(1);
		}
	} else {
		this->maxDepth = 930;
		this->minDepth = 909;
		this->target = 922;
		this->color_tl.x = 80;
		this->color_tl.y = 69;
		this->color_br.x = 562;
		this->color_br.y = 385;
	}
	cout << "Background: " << this->maxDepth << endl;
	cout << "Foreground: " << this->minDepth << endl;
	cout << "Target: " << this->target << endl;
	cout << "Bounding Box: " << this->color_tl.x << ", " << this->color_tl.y << " | " << this->color_br.x << ", " << this->color_br.y << endl;

	ofstream myfile;
	myfile.open ("config.txt");
	myfile << "Background: " << this->maxDepth << endl;
	myfile << "Foreground: " << this->minDepth << endl;
	myfile << "Target: " << this->target << endl;
	myfile << "Bounding Box: " << this->color_tl.x << ", " << this->color_tl.y << " | " << this->color_br.x << ", " << this->color_br.y << endl;
	myfile.close();

	this->translate_XY(this->color_tl.x, this->color_tl.y, &this->depth_tl.x, &this->depth_tl.y, C2D);
	this->translate_XY(this->color_br.x, this->color_br.y, &this->depth_br.x, &this->depth_br.y, C2D);

	cout << "cvSetMouseCallback" << endl;
	cvSetMouseCallback("Setup", NULL);
	cout << "cvDestroyWindow" << endl;
	cvDestroyWindow("Setup");
	return true;
}

bool Collision::detect(int range, float *xx, float *yy, int *color)
{
	bool result = false;
	int ii = 0;
	int jj = 0;
	int pt_xx = 0;
	int pt_yy = 0;
	for(ii = this->depth_tl.x; ii < this->depth_br.x; ii++)
	{
		for(jj = this->depth_tl.y; jj < this->depth_br.y; jj++)
		{
			int lower = this->minDepth + 4;
			int upper = this->maxDepth - 4;
			if(rawDepthData[jj * this->WIDTH + ii] > lower && rawDepthData[jj * this->WIDTH + ii] < upper)
			{
				pt_xx = 0;
				pt_yy = 0;

				this->translate_XY(ii, jj, &pt_xx, &pt_yy, D2C);

				cvRectangle(this->colorFrame, cvPoint(pt_xx - 10, pt_yy - 10), cvPoint(pt_xx + 10, pt_yy + 10), cvScalar(0, 0, 0), 1);
				result = true;

				// Return xx and yy with range [0, 1]
				*xx = (float)(pt_xx - this->color_tl.x) / (this->color_br.x - this->color_tl.x);
				*yy = (float)(pt_yy - this->color_tl.y) / (this->color_br.y - this->color_tl.y);

				ii = this->depth_br.x;
				break;
			}
		}
	}

	if(result == true)
	{
		for(ii = pt_xx - 10; ii < pt_xx + 10; ii++)
			for(jj = pt_yy - 10; jj < pt_yy + 10; jj++)
			{
				int pixel_b = this->rawColorData[3 * (jj * this->WIDTH + ii)];
				int pixel_g = this->rawColorData[3 * (jj * this->WIDTH + ii) + 1];
				int pixel_r = this->rawColorData[3 * (jj * this->WIDTH + ii) + 2];

				if(pixel_b > 120 && pixel_g < 80 && pixel_r < 80)
				{
					*color = 2;
					ii = pt_xx + 10;
					break;
				}
				else if(pixel_b < 80 && pixel_g > 120 && pixel_r > 120)
				{
					*color = 1;
					ii = pt_xx + 10;
					break;
				}
				else if(pixel_b < 80 && pixel_g < 80 && pixel_r > 120)
				{
					*color = 0;
					ii = pt_xx + 10;
					break;
				}
			}
	}
	return result;
}
