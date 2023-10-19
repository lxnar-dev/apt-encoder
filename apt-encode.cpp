#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Utils
template <typename F, typename T>
constexpr T map(F value, F f1, F t1, T f2, T t2) {
  return f2 + ((t2 - f2) * (value - f1)) / (t1 - f1);
}
template <typename T> constexpr T max(T val1, T val2) {
  return val1 > val2 ? val1 : val2;
}
// Constants and config

constexpr size_t CARRIER = 2400;
constexpr size_t BAUD = 4160;
constexpr size_t OVERSAMPLE = 3;

constexpr const char *SYNCA = "000011001100110011001100110011000000000";
constexpr const char *SYNCB = "000011100111001110011100111001110011100";
// Image
class Image {
public:
  Image(const char *);
  void free();
  uint8_t getPixel(size_t, size_t) const;
  size_t width() const;
  size_t height() const;
private:
  size_t m_height;
  uint8_t *m_pixels;
};
Image::Image(const char *path) {
  FILE *f = fopen(path, "r");
  size_t maxValue;
  size_t width;
  // P2
  {
    char buf[2];
    fread(buf, 1, 2, f);
  }
  fscanf(f, "%zu %zu %zu", &width, &m_height, &maxValue);
  m_pixels = (uint8_t *)malloc(width * m_height);
  for (size_t i = 0; i < m_height * width; i++) {
    fscanf(f, "%hhu", &m_pixels[i]);
  }
  fclose(f);
}
void Image::free() { std::free(m_pixels); }
size_t Image::width() const { return 909; }
size_t Image::height() const { return m_height; }
uint8_t Image::getPixel(size_t x, size_t y) const {
  return m_pixels[y * width() + x];
}
// Audio
void write_value(uint8_t value) {
  static double sn = 0;
  for (size_t i = 0; i < OVERSAMPLE; i++) {
    double samp = sin(CARRIER * 2.0 * M_PI * (sn / (BAUD * OVERSAMPLE)));
    samp *= map((int)value, 0, 255, 0.0, 0.7);
    uint8_t buf[1];
    buf[0] = map(samp, -1.0, 1.0, 0, 255);
    fwrite(buf, 1, 1, stdout);
    sn++;
  }
}

int main(int argc, char **argv) {
  // TODO: Improve command line argument parsing

  if (argc < 2) {
    printf("Usage: %s ./image1.pgm ./image2.pgm\n", argv[0]);
    return 1;
  }

  Image img1(argv[1]);
  Image img2(argv[argc < 3 ? 1 : 2]);

  auto height = max(img1.height(), img2.height());
  for (size_t line = 0; line < height; line++) {
    auto frame_line = line % 128;
    // Sync A
    for (size_t i = 0; i < strlen(SYNCA); i++)
      write_value(SYNCA[i] == '0' ? 0 : 255);
    // Space A
    for (size_t i = 0; i < 47; i++)
      write_value(0);
    // Image A
    for (size_t i = 0; i < 909; i++) {
      if (line < img1.height())
        write_value(img1.getPixel(i, line));
      else
        write_value(0);
    }
    // Telemetry A
    for (size_t i = 0; i < 45; i++) {
            size_t wedge = frame_line / 8;
      auto v = 0;
      if (wedge < 8) {
        wedge++;
        v = (int)(255.0 * (wedge % 8 / 8.0));
      }
      write_value(v);
    }

    // Sync B
    for (size_t i = 0; i < strlen(SYNCB); i++)
      write_value(SYNCB[i] == '0' ? 0 : 255);

    // Space B
    for (size_t i = 0; i < 47; i++)
      write_value(255);

    // Image B
    for (size_t i = 0; i < img2.width(); i++) {
      if (line < img2.height())
        write_value(img2.getPixel(i, line));
      else
        write_value(0);
    }

    // Telemetry B
    for (size_t i = 0; i < 45; i++) {
      size_t wedge = frame_line / 8;
      auto v = 0;
      if (wedge < 8) {
        wedge++;
        v = (int)(255.0 * (wedge % 8 / 8.0));
      }
      write_value(v);
    }
  }

  img1.free();
  img2.free();
  return 0;
}
