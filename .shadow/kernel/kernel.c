#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

#define SIDE 16

static int w, h;  // Screen size
static int picW = 2, picH = 2; // Picture size
static int picture[] = {1, 2, 3, 4};

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

static void draw_tile(uint32_t *buf, int x, int y, int tileW, int tileH) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = tileW, .h = tileH, .sync = 1,
    .pixels = pixels,
  };
  for(int i = 0; i < tileH; i++) {
    for(int j = 0; j < tileW; j++) {
      pixels[i * tileW + j] = buf[(y + i) * w + (x + j)];
    }
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

static void scale(uint32_t *buf) {
  // size of buf is h * w
  for(int i = 0; i < h; i++) {
    for(int j = 0; j < w; j++) {
      buf[i * w + j] = picture[(picH / h) * i * picW + (picW / w) * j];
    }
  }
}

void splash() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;

  uint32_t buf[w * h];
  scale(buf);
  for (int x = 0; x * SIDE < w; x ++) {
    for (int y = 0; y * SIDE < h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_tile(buf, x * SIDE, y * SIDE, SIDE, SIDE); // white
      }
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
