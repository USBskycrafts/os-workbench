#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>


static int w, h;  // Screen size
static int p_w = 2, p_h = 2;  // Picture size

static uint32_t picture[4] = {3, 5, 6, 9};

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

void scale(uint32_t* pixels) {
  // pixels is a w * h array that matches screen
  for(int i = 0; i < h; i++) {
    for(int j = 0; j < w; j++) {
      volatile int index = (p_h / h) * i * p_w + (p_w / w) * j;
      assert(index < p_h * p_w);
      pixels[i * w + j] = picture[index];
    }
  }
}

static void draw_picture(uint32_t* pixels, int w, int h) {
  AM_GPU_FBDRAW_T event = {
    .x = 0, .y = 0, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;

  uint32_t pixels[w * h];
  scale(pixels);
  draw_picture(pixels, w, h);
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
