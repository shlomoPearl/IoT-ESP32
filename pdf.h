#include <time.h>
#include <hpdf.h>

typedef struct PDF{
    HPDF_Doc doc;
    HPDF_Font font;
    HPDF_Page page;
    int y_pos;
    int width;
    int border;
} PDF;

void add_page(PDF*);
void set_font_and_size(PDF*, const char*, int);
void write_to_pdf(PDF*, const char*, int);
void save_pdf(PDF*, const char*);
void free_pdf(PDF*);