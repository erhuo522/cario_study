
#include <windows.h>
 
#include "agg_platform_support.h"
#include "agg_win32_bmp.h" 
 
 
#include "cairo.h"
 
 
 static double fx_to_dbl(const FIXED& p)
{
        return double(p.value) + double(p.fract) * (1.0 / 65536.0)+100;
}

void Test_fill(cairo_t* cr)
{
 
	HFONT hFont = CreateFontA(300, 0, 0, 0, FW_THIN, false, false, false,CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_MODERN, "微软雅黑");

	HDC hDC = CreateDCW(L"DISPLAY", NULL, NULL, NULL);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	wchar_t chText[] = L"餐霞饮瀣";
	
	UINT fuFormat = GGO_NATIVE;

	MAT2 mat;
	memset(&mat, 0, sizeof(mat));
	mat.eM11.value = mat.eM22.value = 1; 

	for(int x=0; x<1;x++)
	{
		GLYPHMETRICS gm;
		memset(&gm, 0, sizeof(GLYPHMETRICS));

		UINT ch_code = (UINT)chText[x];

		DWORD dwNeedSize = GetGlyphOutlineW(hDC, ch_code, fuFormat, &gm, 0, NULL, &mat);
		if (dwNeedSize > 0 && dwNeedSize != GDI_ERROR)
		{
			LPBYTE lpBuf = new unsigned char[dwNeedSize];
			DWORD ret = GetGlyphOutlineW(hDC, ch_code, fuFormat, &gm, dwNeedSize, lpBuf, &mat);
			if (ret != GDI_ERROR)
			{
				unsigned char* Buffer = lpBuf;
				unsigned int   Count  = dwNeedSize;

				while (Count > 0)
				{
					//轮廓信息头
					TTPOLYGONHEADER* Header = (TTPOLYGONHEADER*)Buffer;

					//轮廓的起始数据地址
					LPBYTE Start = Buffer + sizeof(TTPOLYGONHEADER); 

					//---------------------------------------
					//闭合轮廓的起点 
					POINTFX StartPoint = Header->pfxStart;

					double s_x = fx_to_dbl(StartPoint.x);
					double s_y = fx_to_dbl(StartPoint.y);  

					cairo_move_to(cr, s_x, s_y);

					//--------------------------------------------
					//由于要升阶贝塞尔曲线，所需要记录EndPoint
					POINTFX EndPoint = StartPoint;

					DWORD Remain = Header->cb - sizeof(TTPOLYGONHEADER);
					while (Remain>0)
					{
						TTPOLYCURVE* Curve = (TTPOLYCURVE*)Start;
						switch (Curve->wType)
						{
						case TT_PRIM_LINE:   //折线
							{
								POINTFX* pPoints =  Curve->apfx;
								for(int i=0; i<Curve->cpfx; i++)
								{
									double x = fx_to_dbl(pPoints[i].x);
									double y = fx_to_dbl(pPoints[i].y);

									cairo_line_to(cr, x, y);
								}
							}
							break;
						case  TT_PRIM_QSPLINE: //二次贝塞尔曲线
							{						 
								POINTFX* pPoints =  Curve->apfx;

								double s_x0 = fx_to_dbl(EndPoint.x);
								double s_y0 = fx_to_dbl(EndPoint.y);  
								double s_x1 = fx_to_dbl(pPoints[0].x);
								double s_y1 = fx_to_dbl(pPoints[0].y);
								double s_x2 = fx_to_dbl(pPoints[1].x);
								double s_y2 = fx_to_dbl(pPoints[1].y);

								//-------------------------- 

								double x1 = 1.0/3.0*s_x0+2.0/3.0*s_x1;
								double y1 = 1.0/3.0*s_y0+2.0/3.0*s_y1;

								double x2 = 2.0/3.0*s_x1+1.0/3.0*s_x2;
								double y2 = 2.0/3.0*s_y1+1.0/3.0*s_y2;

								double x3 = s_x2;
								double y3 = s_y2;  

								cairo_curve_to(cr, x1, y1, x2, y2, x3, y3); 
							}
							break;
						case  TT_PRIM_CSPLINE : //三次贝塞尔曲线
							{
								POINTFX* pPoints =  Curve->apfx;

								double x1 = fx_to_dbl(pPoints[0].x);
								double y1 = fx_to_dbl(pPoints[0].y);
								double x2 = fx_to_dbl(pPoints[1].x);
								double y2 = fx_to_dbl(pPoints[1].y);
								double x3 = fx_to_dbl(pPoints[2].x);
								double y3 = fx_to_dbl(pPoints[2].y);

								cairo_curve_to(cr, x1, y1, x2, y2, x3, y3);
							}
							break;
						} 

						EndPoint = Curve->apfx[Curve->cpfx-1];

						Start += sizeof(TTPOLYCURVE) + (Curve->cpfx - 1)*sizeof(POINTFX);
						Remain -= sizeof(TTPOLYCURVE) + (Curve->cpfx - 1)*sizeof(POINTFX); 
					}

					//绘制闭合轮廓 
					cairo_close_path(cr);

					Buffer += Header->cb;
					Count -= Header->cb;
				}

				cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
				cairo_stroke_preserve(cr);

				cairo_set_source_rgba (cr, 0.4, 1.0, 0.0,0.4); 
				cairo_fill(cr);
			}
		}
	}
}

void Test(cairo_t* cr)
{

	/*
	HFONT CreateFont(
int cHeight, //字体的逻辑高度
int cWidth, //字体的逻辑宽度
int cEscapement, //指定移位向量相对X轴的偏转角度
int cOrientation, //指定字符基线相对X轴的偏转角度
int cWeight, //设置字体粗细程度
DWORD bItalic, //是否启用斜体
DWORD bUnderline, //是否启用下划线
DWORD bStrikeOut, //是否启用删除线
DWORD iCharSet, //指定字符集
DWORD iOutPrecision, //输出精度
DWORD iClipPrecision, //剪裁精度
DWORD iQuality, //输出质量
DWORD iPitchAndFamily, //字体族
LPCSTR pszFaceName //字体名
); 
	*/
	HFONT hFont = CreateFontA(184, 0, 0, 0, FW_THIN, false, false, false,CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_MODERN, "微软雅黑");

	HDC hDC = CreateDCW(L"DISPLAY", NULL, NULL, NULL);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	UINT chText = L'录';
	UINT fuFormat = GGO_NATIVE;

	GLYPHMETRICS gm;
	memset(&gm, 0, sizeof(GLYPHMETRICS));

	MAT2 mat;
	memset(&mat, 0, sizeof(mat));
	mat.eM11.value = mat.eM22.value = 1; 

	DWORD dwNeedSize = GetGlyphOutlineW(hDC, chText, fuFormat, &gm, 0, NULL, &mat);
	if (dwNeedSize > 0 && dwNeedSize != GDI_ERROR)
	{
		LPBYTE lpBuf = new unsigned char[dwNeedSize];
		DWORD ret = GetGlyphOutlineW(hDC, chText, fuFormat, &gm, dwNeedSize, lpBuf, &mat);
		if (ret != GDI_ERROR)
		{
			unsigned char* Buffer = lpBuf;
			unsigned int   Count  = dwNeedSize;

			while (Count > 0)
			{
				//轮廓信息头
				TTPOLYGONHEADER* Header = (TTPOLYGONHEADER*)Buffer;

				//轮廓的起始数据地址
				LPBYTE Start = Buffer + sizeof(TTPOLYGONHEADER); 

				//闭合轮廓的起点
				POINTFX InitPoint  = Header->pfxStart;    

				//每一条线的起点
				POINTFX StartPoint = Header->pfxStart;
				
				DWORD Remain = Header->cb - sizeof(TTPOLYGONHEADER);
				while (Remain>0)
				{
					TTPOLYCURVE* Curve = (TTPOLYCURVE*)Start;
					switch (Curve->wType)
					{
					case TT_PRIM_LINE:   //折线
						{
						printf("折线：%d", Curve->cpfx);

						double s_x = fx_to_dbl(StartPoint.x);
						double s_y = fx_to_dbl(StartPoint.y); 

						cairo_move_to(cr, s_x, s_y);

						POINTFX* pPoints =  Curve->apfx;
						for(int i=0; i<Curve->cpfx; i++)
						{
							 double x = fx_to_dbl(pPoints[i].x);
							 double y = fx_to_dbl(pPoints[i].y);

							 cairo_line_to(cr, x, y);
						}

						cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
						cairo_stroke(cr);

						StartPoint = pPoints[Curve->cpfx-1];
						}
						break;
					case  TT_PRIM_QSPLINE: //二次贝塞尔曲线
						{
						printf("二次贝塞尔：%d", Curve->cpfx);

						double s_x0 = fx_to_dbl(StartPoint.x);
						double s_y0 = fx_to_dbl(StartPoint.y); 

						POINTFX* pPoints =  Curve->apfx;

						double s_x1 = fx_to_dbl(pPoints[0].x);
						double s_y1 = fx_to_dbl(pPoints[0].y);
						double s_x2 = fx_to_dbl(pPoints[1].x);
						double s_y2 = fx_to_dbl(pPoints[1].y);

						//--------------------------

						double x0 = s_x0;
						double y0 = s_y0; 

						double x1 = 1.0/3.0*s_x0+2.0/3.0*s_x1;
						double y1 = 1.0/3.0*s_y0+2.0/3.0*s_y1;
							
						double x2 = 2.0/3.0*s_x1+1.0/3.0*s_x2;
						double y2 = 2.0/3.0*s_y1+1.0/3.0*s_y2;

						double x3 = s_x2;
						double y3 = s_y2; 

						cairo_move_to(cr, x0, y0);

						cairo_curve_to(cr, x1, y1, x2, y2, x3, y3); 
						cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
						cairo_stroke(cr);

						StartPoint = pPoints[Curve->cpfx-1];
					}
						break;
					case  TT_PRIM_CSPLINE : //三次贝塞尔曲线
						{
						printf("三次贝塞尔：%d", Curve->cpfx);

						double s_x = fx_to_dbl(StartPoint.x);
						double s_y = fx_to_dbl(StartPoint.y); 

						cairo_move_to(cr, s_x, s_y);

						POINTFX* pPoints =  Curve->apfx;

						double x1 = fx_to_dbl(pPoints[0].x);
						double y1 = fx_to_dbl(pPoints[0].y);
						double x2 = fx_to_dbl(pPoints[1].x);
						double y2 = fx_to_dbl(pPoints[1].y);
						double x3 = fx_to_dbl(pPoints[2].x);
						double y3 = fx_to_dbl(pPoints[2].y);

						cairo_curve_to(cr, x1, y1, x2, y2, x3, y3);
						cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
						cairo_stroke(cr);

						StartPoint = pPoints[Curve->cpfx-1];
						}
						break;
					} 
					
					Start += sizeof(TTPOLYCURVE) + (Curve->cpfx - 1)*sizeof(POINTFX);
					Remain -= sizeof(TTPOLYCURVE) + (Curve->cpfx - 1)*sizeof(POINTFX); 
				}

				//闭合每一个轮廓
				{
					double s_x = fx_to_dbl(StartPoint.x);
					double s_y = fx_to_dbl(StartPoint.y); 

					double e_x = fx_to_dbl(InitPoint.x);
					double e_y = fx_to_dbl(InitPoint.y); 

					cairo_move_to(cr, s_x, s_y);
					cairo_line_to(cr, e_x, e_y);

					cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
					cairo_stroke(cr);
				}

                Buffer += Header->cb;
                Count -= Header->cb;
			}
		}
	}
}



 

class the_application : public  platform_support
{


public:
	the_application() 
		:m_surface(NULL), m_cr(NULL)
	{
	 
	}

	void on_init(HWND hwnd)
	{

	}

	void on_resize(int sx, int sy)
	{
		m_sx = sx;
		m_sy = sy;

		if(m_cr)
		{	
			cairo_destroy(m_cr);
		}

		if(m_surface)
		{
			cairo_surface_destroy(m_surface);
		}

		//----------------------------

		m_pixel_map.create(m_sx, m_sy, pixel_map::org_color32, 255);

		unsigned char*  data   = m_pixel_map.buf();
	    int             stride = m_pixel_map.stride();
		
		m_surface = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_ARGB32, m_sx, m_sy, stride);
		m_cr = cairo_create(m_surface);
	}

	 
	void on_draw(HDC hdc)
	{
		//清屏
		 cairo_set_source_rgb (m_cr, 1.0, 1.0, 1.0);
		 cairo_paint(m_cr);
		
		 //Test(m_cr);
		 
		Test_fill(m_cr);
		 
		m_pixel_map.blend(hdc,0,0);
	}

	void on_destroy()
	{
		m_pixel_map.destroy();
	 
		if(m_cr)
		{	
			cairo_destroy(m_cr);
		}

		if(m_surface)
		{
			cairo_surface_destroy(m_surface);
		}
	}

private:
	pixel_map  m_pixel_map;
	bool       m_flip_y;

private:
	cairo_surface_t* m_surface;
	cairo_t*         m_cr;

	int        m_sx;
	int        m_sy;

};
 
//******************************************************************************
 


 
//*******************************************************************************
int agg_main()
{
 	 the_application app;
 	 app.caption("cairo Test");
 
 	 if(app.init(800,600, 0))
 	 {
 		 app.run();
 	 }
	 return 0;
}



