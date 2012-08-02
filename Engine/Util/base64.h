//
//  base64.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 11/12/11.
//  Copyright (c) 2011 GameJelly Online. All rights reserved.
//

#ifndef CoreEngine3D_base64_h
#define CoreEngine3D_base64_h

#include <memory.h>
#include <stdlib.h>

void _base64_encode_triple(unsigned char triple[3], char result[4]);
int base64_encode(unsigned char *source, size_t sourcelen, char *target, size_t targetlen);
int _base64_char_value(char base64char);
int _base64_decode_triple(char quadruple[4], unsigned char *result);
size_t base64_decode(char *source, unsigned char *target, size_t targetlen);

#endif
