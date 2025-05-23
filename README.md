# stegano-plus-plus
Hide a secret message in an image

~~~~~~~~~~
Supported extensions: bmp, ppm (P3 type)
Available options:
  -i/--info     : Checks if a file specified in the <path> can be modified.
                  Usage: -i <path>
  -e/--encrypt  : Opens the <path> and creates a mirror file with the <message> embedded.
                  Usage: -e <path> <message>
                  Note: <message> should be surrounded by double quotes.
  -d/--decrypt  : Reads an encrypted message in a file.
                  Only decrypt files that were encrypted with this program.
                  Usage: -d <path>
  -c/--check    : Checks if <message> can be written to a file specified by <path>.
                  Usage: -c <path> <message>
  -h/--help     : Prints this help message.

Example Usage:
  ./bin/stegopp -i there/here/picture.ppm -e here/there/image.ppm "Behind you"

Notes:
  - <path> cannot exceed 70 characters.
  - Maximum 12 arguments.
  - "-e" produces a new file, so using -e and -d on the same file simultaneously will produce gibberish.
  - Only one encryption or decryption per execute (i.e., cannot use -e twice).
  - To encrypt ppm P6 files, you can use an online converter to turn them into P3s.
~~~~~~~~~~

```bash
nix develop
make
make run ARGS="-h" # ./bin/stegopp -h
```
or
```bash
nix build
result/bin/stegopp -h
```
