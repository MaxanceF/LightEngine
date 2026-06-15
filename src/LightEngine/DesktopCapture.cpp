#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef CreateWindow

#include "DesktopCapture.h"
#include <vector>
#include <algorithm>

sf::Image DesktopCapture::Capture()
{
	int W = GetSystemMetrics(SM_CXSCREEN);
	int H = GetSystemMetrics(SM_CYSCREEN);

	HDC     screenDC = GetDC(nullptr);
	HDC     memDC    = CreateCompatibleDC(screenDC);
	HBITMAP hBitmap  = CreateCompatibleBitmap(screenDC, W, H);
	SelectObject(memDC, hBitmap);
	BitBlt(memDC, 0, 0, W, H, screenDC, 0, 0, SRCCOPY);

	BITMAPINFOHEADER bi{};
	bi.biSize        = sizeof(BITMAPINFOHEADER);
	bi.biWidth       = W;
	bi.biHeight      = -H;
	bi.biPlanes      = 1;
	bi.biBitCount    = 32;
	bi.biCompression = BI_RGB;

	std::vector<sf::Uint8> pixels((size_t)W * H * 4);
	GetDIBits(memDC, hBitmap, 0, (UINT)H, pixels.data(),
			  (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteObject(hBitmap);
	DeleteDC(memDC);
	ReleaseDC(nullptr, screenDC);

	for (size_t i = 0; i < pixels.size(); i += 4)
	{
		std::swap(pixels[i], pixels[i + 2]);
		pixels[i + 3] = 255;
	}

	sf::Image img;
	img.create((unsigned int)W, (unsigned int)H, pixels.data());
	return img;
}

void DesktopCapture::MakeOverlay(sf::RenderWindow& overlay)
{
	HWND hwnd = (HWND)overlay.getSystemHandle();

	SetWindowLongPtr(hwnd, GWL_EXSTYLE,
		GetWindowLongPtr(hwnd, GWL_EXSTYLE)
		| WS_EX_LAYERED
		| WS_EX_TRANSPARENT
		| WS_EX_TOPMOST
	);

	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
				 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
}