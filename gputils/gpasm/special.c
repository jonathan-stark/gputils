/* Implements special instruction mnemonics
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "gpasm.h"
#include "gpmsg.h"
#include "special.h"
#include "directive.h"
#include "evaluate.h"

/*------------------------------------------------------------------------------------------------*/

static pnode_t *
_make_constant_list(int value1, int value2)
{
  return mk_list(mk_constant(value1), mk_list(mk_constant(value2), NULL));
}

/*------------------------------------------------------------------------------------------------*/

static pnode_t *
_add_symbol_constant(pnode_t *parms, int value)
{
  return mk_list(PnListHead(parms), mk_list(mk_constant(value), NULL));
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_addcf(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  gpmsg_vmessage(GPM_SPECIAL_MNEMONIC, NULL);

  do_insn("btfsc", _make_constant_list(3, 0));
  do_insn("incf", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_adddcf(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfsc", _make_constant_list(3, 1));
  do_insn("incf", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_b(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfsc", _make_constant_list(3, 0));
  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bdc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfsc", _make_constant_list(3, 1));
  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bz(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfsc", _make_constant_list(3, 2));
  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bnc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfss", _make_constant_list(3, 0));
  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bndc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfss", _make_constant_list(3, 1));
  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bnz(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfss", _make_constant_list(3, 2));
  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_clrc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bcf", _make_constant_list(3, 0));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_clrdc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bcf", _make_constant_list(3, 1));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_clrz(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bcf", _make_constant_list(3, 2));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_lcall(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("pagesel", parms);
  do_insn("call", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_lgoto(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("pagesel", parms);
  do_insn("goto", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_movfw(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (eval_enforce_arity(arity, 1)) {
    do_insn("movf", _add_symbol_constant(parms, 0));
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_negf(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if ((arity == 1) || (arity == 2)) {
    do_insn("comf", _add_symbol_constant(parms, 1));
    do_insn("incf", parms);
  } else {
    eval_enforce_arity(arity, 2);
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_setc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bsf", _make_constant_list(3, 0));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_setdc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bsf", _make_constant_list(3, 1));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_setz(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bsf", _make_constant_list(3, 2));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfss", _make_constant_list(3, 0));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpdc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfss", _make_constant_list(3, 1));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpz(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfss", _make_constant_list(3, 2));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpnc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfsc", _make_constant_list(3, 0));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpndc(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfsc", _make_constant_list(3, 1));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpnz(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfsc", _make_constant_list(3, 2));

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_subcf(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfsc", _make_constant_list(3, 0));
  do_insn("decf", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_subdcf(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;
  (void)arity;

  do_insn("btfsc", _make_constant_list(3, 1));
  do_insn("decf", parms);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_tstf(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  (void)name;

  if (eval_enforce_arity(arity, 1)) {
    do_insn("movf", _add_symbol_constant(parms, 1));
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_mode(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  pnode_t *val;

  (void)name;

  if (eval_enforce_arity(arity, 1)) {
    val = PnListHead(parms);

    if (PnIsConstant(val) && (PnConstant(val) > 0x1f)) {
      gpmsg_vwarning(GPW_RANGE, NULL);
      PnConstant(val) &= 0x1f;
    }

    do_insn("movlw", parms);
    do_insn("movwm", NULL);
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* PIC 12-bit and 14-bit "Special" instruction set. */
const insn_t special[] = {
  { "addcf",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_addcf  },
  { "adddcf", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_adddcf },
  { "b",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_b      },
  { "bc",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bc     },
  { "bdc",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bdc    },
  { "bz",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bz     },
  { "bnc",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bnc    },
  { "bndc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bndc   },
  { "bnz",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bnz    },
  { "clrc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_clrc   },
  { "clrdc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_clrdc  },
  { "clrz",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_clrz   },
  { "lcall",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_lcall  },
  { "lgoto",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_lgoto  },
  { "movfw",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_movfw  },
  { "negf",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_negf   },
  { "setc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_setc   },
  { "setdc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_setdc  },
  { "setz",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_setz   },
  { "skpc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpc   },
  { "skpdc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpdc  },
  { "skpz",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpz   },
  { "skpnc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpnc  },
  { "skpndc", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpndc },
  { "skpnz",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpnz  },
  { "subcf",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_subcf  },
  { "subdcf", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_subdcf },
  { "tstf",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_tstf   }
};

const int num_op_special = TABLE_SIZE(special);

const insn_t op_sx_mode = { 
  "mode", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_mode
};
