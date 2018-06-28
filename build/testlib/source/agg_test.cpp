
#include <windows.h>
 
#include "agg_platform_support.h"
#include "agg_win32_bmp.h" 
 
 
#include "cairo.h"
 
 

 

class the_application : public  platform_support
{


public:
	the_application() 
	{

	}

	void on_init(HWND hwnd)
	{
		HDC hdc = GetDC(hwnd);
		m_pixel_map.create(800,600, pixel_map::org_color24,255);
		ReleaseDC(hwnd,hdc);	

		//������һ�� Cairo �����һ�� Cairo ������ 
		cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 800, 600);
		cairo_t *cr = cairo_create(surface);
		 cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
		/*
		//Բ�� 
		 cairo_arc (cr, 50, 300, 40, 0, 2 * 3.1415/4);
		 cairo_stroke_preserve ( cr) ;
		 cairo_stroke ( cr) ;
		  
		//ʵ�ľ���
		 cairo_set_source_rgb ( cr, 0.6 , 0.6 , 0.0 ) ;
		 cairo_rectangle ( cr, 150 , 20 , 100 , 100 ) ;
		 cairo_fill ( cr) ;

		 */
		//���ľ���
	    cairo_set_source_rgb (cr,  0.627, 0, 0);
		cairo_rectangle ( cr, 20 , 20 , 120 , 80 ) ; 
		//cairo_stroke ( cr) ;

		 //��ֱ�� 
		//cairo_set_source_rgb (cr,  0.627, 0, 0);
		cairo_move_to( cr, 40, 120) ;
		cairo_line_to( cr, 500 , 300);
		 
		cairo_stroke( cr) ;

		//��ͼ���ڴ���������
        m_pixel_map.copy_bits( cairo_image_surface_get_data(surface), false);

		//�������� Cairo �����������ռ�õ��ڴ���Դ��
		cairo_destroy(cr);
		cairo_surface_destroy(surface);
		
		 
	}

	void on_draw(HDC hdc)
	{
		 
		m_pixel_map.blend(hdc,0,0);
		
	}
 
 
	 

private:
	pixel_map  m_pixel_map;
	bool      m_flip_y;


};
 
//******************************************************************************
 


 
//*******************************************************************************
int agg_main()
{
	

 	 the_application app;
 	 app.caption("agg Test");
 
 	 if(app.init(850,650, 0))
 	 {
 		 app.run();
 	 }
	 return 0;
}



