
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

	 
	void test_bentley_ottmann_tessellate_polygon(cairo_t *cr)
	{
		cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING); 

		cairo_set_line_width(cr, 2.5); 

		cairo_move_to(cr, 50, 50);
		cairo_line_to(cr, 500, 150);
		cairo_line_to(cr, 500, 500);
		cairo_line_to(cr, 50, 500);
		cairo_close_path(cr);

		cairo_move_to(cr, 150, 150);
		cairo_line_to(cr, 150, 400);
		cairo_line_to(cr, 400, 400);
		cairo_line_to(cr, 400, 200);
		cairo_close_path(cr);

		double x1,y1,x2,y2;
		cairo_fill_extents(cr, &x1, &y1, &x2, &y2);
	 
		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
		cairo_stroke_preserve(cr);

		cairo_set_source_rgba(cr, 0.0, 1.0, 0.0,0.3);
		cairo_fill(cr);
	}

	void on_draw(HDC hdc)
	{
		//清屏
		cairo_set_source_rgb (m_cr, 1.0, 1.0, 1.0);
		cairo_paint(m_cr);
		 
		test_bentley_ottmann_tessellate_polygon(m_cr);

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



