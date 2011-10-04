#include <GL/glew.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Boring, non-OpenGL-related utility functions
 */

void *file_contents(const char *filename, GLint *length)
{
    FILE *f = fopen(filename, "r");
    void *buffer;

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(*length+1);
    *length = fread(buffer, 1, *length, f);
    fclose(f);
    ((char*)buffer)[*length] = '\0';

    return buffer;
}

static short le_short(unsigned char *bytes)
{
    return bytes[0] | ((char)bytes[1] << 8);
}

void *read_tga(const char *filename, int *width, int *height)
{
    struct tga_header {
       char  id_length;
       char  color_map_type;
       char  data_type_code;
       unsigned char  color_map_origin[2];
       unsigned char  color_map_length[2];
       char  color_map_depth;
       unsigned char  x_origin[2];
       unsigned char  y_origin[2];
       unsigned char  width[2];
       unsigned char  height[2];
       char  bits_per_pixel;
       char  image_descriptor;
    } header;
    int i, color_map_size, pixels_size;
    FILE *f;
    size_t read;
    void *pixels;

    f = fopen(filename, "rb");

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    read = fread(&header, 1, sizeof(header), f);

    if (read != sizeof(header)) {
        fprintf(stderr, "%s has incomplete tga header\n", filename);
        fclose(f);
        return NULL;
    }
    if (header.data_type_code != 2) {
        fprintf(stderr, "%s is not an uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }
    if (header.bits_per_pixel != 24) {
        fprintf(stderr, "%s is not a 24-bit uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }

    for (i = 0; i < header.id_length; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete id string\n", filename);
            fclose(f);
            return NULL;
        }

    color_map_size = le_short(header.color_map_length) * (header.color_map_depth/8);
    for (i = 0; i < color_map_size; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete color map\n", filename);
            fclose(f);
            return NULL;
        }

    *width = le_short(header.width); *height = le_short(header.height);
    pixels_size = *width * *height * (header.bits_per_pixel/8);
    pixels = malloc(pixels_size);

    read = fread(pixels, 1, pixels_size, f);

    if (read != pixels_size) {
        fprintf(stderr, "%s has incomplete image\n", filename);
        fclose(f);
        free(pixels);
        return NULL;
    }

    //printf("header.bits_per_pixel is %d \n", header.bits_per_pixel);
    //printf("pixels[0] is %u \n", *((char *)pixels) + 0);
    //printf("pixels[1] is %u \n", *((char *)pixels) + 1);
    //printf("pixels[2] is %u \n", *((char *)pixels) + 2);
    //printf("pixels[3] is %u \n", *((char *)pixels) + 3);
    //printf("pixels[4] is %u \n", *((char *)pixels) + 4);
    //printf("pixels[5] is %u \n", *((char *)pixels) + 5);
    //printf("sizeof(pixels) is %lu \n", sizeof(pixels));
    //printf("pixels_size is %u \n", pixels_size);

    return pixels;
}

