#include <cairo.h>
#include <stdio.h>
#include <string.h>

void replace_tabs_with_spaces(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '\t') {
            str[i] = ' ';
        }
    }
}
void replace_newline_with_space(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '\n') {
            str[i] = ' ';
        }
    }
}
int main(void) {
    cairo_surface_t *surface;
    cairo_t *cr;

    char *filename = "scheduler.log";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s", filename);
        return 1;
    }

    char line[128];
    int line_count = 0;
    while (fgets(line, sizeof(line), file)) {
        line_count++;
    }
    fclose(file);

    int width = 2000; // Adjust the width as needed
    int height = line_count * 80; // Adjust the height as needed
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create(surface);

    // Set the background color to white
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_select_font_face(cr, "Courier",
        CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 30);
    cairo_set_source_rgb(cr, 0, 0, 0);

    file = fopen(filename, "r");
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        replace_tabs_with_spaces(line); // Replace tabs with spaces
        replace_newline_with_space(line);// Replace new line character with a space at the end
        cairo_move_to(cr, 10, 50 + i * 80);
        cairo_show_text(cr, line);
        i++;
    }
    fclose(file);

    cairo_surface_write_to_png(surface, "output.png");

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return 0;
}
