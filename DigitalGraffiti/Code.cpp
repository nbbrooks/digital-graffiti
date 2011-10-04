//Add winmm.lib in  Project->Properties->Configuration Properties->Linker->Input->Additional Dependencies
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

using namespace std;
 // a simple class declaration part
    
	class  Image
    {
     // private by default, member variables
      bool check_data;
     
      public:
     // returns 1 or 0 for image processed or not
     bool Imageprocessing();
	     
    };
	bool Image::Imageprocessing ()
	{
		std::cout << "Wait..." << std::endl;
		return 1;// Andrew Code
	}
	


	class  Splash
    {
           
      public:
     // returns 1 or 0 for image processed or not
     bool splash(void);
	     
    };
  bool  Splash::splash()
    {
      std::cout << "Ready for Splash" << std::endl;
		return 1;//Nathan Code
    }
  
  // Other classes if required
  class  rectangle
   {
     // private by default, member variables
      int height;
     int width;
      public:
     // public, with two methods
     int area(void);
     void initialize(int, int);
   };

 int rectangle::area(void)
   {
      return (height * width);
   }
  
 void  rectangle::initialize(int initial_height, int initial_width)
 {
       height = initial_height;
       width = initial_width;
 }
 /*Other Classes End */


int main()
{
	system("color fc");
	std::cout << "This is a program for Children's Museum" << std::endl;
	//BACKGROUND MUSIC
	PlaySound(TEXT("c:\\Users\\kartik\\Desktop\\exclusive_rights.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
	Image Imagecheck;
	Splash Splashcheck;

	/*Other Classes, if required*/
	rectangle wall, square;
	wall.initialize(12,10);
	square.initialize(8,8);

	std: cout<<"Area of the wall-->wall.area() = "<<wall.area()<< "\n\n";
	std::cout << "This is GRAFFITTI FOR Children's Museum." << std::endl;
	
	 //	PlaySound(TEXT("SystemStart"), NULL, SND_ALIAS);
	//PlaySound((LPCWSTR) "c:\\Users\kartik\Desktop\exclusive_rights.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOWAIT);
	//Beep(750,300);
	//std::cout << "This is a  C program." << std::endl;
	if (Imagecheck.Imageprocessing() == 1) 
	{
		std::cout << "Back here" << std::endl;	
		if (Splashcheck.splash()==1)
		{
			_getch();
			PlaySound(TEXT("c:\\Users\\kartik\\Desktop\\splat.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
		}

	}
	
	/*for ( int i= 0 ;i <5; i ++)
    {	PlaySound(TEXT("c:\\Users\\kartik\\Desktop\\exclusive_rights.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
 	    std::cout << "This works here" << std::endl;
    }*/
    
	std::cout << "This works here 2" << std::endl;	
     //PlaySound(TEXT( "c:\\Users\kartik\Desktop\exclusive_rights.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NOWAIT);
	 //PlaySound ( "C:\\Users\kartik\Desktop\exclusive_rights.wav", NULL, SND_SYNC);

	// Splash Sound
	system("PAUSE");
	PlaySound(TEXT("c:\\Users\\kartik\\Desktop\\splat.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_NOWAIT);
	system("PAUSE");

	_getch();
	return 0;
}
