// simplewin32.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include    "simplewin32.h"
#define     MAX_LOADSTRING  100

// Global Variables:
HINSTANCE   hInst;                          // current instance
char        szTitle [MAX_LOADSTRING];       // The title bar text
char        szWindowClass [MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM                    RegisterMainClass (HINSTANCE hInstance);
BOOL                    InitInstance (HINSTANCE, int);
LRESULT CALLBACK        WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK        About (HWND, UINT, WPARAM, LPARAM);

HWND    hWndText;
char    szText [241];
unsigned char	*image	= NULL;			// image array
unsigned char	*grey1	= NULL;
long			 bpp, cx = 0, cy = 0;	// image dimension
BITMAPINFO		 bi;

int APIENTRY _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPTSTR lpCmdLine,
                        int nCmdShow)
{
    MSG     msg;
    HACCEL  hAccelTable;

    // Initialize global strings
    strcpy (szTitle, "Computer Vision Programming Ex. 1");
    strcpy (szWindowClass, "simplewin32");
    RegisterMainClass (hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) 
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
    wcex.lpszMenuName       = (LPCTSTR) IDC_SIMPLEWIN32;
    wcex.lpszClassName      = szWindowClass;
    wcex.hIconSm            = LoadIcon (wcex.hInstance, (LPCTSTR) IDI_SMALL);

    return RegisterClassEx (&wcex);
}

BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst   = hInstance; // Store instance handle in our global variable
    hWnd    = CreateWindow (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                            0, 0, 640, 480, NULL, NULL, hInstance, NULL);

//    hWndText = CreateWindow ("STATIC", "0", WS_CHILD | WS_BORDER | WS_VISIBLE | SS_RIGHT,
//               10, 10, 155, 20, hWnd, NULL, hInstance, NULL);
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

void process (unsigned char *ig1, long cx, long cy)
{
	unsigned char	*ig2;
	long			 x, y, m, n;
	double			 h [3][3] = {{2,  2, 2},
								 {2,  2, 2},
								 {2,  2, 2}};
	double			 sum, nrm;

	for (y = 0; y < cy; y ++)
	{
		for (x = 0; x < cx; x ++)
		{
			if (x > 20 && x < 100)
				ig1 [x + y*cx] = 0;
		}
	}

/*
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

	free (ig2);
*/
}

void loadfile (char *lpszpath)
{
	FILE				*fp;
	BITMAPFILEHEADER	 bf;
	long				 line, x, b, g, r;

	fp = fopen (lpszpath, "rb");
	fread (&bf, 1, sizeof (BITMAPFILEHEADER), fp);
	fread (&bi, 1, sizeof (BITMAPINFO), fp);

	cx		= bi.bmiHeader.biWidth;
	cy		= bi.bmiHeader.biHeight;
	bpp		= bi.bmiHeader.biBitCount/8;
	image	= (unsigned char *) malloc (cx*cy*bpp);
	grey1	= (unsigned char *) malloc (cx*cy);

	for (line = 0; line < cy; line ++) {
		fread (image + line*bpp*cx, 1, bpp*cx, fp);

		for (x = 0; x < cx; x ++)
		{
			b = image [bpp*(line*cx + x) + 0];
			g = image [bpp*(line*cx + x) + 1];
			r = image [bpp*(line*cx + x) + 2];

			grey1 [x + line*cx] = (int) (0.5*r+0.2*g+0.3*b);
		}
	}

	process (grey1, cx, cy);

	for (line = 0; line < cy; line ++)
	{
		for (x = 0; x < cx; x ++)
		{
			image [bpp*(line*cx + x) + 0] = grey1 [x + line*cx];
			image [bpp*(line*cx + x) + 1] = grey1 [x + line*cx];
			image [bpp*(line*cx + x) + 2] = grey1 [x + line*cx];
		}
	}

	fclose (fp);
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
LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int         wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC         hdc;

    switch (message) 
    {
        case WM_COMMAND :   wmId    = LOWORD (wParam);
                            wmEvent = HIWORD (wParam); 

                            // Parse the menu selections:
                            switch (wmId)
                            {
                                case IDM_ABOUT  :   DialogBox (hInst, (LPCTSTR) IDD_ABOUTBOX, hWnd, (DLGPROC) About);
                                                    break;

								case IDM_EXIT   :   loadfile ("c:\\home\\document\\423430\\data\\lena.bmp");
													hdc = GetDC (hWnd);
													mydraw (hdc);
													ReleaseDC (hWnd, hdc);
													break;
                                                       
                                default         :   
                                                    return DefWindowProc(hWnd, message, wParam, lParam);
                            }
                            break;

        case WM_PAINT   :   hdc = BeginPaint (hWnd, &ps);
                            mydraw (hdc);
                            EndPaint (hWnd, &ps);
                            break;

        case WM_DESTROY :   PostQuitMessage (0);
                            break;

        default         :   return DefWindowProc (hWnd, message, wParam, lParam);
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

