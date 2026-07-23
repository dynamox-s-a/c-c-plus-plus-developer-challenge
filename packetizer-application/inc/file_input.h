#ifndef FILE_INPUT_H
#define FILE_INPUT_H

/*
 * file_input.h
 * ------------
 * Resolves the runtime input ambiguity required by the challenge
 * (technical requirement #4: the application must accept "a message
 * we type, a file path we pass"). Whatever line the user types at the
 * peer application's prompt is either literal text to send as-is, or
 * a path to a file whose contents should be sent instead.
 */

#include <stddef.h>

/// @brief Decides whether a line typed at runtime should be treated as
/// a file path rather than literal text to send.
///
/// Two checks must BOTH pass, on purpose:
///   1) Syntactic: the string ends in a "." followed by a short
///      alphanumeric extension (e.g. ".txt", ".bin", ".png").
///   2) Existence: stat() confirms a *regular, readable* file actually
///      exists at that path.
///
/// @param input Null-terminated line of input to classify.
/// @returns 1 if `input` should be treated as a file path, 0 otherwise
/// (treat it as literal text).
int input_is_file_path(const char * input);

/// @brief Reads the file at `path` and sends its contents through the
/// packetizer (packetizer_send_data()), one chunk at a time.
///
/// Each chunk is sent with its own call to packetizer_send_data(),
/// which (per its own stop-and-wait/ACK contract) only returns once
/// that chunk has been acknowledged or definitively failed; a failure
/// aborts the transfer immediately rather than sending a file the
/// receiver could never fully reconstruct anyway.
///
/// @param path Path to the file to read and send.
/// @returns 0 on success (the whole file was sent and acknowledged);
/// -1 on failure (message printed to stderr).
int input_send_file(const char * path);

#endif /* FILE_INPUT_H */