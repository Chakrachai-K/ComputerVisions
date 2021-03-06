// simplewin32.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include <math.h>
#include <shobjidl.h> 
#include  "simplewin32.h"
#define MAX_LOADSTRING  100

// Global Variables:
HINSTANCE   hInst;                          // current instance
char        szTitle [MAX_LOADSTRING];       // The title bar text
char        szWindowClass [MAX_LOADSTRING]; // the main window class name
char        szWindowChildClass [MAX_LOADSTRING];
// Forward declarations of functions included in this code module:
ATOM                    RegisterMainClass (HINSTANCE hInstance);
BOOL                    InitInstance (HINSTANCE, int);
BOOL					RegisteChildClass(HINSTANCE, int);
LRESULT CALLBACK        WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK        About (HWND, UINT, WPARAM, LPARAM);

HWND    hWndText;
HWND hWnd, converlutionTool;
HWND converlutionTool1, converlutionTool2, converlutionTool3,
	converlutionTool4, converlutionTool5, converlutionTool6,
	converlutionTool7, converlutionTool8, converlutionTool9;  //textbox
int number;
char    szText [241];
unsigned char	*image	= NULL;			// image array
unsigned char	*grey1	= NULL;
unsigned char	*imageMaster	= NULL;
long			 bpp, cx = 640, cy = 480;	// image dimension
LPOLESTR		filepath;
BITMAPFILEHEADER	 bf;
BITMAPINFO		 bi;

float			maxBrightness = 0;
float 			*prop,*totalsupportEdge,*totalsupportNotEdge,*panel;
float			plabel[2] = {0,1};
boolean			state = true;


int APIENTRY _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPTSTR lpCmdLine,
                        int nCmdShow)
{
    MSG     msg;
    HACCEL  hAccelTable;

    // Initialize global strings
    strcpy (szTitle, "Computer Vision Programming Ex. 1");
    strcpy (szWindowClass, "simplewin32");
	strcpy (szWindowChildClass, "simplewin32Child");
    RegisterMainClass (hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) 
    {
        return FALSE;
    }

	if (!RegisteChildClass (hInstance, nCmdShow)) 
    {
        return FALSE;
    }
    hAccelTable = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDC_SIMPLEWIN32));

    // Main message loop:
    while (GetMessage (&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) 
        {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM RegisterMainClass (HINSTANCE hInstance)
{
    WNDCLASSEX  wcex,wcext;

    wcex.cbSize = sizeof (WNDCLASSEX); 

    wcex.style              = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc        = (WNDPROC)WndProc;
    wcex.cbClsExtra         = 0;
    wcex.cbWndExtra         = 0;
    wcex.hInstance          = hInstance;
    wcex.hIcon              = LoadIcon (hInstance, (LPCTSTR) IDI_SIMPLEWIN32);
    wcex.hCursor            = LoadCursor (NULL, IDC_ARROW);
    wcex.hbrBackground      = (HBRUSH) (COLOR_WINDOW+1);
    wcex.lpszMenuName       = (LPCTSTR) IDC_SIMPLEWIN32;
    wcex.lpszClassName      = szWindowClass;
    wcex.hIconSm            = LoadIcon (wcex.hInstance, (LPCTSTR) IDI_SMALL);
    return RegisterClassEx (&wcex);
}

BOOL RegisteChildClass (HINSTANCE hInstance ,int nCmdShow)
{
    WNDCLASSEX  wcex;

    wcex.cbSize = sizeof (WNDCLASSEX); 

    wcex.style              = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc        = (WNDPROC)WndProc;
    wcex.cbClsExtra         = 0;
    wcex.cbWndExtra         = 0;
    wcex.hInstance          = hInstance;
    wcex.hIcon              = LoadIcon (hInstance, (LPCTSTR) IDI_SIMPLEWIN32);
    wcex.hCursor            = LoadCursor (NULL, IDC_ARROW);
    wcex.hbrBackground      = (HBRUSH) (COLOR_WINDOW+1);
    wcex.lpszMenuName       = NULL;
    wcex.lpszClassName      = szWindowChildClass;
    wcex.hIconSm            = LoadIcon (wcex.hInstance, (LPCTSTR) IDI_SMALL);
	if(!RegisterClassEx (&wcex)){
		return FALSE;
	}
}

BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{

    hInst   = hInstance; // Store instance handle in our global variable
    hWnd    = CreateWindow (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	number = nCmdShow;

   // hWndText = CreateWindow ("STATIC", "0", WS_CHILD | WS_BORDER | WS_VISIBLE | SS_RIGHT,10, 10, 155, 20, hWnd, NULL, hInstance, NULL);
    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    return TRUE;
}

void mydraw (HDC hdc)
{
	if (image == NULL) return;

	SetDIBitsToDevice (hdc, 0, 0, cx, cy, 0, 0, 0, cy, 
					   image, &bi, DIB_RGB_COLORS);
}
void showImage()
{
	long line ,x ;
	for (line = 0; line < cy; line ++)
	{
		for (x = 0; x < cx; x ++)
		{
			image [bpp*(line*cx + x) + 0] = grey1 [x + line*cx];
			image [bpp*(line*cx + x) + 1] = grey1 [x + line*cx];
			image [bpp*(line*cx + x) + 2] = grey1 [x + line*cx];
		}
	}
}

void noiseProcess(unsigned char *ig1, long cx, long cy,boolean status){
	
	unsigned char	*ig2;
	long			 x, y,m,n;
	long			xpixel = 303, ypixel = 106 , swapig1 , swapig2 ,swapx, swapy, swapm, swapn,swap;
	double			sum = 0,t = 100,medean[9];
	int				count = 0,round;
	double			 h [3][3] = {{1,  1, 1},
								 {1,   1, 1},
								 {1,  1, 1}};
	double			 nrm;

	ig2 = (unsigned char *) malloc (cx*cy);
	for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			ig2 [y*cx + x] = ig1 [y*cx + x];
		}
	}

	if(status){
		for (y = 1; y < cy - 1; y ++)
		{
			for (x = 1; x < cx - 1; x ++)
			{
				sum = 0.0;
				nrm = 0.0;
				for (n = -1; n <= 1; n ++)
				{
					for (m = -1; m <= 1; m ++)
					{
						sum += h [n+1][m+1]*ig2 [(y+n)*cx + (x+m)];
						nrm += h [n+1][m+1] < 0.0 ? -h [n+1][m+1] : h [n+1][m+1];
					}
				}
				ig1 [y*cx + x] = (int) ((sum < 0.0 ? -sum : sum)/nrm);
			}
		}
	}else{
		for (y = 1; y < cy - 1; y ++)
		{
			for (x = 1; x < cx - 1; x ++)
			{
				sum = 0.0;
				nrm = 0.0;
				for (n = -1; n <= 1; n ++)
				{
					for (m = -1; m <= 1; m ++)
					{
					
						sum += h [n+1][m+1]*ig2 [(y+n)*cx + (x+m)];
						nrm += h [n+1][m+1] < 0.0 ? -h [n+1][m+1] : h [n+1][m+1];

						//mean
						medean[count] = (h [n+1][m+1]*ig2 [(y+n)*cx + (x+m)] < 0.0 ? h [n+1][m+1]*ig2 [(y+n)*cx + (x+m)]*(-1) : h [n+1][m+1]*ig2 [(y+n)*cx + (x+m)]);
						//medean
						count++;
					}
				}
				count = 0;
				for(round = 0 ; round < 19 ; round ++)
				{
					for(count = 0 ; count < 8 ; count ++)
					{
						if(medean[count] > medean[count+1]){
							swap = medean[count] ;
							medean[count] = medean[count+1];
							medean[count+1] = swap;
						}
					}
				}
				//ig1 [y*cx + x] = (int) ((sum < 0.0 ? -sum : sum)/nrm); //sum = sum*-1 >>mean
			
				ig1 [y*cx + x] = (int) medean[4]; //medean
				count = 0;
			}
		}
	}
	showImage();
	free (ig2);
}

void converlutionProcess (unsigned char *ig1, long cx, long cy)
{
	unsigned char	*ig2;
	long			 x, y, m, n;
	int				f1, f2, f3, f4, f5, f6, f7, f8, f9;

	GetWindowText(converlutionTool1,szText,10);
	f1 = atoi(szText);
	
	GetWindowText(converlutionTool2,szText,10);
	f2 = atoi(szText);
	
	GetWindowText(converlutionTool3,szText,10);
	f3 = atoi(szText);
	
	GetWindowText(converlutionTool4,szText,10);
	f4 = atoi(szText);
	
	GetWindowText(converlutionTool5,szText,10);
	f5 = atoi(szText);
		
	GetWindowText(converlutionTool6,szText,10);
	f6 = atoi(szText);
	
	GetWindowText(converlutionTool7,szText,10);
	f7 = atoi(szText);
		
	GetWindowText(converlutionTool8,szText,10);
	f8 = atoi(szText);
		
	GetWindowText(converlutionTool9,szText,10);
	f9 = atoi(szText);

	double			 h [3][3] = {{f1,  f2, f3},
								 {f4,  f5, f6},
								 {f7,  f8, f9}};
	double			 sum, nrm;

	/*for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			if (x > 20 && x < 100)
				ig1 [x + y*cx] = 0;
		}
	}
	*/

	ig2 = (unsigned char *) malloc (cx*cy);
	for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			ig2 [y*cx + x] = ig1 [y*cx + x];
		}
	}

	for (y = 1; y < cy - 1; y ++)
	{
		for (x = 1; x < cx - 1; x ++)
		{
			sum = 0.0;
			nrm = 0.0;
			for (n = -1; n <= 1; n ++)
			{
				for (m = -1; m <= 1; m ++)
				{
					sum += h [n+1][m+1]*ig2 [(y+n)*cx + (x+m)];
					nrm += h [n+1][m+1] < 0.0 ? -h [n+1][m+1] : h [n+1][m+1];
				}
			}
			ig1 [y*cx + x] = (int) ((sum < 0.0 ? -sum : sum)/nrm);
		}
	}
	showImage();
	free (ig2);

}

void p0(unsigned char *ig2 , double maxBrightness)
{
	double			 h [3] = { -1, 1, 1};
	float			 up, down, brightness = 0,p ,degree,degreeValue;
	long			x,y,n,m = 0;
	char			str[255];
	double			addrress ;

	for (y = 0; y < cy; y ++)
    {
		for (x = 0; x < cx; x ++)
		{
			down = (h [2]*ig2 [(y)*cx + (x + 1)] + h [0]*ig2 [(y)*cx + (x- 1)])/2;
			up   = (h [0]*ig2 [(y + 1)*cx + (x)] + h [2]*ig2 [(y - 1)*cx + (x)])/2;

			brightness = sqrt(pow(down,2)+pow(up,2)) < 0.0 ? -sqrt(pow(down,2)+pow(up,2)) : sqrt(pow(down,2)+pow(up,2)); //�������
			prop[(y)*cx + (x)] = brightness / maxBrightness ;// p0

			//if(prop[(y)*cx + (x)] > 0.5)
			//sprintf_s(str ,"prop[(y)*cx + (x)]=%lf\n",prop[(y)*cx + (x)]);
			//OutputDebugString(str);
			
		}
	}
}

double degree(long y, long x, long n, long m, unsigned char *ig2)
{
	double			 h [3] = { -1, 1, 1};
	double			 up, down, brightness = 0, degree,degreeValue;
	char			str[255];

	if((y + n) < 0 || (y + n) > cy || (x + n) < 0 || (x + n) > cx){
		down = (h [2] * 0 + h [0] * 0)/2;
		up   = (h [0] * 0 + h [2] * 0)/2;
	}else
	{
		down = (h [2]*ig2 [(y + n)*cx + (x + m + 1)] + h [0]*ig2 [(y + n)*cx + (x + m - 1)])/2;
		up   = (h [0]*ig2 [(y + n + 1)*cx + (x + m)] + h [2]*ig2 [(y + n - 1)*cx + (x + m)])/2;
	}

	if(up == 0){

		degree = 90.0;

	}else{

		degreeValue = (down/up);
		degree = (atan(degreeValue))*180/(22/7);
	}
		///sprintf_s(str ,"down = %lf , up = %lf ,brigth = %lf , degreevakue=%lf , degree=%lf\n",down,up,brightness,degreeValue,degree);
		//OutputDebugString(str);
	return degree;
}
void laplace(unsigned char *pic, long cx, long cy){
	unsigned char	*ig2;
	long			 x, y, m, n;
	double			 h [3][3] = {{0,  1, 0},
								 {1,  -4, 1},
								 {0,  1, 0}};
	double			 sum, nrm,maxSum = 0.0,minSum = 0.0,maxCount =0,minCount = 0;
	
	/*for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			if (x > 20 && x < 100)
				ig1 [x + y*cx] = 0;
		}
	}
	*/

	ig2 = (unsigned char *) malloc (cx*cy);
	for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			ig2 [y*cx + x] = pic [y*cx + x];
		}
	}

	for (y = 1; y < cy - 1; y ++)
	{
		for (x = 1; x < cx - 1; x ++)
		{
			sum = 0.0;
			nrm = 0.0;
			for (n = -1; n <= 1; n ++)
			{
				for (m = -1; m <= 1; m ++)
				{
					sum += h [n+1][m+1]*ig2 [(y+n)*cx + (x+m)];
					nrm += h [n+1][m+1] < 0.0 ? -h [n+1][m+1] : h [n+1][m+1];
				}
			}
			pic [y*cx + x] = (int) ((sum < 0.0 ? -sum : sum)/nrm);
		}
	}

	free(ig2);

}

void labelPixel(unsigned char *pic,long cx, long cy){

	unsigned char	*ig2;
	long			 x, y, m, n;
	double			 h [3][3] = {{0,  1, 0},
								 {1,  -4, 1},
								 {0,  1, 0}};
	double			 sum, nrm,maxSum = 0.0,minSum = 0.0,maxCount =0,minCount = 0;
	char			str[254];
	//panel [20*cx + 20] = (float)1;
	for (y = 0; y < cy ; y ++)
	{
		for (x = 0; x < cx ; x ++)
		{
			if(pic [y*cx + x] != 0){
				maxSum += pic [y*cx + x];
				maxCount += 1;
			}else{
				minSum += pic [y*cx + x];
				minCount += 1;
			}
		}
	}
	//sprintf_s(str ,"maxSum = %lf \n",maxSum);
	//OutputDebugString(str);
	for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			if(pic [y*cx + x] != 0){

				//panel [y*cx + x] = (float)1;
				plabel[1] = maxSum / maxCount;;
			}else
			{
				//panel [(y*cx + x)] = minSum / minCount;
				plabel[0] = minSum / minCount;;
			}
		//	sprintf_s(str ,"label [(y*cx + x)] = %lf \n",label [(y*cx + x)]);
			//OutputDebugString(str);

		}
	}
}
float support(float num,long y ,long x , float *pop,float *p){
	int			count;
	long		n,m;
	float		r,q = 0,qsupport = 0;
		panel[0] = 0.0;
	panel[1] = 1.0;
	for (n = -2; n <= 2; n ++)
	{
		for (m = -2; m <= 2; m ++)
		{
			q= 0;
			r = 0;
				if((y + n) >= 0 || (y + n) <= cy || (x + n) >= 0 || (x + n) <= cx){
					r = fabs(1.0 - fabs(degree(y,x,0,0,imageMaster) - degree(y,x,n,m,imageMaster))/180)*pop[(y+n)*cx +(x+m)] +(0.5*(1-pop[(y+n)*cx +(x+m)]));
					r = r + (0.5*(1-pop[(y+n)*cx +(x+m)])) + (0.5 * pop[(y+n)*cx +(x+m)]);
						q = r;
			}
			qsupport = qsupport + q;
		}
	}

	return qsupport;
}


void relaxtion(unsigned char *ig1, long cx, long cy)
{
	unsigned char *ig2,*ori;
	long			x,y,n,m;
	double			 h [3] = { -1, 1, 1};
	float			down,up;
	double			num = 2;
	int				count,mnn;
	char			str[254];
	float			maxCount = 0,minCount = 0,maxSum = 0,minSum = 0;

    ig2 = (unsigned char *) malloc (cx*cy);
	ori = (unsigned char *) malloc (cx*cy);



    for (y = 0; y < cy; y ++)
    {
		for (x = 0; x < cx; x ++)
		{
			ig2 [y*cx + x] = ig1 [y*cx + x];
			ori[y*cx + x] = ig1 [y*cx + x];
			down = (h [2]*ig1 [(y)*cx + (x+1)] + h [0]*ig1 [(y)*cx + (x-1)])/2;
			up = (h [0]*ig1 [(y+1)*cx + (x)] + h [2]*ig1 [(y-1)*cx + (x)])/2;
			if(fabs(sqrt(pow(down,2)+pow(up,2))) > maxBrightness)
			{
				maxBrightness = fabs(sqrt(pow(down,2)+pow(up,2)));
			}
		}
	}

	//laplace(ori,cx,cy);
	//labelPixel(ori,cx,cy);
	if(state){
		prop = (float *) malloc (cx*cy*sizeof(float*));
		totalsupportEdge = (float *) malloc (cx*cy*sizeof(float*));
		totalsupportNotEdge = (float *) malloc (cx*cy*sizeof(float*));
		panel = (float *) malloc (cx*cy*sizeof(float*));
		p0(ig2,maxBrightness);
		state = false;
		for (y = 0; y < cy ; y ++)
		{
			for (x = 0; x < cx ; x ++)
			{
				if(ori [y*cx + x] != 0){
					maxSum += ori [y*cx + x];
					maxCount += 1;
				}else{
					minSum += ori [y*cx + x];
					minCount += 1;
				}
			}
	}
	//sprintf_s(str ,"maxSum = %lf \n",maxSum);
	//OutputDebugString(str);
	for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			if(ori [y*cx + x] != 0){

				panel [y*cx + x] =  1.0;
				plabel[1] = 0.0;
			}else
			{
				panel [(y*cx + x)] = 0.0;
				plabel[0] = 0.0;
			}
		//	sprintf_s(str ,"label [(y*cx + x)] = %lf \n",label [(y*cx + x)]);
			//OutputDebugString(str);

		}
	}
	}
	for(int i = 0; i< 5;i++){
	for (y = 0; y < cy; y ++)
    {
		for (x = 0; x < cx; x ++)
		{
			totalsupportEdge[y*cx + x] = support(num,y,x,prop,panel);
			totalsupportNotEdge[y*cx + x] = support(num,y,x,prop,panel);
				//sprintf_s(str ,"prop[y*cx + x] = %lf ,prop[+1] = %lf\n",prop[y*cx + x],(prop[y*cx + x]*totalsupportEdge[y*cx + x])/((prop[y*cx + x]*totalsupportEdge[y*cx + x])  + ((1 - prop[y*cx + x])*totalsupportNotEdge[y*cx + x])));
				//OutputDebugString(str);
				prop[y*cx + x] = (prop[y*cx + x]*totalsupportEdge[y*cx + x])/((prop[y*cx + x]*totalsupportEdge[y*cx + x])  + ((1 - prop[y*cx + x])*totalsupportNotEdge[y*cx + x]));

				//sprintf_s(str ,"label [(y*cx + x)] = %lf ,p0 = %lf ,p1 = %lf\n",panel [(y*cx + x)],plabel[1],plabel[0]);
				//OutputDebugString(str);
				if(255 * prop[y*cx + x] != 0 ){
					panel [y*cx + x] = plabel[1];
				}
				ig1[y*cx + x] = 255 * prop[y*cx + x];
		}
	}
	}
	showImage();
	free(ig2);
	//free(label);
}

void loadfile (LPOLESTR lpszpath)
{
	FILE				*fp;
	long				 line, x, b, g, r;

	fp = _wfopen (lpszpath, L"rb");
	fread (&bf, 1, sizeof (BITMAPFILEHEADER), fp);
	fread (&bi, 1, sizeof (BITMAPINFO), fp);

	cx		= bi.bmiHeader.biWidth;
	cy		= bi.bmiHeader.biHeight;
	bpp		= bi.bmiHeader.biBitCount/8;
	image	= (unsigned char *) malloc (cx*cy*bpp);
	grey1	= (unsigned char *) malloc (cx*cy);
	imageMaster = (unsigned char *) malloc (cx*cy);

	for (line = 0; line < cy; line ++) {
		fread (image + line*bpp*cx, 1, bpp*cx, fp);

		for (x = 0; x < cx; x ++)
		{
			b = image [bpp*(line*cx + x) + 0];
			g = image [bpp*(line*cx + x) + 1];
			r = image [bpp*(line*cx + x) + 2];

			grey1 [x + line*cx] = (int) (0.5*r+0.2*g+0.3*b);
			imageMaster [x + line*cx] = (int) (0.5*r+0.2*g+0.3*b);
		}
	}
	showImage();
	fclose (fp);
}

int WINAPI openDiarog()
{
	long line,x;
	PAINTSTRUCT ps;
    HDC         hdc;


    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
		
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		//set find to bmp
		LPCWSTR fulltype = L"Bitmap";
		LPCWSTR shorttype = L"*.bmp";
		COMDLG_FILTERSPEC temp[] = { 
			{fulltype, shorttype}
		}; 

		hr = pFileOpen->SetFileTypes(_countof(temp), temp); 

        if (SUCCEEDED(hr))
        {

            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {

                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filepath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
						loadfile(filepath);

                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return 0;
}

int WINAPI saveDiarog()
{

	FILE				*fout;
	long				px,py;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
		
        IFileSaveDialog *pFileSave;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		//set find to bmp
		LPCWSTR fulltype = L"Bitmap";
		LPCWSTR shorttype = L"*.bmp";
		COMDLG_FILTERSPEC temp[] = { 
			{fulltype, shorttype}
		}; 

		hr = pFileSave->SetFileTypes(_countof(temp), temp); 
		hr = pFileSave->SetDefaultExtension(shorttype);

        if (SUCCEEDED(hr))
        {

            hr = pFileSave->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {

					 LPOLESTR pszFilePath;

                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                       // MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
						//filepath = pszFilePath;
						MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
						fout = _wfopen(pszFilePath, L"wb");
						fwrite(&bf, 1, sizeof(bf), fout);
						fwrite(&bi, 1, sizeof(bi), fout);

						for (py = 0; py < cy; py ++)
						{
							for (px = 0; px < cx; px ++)
							{
								fputc(image [bpp*(py*cx + px) + 0],fout);	
								fputc(image [bpp*(py*cx + px) + 1],fout);
								fputc(image [bpp*(py*cx + px) + 2],fout);
							}
						}
						fclose(fout);
                    }
                    pItem->Release();
                }
            }
            pFileSave->Release();
        }
        CoUninitialize();
    }
    return 0;
}

void createConverlutionTool()
{
converlutionTool = CreateWindowEx(NULL,szWindowChildClass, "Converlution Tool",  WS_CAPTION | WS_SYSMENU ,CW_USEDEFAULT, CW_USEDEFAULT, 481, 192, NULL, NULL, NULL, NULL);
					CreateWindow("BUTTON","Test",WS_CHILD | WS_VISIBLE,363,35,50,20,converlutionTool,(HMENU)IDM_OKCON,hInst,0);
					CreateWindow("BUTTON","Save",WS_CHILD | WS_VISIBLE,363,73,50,20,converlutionTool,(HMENU)IDM_SAVECON,hInst,0);
					CreateWindow("BUTTON","Cancle",WS_CHILD | WS_VISIBLE,363,115,50,20,converlutionTool,(HMENU)IDM_CANCLECON,hInst,0);
converlutionTool1 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,12,36,100,20,converlutionTool,(HMENU)IDM_CTEXT1,hInst,0);
converlutionTool2 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,118,36,100,20,converlutionTool,(HMENU)IDM_CTEXT2,hInst,0);
converlutionTool3 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,224,36,100,20,converlutionTool,(HMENU)IDM_CTEXT3,hInst,0);
converlutionTool4 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,12,74,100,20,converlutionTool,(HMENU)IDM_CTEXT4,hInst,0);
converlutionTool5 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,118,74,100,20,converlutionTool,(HMENU)IDM_CTEXT5,hInst,0);
converlutionTool6 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,224,74,100,20,converlutionTool,(HMENU)IDM_CTEXT6,hInst,0);
converlutionTool7 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,12,115,100,20,converlutionTool,(HMENU)IDM_CTEXT7,hInst,0);
converlutionTool8 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,118,115,100,20,converlutionTool,(HMENU)IDM_CTEXT8,hInst,0);
converlutionTool9 = CreateWindow("EDIT","0",WS_CHILD | WS_VISIBLE |WS_BORDER,224,115,100,20,converlutionTool,(HMENU)IDM_CTEXT9,hInst,0);
					ShowWindow(converlutionTool,number);
					UpdateWindow(converlutionTool);
}
void paint(HWND hWnds)
{
	HDC         hdc;
	hdc = GetDC (hWnds);
	mydraw (hdc);
	ReleaseDC (hWnds, hdc);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc (HWND hWnds, UINT message, WPARAM wParam, LPARAM lParam)
{
    int         wmId, wmEvent , i;
    PAINTSTRUCT ps;
    HDC         hdc;
	long		y,x;
	boolean		status;

    switch (message) 
    {
        case WM_COMMAND :   wmId    = LOWORD (wParam);
                            wmEvent = HIWORD (wParam); 

                            // Parse the menu selections:
                            switch (wmId)
                            {
								case IDM_ABOUT					:   DialogBox (hInst, (LPCTSTR) IDD_ABOUTBOX, hWnds, (DLGPROC) About);
																		break;

								case IDM_OPENFILE				:	openDiarog();
																	paint(hWnds);
																	if(filepath)
																	CoTaskMemFree(filepath);
																	SetWindowPos(hWnd,NULL,0,0,cx+16,cy+59,NULL);
																			state = true;
																	break;
								case IDM_SAVEFILE				:	saveDiarog();
																	break;
								
								case IDM_EXIT					:   PostQuitMessage (0);
																	break;
								case IDM_CONVERLUTION			:	if(converlutionTool == NULL){
																		createConverlutionTool();
																		state = true;
																	}
																	break;
								case IDM_OKCON					:	converlutionProcess (grey1, cx, cy);
																	paint(hWnd);
																	break;
								case IDM_CANCLECON				:	grey1	= (unsigned char *) malloc (cx*cy);
																	for (y = 0; y < cy; y ++)
																	{
																		for (x = 0; x < cx; x ++)
																		{
																			grey1 [y*cx + x] = imageMaster [y*cx + x];
																		}
																	}
																	showImage ();
																	paint(hWnd);
																	break;
								case IDM_SAVECON				:	imageMaster = (unsigned char *) malloc (cx*cy);
																	for (y = 0; y < cy; y ++)
																	{
																		for (x = 0; x < cx; x ++)
																		{
																			imageMaster [y*cx + x] = grey1 [y*cx + x];
																		}
																	}
																	paint(hWnd);
																	break;

								case IDM_MEAN					:	noiseProcess(grey1, cx, cy,true);
									state = true;
																	paint(hWnd);
																	break;

								case IDM_MEDIAN					:	noiseProcess(grey1, cx, cy,false);
									state = true;
																	paint(hWnd);
																	break;
								case IDM_RL						:	//for(i = 0;i<=34;i++)
																	relaxtion(grey1, cx, cy);
																	paint(hWnd);
																	break;
								default					         :   
																	return DefWindowProc(hWnds, message, wParam, lParam);
                            }
                            break;

        case WM_PAINT   :    if(hWnds != converlutionTool){
								hdc = BeginPaint (hWnds, &ps);
								mydraw (hdc);
								EndPaint (hWnds, &ps);
							 }
                            break;

		case WM_DESTROY : 	if(hWnds == converlutionTool){
								DestroyWindow(hWnds);
								converlutionTool = NULL;
							}else{
								PostQuitMessage (0);
							}

							break;

        default         :   return DefWindowProc (hWnds, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
LRESULT CALLBACK About (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG  :   return TRUE;

        case WM_COMMAND     :   if (LOWORD (wParam) == IDOK || LOWORD (wParam) == IDCANCEL)
                                {
                                    EndDialog (hDlg, LOWORD (wParam));
                                    return TRUE;
                                }
                                break;
    }
    return FALSE;
}

LRESULT CALLBACK Event (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG  :   return TRUE;

        case WM_COMMAND     :   if (LOWORD (wParam) == IDOK || LOWORD (wParam) == IDCANCEL)
                                {
                                    EndDialog (hDlg, LOWORD (wParam));
                                    return TRUE;
                                }
                                break;
    }
    return FALSE;
}

