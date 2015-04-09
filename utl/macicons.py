#!/usr/bin/env python
# -*- coding: utf-8 -*-
#  macicons.py - icon format converter for the MyMan video game
#  Copyright 2013, Benjamin C. Wiley Sittler <bsittler@gmail.com>
# 
#   Permission is hereby granted, free of charge, to any person
#   obtaining a copy of this software and associated documentation
#   files (the "Software"), to deal in the Software without
#   restriction, including without limitation the rights to use, copy,
#   modify, merge, publish, distribute, sublicense, and/or sell copies
#   of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
# 
#   The above copyright notice and this permission notice shall be
#   included in all copies or substantial portions of the Software.
# 
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#   NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.

import getopt
import struct
import sys
import zlib

PALETTE_16_COLORS = [
    i.decode('hex') for i in (
        'FFFFFF', 'FFFF00', 'FF6600', 'FF0000',
        'FF0088', '6600BB', '2200DD', '00BBFF',
        '00CC00', '007700', '664400', 'AA7744',
        'CCCCCC', '888888', '555555', '000000')]

assert len(PALETTE_16_COLORS) == 16

def cquote(s):
    os = ''
    for ch in s:
        if ord(ch) >= 32 and ord(ch) < 128:
            if ch in '\'\\\"':
                ch = '\\' + ch
        else:
            ch = '\\%03o' % ord(ch)
        os += ch
    return os

def net_uint16(data):
    assert len(data) == 2
    return (ord(data[0]) << 8) | ord(data[1])

def net_uint32(data):
    assert len(data) == 4
    return (ord(data[0]) << 24) | (ord(data[1]) << 16) | (ord(data[2]) << 8) | ord(data[3])

def net_sint32(data):
    si = net_uint32(data)
    if si & 0x80000000:
        si -= 0x100000000
    return si

def paeth_predictor(a, b, c):
    p = a + b - c
    pa = abs(p - a)
    pb = abs(p - b)
    pc = abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    elif pb <= pc:
        return b
    else:
        return c

def convert_icns(filename, data, chunks, excluded_chunktypes, recurse = True):
    while len(data) > 0:
        assert len(data) >= 8, '' + filename + ': incomplete chunk header'
        chunksize = net_uint32(data[4:8])
        assert chunksize <= len(data), '' + filename + ': incomplete chunk'
        chunktype = data[:4]
        chunkdata = data[8:chunksize]
        chunks[chunktype] = chunks.get(chunktype, [])
        chunks[chunktype].append(chunkdata)
        data = data[chunksize:]
        if chunktype in ('TOC ', 'icnV'):
            #print >> sys.stderr, '' + filename + ': chunk \"' + cquote(chunktype) + '\": ignoring chunk'
            pass
        elif chunktype not in excluded_chunktypes and chunktype in (
            'icm#', 'icm4', 'icm8', 'im32', 'm8mk',
            'ics#', 'ics4', 'ics8', 'is32', 's8mk',
            'ICN#', 'icl4', 'icl8', 'il32', 'l8mk', 'ICON',
            'ich#', 'ich4', 'ich8', 'ih32', 'h8mk',
            'ict#', 'ict4', 'ict8', 'it32', 't8mk',
            'icns'):
            sys.stdout.write('data \'%s\' (-16455, purgeable) {\n' % cquote(chunktype))
            if chunktype == 'icns':
                sys.stdout.write('\t"%s" $"%08X"\n' % (cquote(chunktype), 8 + len(chunkdata)))
            sys.stdout.write('\t// %u bytes from %s\n' % (chunksize - 8, filename + ': chunk \"' + cquote(chunktype) + '\"'))
            for i in xrange(len(chunkdata)):
                if not (i % 16):
                    sys.stdout.write('\t$"')
                elif not (i % 2):
                    sys.stdout.write(' ')
                sys.stdout.write('%02X' % ord(chunkdata[i]))
                if (i % 16) == 15 or (i + 1) == len(chunkdata):
                    sys.stdout.write('"\n')
            sys.stdout.write('};\n')
        else:
            #print >> sys.stderr, '' + filename + ': chunk \"' + cquote(chunktype) + '\": skipping chunk'
            pass
        if chunktype == 'icns' and recurse:
            convert_icns(filename + ': \"' + cquote(chunktype) + '\"', chunkdata, chunks, ('icns',))
    assert len(data) == 0, '' + filename + ': unexpected data at end of file'

def convert_png(filename, data, chunks, excluded_chunktypes):
    assert len(data) >= 8 and data[:8] == ('\x89' + 'PNG' + '\x0D\x0A\x1A\x0A'), '' + filename + ': wrong magic number'
    data = data[8:]
    chunkid = 0
    pngchunks = {}
    width = None
    height = None
    bit_depth = None
    color_type = None
    cpp = None
    bpp = None
    ppb = None
    filter_method = None
    interlace_method = None
    palette_data = None
    transparency_data = None
    image_data = ''
    while len(data) > 0:
        assert len(data) >= 12, '' + filename + ': incomplete chunk'
        chunksize = net_sint32(data[:4])
        assert chunksize >= 0, '' + filename + ': chunk size must fit in 31 bits'
        chunktype = data[4:8]
        assert chunksize <= (len(data) - 12), '' + filename + ': incomplete chunk'
        chunkcrc = net_sint32(data[chunksize+8:chunksize+12])
        chunkdata = data[8:8+chunksize]
        actualcrc = zlib.crc32(data[4:8+chunksize])
        data = data[12+chunksize:]
        assert actualcrc == chunkcrc, '' + filename + ': chunk #' + chunkid + '\"' + cquote(chunktype) + '\": CRC error: got ' + ('%08X' % actualcrc) + ' but expected ' + ('%08X' % chunkcrc)
        if chunkid == 0:
            assert chunktype == 'IHDR', '' + filename + ': first chunk must be \"IHDR\" but is actually \"' + cquote(chunktype) + '\"'
        if chunkid != 0:
            assert chunktype != 'IHDR', '' + filename + ': found unexpected \"IHDR\" chunk in non-initial position'
        if len(data) == 0:
            assert chunktype == 'IEND', '' + filename + ': last chunk must be \"IEND\" but is actually \"' + cquote(chunktype) + '\"'
        if len(data) != 0:
            assert chunktype != 'IEND', '' + filename + ': found unexpected \"IEND\" chunk in non-final position'
        if chunktype in ('IHDR', 'PLTE', 'tRNS', 'IDAT', 'IEND'):
            pngchunks[chunktype] = pngchunks.get(chunktype, 0) + 1
            if chunktype == 'IHDR':
                assert len(chunkdata) >= 13, '' + filename + ': short \"IHDR\" chunk'
                width = net_sint32(chunkdata[:4])
                assert width > 0, '' + filename + ': image width must be positive'
                height = net_sint32(chunkdata[4:8])
                assert height > 0, '' + filename + ': image height must be positive'
                bit_depth = ord(chunkdata[8])
                assert bit_depth in (1, 2, 4, 8, 16), '' + filename + ': invalid bit depth: ' + str(bit_depth)
                color_type = ord(chunkdata[9])
                assert color_type in (0, 2, 3, 4, 6), '' + filename + ': invalid color type: ' + str(color_type)
                if color_type & 1:
                    assert bit_depth <= 8, '' + filename + ': image with palette cannot have bit depth ' + str(bit_depth)
                if bit_depth in (1, 2, 4):
                    assert color_type in (0, 3), '' + filename + ': invalid color type for bit depth ' + str(bit_depth) + ': ' + str(color_type)
                cpp = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}[color_type]
                bpp = (cpp * bit_depth + 7) // 8
                ppb = 1
                if bit_depth < 8:
                    ppb = 8 // bit_depth
                compression_method = ord(chunkdata[10])
                assert compression_method == 0, '' + filename + ': unsupported compression method: ' + str(compression_method)
                filter_method = ord(chunkdata[11])
                assert filter_method == 0, '' + filename + ': unsupported filter method: ' + str(filter_method)
                interlace_method = ord(chunkdata[12])
                assert interlace_method in (0, 1), '' + filename + ': unsupported interlace method: ' + str(interlace_method)
                image_data = ''
            elif chunktype == 'PLTE':
                assert pngchunks.get('tRNS', 0) == 0, '' + filename + ': \"PLTE\" chunk not allowed after \"tRNS\" chunk(s)'
                assert pngchunks.get('IDAT', 0) == 0, '' + filename + ': \"PLTE\" chunk not allowed after \"IDAT\" chunk(s)'
                assert chunksize <= 256 * 3, '' + filename + ': \"PLTE\" chunk too long: ' + str(chunksize) + ' bytes'
                assert chunksize % 3 == 0, '' + filename + ': \"PLTE\" chunk size must be an integer multiple of 3 bytes'
                palette_data = [ (ord(chunkdata[k]), ord(chunkdata[k + 1]), ord(chunkdata[k + 2])) for k in xrange(0, chunksize, 3) ]
            elif chunktype == 'tRNS':
                assert pngchunks.get('IDAT', 0) == 0, '' + filename + ': \"tRNS\" chunk not allowed after \"IDAT\" chunk(s)'
                assert color_type not in (4, 6), '' + filename + ': \"tRNS\" chunk not permitted when alpha channel is present'
                assert chunksize > 0, '' + filename + ': \"tRNS\" chunk size must be positive'
                if color_type == 0:
                    assert chunksize == 2, '' + filename + ': \"tRNS\" chunk size for greyscale image must be 2 bytes but is ' + str(chunksize)
                    transparency_data = (net_uint16(chunkdata[:2]), net_uint16(chunkdata[:2]), net_uint16(chunkdata[:2]))
                    assert transparency_data[0] < (1 << bit_depth), '' + filename + '\: greyscale in \"tRNS\" chunk does not fit in ' + str(bit_depth) + ' bits'
                elif color_type == 2:
                    assert chunksize == 6, '' + filename + ': \"tRNS\" chunk size for truecolor image must be 6 bytes but is ' + str(chunksize)
                    transparency_data = (net_uint16(chunkdata[:2]), net_uint16(chunkdata[2:4]), net_uint16(chunkdata[4:]))
                    assert transparency_data[0] < (1 << bit_depth), '' + filename + '\: red level in \"tRNS\" chunk does not fit in ' + str(bit_depth) + ' bits'
                    assert transparency_data[1] < (1 << bit_depth), '' + filename + '\: green level in \"tRNS\" chunk does not fit in ' + str(bit_depth) + ' bits'
                    assert transparency_data[2] < (1 << bit_depth), '' + filename + '\: blue level in \"tRNS\" chunk does not fit in ' + str(bit_depth) + ' bits'
                elif color_type == 1:
                    assert pngchunks.get('PLTE', 0) == 1, '' + filename + ': \"tRNS\" chunk must follow \"PLTE\" chunk for palettized image'
                    assert chunksize <= (1 << bit_depth), '' + filename + ': \"tRNS\" chunk size for image with ' + str(1 << bit_depth) + '-color palette has too many entries: ' + str(chunksize)
                    transparency_data = [ord(i) for i in chunkdata]
            elif chunktype == 'IDAT':
                if color_type & 1:
                    assert pngchunks.get('PLTE', 0) == 1, '' + filename + ': missing required \"PLTE\" chunk'
                image_data += zlib.decompress(chunkdata)
        else:
            #print >> sys.stderr, '' + filename + ': chunk \"' + cquote(chunktype) + '\": ignoring chunk'
            pass
        chunkid += 1
    assert len(data) == 0, '' + filename + ': unexpected data at end of file'
    num_IHDRs = pngchunks.get('IHDR', 0)
    assert num_IHDRs == 1, '' + filename + ': expected exactly one \"IHDR\" chunk but found ' + str(num_IHDRs)
    num_PLTEs = pngchunks.get('PLTE', 0)
    assert num_PLTEs <= 1, '' + filename + ': expected at most one \"PLTE\" chunk but found ' + str(num_PLTEs)
    num_tRNSs = pngchunks.get('tRNS', 0)
    assert num_tRNSs <= 1, '' + filename + ': expected at most one \"tRNS\" chunk but found ' + str(num_tRNSs)
    num_IDATs = pngchunks.get('IDAT', 0)
    assert num_IDATs >= 1, '' + filename + ': expected at least one \"IDAT\" chunk but found ' + str(num_IDATs)
    num_IENDs = pngchunks.get('IEND', 0)
    assert num_IENDs == 1, '' + filename + ': expected exactly one \"IEND\" chunk but found ' + str(num_IENDs)
    #print >> sys.stderr, filename, width, height, bit_depth, color_type, compression_method, filter_method, 'interlaced? ' + str(interlace_method), len(image_data), width * height
    rsz = ((width + 7) // 8) * 8
    red_plane = [chr(0) for i in xrange(rsz * height)]
    green_plane = [chr(0) for i in xrange(rsz * height)]
    blue_plane = [chr(0) for i in xrange(rsz * height)]
    alpha_plane = [chr(0) for i in xrange(rsz * height)]
    for interlaced_pass in xrange(7):
        if compression_method == 0:
            if interlaced_pass > 0:
                break
            x0, y0, step_x, step_y = 0, 0, 1, 1
        else:
            x0, y0, step_x, step_y = (
                (0, 0, 8, 8),
                (4, 0, 8, 8),
                (0, 4, 4, 8),
                (2, 0, 4, 4),
                (0, 2, 2, 4),
                (1, 0, 2, 2),
                (0, 1, 1, 2))[interlaced_pass]
        if x0 >= width or y0 >= height:
            continue
        bpl = 1 + (cpp * bit_depth * ((width - x0 + step_x - 1) / step_x) + 7) // 8
        lpi = (height - y0 + step_y - 1) // step_y
        assert len(image_data) >= bpl * lpi, '' + filename + ': not enough image data for pass ' + str(interlaced_pass)
        filter_type = None
        sl = [[0 for i in xrange(bpl)] for j in xrange(2)]
        for vy in xrange(lpi):
            filter_type = ord(image_data[vy * bpl])
            assert filter_type in (0, 1, 2, 3, 4), '' + filename + ': unsupported filter type in pass ' + str(interlaced_pass) + ' on scanline ' + str(vy) + ': ' + filter_type
            y = y0 + vy * step_y
            for i in xrange(1, bpl):
                d = ord(image_data[vy * bpl + i])
                a, c = 0, 0
                if i > bpp:
                    a = sl[vy % 2][i - bpp]
                    c = sl[(vy + 1) % 2][i - bpp]
                b = sl[(vy + 1) % 2][i]
                if filter_type == 0:
                    # None
                    e = d
                elif filter_type == 1:
                    # Sub
                    e = (d + a) % 256
                elif filter_type == 2:
                    # Up
                    e = (d + b) % 256
                elif filter_type == 3:
                    # Average
                    e = (d + ((a + b) // 2)) % 256
                elif filter_type == 4:
                    # Paeth
                    e = (d + paeth_predictor(a, b, c)) % 256
                sl[vy % 2][i] = e
            for x in xrange(x0, width, step_x):
                vy_bits = 8 + (((x - x0) // step_x) * cpp * bit_depth)
                px = sl[vy % 2][vy_bits // 8:vy_bits // 8 + bpp]
                effective_depth = bit_depth
                if color_type in (0, 3, 4):
                    mask_level = (1 << bit_depth) - 1
                    if bit_depth == 16:
                        red_level = net_uint16(px[:2])
                    else:
                        red_level = (px[0] >> (8 - bit_depth - (vy_bits % 8))) & mask_level
                    green_level, blue_level = red_level, red_level
                    if color_type in (0, 3):
                        alpha_level = mask_level
                    elif bit_depth == 16:
                        alpha_level = net_uint16(px[2:4])
                    else:
                        alpha_level = ord(px[1])
                elif color_type in (2, 6):
                    mask_level = (1 << bit_depth) - 1
                    if bit_depth == 16:
                        red_level = net_uint16(px[:2])
                        green_level = net_uint16(px[2:4])
                        blue_level = net_uint16(px[4:6])
                        if color_type == 6:
                            alpha_level = net_uint16(px[6:8])
                        else:
                            alpha_level = 0xFFFF
                    else:
                        red_level = ord(px[0])
                        green_level = ord(px[1])
                        blue_level = ord(px[2])
                        if color_type == 6:
                            alpha_level = ord(px[3])
                        else:
                            alpha_level = 0xFF
                if color_type == 3:
                    if transparency_data is not None and red_level < len(transparency_data):
                        alpha_level = transparency_data[red_level]
                    else:
                        alpha_level = 0xFF
                    assert red_level < len(palette_data), '' + filename + ': \"PLTE\" chunk is missing entry for pixel with index ' + str(red_level)
                    red_level, green_level, blue_level = palette_data[red_level]
                    effective_depth = 8
                if color_type in (0, 2) and transparency_data is not None and (red_level, green_level, blue_level) == transparency_data:
                    alpha_level = 0
                while effective_depth < 16:
                    red_level = (red_level << effective_depth) | red_level
                    green_level = (green_level << effective_depth) | green_level
                    blue_level = (blue_level << effective_depth) | blue_level
                    alpha_level = (alpha_level << effective_depth) | alpha_level
                    effective_depth += effective_depth
                red_plane[y * rsz + x] = chr(red_level >> 8)
                green_plane[y * rsz + x] = chr(green_level >> 8)
                blue_plane[y * rsz + x] = chr(blue_level >> 8)
                alpha_plane[y * rsz + x] = chr(alpha_level >> 8)
        image_data = image_data[lpi * bpl:]
    assert len(image_data) == 0, '' + filename + ': unexpected data after image'
    for color24_type, mask8_type, sz in (('im32', 'm8mk', 12),
                                         ('is32', 's8mk', 16),
                                         ('il32', 'l8mk', 32),
                                         ('ih32', 'h8mk', 48),
                                         ('it32', 't8mk', 128)):
        if (width, height) == (sz, sz):
            synthesize_color24(filename, color24_type, mask8_type, sz, chunks, red_plane, green_plane, blue_plane, alpha_plane)
            break

def convert(filename, data, chunks, excluded_chunktypes):
    if len(data) >= 8 and data[:4] == 'icns' and net_uint32(data[4:8]) == len(data):
        convert_icns(filename, data, chunks, excluded_chunktypes)
    elif len(data) >= 8 and data[:8] == ('\x89' + 'PNG' + '\x0D\x0A\x1A\x0A'):
        convert_png(filename, data, chunks, excluded_chunktypes)
    else:
        assert 0, '' + filename + ': unrecognized data format (not "icns" or PNG)'

def unpackbitsmod(filename, sz, packed):
    unpacked = [ chr(0) for x in xrange(sz) ]
    i = 0
    j = 0
    while i < len(packed):
        k = ord(packed[i])
        i += 1
        if k < 128:
            reps = 1
            ll = k + 1
        else:
            reps = k - 125
            ll = 1
        for m in xrange(reps):
            for n in xrange(ll):
                unpacked[j + m * ll + n] = packed[i + n]
        i += ll
        j += ll * reps
    assert j == sz, '' + filename + ': wrong output size for packed data: got ' + str(j) + ' vs. expected ' + str(sz)
    return ''.join(unpacked)

def packbitsmod(filename, unpacked):
    packed = ''
    i = 0
    while i < len(unpacked):
        reps = 1
        ll = 1
        if unpacked[i+1:i+3] == 2 * unpacked[i]:
            reps = 3
            while unpacked[i + reps:i + reps + 1] == unpacked[i] and reps < 130:
                reps += 1
        else:
            while (i + ll) < len(unpacked) and unpacked[i + ll + 1:i + ll + 3] != 2 * unpacked[i + ll] and ll < 128:
                ll += 1
        if reps == 1:
            packed += chr(ll - 1) + unpacked[i:i+ll]
        else:
            packed += chr(reps + 125) + unpacked[i]
        i += ll * reps
    check = unpackbitsmod(filename, len(unpacked), packed)
    assert check == unpacked, '' + filename + ': packbitsmod failed to round-trip correctly: got\n ' + str(len(check)) + ':' + check.encode('hex') + '\nvs. expected\n ' + str(len(unpacked)) + ':' + unpacked.encode('hex') + '\nfor packed result\n ' + packed.encode('hex')
    return packed

def unpackbitsmod_rgb(filename, sz, data, color24_type):
    if color24_type == 'it32':
        data = data[4:]
    rgb = unpackbitsmod(filename, sz * 3, data)
    return rgb[:sz], rgb[sz:sz+sz], rgb[sz+sz:]

def synthesize_mono(filename, mono_type, color24_type, mask8_type, sz, chunks, old_type = None):
    rsz = ((sz + 7) // 8) * 8
    if chunks.has_key(color24_type) and chunks.has_key(mask8_type) and not chunks.has_key(mono_type):
        red, green, blue = unpackbitsmod_rgb(filename + ': chunk \"' + cquote(color24_type) + '\"', rsz * sz, chunks[color24_type][0], color24_type)
        bitmap = [chr(0) for i in xrange((rsz / 8) * sz)]
        mask = [chr(0) for i in xrange((rsz / 8) * sz)]
        for r in xrange(sz):
            for c in xrange(rsz):
                q = (299 * ord(red[r * rsz + c]) + 587 * ord(green[r * rsz + c]) + 114 * ord(blue[r * rsz + c])) / 1000
                p = ord(chunks[mask8_type][0][r * rsz + c])
                if p >= 128:
                    mask[r * (rsz / 8) + (c / 8)] = chr(ord(mask[r * (rsz / 8) + (c / 8)]) | (0x80 >> (c % 8)))
                else:
                    q = 255
                if q < 25:
                    bitmap[r * (rsz / 8) + (c / 8)] = chr(ord(bitmap[r * (rsz / 8) + (c / 8)]) | (0x80 >> (c % 8)))
        chunkdata = ('%-4s' % mono_type) + ('%08X' % (8 + len(bitmap) + len(mask))).decode('hex') + ''.join(bitmap) + ''.join(mask)
        convert_icns(filename + ': \"icns\" for synthesized \"' + cquote(mono_type) + '\"', chunkdata, chunks, ('icns',))
    if chunks.has_key(mono_type) and old_type is not None and not chunks.has_key(old_type):
        bitmap = chunks[mono_type][0][:(rsz / 8) * sz]
        mask = chunks[mono_type][0][(rsz / 8) * sz:]
        bitmap = ''.join(chr(ord(bitmap[i]) & ord(mask[i])) for i in xrange(len(bitmap)))
        chunkdata = ('%-4s' % old_type) + ('%08X' % (8 + len(bitmap))).decode('hex') + bitmap
        convert_icns(filename + ': \"icns\" for synthesized \"' + cquote(old_type) + '\"', chunkdata, chunks, ('icns',))

def synthesize_color4(filename, color4_type, color24_type, mono_type, sz, chunks):
    rsz = ((sz + 7) // 8) * 8
    if chunks.has_key(color24_type) and chunks.has_key(mono_type) and not chunks.has_key(color4_type):
        red, green, blue = unpackbitsmod_rgb(filename + ': chunk \"' + cquote(color24_type) + '\"', rsz * sz, chunks[color24_type][0], color24_type)
        nybblemap = [chr(0) for i in xrange(((rsz + 1) / 2) * sz)]
        for r in xrange(sz):
            for c in xrange(rsz):
                q_red, q_green, q_blue = ord(red[r * rsz + c]), ord(green[r * rsz + c]), ord(blue[r * rsz + c])
                px = (min(((ord(PALETTE_16_COLORS[i][0]) - q_red) ** 2 +
                          (ord(PALETTE_16_COLORS[i][1]) - q_green) ** 2 +
                          (ord(PALETTE_16_COLORS[i][2]) - q_blue) ** 2, i) for i in xrange(len(PALETTE_16_COLORS))))[1]
                nybblemap[r * ((rsz + 1) / 2) + (c / 2)] = chr(ord(nybblemap[r * ((rsz + 1) / 2) + (c / 2)]) | ((0x10 * px) >> (4 * (c % 2))))
        chunkdata = ('%-4s' % color4_type) + ('%08X' % (8 + len(nybblemap))).decode('hex') + ''.join(nybblemap)
        convert_icns(filename + ': \"icns\" for synthesized \"' + cquote(color4_type) + '\"', chunkdata, chunks, ('icns',))

def synthesize_color8(filename, color8_type, color24_type, mono_type, sz, chunks):
    rsz = ((sz + 7) // 8) * 8
    if chunks.has_key(color24_type) and chunks.has_key(mono_type) and not chunks.has_key(color8_type):
        red, green, blue = unpackbitsmod_rgb(filename + ': chunk \"' + cquote(color24_type) + '\"', rsz * sz, chunks[color24_type][0], color24_type)
        pixmap = [chr(0) for i in xrange(rsz * sz)]
        for r in xrange(sz):
            for c in xrange(rsz):
                q_red, q_green, q_blue = ord(red[r * rsz + c]), ord(green[r * rsz + c]), ord(blue[r * rsz + c])
                rgb6_red = 5 - (q_red * 5) // 255
                rgb6_green = 5 - (q_green * 5) // 255
                rgb6_blue = 5 - (q_blue * 5) // 255
                rgb12_red = 11 - (q_red * 11) / 255
                rgb12_green = 11 - (q_green * 11) // 255
                rgb12_blue = 11 - (q_blue * 11) // 255
                pal = []
                if (rgb6_red, rgb6_red) == (rgb6_green, rgb6_blue) and rgb12_red > 0 and rgb12_red < 11:
                    pal.append((chr(255 - (rgb12_red * 255) // 11) * 3, 245 + rgb12_red))
                if (rgb6_red, rgb6_green) == (5, 5) and rgb12_blue > 0 and rgb12_blue < 11:
                    pal.append((2 * chr(0) + chr(255 - (rgb12_blue * 255) // 11), 235 + rgb12_blue))
                if (rgb6_red, rgb6_blue) == (5, 5) and rgb12_green > 0 and rgb12_green < 11:
                    pal.append((chr(0) + chr(255 - (rgb12_green * 255) // 11) + chr(0), 225 + rgb12_green))
                if (rgb6_green, rgb6_blue) == (5, 5) and rgb12_red > 0 and rgb12_red < 11:
                    pal.append((chr(255 - (rgb12_red * 255) // 11) + 2 * chr(0), 215 + rgb12_red))
                if (rgb6_red, rgb6_green, rgb6_blue) == (5, 5, 5):
                    pal.append((chr(0) * 3, 255))
                else:
                    pal.append((chr(255 - (rgb6_red * 255) // 5) + chr(255 - (rgb6_green * 255) // 5) + chr(255 - (rgb6_blue * 255) // 5), (rgb6_red * 6 + rgb6_green) * 6 + rgb6_blue))
                px = (min(((ord(pal[i][0][0]) - q_red) ** 2 +
                          (ord(pal[i][0][1]) - q_green) ** 2 +
                          (ord(pal[i][0][2]) - q_blue) ** 2, pal[i][1]) for i in xrange(len(pal))))[1]
                pixmap[r * rsz + c] = chr(px)
        chunkdata = ('%-04s' % color8_type) + ('%08X' % (8 + len(pixmap))).decode('hex') + ''.join(pixmap)
        convert_icns(filename + ': \"icns\" for synthesized \"' + cquote(color8_type) + '\"', chunkdata, chunks, ('icns',))

def synthesize_color24(filename, color24_type, mask8_type, sz, chunks, red, green, blue, alpha):
    rsz = ((sz + 7) // 8) * 8
    assert len(red) == sz * rsz, '' + filename + ': red plane has wrong length for \"' + cquote(color24_type) + '\"'
    assert len(green) == sz * rsz, '' + filename + ': green plane has wrong length for \"' + cquote(color24_type) + '\"'
    assert len(blue) == sz * rsz, '' + filename + ': blue plane has wrong length for \"' + cquote(color24_type) + '\"'
    assert len(alpha) == sz * rsz, '' + filename + ': alpha plane has wrong length for \"' + cquote(mask8_type) + '\"'
    if not chunks.has_key(mask8_type):
        chunkdata = ('%-04s' % mask8_type) + ('%08X' % (8 + len(alpha))).decode('hex') + ''.join(alpha)
        convert_icns(filename + ': \"icns\" for synthesized \"' + cquote(mask8_type) + '\"', chunkdata, chunks, ('icns',))
    if not chunks.has_key(color24_type):
        red_data = packbitsmod(filename + ': synthesized \"' + cquote(color24_type) + '\": red', ''.join(red))
        green_data = packbitsmod(filename + ': synthesized \"' + cquote(color24_type) + '\": green', ''.join(green))
        blue_data = packbitsmod(filename + ': synthesized \"' + cquote(color24_type) + '\": blue', ''.join(blue))
        data = red_data + green_data + blue_data
        if color24_type == 'it32':
            data = chr(0) * 4 + data
        chunkdata = ('%-04s' % color24_type) + ('%08X' % (8 + len(data))).decode('hex') + ''.join(data)
        convert_icns(filename + ': \"icns\" for synthesized \"' + cquote(color24_type) + '\"', chunkdata, chunks, ('icns',))

def synthesize_icns(filename, chunks):
    if not chunks.has_key('icns'):
        wrapped = ''
        for chunktype in (
            'icm#', 'icm4', 'icm8', 'im32', 'm8mk',
            'ics#', 'ics4', 'ics8', 'is32', 's8mk',
            'ICN#', 'icl4', 'icl8', 'il32', 'l8mk', 'ICON',
            'ich#', 'ich4', 'ich8', 'ih32', 'h8mk',
            'ict#', 'ict4', 'ict8', 'it32', 't8mk'):
            if chunks.has_key(chunktype):
                chunkdata = chunks[chunktype][0]
                wrapped += ('%-4s' % chunktype) + ('%08X' % (8 + len(chunkdata))).decode('hex') + chunkdata
        chunkdata = ('%-4s' % 'icns') + ('%08X' % (8 + len(wrapped))).decode('hex') + wrapped
        convert_icns(filename + ': synthesized \"icns\"', chunkdata, chunks, (), recurse = False)

def main(argv):
    chunks = {}
    opts, args = getopt.gnu_getopt(argv, 'o:', ['outfile='])
    for opt, optarg in opts:
        if opt in ('-o', '--outfile'):
            sys.stdout = file(optarg, 'wb')
        else:
            assert False, 'unhandled option ' + `opt`
    sys.stdout.write(
        '// -*- mode: c++; coding: mac-roman -*-' + '\n' +
        '//' + '\n' +
        '// NOTE: This file was automatically generated by:' + '\n' +
        '//' + '\n' +
        '//     $ ' + (' \\' + '\n' +
                       '//         ').join(cquote(i) for i in argv) + '\n' +
        '//' + '\n' +
        '// Changes may be overwritten.' + '\n' +
        '\n')
    if len(args) == 1:
        convert('(stdin)', sys.stdin.read(), chunks, ())
    else:
        for filename in args[1:]:
            if filename == '-':
                data = sys.stdin.read()
                filename = '(stdin)'
            else:
                data = file(filename, 'rb').read()
            convert(cquote(filename), data, chunks, ())
    filename = '(synthesized)'
    synthesize_mono(filename, 'icm#', 'im32', 'm8mk', 12, chunks)
    synthesize_color4(filename, 'icm4', 'im32', 'icm#', 12, chunks)
    synthesize_color8(filename, 'icm8', 'im32', 'icm#', 12, chunks)
    synthesize_mono(filename, 'ics#', 'is32', 's8mk', 16, chunks)
    synthesize_color4(filename, 'ics4', 'is32', 'ics#', 16, chunks)
    synthesize_color8(filename, 'ics8', 'is32', 'ics#', 16, chunks)
    synthesize_mono(filename, 'ICN#', 'il32', 'l8mk', 32, chunks, old_type = 'ICON')
    synthesize_color4(filename, 'icl4', 'il32', 'ICN#', 32, chunks)
    synthesize_color8(filename, 'icl8', 'il32', 'ICN#', 32, chunks)
    synthesize_mono(filename, 'ich#', 'ih32', 'h8mk', 48, chunks)
    synthesize_color4(filename, 'ich4', 'ih32', 'ich#', 48, chunks)
    synthesize_color8(filename, 'ich8', 'ih32', 'ich#', 48, chunks)
    synthesize_mono(filename, 'ict#', 'it32', 't8mk', 128, chunks)
    synthesize_color4(filename, 'ict4', 'it32', 'ict#', 128, chunks)
    synthesize_color8(filename, 'ict8', 'it32', 'ict#', 128, chunks)
    synthesize_icns(filename, chunks)

if __name__ == '__main__':
    main(sys.argv)
