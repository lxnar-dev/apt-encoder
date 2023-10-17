# Synopsis

   apt-encoder reads image files and turns them into sound data modulated as APT
   signals. APT is a way to send images as audio, and it is used by NOAA weather
   satellites. It is a simple format that a lot of people recognize by sound,
   and decoders for it are easily available. This makes it a good candidate for
   simple radio faxing.

# User manual


# *Building the software*

   In order to build the project, run `make`. It will produce a binary called
   'apt-encode'. You will need to have Make and a C++ compiler on your system.

# *Picture format*

   The program needs pictures encoded in the PGM format [1]. You can use an
   image editor, such as GIMP, in order to create these files. The format needed
   by the program is P2, also known as the 'Text' encoding of PGM.

   The images will need to have a width of 909 pixels, and they can have any
   height. The program can handle images of different heights. If two images
   with different heights are encoded, the shorter image will be padded with
   black pixels.

# *Configuration parameters*

   The program has a few knobs that can be tweaked. These are located in the
   "Constants and config" section of the C++ file. If you want the signal to be
   understood by existing decoders, you should only change the OVERSAMPLE
   option.

   CARRIER: This option determines the carrier frequency of the output. It is
   defined to be 2400 Hz by the APT specification.

   BAUD: Determines the speed of the transmission. The APT specification defines
   this to be 4160.

   OVERSAMPLE: How many audio samples to output for each word. The output sample
   rate is BAUD * OVERSAMPLE.

   SYNCA and SYNCB: Sync words that appear before image A and image B
   respectively.

# Usage examples

# *Encoding the same image for both sections*

   In order to provide some data redundancy, you can transmit the same picture
   for both images of the transmission. Just pass the same file to the program
   twice.

   ./apt-encode ~/image1.pgm ~/image2.pgm

# *Playing the output from the computer*

   You can play the output signal from the computer sound card / speakers.

   ./apt-encode ~/image1.pgm ~/image2.pgm | aplay -r 8320

   Replace 8320 with the actual sample rate based on the oversample value.

# *Creating WAV files*

   ./apt-encode ~/image1.pgm ~/image2.pgm > encode.raw
   sox -t raw -b 8 -e unsigned -c 1 -r 8320 encode.raw -r 11025 ~/apt.wav

   This creates a WAV file with a sample rate of 11025.
