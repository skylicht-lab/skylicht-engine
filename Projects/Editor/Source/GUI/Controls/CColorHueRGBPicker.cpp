/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CColorHueRGBPicker.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			void rgbToHSV(SGUIColor& c, unsigned char& h, unsigned char& s, unsigned char& v)
			{
				float fR = c.R / 255.0f;
				float fG = c.G / 255.0f;
				float fB = c.B / 255.0f;

				float fCMax = max(max(fR, fG), fB);
				float fCMin = min(min(fR, fG), fB);
				float fDelta = fCMax - fCMin;

				float fH = 0.0f;
				float fS = 0.0f;
				float fV = 0.0f;

				if (fDelta > 0)
				{
					if (fCMax == fR)
					{
						fH = 60.0f * (fmod(((fG - fB) / fDelta), 6.0f));
					}
					else if (fCMax == fG)
					{
						fH = 60.0f * (((fB - fR) / fDelta) + 2.0f);
					}
					else if (fCMax == fB) {
						fH = 60.0f * (((fR - fG) / fDelta) + 4.0f);
					}

					if (fCMax > 0.0f)
					{
						fS = fDelta / fCMax;
					}
					else
					{
						fS = 0.0f;
					}

					fV = fCMax;
				}
				else
				{
					fH = 0;
					fS = 0;
					fV = fCMax;
				}

				if (fH < 0)
				{
					fH = 360 + fH;
				}

				h = (unsigned char)(fH * 255.0f / 360.0f);
				s = (unsigned char)(fS * 255.0f);
				v = (unsigned char)(fV * 255.0f);
			}

			void hsvToRGB(unsigned char h, unsigned char s, unsigned char v, SGUIColor& c)
			{
				float fH = h * 360.0f / 255.0f;
				float fS = s / 255.0f;
				float fV = v / 255.0f;

				float fR = 0.0f;
				float fG = 0.0f;
				float fB = 0.0f;

				float fC = fV * fS; // Chroma
				float fHPrime = fmod(fH / 60.0f, 6.0f);
				float fX = fC * (1.0f - fabs(fmod(fHPrime, 2.0f) - 1.0f));
				float fM = fV - fC;

				if (0.0f <= fHPrime && fHPrime < 1.0f) {
					fR = fC;
					fG = fX;
					fB = 0;
				}
				else if (1.0f <= fHPrime && fHPrime < 2.0f) {
					fR = fX;
					fG = fC;
					fB = 0;
				}
				else if (2.0f <= fHPrime && fHPrime < 3.0f) {
					fR = 0.0f;
					fG = fC;
					fB = fX;
				}
				else if (3.0f <= fHPrime && fHPrime < 4.0f) {
					fR = 0.0f;
					fG = fX;
					fB = fC;
				}
				else if (4.0f <= fHPrime && fHPrime < 5.0f) {
					fR = fX;
					fG = 0;
					fB = fC;
				}
				else if (5.0f <= fHPrime && fHPrime < 6.0f) {
					fR = fC;
					fG = 0.0f;
					fB = fX;
				}
				else {
					fR = 0.0f;
					fG = 0.0f;
					fB = 0.0f;
				}

				fR += fM;
				fG += fM;
				fB += fM;

				c.R = (unsigned char)(fR * 255.0f);
				c.G = (unsigned char)(fG * 255.0f);
				c.B = (unsigned char)(fB * 255.0f);
				c.A = 255;
			}

			CColorHueRGBPicker::CColorHueRGBPicker(CBase* parent) :
				CBase(parent)
			{
				int s = 256;
				CRenderer* render = CRenderer::getRenderer();

				m_hsvImage = render->createImage(s, s, false);
				m_hsv = new CRawImage(this);
				m_hsv->setPos(15.0f, 10.0f);
				m_hsv->setSize((float)s, (float)s);
				m_hsv->setImage(m_hsvImage, SRect(0.0f, 0.0f, (float)s, (float)s));

				m_hueImage = render->createImage(32, s, false);
				setupHUEBitmap();

				m_hue = new CRawImage(this);
				m_hue->setPos(290.0f, 10.0f);
				m_hue->setSize(20.0f, (float)s);
				m_hue->setImage(m_hueImage, SRect(0.0f, 0.0f, 32.0f, (float)s));

				m_buttonOK = new CButton(this);
				m_buttonOK->setLabel(L"OK");
				m_buttonOK->setTextAlignment(ETextAlign::TextCenter);
				m_buttonOK->setBounds(210.0f, 275.0f, 100.0f, 20.0f);

				m_buttonCancel = new CButton(this);
				m_buttonCancel->setLabel(L"Cancel");
				m_buttonCancel->setTextAlignment(ETextAlign::TextCenter);
				m_buttonCancel->setBounds(210.0f, 300.0f, 100.0f, 20.0f);

				m_textboxHex = new CTextBox(this);
				m_textboxHex->setBounds(15.0f, 275.0f, 180.0f, 20.0f);

				m_textboxColor = new CTextBox(this);
				m_textboxColor->setBounds(15.0f, 300.0f, 180.0f, 20.0f);
			}

			CColorHueRGBPicker::~CColorHueRGBPicker()
			{
				CRenderer* render = CRenderer::getRenderer();
				render->removeImage(m_hsvImage);
				render->removeImage(m_hueImage);
			}

			void CColorHueRGBPicker::setColor(const SGUIColor& c)
			{
				m_color = c;

				setupHSVBitmap();

				updateColorText();
			}

			void CColorHueRGBPicker::updateColorText()
			{
				unsigned char r = m_color.R;
				unsigned char g = m_color.G;
				unsigned char b = m_color.B;
				unsigned char a = m_color.A;

				WCHAR text[512];

				swprintf(text, 512, L"%02X%02X%02X,%02X", r, g, b, a);
				m_textboxHex->setString(text);

				swprintf(text, 512, L"%d,%d,%d,%d", r, g, b, a);
				m_textboxColor->setString(text);
			}

			void CColorHueRGBPicker::setupHSVBitmap()
			{
				unsigned char h, s, v;
				rgbToHSV(m_color, h, s, v);

				s = 255;
				v = 255;

				SGUIColor color;
				hsvToRGB(h, s, v, color);

				float r = (float)(color.R / 255.0f);
				float g = (float)(color.G / 255.0f);
				float b = (float)(color.B / 255.0f);

				u32 width = m_hsvImage->getSize().Width;
				u32 height = m_hsvImage->getSize().Height;

				unsigned char* bitmapData = (unsigned char*)m_hsvImage->lock();
				unsigned char* p = bitmapData;

				float lx = 1.0f;
				float ly = 1.0f;
				float lz = 1.0f;

				float invH = 1.0f / (float)height;
				float invW = 1.0f / (float)width;

				float invR = r / (float)height;
				float invG = g / (float)height;
				float invB = b / (float)height;

				int p1 = 0;
				int p2 = 1;
				int p3 = 2;

				if (CRenderer::getRenderer()->useImageDataBGR())
				{
					p1 = 2;
					p3 = 0;
				}

				for (u32 i = 0; i < height; i++)
				{
					float x = lx;
					float y = ly;
					float z = lz;

					float fx = (r - lx) * invW;
					float fy = (g - ly) * invW;
					float fz = (b - lz) * invW;

					for (u32 j = 0; j < width; j++)
					{
						// RGB
						p[p1] = (unsigned char)(x * 255.0f);
						p[p2] = (unsigned char)(y * 255.0f);
						p[p3] = (unsigned char)(z * 255.0f);

						// alpha
						p[3] = 255;
						p += 4;

						x = lx + j * fx;
						y = ly + j * fy;
						z = lz + j * fz;
					}

					lx = 1.0f - i * invH;
					ly = 1.0f - i * invH;
					lz = 1.0f - i * invH;

					r = r - invR;
					g = g - invG;
					b = b - invB;
				}

				m_hsvImage->unlock();
			}

			void CColorHueRGBPicker::setupHUEBitmap()
			{
				u32 w = m_hueImage->getSize().Width;
				u32 h = m_hueImage->getSize().Height;

				unsigned char* bitmapData = (unsigned char*)m_hueImage->lock();
				unsigned char* p = bitmapData;

				SGUIColor colorTable[] =
				{
					SGUIColor(255,255,0,0),
					SGUIColor(255,255,0,255),
					SGUIColor(255,0,0,255),
					SGUIColor(255,0,255,255),
					SGUIColor(255,0,255,0),
					SGUIColor(255,255,255,0),
					SGUIColor(255,255,0,0)
				};

				float seg = h / 6.0f;

				int p1 = 0;
				int p2 = 1;
				int p3 = 2;

				if (CRenderer::getRenderer()->useImageDataBGR())
				{
					p1 = 2;
					p3 = 0;
				}

				for (u32 i = 0; i < h; i++)
				{
					int s = (int)(i / seg);

					float fr = colorTable[s].R;
					float fg = colorTable[s].G;
					float fb = colorTable[s].B;

					float tr = colorTable[s + 1].R;
					float tg = colorTable[s + 1].G;
					float tb = colorTable[s + 1].B;

					float f = (i - s * seg) / (float)seg;
					float invertf = 1.0f - f;

					float r = fr * invertf + tr * f;
					float g = fg * invertf + tg * f;
					float b = fb * invertf + tb * f;

					for (u32 j = 0; j < w; j++)
					{
						// RGB
						p[p1] = (unsigned char)r;
						p[p2] = (unsigned char)g;
						p[p3] = (unsigned char)b;

						// alpha
						p[3] = 255;
						p += 4;
					}
				}

				m_hueImage->unlock();
				m_hueImage->regenerateMipMapLevels();
			}
		}
	}
}