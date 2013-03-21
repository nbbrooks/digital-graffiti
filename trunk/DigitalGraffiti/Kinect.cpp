#include "stdafx.h"

/**
* Interfaces with Microsoft Kinect to detect locations and colors of plastic ball collisions in specified collision region
* @author Andrew Sarratori, Nathan Brooks
*/
const char* Kinect::SAMPLE_XML_PATH = "C:/Program Files (x86)/OpenNI/Data/SamplesConfig.xml";
const float Kinect::KINECT_WAIT = 2.0f;
const std::string Kinect::CONFIG_FILE = "config.txt";

Kinect::Kinect()
{
	time_t delayStartTime;
	time_t delayEndTime;

	// Set up OpenNI stuff
	xn::EnumerationErrors errors;
	rc = g_context.InitFromXmlFile(SAMPLE_XML_PATH, &errors);
	if (rc == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		fprintf(stderr, "Open failed: %s\n", strError);
		exit(-1);
	}
	else if (rc != XN_STATUS_OK)
	{
		fprintf(stderr, "Open failed: %s\n", xnGetStatusString(rc));
		exit(-1);
	}

	rc = g_context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depth);
	rc = g_context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);

	// Initialize the containers for the depth and color frames
	depthFrame = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, K_RGB);
	colorFrame = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, K_RGB);

	// Delay to guarantee the Kinect is ready for polling
	delayStartTime	= time(0);
	delayEndTime = time(0);
	while (difftime(delayEndTime, delayStartTime) < Kinect::KINECT_WAIT) delayEndTime = time(0);

	// Grab images
	g_depth.GetMetaData(g_depthMD);
	g_image.GetMetaData(g_imageMD);

	// Apply calibration
	g_depth.GetAlternativeViewPointCap().SetViewPoint(g_image);
}

Kinect::~Kinect()
{
	// Destroy all created objects and release Kinect
	cvReleaseImage(&depthFrame);
	cvReleaseImage(&colorFrame);
}

void Kinect::on_mouse(int click_event, int xx, int yy, int flags, void *param)
{
	if(click_event == CV_EVENT_LBUTTONDOWN)
	{
		(*(int *)param) = yy * WIDTH + xx;
	}
}

void Kinect::num2rgb(short *nums, unsigned char *rgbs, int numlen, int min, int max, int select)
{
	if(select == 1)
	{
		//#pragma omp parallel for
		for(int ii = 0; ii < numlen; ii++)
		{
			float wavelength = 0.0f;
			float red			= 0.0f;
			float green			= 0.0f;
			float blue			= 0.0f;
			float factor		= 0.0f;
			float gamma			= 1.0f;

			int adj_min = min - 5;
			int adj_max = max + 5;

			if(nums[ii] < adj_min)			wavelength = 0.0f;
			else if(nums[ii] <= adj_max)	wavelength = ((float)nums[ii] - (float)adj_min) / ((float)adj_max - (float)adj_min) * (750.0f - 350.0f) + 350.0f;
			else						wavelength = 0.0f;

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

			rgbs[3 * ii]		= (unsigned char)floor(255.0f * pow(blue * factor, gamma));
			rgbs[3 * ii + 1]	= (unsigned char)floor(255.0f * pow(green * factor, gamma));
			rgbs[3 * ii + 2]	= (unsigned char)floor(255.0f * pow(red * factor, gamma));
		}
	}
	else
	{
		//#pragma omp parallel for
		for(int ii = 0; ii < numlen; ii++)
		{
			if(nums[ii] >= max - BUFFER)
			{
				rgbs[3 * ii]		= 0;
				rgbs[3 * ii + 1]	= 0;
				rgbs[3 * ii + 2]	= 255;
			}
			else if(nums[ii] <= min - BUFFER)
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
}

void Kinect::avgColor(unsigned char *rgbs, int xx_min, int yy_min, int xx_max, int yy_max, int color[3])
{
	int ii = 0;
	int jj = 0;

	for(ii = yy_min; ii < yy_max; ii++)
		for(jj = xx_min; jj < xx_max; jj++)
		{
			color[0] += rgbs[3 * (ii * WIDTH + jj)];
			color[1] += rgbs[3 * (ii * WIDTH + jj) + 1];
			color[2] += rgbs[3 * (ii * WIDTH + jj) + 2];
		}

		color[0] /= (2 * BALL_BOX * 2 * BALL_BOX);
		color[1] /= (2 * BALL_BOX * 2 * BALL_BOX);
		color[2] /= (2 * BALL_BOX * 2 * BALL_BOX);
}

int Kinect::getWidth() 
{
	return WIDTH;
}

int Kinect::getHeight() 
{
	return HEIGHT;
}

void Kinect::pollFrames(int select) 
{
	// Wait for new frame (maybe?)
	this->rc = g_context.WaitAndUpdateAll();
	if (this->rc != XN_STATUS_OK)
	{
		fprintf(stderr, "Read failed: %s\n", xnGetStatusString(this->rc));
	}

	// Grab images
	g_depth.GetMetaData(g_depthMD);
	g_image.GetMetaData(g_imageMD);

	memcpy(this->rawDepthData, g_depthMD.Data(), 2 * AREA);
	memcpy(this->rawColorData, g_imageMD.Data(), 3 * AREA);

	if(select == 1)
	{
		num2rgb(this->rawDepthData, this->rgbDepthData, AREA, this->minVal, this->maxVal, CONTINUOUS);
		cvSetData(this->depthFrame, this->rgbDepthData, this->depthFrame->widthStep);
	}
	else if(select == 0)
	{
		num2rgb(this->rawDepthData, this->rgbDepthData, AREA, this->minVal, this->maxVal, DISCRETE);
		cvSetData(this->depthFrame, this->rgbDepthData, this->depthFrame->widthStep);
	}

	cvSetData(this->colorFrame, this->rawColorData, this->colorFrame->widthStep);
	cvCvtColor(this->colorFrame, this->colorFrame, CV_BGR2RGB);
}

void Kinect::initialize(bool createConfig)
{
	if(!DigitalGraffiti::fileExists(CONFIG_FILE))
	{
		createConfig =  true;
	}

	if(createConfig)
	{
		// Create new config file, which will overwrite the existing one (if one exists)
		this->minVal = 0;
		this->maxVal = 4095;
		int state = 0;
		int point = -1;
		int xx, yy;
		const char *instruction = "Click Upper Left Projector Corner";
		IplImage *frame = this->colorFrame;

		// Initialize mouse callback for click events and create display window
		cvNamedWindow("Setup");
		cvSetMouseCallback("Setup", &on_mouse, &(point));
		cvInitFont(&this->myFont, CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0f, 1.0f, 0, 1);

		while(state < 8)
		{
			// Poll for current frames
			this->pollFrames(CONTINUOUS);

			// If click event captured set approriate storage variables
			// STATE == 0 - 1: Generate bounding box points
			// STATE == 2 - 4: Generate ball color threshold values
			// STATE == 5 - 6: Generate depth threshold values
			if(point != -1)
			{
				// Record response and advance to next config variable
				switch(state)
				{
				case 0:
					this->tl = cvPoint(point % WIDTH, point / WIDTH);
					instruction = "Click Lower Right Projector Corner";
					frame = this->colorFrame;
					break;
				case 1:
					this->br = cvPoint(point % WIDTH, point / WIDTH);
					instruction = "Click Center of Red Ball (avoid glare)";
					frame = this->colorFrame;
					break;
				case 2:
					this->avg_Red[0] = 0;
					this->avg_Red[1] = 0;
					this->avg_Red[2] = 0;
					xx = point % WIDTH;
					yy = point / WIDTH;
					this->avgColor(this->rawColorData, xx - BALL_BOX, yy - BALL_BOX, xx + BALL_BOX, yy + BALL_BOX, this->avg_Red);
					instruction = "Click Center of Yellow Ball (avoid glare)";
					frame = this->colorFrame;
					break;
				case 3:
					this->avg_Yellow[0] = 0;
					this->avg_Yellow[1] = 0;
					this->avg_Yellow[2] = 0;
					xx = point % WIDTH;
					yy = point / WIDTH;
					this->avgColor(this->rawColorData, xx - BALL_BOX, yy - BALL_BOX, xx + BALL_BOX, yy + BALL_BOX, this->avg_Yellow);
					instruction = "Click Center of Blue Ball (avoid glare)";
					frame = this->colorFrame;
					break;
				case 4:
					this->avg_Blue[0] = 0;
					this->avg_Blue[1] = 0;
					this->avg_Blue[2] = 0;
					xx = point % WIDTH;
					yy = point / WIDTH;
					this->avgColor(this->rawColorData, xx - BALL_BOX, yy - BALL_BOX, xx + BALL_BOX, yy + BALL_BOX, this->avg_Blue);
					instruction = "Click Farthest Distance (~5cm from wall)";
					frame = this->depthFrame;
					break;
				case 5:
					this->maxVal = this->rawDepthData[point];
					instruction = "Click Nearest Distance (~30cm from wall)";
					frame = this->depthFrame;
					break;
				case 6:
					this->minVal = this->rawDepthData[point];
					instruction = "Click to Close Initialization";
					frame = this->colorFrame;
					break;
				default:
					break;
				}
				state++;
				point = -1;
			}

			// Draw text background
			cvDrawRect(frame, cvPoint(0, HEIGHT), cvPoint(WIDTH, HEIGHT - 30), cvScalar(255, 255, 255), CV_FILLED);
			// Draw text
			cvPutText(frame, instruction, cvPoint(10, HEIGHT - 15), &this->myFont, cvScalar(0,0,0));
			cvShowImage("Setup", frame);
			// Pause, required for OpenCV functions
			cvWaitKey(1);
		}

		// Error-check so TL point and BR point have correct values
		if(this->tl.x > this->br.x)
		{
			int temp = this->tl.x;
			this->tl.x = this->br.x;
			this->br.x = temp;
		}
		if(this->tl.y > this->br.y)
		{
			int temp = this->tl.y;
			this->tl.y = this->br.y;
			this->br.y = temp;
		}

		this->roi.x = this->tl.x;
		this->roi.y = this->tl.y;
		this->roi.width = this->br.x - this->tl.x + 1;
		this->roi.height = this->br.y - this->tl.y + 1;

		// Write values to config file
		ofstream conf;
		conf.open (CONFIG_FILE, conf.trunc);
		conf << this->maxVal << endl;
		conf << this->minVal << endl;
		conf << this->avg_Red[0] << endl;
		conf << this->avg_Red[1] << endl;
		conf << this->avg_Red[2] << endl;
		conf << this->avg_Yellow[0] << endl;
		conf << this->avg_Yellow[1] << endl;
		conf << this->avg_Yellow[2] << endl;
		conf << this->avg_Blue[0] << endl;
		conf << this->avg_Blue[1] << endl;
		conf << this->avg_Blue[2] << endl;
		conf << this->tl.x << endl;
		conf << this->tl.y << endl;
		conf << this->br.x << endl;
		conf << this->br.y << endl;
		conf.close();

		// Release mouse callback and destroy window
		cvSetMouseCallback("Setup", NULL);
		cvDestroyWindow("Setup");
	}
	else
	{
		// Read in config file
		char buffer[BUFFER_LENGTH];
		int ii = 0;

		ifstream conf;
		conf.open(CONFIG_FILE);

		// Read depth camera reference values
		conf.getline(buffer, BUFFER_LENGTH);
		this->maxVal = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->minVal = atoi(buffer);

		// Read red ball reference values
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Red[0] = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Red[1] = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Red[2] = atoi(buffer);

		// Read yellow ball reference values
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Yellow[0] = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Yellow[1] = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Yellow[2] = atoi(buffer);

		// Read blue ball reference values
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Blue[0] = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Blue[1] = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->avg_Blue[2] = atoi(buffer);

		// Read bounding box reference values
		conf.getline(buffer, BUFFER_LENGTH);
		this->tl.x = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->tl.y = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->br.x = atoi(buffer);
		conf.getline(buffer, BUFFER_LENGTH);
		this->br.y = atoi(buffer);

		this->roi.x = this->tl.x;
		this->roi.y = this->tl.y;
		this->roi.width = this->br.x - this->tl.x + 1;
		this->roi.height = this->br.y - this->tl.y + 1;

		conf.close();
	}

	cout << "----------" << endl;
	cout << "Background: " << this->maxVal << endl;
	cout << "Foreground: " << this->minVal << endl;
	cout << "Avg Red: " << this->avg_Red[0] << "," << this->avg_Red[1] << "," << this->avg_Red[2] << endl;
	cout << "Avg Yellow: " << this->avg_Yellow[0] << "," << this->avg_Yellow[1] << "," << this->avg_Yellow[2] << endl;
	cout << "Avg Blue: " << this->avg_Blue[0] << "," << this->avg_Blue[1] << "," << this->avg_Blue[2] << endl;
	cout << "Bounding Box: " << this->tl.x << "," << this->tl.y << " || " << this->br.x << "," << this->br.y << endl;
	cout << "----------" << endl;
}

bool Kinect::detectSingle(int *xx, int *yy, int *color)
{
	bool result = false;
	int ii = 0;
	int jj = 0;
	int temp_xx = 0;
	int temp_yy = 0;

	for(ii = this->tl.y; ii < this->br.y; ii += 2)
		for(jj = this->tl.x; jj < this->br.x; jj += 2)
		{
			int lower = this->minVal - BUFFER;
			int upper = this->maxVal - BUFFER;

			if(rawDepthData[ii * WIDTH + jj] > lower && rawDepthData[ii * WIDTH + jj] < upper)
			{
				temp_xx = jj;
				temp_yy = ii;
				*xx = jj;
				*yy = ii;

				cvRectangle(this->colorFrame, cvPoint(temp_xx - 10, temp_yy - 10), cvPoint(temp_xx + 10, temp_yy + 10), cvScalar(0, 0, 0), CV_FILLED);
				result = true;
				ii = HEIGHT;
				break;
			}
		}

		if(result == true)
		{
			for(ii = temp_yy - 100; ii < temp_yy + 100; ii++)
				for(jj = temp_xx - 100; jj < temp_xx + 100; jj++)
				{
					int pixel_b = this->rawColorData[3 * (ii * WIDTH + jj)];
					int pixel_g = this->rawColorData[3 * (ii * WIDTH + jj) + 1];
					int pixel_r = this->rawColorData[3 * (ii * WIDTH + jj) + 2];

					if(pixel_b > 120 && pixel_g < 80 && pixel_r < 80)
					{
						*color = 2;
						ii = HEIGHT;
						break;
					}
					else if(pixel_b < 80 && pixel_g > 120 && pixel_r > 120)
					{
						*color = 1;
						ii = HEIGHT;
						break;
					}
					else if(pixel_b < 80 && pixel_g < 80 && pixel_r > 120)
					{
						*color = 0;
						ii = HEIGHT;
						break;
					}
				}
		}

		return result;
}

bool Kinect::detectMulti(vector<int> *hits)
{
	bool result = false, valid_hit = false, valid_color = false;
	int ii = 0, jj = 0;
	int temp_xx = 0, temp_yy = 0, temp_color = 0;

	IplImage *gray = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, K_BAW);
	IplImage *binary = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, K_BAW);
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *contour = 0;

	cvCvtColor(this->depthFrame, gray, CV_RGB2GRAY);
	cvFindContours(binary, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(this->roi.x, this->roi.y));
	cvSetImageROI(binary, this->roi); 
	cvThreshold(gray, binary, gray->imageData[this->roi.y * WIDTH + this->roi.y], 1, CV_THRESH_BINARY);

	while(contour != 0)
	{
		valid_hit = false;
		valid_color = false;
		CvRect rect = cvBoundingRect(contour);

		if(rect.width < 20 && rect.height < 20)
		{
			for(ii = rect.y; ii < rect.y + rect.height; ii++)
				for(jj = rect.x; jj < rect.x + rect.width; jj++)
				{
					if(this->rgbDepthData[3 * (ii * WIDTH + jj) + 1] == 255)
					{
						temp_xx = rect.x + rect.width / 2;
						temp_yy = rect.y + rect.height / 2;
						valid_hit = true;
						ii = HEIGHT;
						break;
					}
				}
		}

		if(valid_hit)
		{
			for(ii = temp_yy - 50; ii < temp_yy + 50; ii++)
				for(jj = temp_xx - 50; jj < temp_xx + 50; jj++)
				{
					int pixel_b = this->rawColorData[3 * (ii * WIDTH + jj)];
					int pixel_g = this->rawColorData[3 * (ii * WIDTH + jj) + 1];
					int pixel_r = this->rawColorData[3 * (ii * WIDTH + jj) + 2];

					if(pixel_b > 120 && pixel_g < 80 && pixel_r < 80)
					{
						temp_color = 2;
						valid_color = true;
						ii = HEIGHT;
						break;
					}
					else if(pixel_b < 80 && pixel_g > 120 && pixel_r > 120)
					{
						temp_color = 1;
						valid_color = true;
						ii = HEIGHT;
						break;
					}
					else if(pixel_b < 80 && pixel_g < 80 && pixel_r > 120)
					{
						temp_color = 0;
						valid_color = true;
						ii = HEIGHT;
						break;
					}
				}
		}

		if(valid_hit && valid_color)
		{
			hits->push_back(temp_xx);
			hits->push_back(temp_yy);
			hits->push_back(temp_color);
			result = true;
		}

		contour = contour->h_next;
	}

	cvResetImageROI(gray);
	cvRelease((void**)&contour);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&binary);
	cvReleaseImage(&gray);

	return result;
}

bool Kinect::detectMultiFeedback(vector<int> *feedbackOld, vector<int> *hits, vector<int> *feedbackNew)
{
	bool result = false;
	bool valid_hit = false;
	bool valid_color = false;
	int ii = 0;
	int jj = 0;
	int temp_xx = 0;
	int temp_yy = 0;
	int temp_color = 0;

	// Convert to image formats OpenCV functions require
	IplImage *gray = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, K_BAW);
	IplImage *binary = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, K_BAW);
	// Contour objects
	CvMemStorage *storage = cvCreateMemStorage(0);
	// Contour struct pointer
	CvSeq *contour = 0;
	// Convert to image formats OpenCV functions require
	cvCvtColor(this->depthFrame, gray, CV_RGB2GRAY);
	cvThreshold(gray, binary, gray->imageData[this->roi.y * WIDTH + this->roi.x], 255, CV_THRESH_BINARY);
	cvSetImageROI(binary, this->roi); 
	cvFindContours(binary, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(this->roi.x, this->roi.y));

	while(contour != 0)
	{
		valid_hit = false;
		valid_color = false;

		CvRect rect = cvBoundingRect(contour);

		// Check if contour size is consistent with ball
		if(rect.width < CONTOUR_WIDTH && rect.height < CONTOUR_HEIGHT)
		{
			// Check if contour contains pixel inside collision range
			for(ii = rect.y; ii < rect.y + rect.height; ii++)
			{
				for(jj = rect.x; jj < rect.x + rect.width; jj++)
				{
					if(this->rgbDepthData[3 * (ii * WIDTH + jj) + 1] == 255)
					{
						temp_xx = rect.x + rect.width / 2;
						temp_yy = rect.y + rect.height / 2;

						valid_hit = true;
						ii = HEIGHT;
						break;
					}
				}
			}
			if(DigitalGraffiti::DEBUG)
			{
				if(valid_hit) {
					cvRectangle(this->depthFrame, cvPoint(rect.x - 2, rect.y - 2), cvPoint(rect.x + rect.width + 2, rect.y + rect.height + 2), cvScalar(0, 0, 0));
					cvRectangle(this->colorFrame, cvPoint(rect.x - 2, rect.y - 2), cvPoint(rect.x + rect.width + 2, rect.y + rect.height + 2), cvScalar(0, 0, 0));
				}
				else cvRectangle(this->depthFrame, cvPoint(rect.x - 2, rect.y - 2), cvPoint(rect.x + rect.width + 2, rect.y + rect.height + 2), cvScalar(255, 255, 255));
			}
		}
		// Do we have a collision?
		if(valid_hit)
		{
			if(DigitalGraffiti::DEBUG)
			{
				cvShowImage("Depth", this->depthFrame);
				cvMoveWindow("Depth", 0, 0);
				cvShowImage("Color", this->colorFrame);
				cvMoveWindow("Color", 700, 0);
				cvWaitKey(1);
			}

			// Look at center contour pixel color
			int pixel_b = this->rawColorData[3 * (temp_yy * WIDTH + temp_xx)];
			int pixel_g = this->rawColorData[3 * (temp_yy * WIDTH + temp_xx) + 1];
			int pixel_r = this->rawColorData[3 * (temp_yy * WIDTH + temp_xx) + 2];

			// Expand from center pixel
			int numRed = 0;
			int numYellow = 0;
			int numBlue = 0;
			for(ii = rect.y; ii < rect.y + rect.height; ii++)
			{
				for(jj = rect.x; jj < rect.x + rect.width; jj++)
				{
					int b = this->rawColorData[3 * (ii * WIDTH + jj)];
					int g = this->rawColorData[3 * (ii * WIDTH + jj) + 1];
					int r = this->rawColorData[3 * (ii * WIDTH + jj) + 2];
					if(r > 100
						&& r > 2 * g
						&& r > 2 * b)
					{
						numRed++;
					} else if(r > 100
						&& g > 100
						&& r > 2 * b
						&& g > 2 * b)
					{
						numYellow++;
					} else if(b > 100
						&& b > 2 * r
						&& b > 2 * g)
					{
						numBlue++;
					}
				}
			}
			if(numRed > 5 && numRed >= numYellow && numRed >= numBlue)
			{
				temp_color = DigitalGraffiti::RED_BALL;
				valid_color = true;
			} 
			else if(numYellow > 5 && numYellow >= numRed && numYellow >= numBlue)
			{
				temp_color = DigitalGraffiti::YELLOW_BALL;
				valid_color = true;
			} 
			else if(numBlue > 5 && numBlue >= numRed && numBlue >= numYellow)
			{
				temp_color = DigitalGraffiti::BLUE_BALL;
				valid_color = true;
			}
		}

		if(valid_hit && valid_color)
		{
			// Add hit and color to new feedback list
			feedbackNew->push_back(temp_xx);
			feedbackNew->push_back(temp_yy);
			feedbackNew->push_back(temp_color);

			// Filter out collisions from last frame (AKA doubletaps)
			bool newBall = true;
			if(feedbackOld != NULL) 
			{
				for(unsigned int ii = 0; ii < feedbackOld->size(); ii+=3) 
				{
					if((squareDistance(temp_xx, feedbackOld->operator[](ii), temp_yy, feedbackOld->operator[](ii + 1)) < FEEDBACK_DIST_THRESH)
						&& (temp_color == feedbackOld->operator[](ii + 2)))
					{
						newBall = false;
						break;
					}
				}
			}
			if(newBall) 
			{
				hits->push_back(temp_xx);
				hits->push_back(temp_yy);
				hits->push_back(temp_color);
				result = true;
			}
		}

		contour = contour->h_next;
	}

	cvResetImageROI(gray);
	cvRelease((void**)&contour);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&binary);
	cvReleaseImage(&gray);

	return result;
}

int Kinect::squareDistance(int x1, int x2, int y1, int y2, int z1, int z2)
{
	return (int) (pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0) + pow(z1 - z2, 2.0));
}