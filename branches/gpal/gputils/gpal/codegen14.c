/* code generation for 14 bit pics
   Copyright (C) 2003
   Craig Franklin

This file is part of gputils.

gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "stdhdr.h"

#include "libgputils.h"
#include "gpal.h"
#include "symbol.h"
#include "analyze.h"
#include "codegen.h"
#include "codegen14.h"

/****************************************************************************/
/* Common                                                                   */
/****************************************************************************/

#define ADD_BANKSEL(x) if(add_banksel) codegen_write_asm("banksel %s", x);

/* load a constant into the working register */

static void
load_constant14(int value, enum size_tag size)
{
  int num_bytes;
  
  if ((size == size_uint8) || (size == size_uint8)) {
    codegen_write_asm("movlw %#x", value & 0xff);
    return;
  }

  num_bytes = prim_size(size);

  switch (num_bytes) {
  case 4:
    codegen_write_asm("movlw %#x", (value >> 24) & 0xff);
    codegen_write_asm("movwf %s + 3", WORKING_LABEL);
    /* fall through */
  case 3:
    codegen_write_asm("movlw %#x", (value >> 16) & 0xff);
    codegen_write_asm("movwf %s + 2", WORKING_LABEL);
    /* fall through */
  case 2:
    codegen_write_asm("movlw %#x", (value >> 8) & 0xff);
    codegen_write_asm("movwf %s + 1", WORKING_LABEL);
    /* fall through */
  case 1:
    codegen_write_asm("movlw %#x", value & 0xff);
    codegen_write_asm("movwf %s", WORKING_LABEL);
  }

}

/* load a file into the working register */

static void
load_file14(char *name, enum size_tag size, int offset, gp_boolean add_banksel)
{
  int num_bytes;
  char offset_buffer[64];
  
  if (offset == 0) {
    offset_buffer[0] = '\0';
  } else if (offset < 0) {
    sprintf(offset_buffer, " - %#x", -offset);
  } else {
    sprintf(offset_buffer, " + %#x", offset);
  }
  
  /* W is used as the working register for single byte types. */
  if ((size == size_uint8) || (size == size_uint8)) {
    ADD_BANKSEL(name);
    codegen_write_asm("movf %s%s, w", name, offset_buffer);
    ADD_BANKSEL(LOCAL_DATA_LABEL);
    return;
  }

  num_bytes = prim_size(size);

  switch (num_bytes) {
  case 4:
    ADD_BANKSEL(name);
    codegen_write_asm("movf %s%s + 3, w", name, offset_buffer);
    ADD_BANKSEL(WORKING_LABEL);
    codegen_write_asm("movwf %s + 3", WORKING_LABEL);
    /* fall through */
  case 3:
    ADD_BANKSEL(name);
    codegen_write_asm("movf %s%s + 2, w", name, offset_buffer);
    ADD_BANKSEL(WORKING_LABEL);
    codegen_write_asm("movwf %s + 2", WORKING_LABEL);
    /* fall through */
  case 2:
    ADD_BANKSEL(name);
    codegen_write_asm("movf %s%s + 1, w", name, offset_buffer);
    ADD_BANKSEL(WORKING_LABEL);
    codegen_write_asm("movwf %s + 1", WORKING_LABEL);
    /* fall through */
  case 1:
    ADD_BANKSEL(name);
    codegen_write_asm("movf %s%s, w", name, offset_buffer);
    ADD_BANKSEL(WORKING_LABEL);
    codegen_write_asm("movwf %s", WORKING_LABEL);
  }

}

/* store the working register to a file */

static void
store_file14(char *name, enum size_tag size, int offset, gp_boolean add_banksel)
{
  int num_bytes;
  char offset_buffer[64];
  
  if (offset == 0) {
    offset_buffer[0] = '\0';
  } else if (offset < 0) {
    sprintf(offset_buffer, " - %#x", -offset);
  } else {
    sprintf(offset_buffer, " + %#x", offset);
  }
  
  if ((size == size_uint8) || (size == size_uint8)) {
    ADD_BANKSEL(name);
    codegen_write_asm("movwf %s%s", name, offset_buffer);
    ADD_BANKSEL(LOCAL_DATA_LABEL);
    return;
  }

  num_bytes = prim_size(size);

  switch (num_bytes) {
  case 4:
    codegen_write_asm("movf %s + 3, w", WORKING_LABEL);
    ADD_BANKSEL(name);
    codegen_write_asm("movwf %s%s + 3", name, offset_buffer);
    /* fall through */
  case 3:
    ADD_BANKSEL(WORKING_LABEL);
    codegen_write_asm("movf %s + 2, w", WORKING_LABEL);
    ADD_BANKSEL(name);
    codegen_write_asm("movwf %s%s + 2", name, offset_buffer);
    /* fall through */
  case 2:
    ADD_BANKSEL(WORKING_LABEL);
    codegen_write_asm("movf %s + 1, w", WORKING_LABEL);
    ADD_BANKSEL(name);
    codegen_write_asm("movwf %s%s + 1", name, offset_buffer);
    /* fall through */
  case 1:
    ADD_BANKSEL(WORKING_LABEL);
    codegen_write_asm("movf %s, w", WORKING_LABEL);
    ADD_BANKSEL(name);
    codegen_write_asm("movwf %s%s", name, offset_buffer);
    ADD_BANKSEL(WORKING_LABEL);
  }

}

/* convert the working register into a boolean */

static void
gen_boolean(enum size_tag size)
{
  int num_bytes;

  if ((size == size_uint8) || (size == size_uint8)) {
    codegen_write_asm("btfss STATUS, Z");
    codegen_write_asm("movlw 1");
    return;
  }

  num_bytes = prim_size(size);

  codegen_write_asm("movf %s, w", WORKING_LABEL);

  switch (num_bytes) {
  case 4:
    codegen_write_asm("iorwf %s + 3, w", WORKING_LABEL);
    /* fall through */
  case 3:
    codegen_write_asm("iorwf %s + 2, w", WORKING_LABEL);
    /* fall through */
  case 2:
    codegen_write_asm("iorwf %s + 1, w", WORKING_LABEL);
  }

  codegen_write_asm("btfss STATUS, Z");
  codegen_write_asm("movlw 1");
  codegen_write_asm("movwf %s", WORKING_LABEL);

  codegen_write_asm("movlw 0");
  switch (num_bytes) {
  case 4:
    codegen_write_asm("movwf %s + 3", WORKING_LABEL);
    /* fall through */
  case 3:
    codegen_write_asm("movwf %s + 2", WORKING_LABEL);
    /* fall through */
  case 2:
    codegen_write_asm("movwf %s + 1", WORKING_LABEL);
  }

}

/****************************************************************************/
/* Arithmetic                                                               */
/****************************************************************************/

/* add the lowest byte */
static void
add_setup(gp_boolean is_const, int value, char *name)
{
  codegen_write_asm("movf %s, w", WORKING_LABEL);
  if (is_const) {
    codegen_write_asm("addlw %#x", value);
  } else {
    codegen_write_asm("addwf %s, w", name);
  }
  codegen_write_asm("movwf %s", WORKING_LABEL);
}

/* add the next byte */
static void
add_next(gp_boolean is_const, int value, char *name, int byte)
{
  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  codegen_write_asm("btfsc STATUS, C");
  codegen_write_asm("incfsz %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("addlw %#x", value);    
  } else {
    codegen_write_asm("addwf %s + %i, w", name, byte);
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);
}

static void
do_add(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("addlw %#x", value & 0xff);
    } else {
      codegen_write_asm("addwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    add_setup(is_const, (value & 0xff), name);
    add_next(is_const, (value >> 8) && 0xff, name, 1);
    break;
  case size_uint24:
  case size_int24:
    add_setup(is_const, (value & 0xff), name);
    add_next(is_const, (value >> 8) && 0xff, name, 1);
    add_next(is_const, (value >> 16) && 0xff, name, 2);
    break;
  case size_uint32:
  case size_int32:
    add_setup(is_const, (value & 0xff), name);
    add_next(is_const, (value >> 8) && 0xff, name, 1);
    add_next(is_const, (value >> 16) && 0xff, name, 2);
    add_next(is_const, (value >> 24) && 0xff, name, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

/* substract the lowest byte */
static void
sub_setup(gp_boolean is_const, int value, char *name)
{
  codegen_write_asm("movf %s, w", WORKING_LABEL);
  if (is_const) {
    codegen_write_asm("sublw %#x", value);    
  } else {
    codegen_write_asm("subwf %s, w", name);
  }
  codegen_write_asm("movwf %s", WORKING_LABEL);
}

/* subtract the next byte */
static void
sub_next(gp_boolean is_const, int value, char *name, int byte)
{
  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  codegen_write_asm("btfss STATUS, C");
  codegen_write_asm("incfsz %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("sublw %#x", value);    
  } else {
    codegen_write_asm("subwf %s + %i, w", name, byte);
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);
}

static void
do_sub(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("sublw %#x", value);
    } else {
      codegen_write_asm("subwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    sub_setup(is_const, (value & 0xff), name);
    sub_next(is_const, (value >> 8) && 0xff, name, 1);
    break;
  case size_uint24:
  case size_int24:
    sub_setup(is_const, (value & 0xff), name);
    sub_next(is_const, (value >> 8) && 0xff, name, 1);
    sub_next(is_const, (value >> 16) && 0xff, name, 2);
    break;
  case size_uint32:
  case size_int32:
    sub_setup(is_const, (value & 0xff), name);
    sub_next(is_const, (value >> 8) && 0xff, name, 1);
    sub_next(is_const, (value >> 16) && 0xff, name, 2);
    sub_next(is_const, (value >> 24) && 0xff, name, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_mult(enum size_tag size, gp_boolean is_const, int value, char *name)
{
  char *reg1 = NULL;
  char *label1 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_uint16:
  case size_uint24:
  case size_uint32:
    reg1 = codegen_get_temp(size);
    label1 = codegen_next_label();
    store_file14(reg1, size, 0, false);
    load_constant14(0, size);
    codegen_write_label(label1);
    do_add(size, is_const, value, name);
    /* FIXME: need multibyte decfsz */
    codegen_write_asm("decfsz %s, f", reg1);
    codegen_write_asm("goto %s", label1);
    break;
  case size_int8:
  case size_int16:
  case size_int24:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

  if (label1)
    free(label1);
}

static void
do_div(enum size_tag size, gp_boolean is_const, int value, char *name)
{
  char *reg1 = NULL;
  char *reg2 = NULL;
  char *label1 = NULL;
  char *label2 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
    reg1 = codegen_get_temp(size);
    reg2 = codegen_get_temp(size);
    label1 = codegen_next_label();
    label2 = codegen_next_label();
    codegen_write_asm("movwf %s", reg1);    /* store the divisor in temp*/
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
    } else {
      codegen_write_asm("movf %s, w", name);
    }    
    codegen_write_asm("movwf %s", reg2);    /* store the dividend */
    codegen_write_asm("movf %s, w", reg1);  /* move the divisor into w */
    codegen_write_asm("clrf %s", reg1);     /* clear the result */
    codegen_write_label(label1);
    codegen_write_asm("subwf %s, f", reg2); /* sub the divisor from the dividend */
    codegen_write_asm("btfsc STATUS, C");
    codegen_write_asm("goto %s", label2);
    codegen_write_asm("incf %s, f", reg1);  /* increment the result */
    codegen_write_asm("goto %s", label1);
    codegen_write_label(label2);
    codegen_write_asm("movf %s, w", reg1);  /* move the result into w */
    break;
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

  if (reg2)
    free(reg2);

  if (label1)
    free(label1);

  if (label2)
    free(label2);

}

static void
do_mod(enum size_tag size, gp_boolean is_const, int value, char *name)
{
  char *reg1 = NULL;
  char *reg2 = NULL;
  char *label1 = NULL;
  char *label2 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
    reg1 = codegen_get_temp(size);
    reg2 = codegen_get_temp(size);
    label1 = codegen_next_label();
    codegen_write_asm("movwf %s", reg1);    /* store the divisor in temp*/
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
    } else {
      codegen_write_asm("movf %s, w", name);
    } 
    codegen_write_asm("movwf %s", reg2);    /* store the dividend */
    codegen_write_asm("movf %s, w", reg1);  /* move the divisor into w */
    codegen_write_label(label1);
    codegen_write_asm("subwf %s, f", reg2); /* sub the divisor from the dividend */
    codegen_write_asm("btfss STATUS, C");
    codegen_write_asm("goto %s", label1);
    codegen_write_asm("addwf %s, w", reg2); /* move the modulus into w */
    break;
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

  if (reg2)
    free(reg2);

  if (label1)
    free(label1);

  if (label2)
    free(label2);

}

static void
do_neg(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    do_sub(size, true, 0, NULL);
  case size_float:
  default:
    assert(0);
  }

}

/****************************************************************************/
/* Logical                                                                  */
/****************************************************************************/

static void
and_byte(gp_boolean is_const, int value, char *name, int byte)
{

  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("andlw %#x", value);
  } else {
    codegen_write_asm("andwf %s, w", name);
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);

}

static void
do_and(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("andlw %#x", value);
    } else {
      codegen_write_asm("andwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    and_byte(is_const, value & 0xff, name, 0);
    and_byte(is_const, (value >> 8) & 0xff, name, 1);
    break;
  case size_uint24:
  case size_int24:
    and_byte(is_const, value & 0xff, name, 0);
    and_byte(is_const, (value >> 8) & 0xff, name, 1);
    and_byte(is_const, (value >> 16) & 0xff, name, 2);
    break;
  case size_uint32:
  case size_int32:
    and_byte(is_const, value & 0xff, name, 0);
    and_byte(is_const, (value >> 8) & 0xff, name, 1);
    and_byte(is_const, (value >> 16) & 0xff, name, 2);
    and_byte(is_const, (value >> 24) & 0xff, name, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
or_byte(gp_boolean is_const, int value, char *name, int byte)
{

  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("iorlw %#x", value);
  } else {
    codegen_write_asm("iorwf %s, w", name);
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);

}

static void
do_or(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("iorlw %#x", value);
    } else {
      codegen_write_asm("iorwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    or_byte(is_const, value & 0xff, name, 0);
    or_byte(is_const, (value >> 8) & 0xff, name, 1);
    break;
  case size_uint24:
  case size_int24:
    or_byte(is_const, value & 0xff, name, 0);
    or_byte(is_const, (value >> 8) & 0xff, name, 1);
    or_byte(is_const, (value >> 16) & 0xff, name, 2);
    break;
  case size_uint32:
  case size_int32:
    or_byte(is_const, value & 0xff, name, 0);
    or_byte(is_const, (value >> 8) & 0xff, name, 1);
    or_byte(is_const, (value >> 16) & 0xff, name, 2);
    or_byte(is_const, (value >> 24) & 0xff, name, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
xor_byte(gp_boolean is_const, int value, char *name, int byte)
{

  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("xorlw %#x", value);
  } else {
    codegen_write_asm("xorwf %s, w", name);
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);

}

static void
do_xor(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("xorlw %#x", value);
    } else {
      codegen_write_asm("xorwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    xor_byte(is_const, value & 0xff, name, 0);
    xor_byte(is_const, (value >> 8) & 0xff, name, 1);
    break;
  case size_uint24:
  case size_int24:
    xor_byte(is_const, value & 0xff, name, 0);
    xor_byte(is_const, (value >> 8) & 0xff, name, 1);
    xor_byte(is_const, (value >> 16) & 0xff, name, 2);
    break;
  case size_uint32:
  case size_int32:
    xor_byte(is_const, value & 0xff, name, 0);
    xor_byte(is_const, (value >> 8) & 0xff, name, 1);
    xor_byte(is_const, (value >> 16) & 0xff, name, 2);
    xor_byte(is_const, (value >> 24) & 0xff, name, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_not(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    codegen_write_asm("xorlw 1");
    break;
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    codegen_write_asm("movlw 1");
    codegen_write_asm("xorwf %s, f", WORKING_LABEL);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
com_byte(int byte)
{
  codegen_write_asm("movlw 0xff");
  codegen_write_asm("xorwf %s + %i, f", WORKING_LABEL, byte);
}

static void
do_com(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    codegen_write_asm("xorlw 0xff"); 
    break;
  case size_uint16:
  case size_int16:
    com_byte(0);
    com_byte(1);
    break;
  case size_uint24:
  case size_int24:
    com_byte(0);
    com_byte(1);
    com_byte(2);
    break;
  case size_uint32:
  case size_int32:
    com_byte(0);
    com_byte(1);
    com_byte(2);
    com_byte(3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

/****************************************************************************/
/* Shift                                                                    */
/****************************************************************************/

static void
do_lsh(enum size_tag size, gp_boolean is_const, int value, char *name)
{
  char *reg1 = NULL;
  char *reg2 = NULL;
  char *label1 = NULL;
  char *label2 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    reg1 = codegen_get_temp(size);
    reg2 = codegen_get_temp(size);
    label1 = codegen_next_label();
    label2 = codegen_next_label();
    codegen_write_asm("movwf %s", reg2);    /* store the number of shifts in temp*/
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
    } else {
      codegen_write_asm("movwf %s", name);
    }
    codegen_write_asm("movwf %s", reg1);    /* store the number to be shifted */
    codegen_write_asm("movf %s", reg2);     /* test number of shifts */
    codegen_write_asm("btfsc STATUS, Z");
    codegen_write_asm("goto %s", label2);
    codegen_write_label(label1);
    codegen_write_asm("bcf STATUS, C");
    codegen_write_asm("rlf %s, f", reg1); 
    codegen_write_asm("decfsz %s", reg2);
    codegen_write_asm("goto %s", label1);
    codegen_write_label(label2);
    codegen_write_asm("movf %s, w", reg1);  /* move the result into w */
    break;
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

  if (reg2)
    free(reg2);

  if (label1)
    free(label1);

  if (label2)
    free(label2);

}

static void
do_rsh(enum size_tag size, gp_boolean is_const, int value, char *name)
{
  char *reg1 = NULL;
  char *reg2 = NULL;
  char *label1 = NULL;
  char *label2 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    reg1 = codegen_get_temp(size);
    reg2 = codegen_get_temp(size);
    label1 = codegen_next_label();
    label2 = codegen_next_label();
    codegen_write_asm("movwf %s", reg2);    /* store the number of shifts in temp*/
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
    } else {
      codegen_write_asm("movwf %s", name);
    }
    codegen_write_asm("movwf %s", reg1);    /* store the number to be shifted */
    codegen_write_asm("movf %s", reg2);     /* test number of shifts */
    codegen_write_asm("btfsc STATUS, Z");
    codegen_write_asm("goto %s", label2);
    codegen_write_label(label1);
    codegen_write_asm("bcf STATUS, C");
    codegen_write_asm("rrf %s, f", reg1); 
    codegen_write_asm("decfsz %s", reg2);
    codegen_write_asm("goto %s", label1);
    codegen_write_label(label2);
    codegen_write_asm("movf %s, w", reg1);  /* move the result into w */
    break;
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

  if (reg2)
    free(reg2);

  if (label1)
    free(label1);

  if (label2)
    free(label2);

}

/****************************************************************************/
/* Compare                                                                  */
/****************************************************************************/

static void
do_eq(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    do_xor(size, is_const, value, name);    
    gen_boolean(size);
    do_xor(size, true, 1, NULL);    
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_ne(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    do_xor(size, is_const, value, name);    
    gen_boolean(size);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_lt(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
    do_sub(size, is_const, value, name);    
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 1");
    codegen_write_asm("xorlw 1");
    break;
  case size_uint16:
  case size_uint24:
  case size_uint32:

  case size_int8:
  case size_int16:
  case size_int24:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

}

static void
do_gt(enum size_tag size, gp_boolean is_const, int value, char *name)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
    if (is_const) {
      codegen_write_asm("sublw %#x", value);
    } else {
      codegen_write_asm("subwf %s, w", name);
    }
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 1");
    break;
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

}

static void
do_gte(enum size_tag size, gp_boolean is_const, int value, char *name)
{
  char *reg1 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
    reg1 = codegen_get_temp(size);
    codegen_write_asm("movwf %s", reg1);
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
    } else {
      codegen_write_asm("movf %s, w", name);
    }
    codegen_write_asm("subwf %s, w", reg1);
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 1");
    codegen_write_asm("xorlw 1");
    break;
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

}

static void
do_lte(enum size_tag size, gp_boolean is_const, int value, char *name)
{
  char *reg1 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
    reg1 = codegen_get_temp(size);
    codegen_write_asm("movwf %s", reg1);
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
    } else {
      codegen_write_asm("movf %s, w", name);
    }    
    codegen_write_asm("subwf %s, w", reg1);
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 1");
    break;
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

}

static void
codegen14(enum node_op op, 
          enum size_tag size,
          gp_boolean is_const,
          int value,
          char *name)
{
  switch (op) {
  case op_add:
    do_add(size, is_const, value, name);
    break;
  case op_sub:
    do_sub(size, is_const, value, name);
    break;
  case op_mult:
    do_mult(size, is_const, value, name);
    break;
  case op_div:
    do_div(size, is_const, value, name);
    break;
  case op_mod:
    do_mod(size, is_const, value, name);
    break;
  case op_neg:
    do_neg(size, is_const, value, name);
    break;
  case op_com:
    do_com(size, is_const, value, name);
    break;
  case op_and:
    do_and(size, is_const, value, name);
    break;
  case op_or:
    do_or(size, is_const, value, name);
    break;
  case op_xor:
    do_xor(size, is_const, value, name);
    break;
  case op_not:
    do_not(size, is_const, value, name);
    break;
  case op_lsh:
    do_lsh(size, is_const, value, name);
    break;
  case op_rsh:
    do_rsh(size, is_const, value, name);
    break;
  case op_eq:
    do_eq(size, is_const, value, name);
    break;
  case op_ne:
    do_ne(size, is_const, value, name);
    break;
  case op_gt:
    do_gt(size, is_const, value, name);
    break;
  case op_lt:
    do_lt(size, is_const, value, name);
    break;
  case op_gte:
    do_gte(size, is_const, value, name);
    break;
  case op_lte: 
    do_lte(size, is_const, value, name);
    break;
  default:
    assert(0); /* Unhandled binary operator */
  }

}

struct function_pointer_struct codegen14_func = {
  (long int)codegen14,
  (long int)load_constant14,
  (long int)load_file14,
  (long int)store_file14,
};
