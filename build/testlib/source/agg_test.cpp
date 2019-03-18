
#include <windows.h>
 
#include "agg_platform_support.h"
#include "agg_win32_bmp.h" 
 
 
#include "cairo.h"
 
#define   M_PI   3.1415926
 

 

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

	void  draw_test(cairo_t* cr)
	{
		//圆形 
		 cairo_set_source_rgb (cr, 1.0, 0.0, 0.4); 
		 cairo_arc (cr, 390, 320, 40, 0, 2 * 3.1415);
		 cairo_stroke_preserve (cr);
		 cairo_set_source_rgb (cr, 0.3, 0.4, 0.6);
         cairo_fill(cr);

		//实心矩形
		 cairo_set_source_rgb (cr, 0.6 , 0.6 , 0.0 ) ;
		 cairo_rectangle (cr, 380 , 130 , 100 , 100 ) ;
		 cairo_fill(cr);
		 
		//空心矩形
	    cairo_set_source_rgb (cr,  0.627, 0, 0);
		cairo_rectangle (cr, 420 , 20 , 120 , 80 ) ; 
		cairo_stroke(cr) ;

		 //画直线 
		cairo_set_source_rgb (cr,  0.627, 0, 0);
		cairo_move_to(cr, 40, 120) ;
		cairo_line_to(cr, 500 , 300);
		cairo_stroke(cr) ;
	}

	void  draw_image(cairo_t* cr)
	{

		unsigned char* pixel = new  unsigned char[64*64*4];
		for(int i=0; i<64; i++)
		{
			for(int j=0; j<64; j++)
			{
				pixel[(i*64+j)*4+0]=255;
				pixel[(i*64+j)*4+1]=0;
				pixel[(i*64+j)*4+2]=0;
				pixel[(i*64+j)*4+3]=255;
			}
		}

		cairo_surface_t* image = cairo_image_surface_create_for_data(pixel, CAIRO_FORMAT_ARGB32, 64, 64, 64*4);

		int w = cairo_image_surface_get_width(image);
		int h = cairo_image_surface_get_height(image);

		cairo_translate(cr,600,200);
		cairo_rotate(cr,45*M_PI/180.0);

		 cairo_set_source_surface (cr, image, -w*0.5, -h*0.5);
		 cairo_paint(cr);

		 cairo_identity_matrix(cr);

	}
	void draw_gradient1(cairo_t *cr)
	{
		cairo_pattern_t *pat1 = cairo_pattern_create_linear(0.0, 0.0,  350.0, 350.0);

		int count = 1;
		for (double j = 0.1; j < 1; j += 0.1 )
		{
			if (( count % 2 )) 
			{
				cairo_pattern_add_color_stop_rgb(pat1, j, 0, 0, 0);
			}
			else 
			{ 
				cairo_pattern_add_color_stop_rgb(pat1, j, 1, 0, 0);
			}
			count++;
		}

		cairo_rectangle(cr, 20, 20, 300, 100);
		cairo_set_source(cr, pat1);
		cairo_fill(cr);  

		cairo_pattern_destroy(pat1);
	}

	void draw_gradient2(cairo_t *cr)
	{
		cairo_pattern_t *pat2 = cairo_pattern_create_linear(0.0, 0.0,  350.0, 0.0);

		int count = 1;
		for (double i = 0.05; i < 0.95; i += 0.025 ) {
			if (( count % 2 ))  {
				cairo_pattern_add_color_stop_rgb(pat2, i, 0, 0, 0);
			} else { 
				cairo_pattern_add_color_stop_rgb(pat2, i, 0, 0, 1);
			}
			count++;
		}

		cairo_rectangle(cr, 20, 140, 300, 100);
		cairo_set_source(cr, pat2);
		cairo_fill(cr);  

		cairo_pattern_destroy(pat2);
	}

	void draw_gradient3(cairo_t *cr)
	{
		cairo_pattern_t *pat3 = cairo_pattern_create_linear(20.0, 260.0, 20.0, 360.0);

		cairo_pattern_add_color_stop_rgb(pat3, 0.1, 0, 0, 0);
		cairo_pattern_add_color_stop_rgb(pat3, 0.5, 1, 1, 0);
		cairo_pattern_add_color_stop_rgb(pat3, 0.9, 0, 0, 0);

		cairo_rectangle(cr, 20, 260, 300, 100);
		cairo_set_source(cr, pat3);
		cairo_fill(cr);  

		cairo_pattern_destroy(pat3);
	}

	void on_draw(HDC hdc)
	{
		//清屏
		 cairo_set_source_rgb (m_cr, 1.0, 1.0, 1.0);
		 cairo_paint(m_cr);
		
		 //cairo_set_antialias(m_cr,CAIRO_ANTIALIAS_SUBPIXEL);
		 
		draw_test(m_cr);
		draw_image(m_cr);

		draw_gradient1(m_cr);
		draw_gradient2(m_cr);
		draw_gradient3(m_cr);  
		 
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



