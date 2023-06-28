#include "algorithms.h"
#include <GL/glut.h>
#include <pthread.h>
#include <ft2build.h>
#include FT_FREETYPE_H


int window_width = 1920;
int window_height = 1080;
int vpadding = 150;
int rect_width = 5;
int space = 1;

struct Algo algos[1];
int selected_algo = 0;

struct AlgoArgs algo_args;

FT_Library ft_library;
FT_Face ft_face;

pthread_t thread_id;


void render_text(int x, int y, char* text) {
	for (const char *c = text; *c; c++) {

		// Get glyph index from character code
		FT_UInt glyph_index = FT_Get_Char_Index(ft_face, *c);

		if (glyph_index == 0) {
			fprintf(stderr, "Given character code has no glyph image in the face\n");
			exit(1);
		}

		// Load glyph image
		if (FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT)) {
			fprintf(stderr, "Failed to load glyph.\n");
			exit(1);
		}

		// Render glyph
		if (FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL)) {
			fprintf(stderr, "Failed to render glyph.\n");
			exit(1);
		}

		FT_GlyphSlot slot = ft_face->glyph;
		FT_Bitmap* glyph_bitmap = &slot->bitmap;

		// Flip the bitmap vertically
		unsigned char* flipped_bitmap = (unsigned char*)malloc(glyph_bitmap->width * glyph_bitmap->rows);

		for (int row = 0; row < glyph_bitmap->rows; row++) {
			unsigned char* src_row = glyph_bitmap->buffer + (row * glyph_bitmap->width);
			unsigned char* dest_row = flipped_bitmap + ((glyph_bitmap->rows - row - 1) * glyph_bitmap->width);
			memcpy(dest_row, src_row, glyph_bitmap->width);
		}

		glyph_bitmap->buffer = flipped_bitmap;

		// Calculate the adjusted y position based on the glyph's bearing
		int adjusted_y = y + (slot->bitmap_top - glyph_bitmap->rows);

		glRasterPos2f(x, adjusted_y);
		glDrawPixels(glyph_bitmap->width, glyph_bitmap->rows, GL_LUMINANCE, GL_UNSIGNED_BYTE, glyph_bitmap->buffer);

		x += slot->advance.x / 64;
	}
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);

	int x = 0;
	for (int i = 0; i < algo_args.arr_size; i++) {

		if (algo_args.arr[i].current) {
			glColor3f(1.0, 1.0, 1.0);
		} else {
			glColor3f(1.0, 0.7569, 0.0);
		}

		// Bottom left
		glVertex2f(x, vpadding);

		// Top left
		glVertex2f(x, vpadding + algo_args.arr[i].value);

		// Top right
		glVertex2f(x + rect_width, vpadding + algo_args.arr[i].value);

		// Bottom right
		glVertex2f(x + rect_width, vpadding);

		x += rect_width + space;

		algo_args.arr[i].current = false;
	}

	glEnd();

	// Render text
	char text[256];

	// Top: Column 1
	sprintf(text, "Algorithm: %s", algos[selected_algo].name);
	render_text(20, window_height - 50, text);

	sprintf(text, "Delay: %u microseconds", algo_args.delay);
	render_text(20, window_height - 80, text);

	// Top: Column 2
	sprintf(text, "Number of elements: %i", algo_args.arr_size);
	render_text(500, window_height - 50, text);

	sprintf(text, "Comparisons: %i", algo_args.comparisons);
	render_text(500, window_height - 80, text);

	// Bottom: Column 1
	render_text(20, vpadding - 50, "Press a or s to select an algorithm.");
	render_text(20, vpadding - 80, "Press u or d to modify speed.");
	render_text(20, vpadding - 110, "Press r to randomize the array.");

	// Bottom: Column 2
	render_text(800, vpadding - 50, "Press enter to run the algorithm.");
	render_text(800, vpadding - 80, "Press p to pause the algorithm.");

	glutSwapBuffers();
}


void idle() {
	glutPostRedisplay();	
}


void keyboard(unsigned char key, int x, int y) {

	// s: Next algorithm
	if (key == 115) {

	}

	// a: Previous algorithm
	if (key == 97) {

	}

	// r: Reset state
	if (key == 114) {
		randomize_array(algo_args.arr, algo_args.arr_size);
	}

	// u: Increase speed
	if (key == 117) {
		algo_args.delay += 10;
	}

	// d: reduce speed
	if (key == 100) {
		if (algo_args.delay > 10) {
			algo_args.delay -= 10;
		}
	}

	// enter: Run program
	if (key == 13) {
		pthread_create(&thread_id, NULL, algos[selected_algo].function, (void *)&algo_args);
	}

	// p: Pause program
	if (key == 112) {

	}
}


void setup_gl() {

	// Set background dark
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Set point color and size to 1 pixel
	glColor3f(1.0, 0.7569, 0.0);
	glPointSize(5.0);

	// Matrix projection and reset with identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/*
	 * Creates projection matrix
	 * x increases from left to right (0 to WINDOW_WIDTH)
	 * y increases from bottom to top (0 to WINDOW_HEIGHT)
	 */

	gluOrtho2D(0, window_width, 0, window_height);

	/*
	 * This fucking line... I spent a day rendering weird symbols
	 * because the padding that adds FreeType to each row of the bitmap
	 * does not match the padding expected by GL.
	 */

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}


void setup_freetype() {

	// Init library
	if (FT_Init_FreeType(&ft_library)) {
		fprintf(stderr, "Failed to initialize FreeType library\n");
		exit(1);
	}

	// Load font
	if (FT_New_Face(ft_library, "fonts/JetBrainsMono-Regular.ttf", 0, &ft_face)) {
		fprintf(stderr, "Failed to load font\n");
		exit(1);
	}

	// Set font size
	if (FT_Set_Pixel_Sizes(ft_face, 0, 24)) {
		fprintf(stderr, "Failed to set font size.\n");
		FT_Done_Face(ft_face);
		FT_Done_FreeType(ft_library);

		exit(1);
	}
}


int main(int argc, char** argv) {
	algo_args.comparisons = 0;
	algo_args.delay = 100;

	strcpy(algos[0].name, "Bubble sort");
	algos[0].function = bubble_sort;

	algo_args.arr_size = window_width / (rect_width + space);
	algo_args.arr = malloc(algo_args.arr_size * sizeof(struct Element));

	create_array(algo_args.arr, algo_args.arr_size, window_height, vpadding);
	randomize_array(algo_args.arr, algo_args.arr_size);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Algorithm animator");

	setup_gl();
	setup_freetype();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();

	free(algo_args.arr);

	FT_Done_Face(ft_face);
	FT_Done_FreeType(ft_library);

	return 0;
}
