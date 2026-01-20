#include <time.h>
#include <hpdf.h>

HPDF_Page add_page(HPDF_Doc*);
void set_font_and_size(HPDF_Doc*, HPDF_Font*,HPDF_Page*, const char*, int);
void write_to_pdf(HPDF_Doc*,HPDF_Page*, const char*, int, int);
void save_pdf(HPDF_Doc*, const char*);
void free_pdf(HPDF_Doc*);