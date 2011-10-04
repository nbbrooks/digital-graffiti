class Collision
{
private:
	static const int WIDTH		= 640;
	static const int HEIGHT		= 480;
	static const int AREA		= WIDTH * HEIGHT;
	static const int BAW		= 1;
	static const int RGB		= 3;
	static const bool C2D		= true;
	static const bool D2C		= false;

	CLNUICamera kinect;
	bool success;
	CvMat *c2d_map;
	CvMat *d2c_map;
	time_t time_Start;
	time_t time_End;

	int state;
	int point;
	CvFont myFont;

	int minVal;
	int maxVal;
	int target;
	CvPoint depth_tl;
	CvPoint depth_br;
	CvPoint color_tl;
	CvPoint color_br;
	short rawDepthData[AREA];
	unsigned char rawColorData[AREA * 3];
	unsigned char rgbDepthData[AREA * 3];

	static void on_mouse(int click_event, int xx, int yy, int flags, void *param);
	static void num2spectrum(short *nums, unsigned char *rgbs, int numlen, int min, int max);
	static void num2rgb(short *nums, unsigned char *rgbs, int numlen, int min, int max);

public:
	static const int NONE		= -1;
	static const int SPECTRUM	= 0;
	static const int DISTINCT	= 1;

	IplImage *depthFrame;
	IplImage *colorFrame;

	Collision()
	{
		// Initialize the Kinect for polling
		this->kinect	= CreateNUICamera(GetNUIDeviceSerial(0));
		this->success	= StartNUICamera(this->kinect);

		// Initialize the containers for the depth and color frames
		this->depthFrame = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, RGB);
		this->colorFrame = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, RGB);

		// Create the translation map for depth to color corrdinates and vice versa
		c2d_map = cvCreateMat(2, 3, CV_32FC1);
		d2c_map = cvCreateMat(2, 3, CV_32FC1);

		CvPoint2D32f src[3];
		CvPoint2D32f dst[3];

		src[0].x = 10;			dst[0].x = 20;
		src[0].y = 40;			dst[0].y = 0;
		src[1].x = 11;			dst[1].x = 22;
		src[1].y = 479;			dst[1].y = 465;
		src[2].x = 304;			dst[2].x = 320;
		src[2].y = 262;			dst[2].y = 240;

		cvGetAffineTransform(src, dst, c2d_map);
		cvGetAffineTransform(dst, src, d2c_map);

		// Delay to guarantee the Kinect is ready for polling
		this->time_Start	= time(0);
		this->time_End		= time(0);
		while (difftime(this->time_End, this->time_Start) < 2.0) this->time_End = time(0);
	}
	~Collision()
	{
		cvReleaseImage(&this->depthFrame);
		cvReleaseImage(&this->colorFrame);
		cvReleaseMat(&this->c2d_map);
		cvReleaseMat(&this->d2c_map);
		StopNUICamera(this->kinect);
		DestroyNUICamera(this->kinect);
	}

	void translate_XY(int src_xx, int src_yy, int *dst_xx, int *dst_yy, bool select);
	void pollDepthFrame(int select);
	void pollColorFrame();
	
	bool initialize();
	bool detect(int range, float *xx, float *yy, int *color);
};