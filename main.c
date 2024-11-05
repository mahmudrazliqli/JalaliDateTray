// main.c
#include <gtk/gtk.h>
#include <time.h>
#include <cairo.h>



int g_days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int j_days_in_month[12] = { 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29 };
void gregorian_to_jalali(int *j_y, int *j_m, int *j_d,int  g_y, int  g_m, int  g_d)
{
	int gy, gm, gd;
	int jy, jm, jd;
	long g_day_no, j_day_no;
	int j_np;

	int i;

	gy = g_y - 1600;
	gm = g_m - 1;
	gd = g_d - 1;

	g_day_no = 365 * gy + (gy + 3) / 4 - (gy + 99) / 100 + (gy + 399) / 400;
	for (i = 0; i < gm; ++i)
		g_day_no += g_days_in_month[i];
	if (gm > 1 && ((gy % 4 == 0 && gy % 100 != 0) || (gy % 400 == 0)))
		/* leap and after Feb */
		++g_day_no;
	g_day_no += gd;

	j_day_no = g_day_no - 79;

	j_np = j_day_no / 12053;
	j_day_no %= 12053;

	jy = 979 + 33 * j_np + 4 * (j_day_no / 1461);
	j_day_no %= 1461;

	if (j_day_no >= 366) {
		jy += (j_day_no - 1) / 365;
		j_day_no = (j_day_no - 1) % 365;
	}

	for (i = 0; i < 11 && j_day_no >= j_days_in_month[i]; ++i) {
		j_day_no -= j_days_in_month[i];
	}
	jm = i + 1;
	jd = j_day_no + 1;
	*j_y = jy;
	*j_m = jm;
	*j_d = jd;
}



static GtkStatusIcon *status_icon;
static GtkWidget *window;
static guint timeout_id;

// Function to create a pixbuf with white text
static GdkPixbuf* create_text_pixbuf(const char *text) {
    // Create a surface to draw on
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 22, 22);
    cairo_t *cr = cairo_create(surface);
    
    // Clear the surface with transparency
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Draw a circle
    //cairo_arc(cr, 0, 0, 10, 0, 2 * G_PI); // Circle
   // cairo_set_source_rgba(cr, 0.5, 1.0, 1.0, 0.5);
    //cairo_paint(cr);
    
    // Set up the text
    //cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    //cairo_select_font_face(cr, "Noto Sans Arabic", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_select_font_face(cr, "Vazir", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	
    cairo_set_font_size(cr, 20);
    
    // Set white color for text
    cairo_set_source_rgb(cr, 0, 0, 0);  // RGB values for white
    
    // Center the text
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    double x = (22 - extents.width) / 2 - extents.x_bearing;
    double y = (22 + extents.height) / 2;
    
    // Draw the text
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);
    
    // Convert surface to pixbuf
    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 22, 22);
    
    // Clean up
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    
    return pixbuf;
}


void replaceCharacter(char *str, char oldChar, const char *newChar) {
    char buffer[1000]; // Temporary buffer to hold the modified string
    int i, j = 0;

    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == oldChar) {
            // If the character matches, copy the new character
            for (int k = 0; newChar[k] != '\0'; k++) {
                buffer[j++] = newChar[k]; // Copy each byte of the new character
            }
        } else {
            buffer[j++] = str[i]; // Copy the original character
        }
    }
    buffer[j] = '\0'; // Null-terminate the new string
    strcpy(str, buffer); // Copy the modified string back to the original
}

static void update_icon_label() {
	time_t bin_time;
	struct tm *br_time;
	time(&bin_time);
	br_time = localtime(&bin_time);

	const char *days_of_week[] = {  "یکشنبه", "دوشنبه", "سه شنبه", "چهارشنبه", "پنجشنبه","جمعه","شنبه"};
	const char *jalali_months[] = {"فروردین", "اردیبهشت", "خرداد", "تیر", "مرداد", "شهریور", "مهر", "آبان", "آذر", "دی", "بهمن", "اسفند"};
	
	
	int jalali_sal, jalali_mah, jalali_ruz;
	gregorian_to_jalali(&jalali_sal, &jalali_mah, &jalali_ruz,1900 + br_time->tm_year,1 + br_time->tm_mon,br_time->tm_mday);

	char day_str[23];
	snprintf(day_str, 5, "%d", jalali_ruz);
	// 1234567890 to ۱۲۳۴۵۶۷۸۹۰
	const char *newChar = "۱";
	newChar = "۱";replaceCharacter(day_str,'1',newChar);
	newChar = "۲";replaceCharacter(day_str,'2',newChar);
	newChar = "۳";replaceCharacter(day_str,'3',newChar);
	newChar = "۴";replaceCharacter(day_str,'4',newChar);
	newChar = "۵";replaceCharacter(day_str,'5',newChar);
	newChar = "۶";replaceCharacter(day_str,'6',newChar);
	newChar = "۷";replaceCharacter(day_str,'7',newChar);
	newChar = "۸";replaceCharacter(day_str,'8',newChar);
	newChar = "۹";replaceCharacter(day_str,'9',newChar);
	newChar = "۰";replaceCharacter(day_str,'0',newChar);
	GdkPixbuf *pixbuf = create_text_pixbuf(day_str);
	gtk_status_icon_set_from_pixbuf(status_icon, pixbuf);

	char tooltip_str[256];
	snprintf(tooltip_str, 50,"%s %d/%d/%d \n   %d %s",days_of_week[br_time->tm_wday], jalali_sal, jalali_mah, jalali_ruz, jalali_ruz, jalali_months[jalali_mah]);	
	// 1234567890 to ۱۲۳۴۵۶۷۸۹۰
	newChar = "۱";replaceCharacter(tooltip_str,'1',newChar);
	newChar = "۲";replaceCharacter(tooltip_str,'2',newChar);
	newChar = "۳";replaceCharacter(tooltip_str,'3',newChar);
	newChar = "۴";replaceCharacter(tooltip_str,'4',newChar);
	newChar = "۵";replaceCharacter(tooltip_str,'5',newChar);
	newChar = "۶";replaceCharacter(tooltip_str,'6',newChar);
	newChar = "۷";replaceCharacter(tooltip_str,'7',newChar);
	newChar = "۸";replaceCharacter(tooltip_str,'8',newChar);
	newChar = "۹";replaceCharacter(tooltip_str,'9',newChar);
	newChar = "۰";replaceCharacter(tooltip_str,'0',newChar);
	gtk_status_icon_set_tooltip_text(status_icon, tooltip_str);
    
    	g_object_unref(pixbuf);
}

static void show_window(GtkMenuItem *item, gpointer user_data) {
    gtk_widget_show_all(window);
    gtk_window_present(GTK_WINDOW(window));
}

static void quit_app(GtkMenuItem *item, gpointer user_data) {
    gtk_main_quit();
}

static void popup_menu(GtkStatusIcon *status_icon, guint button,
                      guint activate_time, gpointer user_data) {
    GtkWidget *menu;
    GtkWidget *menu_item;

    menu = gtk_menu_new();

    // Show window menu item
    menu_item = gtk_menu_item_new_with_label("تاریخ جلالی");
  //  g_signal_connect(menu_item, "activate", G_CALLBACK(show_window), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // Separator
    menu_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // Quit menu item
    menu_item = gtk_menu_item_new_with_label("خروج");
    g_signal_connect(menu_item, "activate", G_CALLBACK(quit_app), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    gtk_widget_show_all(menu);
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, button, activate_time);
}

static gboolean update_timer(gpointer user_data) {
    update_icon_label();
    return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Day Display");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

    // Create status icon (initially empty)
    status_icon = gtk_status_icon_new();
    gtk_status_icon_set_visible(status_icon, TRUE);

    // Connect signals
    g_signal_connect(status_icon, "popup-menu", G_CALLBACK(popup_menu), NULL);

    // Set up timer for updates
    update_icon_label();  // Initial update
    timeout_id = g_timeout_add_seconds(60, update_timer, NULL);

    gtk_main();

    // Cleanup
    if (timeout_id > 0) {
        g_source_remove(timeout_id);
    }
    g_object_unref(status_icon);

    return 0;
}
