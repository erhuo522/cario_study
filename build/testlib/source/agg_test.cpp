
#include <windows.h>
 
#include "agg_platform_support.h"
#include "agg_win32_bmp.h" 
 
 
#include "cairo.h"
 
 

 

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

		m_pixel_map.create(m_sx, m_sy, pixel_map::org_color24, 255);
	 
		if(m_cr)
		{	
			cairo_destroy(m_cr);
		}

		if(m_surface)
		{
			cairo_surface_destroy(m_surface);
		}
		
		m_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, m_sx, m_sy);
		m_cr = cairo_create(m_surface);
	}

	void on_draw(HDC hdc)
	{
		//清屏
		 cairo_set_source_rgb (m_cr, 1.0, 1.0, 1.0);
		 cairo_paint(m_cr);
		
		 //cairo_set_antialias(m_cr,CAIRO_ANTIALIAS_SUBPIXEL);
		 
		//圆形 
		 cairo_set_source_rgb (m_cr, 1.0, 0.0, 0.4); 
		 cairo_arc (m_cr, 350, 300, 40, 0, 2 * 3.1415);
		 cairo_stroke_preserve (m_cr);
		 cairo_set_source_rgb (m_cr, 0.3, 0.4, 0.6);
         cairo_fill(m_cr);

		//实心矩形
		 cairo_set_source_rgb (m_cr, 0.6 , 0.6 , 0.0 ) ;
		 cairo_rectangle (m_cr, 250 , 80 , 100 , 100 ) ;
		 cairo_fill (m_cr) ;

		 
		//空心矩形
	    cairo_set_source_rgb (m_cr,  0.627, 0, 0);
		cairo_rectangle (m_cr, 20 , 20 , 120 , 80 ) ; 
		cairo_stroke ( m_cr) ;

		 //画直线 
		cairo_set_source_rgb (m_cr,  0.627, 0, 0);
		cairo_move_to(m_cr, 40, 120) ;
		cairo_line_to(m_cr, 500 , 300);
		cairo_stroke(m_cr) ;

		//把图像内存数据拿走
        m_pixel_map.copy_bits( cairo_image_surface_get_data(m_surface), false);

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



