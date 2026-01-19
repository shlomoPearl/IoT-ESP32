#include <time.h>
#include <hpdf.h>

#define TITLE "EVENT REPORT"
#define T_FONT "Helvetica-Bold"
#define T_SIZE 24
#define B_FONT "Helvetica"
#define B_SIZE 14

typedef struct {
    char* t_font;
    char* t_size;
    char* b_font;
    char* b_size;
    char* title;
    char* dt_gen;

} Report_PDF;

HPDF_Page get_pdf_report();
void write_event(HPDF_Page, const char*);
void save(HPDF_Doc, const char*);
void free_pdf(HPDF_Doc);

