#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

#define SIDE 16
#define picW 640
#define picH 400 // Picture size

static int w, h;  // Screen size

static uint32_t picture[picW * picW];

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = { AM_KEYS(KEYNAME) };

static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);

  if(event.keycode == AM_KEY_ESCAPE && event.keydown) {
    halt(0);
  }

  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}

static void get_point(int *picX, int *picY, int pixelX, int pixelY) {
  *picX = picW / w * pixelX;
  *picY = picH / h * pixelY;
}

static void draw_tile(int x, int y, int tileW, int tileH) {
  uint32_t pixels[tileW * tileW]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = tileW, .h = tileH, .sync = 1,
    .pixels = pixels,
  };
  for(int i = 0; i < tileH; i++) {
    for(int j = 0; j < tileW; j++) {
      int picX, picY;
      get_point(&picX, &picY, x + j, y + i);
      pixels[i * tileW + j] = picture[picX + picY * w];
    }
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;

  for (int x = 0; x * SIDE < w; x ++) {
    for (int y = 0; y * SIDE < h; y++) {
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE);
    }
  }
}

void  unittest() {
  const char* s = "Hello World\n";
  assert(strcmp(s, "Hello") > 0);
  assert(strncmp(s, "Hello", 3) == 0);
  assert(strncmp(s, "Hello", 8) > 0);
  halt(0);
}


// Operating system is a C program!
int main(const char *args) {
  ioe_init();

  puts("mainargs = \"");
  puts(args);  // make run mainargs=xxx
  puts("\"\n");

  #ifdef DEBUG
  //unittest();
  #endif

  splash();

  puts("Press any key to see its key code...\n");
  while (1) {
    print_key();
  }
  return 0;
}
