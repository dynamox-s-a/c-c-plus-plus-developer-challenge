#include "file_input.h"
#include "packetizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/stat.h>

/*
 * Longest extension we still consider plausible (e.g. "jpeg", "tar",
 * "sqlite3"). Anything longer is more likely a sentence that happens
 * to contain a dot than an actual filename.
 */
#define MAX_PLAUSIBLE_EXTENSION_LEN 10

int input_is_file_path(const char * input)
{
    if (input == NULL || input[0] == '\0')
    {
        return 0;
    }

    /* --- Syntactic check: "<something>.<short alphanumeric ext>" --- */
    const char * dot = strrchr(input, '.');
    if (dot == NULL || dot == input)
    {
        /* No dot at all, or the dot is the very first character (e.g.
         * "." or "..") -- not a plausible "name.ext" shape. */
        return 0;
    }

    size_t ext_len = strlen(dot + 1);
    if (ext_len == 0 || ext_len > MAX_PLAUSIBLE_EXTENSION_LEN)
    {
        return 0;
    }

    for (size_t i = 0; i < ext_len; i++)
    {
        if (!isalnum((unsigned char)dot[1 + i]))
        {
            return 0;
        }
    }

    /* --- Existence check: does a real, regular file live there? --- */
    struct stat st;
    if (stat(input, &st) != 0)
    {
        /* Looks like a filename, but nothing readable actually exists
         * at that path -- treat it as literal text instead of failing
         * outright, so a message that happens to look file-ish is
         * still sendable. */
        return 0;
    }

    return S_ISREG(st.st_mode) ? 1 : 0;
}

int input_send_file(const char * path)
{
    FILE * file = fopen(path, "rb");
    if (file == NULL)
    {
        perror("input_send_file: fopen");
        return -1;
    }

    struct stat st;
    long file_size = (stat(path, &st) == 0) ? (long)st.st_size : -1;

    //Used a static buffer of 16Kb in order to send large chunks of data through the packetizer.
    //If a file over 16Kb is read, the file will be split in smaller packets. That's not a packetizer
    //flaw, but a process that should be decided the application.
    uint16_t chunk_size = 16 * 1024;
    uint8_t chunk[16 * 1024] = {0};


    printf("Sending file '%s'", path);
    if (file_size >= 0)
    {
        printf(" (%ld bytes)", file_size);
    }
    printf(" in chunks of up to %u bytes...\n", chunk_size);

    long total_sent = 0;
    size_t bytes_read;

    while ((bytes_read = fread(chunk, 1, chunk_size, file)) > 0)
    {
        int rc = packetizer_send_data(chunk, (uint32_t)bytes_read);
        if (rc != 0)
        {
            fprintf(stderr,
                    "\nFailed to send file chunk at offset %ld (error %d); aborting transfer.\n",
                    total_sent, rc);
            fclose(file);
            return -1;
        }

        total_sent += (long)bytes_read;

        if (file_size > 0)
        {
            printf("\rSent %ld / %ld bytes", total_sent, file_size);
        }
        else
        {
            printf("\rSent %ld bytes", total_sent);
        }
        fflush(stdout);
    }

    int had_read_error = ferror(file);

    fclose(file);

    if (had_read_error)
    {
        fprintf(stderr, "\nError reading '%s' while sending.\n", path);
        return -1;
    }

    printf("\nFile transfer of '%s' complete (%ld bytes).\n", path, total_sent);
    return 0;
}