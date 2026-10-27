#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2016-2021, Linaro Limited

from PIL import ImageDraw
from PIL import ImageFont
from PIL import Image
import sys


def get_args():
    from argparse import ArgumentParser

    parser = ArgumentParser()
    parser.add_argument('--font_file', required=True,
                        help='Name of font file')
    parser.add_argument('--font_size', type=int, default=40,
                        help='Size of font')
    parser.add_argument('--font_name', required=True,
                        help='Name of font in program')
    parser.add_argument('--out_dir', required=True,
                        help='Out directory')
    parser.add_argument('--verbose', default=False,
                        help='Print informational messages')
    return parser.parse_args()


def c_hex(bstr):
    s = []
    for n, x in enumerate(bstr):
        if n % 8 == 0:
            if n == 0:
                s.append("\t")
            else:
                s.append("\n\t")
        else:
            s.append(" ")

        s.append("0x{:02x},".format(x))

    return "".join(s)


header_template = """/*
 * This file is auto generated with
 * {cmd_line}
 * do not edit.
 */
"""

h_file_template = """#ifndef __{FONT_NAME}_H
#define __{FONT_NAME}_H

#include "font.h"

extern const struct font font_{font_name};

#endif /*__{FONT_NAME}_H*/
"""

letter_template = """/* {letter} */
static const unsigned char letter_{id}[] = {{
{c_hex}
}};

"""

font_letter_template = """\t{{ letter_{id}, sizeof(letter_{id}), {width}}},
"""

font_template = """const struct font font_{name} = {{
    .first = 0x{first:02x},
    .last = 0x{last:02x},
    .letters = letters,
    .height = {height},
    .max_width = {max_width},
}};
"""


def main():
    args = get_args()

    font_name = args.font_name
    out_dir = args.out_dir

    range_first = 0x20
    range_last = 0x7d
    c_letters = {}

    letters = range(range_first, range_last + 1)

    font = ImageFont.truetype(args.font_file, args.font_size)
    ascent, descent = font.getmetrics()
    text_height = font.font.height + 2

    for x in letters:
        letter = chr(x)
        (width, height), (offset_x, offset_y) = font.font.getsize(letter)
        text_width = width + 2 + offset_x
        letter_img = Image.new("1", (text_width, text_height), 0)
        draw = ImageDraw.Draw(letter_img)
        draw.fontmode = "1"
        draw.text((0, 0), letter, font=font, fill=1)
        c_letters[x] = {'img': letter_img.tobytes(),
                        'width': text_width}

    if args.verbose:
        print("Writing " + out_dir + "/" + font_name + ".c")

    with open(out_dir + "/" + font_name + ".c", 'w+') as f:
        f.write(header_template.format(cmd_line=' '.join(sys.argv)))
        f.write('#include "font.h"\n\n')
        for x in letters:
            f.write(letter_template.format(letter=chr(x),
                                           id="{:02x}".format(x),
                    c_hex=c_hex(c_letters[x]['img'])))

        f.write("static const struct font_letter letters[] = {\n")
        for x in letters:
            f.write(font_letter_template.format(id="{:02x}".format(x),
                    width=c_letters[x]['width']))
        f.write("};\n\n")

        f.write(font_template.format(name=args.font_name,
                first=letters[0],
                last=letters[-1],
                height=text_height,
                max_width=max(i['width'] for i in c_letters.values())))

    if args.verbose:
        print("Writing " + out_dir + "/" + font_name + ".h")

    with open(out_dir + "/" + font_name + ".h", 'w+') as f:
        f.write(header_template.format(cmd_line=' '.join(sys.argv)))
        f.write(h_file_template.format(font_name=font_name,
                                       FONT_NAME=font_name.upper()))


if __name__ == "__main__":
    main()
