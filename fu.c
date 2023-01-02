
/*
 * fu.c
 * 
   This module was originally written by Paul Kohout.  It was later
   adapted for the RISC-V ISA.

 * Donald Yeung
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fu.h"
#include "pipeline.h"

#define MAX_FIELD_LENGTH 100

const char fu_group_int_name[] = "INT";
const char fu_group_mem_name[] = "MEM";
const char fu_group_add_name[] =  "ADD";
const char fu_group_mult_name[] =  "MULT";
const char fu_group_div_name[] =  "DIV";


/*
  {{name, fu_group_num, operation, data_type}, sub_table}
*/

const op_level1_t op_table[] = {
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_int_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_fp_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"jal",    FU_GROUP_BRANCH, OPERATION_JAL,  DATA_TYPE_NONE},NULL},
  {{"jalr",   FU_GROUP_BRANCH, OPERATION_JALR, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_br_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_imm_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_load_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_store_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"halt",   FU_GROUP_HALT,   OPERATION_NONE, DATA_TYPE_NONE},NULL}
};

const op_level2_t op_int_table[] = {
  {{"add",    FU_GROUP_INT,    OPERATION_ADD,  DATA_TYPE_NONE},NULL},
  {{"and",    FU_GROUP_INT,    OPERATION_AND,  DATA_TYPE_NONE},NULL},
  {{"or",     FU_GROUP_INT,    OPERATION_OR,   DATA_TYPE_NONE},NULL},
  {{"xor",    FU_GROUP_INT,    OPERATION_XOR,  DATA_TYPE_NONE},NULL},
  {{"sll",    FU_GROUP_INT,    OPERATION_SLL,  DATA_TYPE_NONE},NULL},
  {{"srl",    FU_GROUP_INT,    OPERATION_SRL,  DATA_TYPE_NONE},NULL},
  {{"slt",    FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_intrr_table},
};

const op_level2_t op_fp_table[] = {
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_fprr_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level2_t op_br_table[] = {
  {{"beq",    FU_GROUP_BRANCH, OPERATION_BEQ,  DATA_TYPE_NONE},NULL},
  {{"bne",    FU_GROUP_BRANCH, OPERATION_BNE,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level2_t op_imm_table[] = {
  {{"addi",   FU_GROUP_INT,    OPERATION_ADD,  DATA_TYPE_NONE},NULL},
  {{"andi",   FU_GROUP_INT,    OPERATION_AND,  DATA_TYPE_NONE},NULL},
  {{"ori",    FU_GROUP_INT,    OPERATION_OR,   DATA_TYPE_NONE},NULL},
  {{"xori",   FU_GROUP_INT,    OPERATION_XOR,  DATA_TYPE_NONE},NULL},
  {{"slli",   FU_GROUP_INT,    OPERATION_SLL,  DATA_TYPE_NONE},NULL},
  {{"srli",   FU_GROUP_INT,    OPERATION_SRL,  DATA_TYPE_NONE},NULL},
  {{"slti",   FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},NULL},
  {{"sltiu",  FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},NULL},
};

const op_level2_t op_load_table[] = {
  {{"lw",     FU_GROUP_MEM,    OPERATION_LOAD, DATA_TYPE_W},   NULL},
  {{"flw",    FU_GROUP_MEM,    OPERATION_LOAD, DATA_TYPE_F},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level2_t op_store_table[] = {
  {{"sw",     FU_GROUP_MEM,    OPERATION_STORE,DATA_TYPE_W},   NULL},
  {{"fsw",    FU_GROUP_MEM,    OPERATION_STORE,DATA_TYPE_F},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level3_t op_intrr_table[] = {
  {"sltu",   FU_GROUP_INT,    OPERATION_SLTU, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {"sub",    FU_GROUP_INT,    OPERATION_SUB,  DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE}
};

const op_level3_t op_fprr_table[] = {
  {"fadd.s", FU_GROUP_ADD,    OPERATION_ADD,  DATA_TYPE_F},
  {"fsub.s", FU_GROUP_ADD,    OPERATION_SUB,  DATA_TYPE_F},
  {"fmult.s",FU_GROUP_MULT,   OPERATION_MULT, DATA_TYPE_F},
  {"fdiv.s", FU_GROUP_DIV,    OPERATION_DIV,  DATA_TYPE_F},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE}
};


state_t *
state_create(int *data_count,FILE *bin_file,FILE *fu_file,int wbpi,int wbpf) {
  state_t *state;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  if (wbpi <= 0 || wbpf <= 0) {
    fprintf (stderr, "error: register files must have at least one writeback port each");
    return NULL;
  }

  state = (state_t *)malloc(sizeof(state_t));
  if(state == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return NULL;
  }

  memset(state, 0, sizeof(state_t));

  for (i = 0; i < NUMREGS; i++) {
    state->rf_int.tag[i] = -1;
    state->rf_fp.tag[i] = -1;
  }

  /* allocate and initialize queues */
  state->IQ = (IQ_t *)malloc(sizeof(IQ_t)*IQ_SIZE);
  state->IQ_head = state->IQ_tail = 0;
  state->CQ = (CQ_t *)malloc(sizeof(CQ_t)*CQ_SIZE);
  state->CQ_head = state->CQ_tail = 0;
  state->ROB = (ROB_t *)malloc(sizeof(ROB_t)*ROB_SIZE);
  state->ROB_head = state->ROB_tail = 0;

  /* read machine-code file into instruction/data memory (starting at address 0) */
  i=0;
  while (!feof(bin_file)) {
    if (fread(&state->mem[i], 1, 1, bin_file) != 0) {
      i++;
    } else if (!feof(bin_file)) {
      fprintf(stderr, "error: cannot read address 0x%X from binary file\n", i);
      return NULL;      
    }
  }
  if (data_count != NULL)
    *data_count = i;

  /* allocate and initialize functional unit linked lists */
  while (!feof(fu_file)) {

    sprintf(field_fmt, " %%%d[^,\n]", MAX_FIELD_LENGTH);
    if (fscanf(fu_file, field_fmt, field) != 1) {
      fprintf(stderr, "error: cannot parse options file\n");
      return NULL;
    }
    i = strlen(field)-1;
    while (i > 0 && (field[i] == ' ' || field[i] == '\t'))
      field[i--] = 0;

    if (strcmp(field, fu_group_int_name) == 0) {
      if (fu_int_read(&state->fu_int_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_mem_name) == 0) {
      if (fu_mem_read(&state->fu_mem_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_add_name) == 0) {
      if (fu_fp_read(&state->fu_add_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_mult_name) == 0) {
      if (fu_fp_read(&state->fu_mult_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_div_name) == 0) {
      if (fu_fp_read(&state->fu_div_list, fu_file) != 0) {
	return NULL;
      }
    } else {
      fprintf(stderr,"error: invalid functional unit group name\n");
      return NULL;
    }
  }
  if (state->fu_int_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_int_name);
    return NULL;
  }
  if (state->fu_mem_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_mem_name);
    return NULL;
  }
  if (state->fu_add_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_add_name);
    return NULL;
  }
  if (state->fu_mult_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_mult_name);
    return NULL;
  }
  if (state->fu_div_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_div_name);
    return NULL;
  }

  /* allocate and initialize integer writeback port array */
  state->wb_port_int = (wb_port_int_t *)malloc(wbpi * sizeof(wb_port_int_t));
  if (state->wb_port_int == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return NULL;
  }
  for (i = 0; i < wbpi; i++)
    state->wb_port_int[i].tag = -1;
  state->wb_port_int_num = wbpi;

  /* allocate and initialize floating point writeback port array */
  state->wb_port_fp = (wb_port_fp_t *)malloc(wbpf * sizeof(wb_port_fp_t));
  if (state->wb_port_fp == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return NULL;
  }
  for (i = 0; i < wbpf; i++)
    state->wb_port_fp[i].tag = -1;
  state->wb_port_fp_num = wbpf;

  /* initialize control update */
  state->branch_tag = -1;

  state->fetch_lock = FALSE;
  state->halt = 0;

  return state;
}


/* functions to parse FU file */
int
fu_int_read(fu_int_t **fu_int_list, FILE *file) {
  fu_int_t *fu_int;
  fu_int_stage_t *stage;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  sprintf(field_fmt, ", %%%d[^,\n]", MAX_FIELD_LENGTH);
  if (fscanf(file, field_fmt, field) != 1) {
    fprintf(stderr, "error: cannot parse options file\n");
    return -1;
  }
  i = strlen(field)-1;
  while (i > 0 && (field[i] == ' ' || field[i] == '\t'))
    field[i--] = 0;

  if (*fu_int_list == NULL) {
    *fu_int_list = (fu_int_t *)malloc(sizeof(fu_int_t));
    fu_int = *fu_int_list;
  } else {
    fu_int = *fu_int_list;
    while (fu_int->next != NULL) fu_int = fu_int->next;
    fu_int->next = (fu_int_t *)malloc(sizeof(fu_int_t));
    fu_int = fu_int->next;
  }
  if (fu_int == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }

  fu_int->name = (char *)malloc(strlen(field)+1);
  if (fu_int->name == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }
  fu_int->stage_list = NULL;
  fu_int->next = NULL;
  strcpy(fu_int->name, field);

  stage = NULL;
  while (fscanf(file, ", %d", &num) == 1) {

    if(num <= 0) {
      fprintf(stderr, "error: functional unit '%s' has a stage with an invalid number of cycles\n", fu_int->name);
      return -1;
    }

    stage = (fu_int_stage_t *)malloc(sizeof(fu_int_stage_t));
    if (stage == NULL) {
      fprintf(stderr, "error: unable to allocate resources\n");
      return -1;
    }
    stage->prev = fu_int->stage_list;
    fu_int->stage_list = stage;
    stage->num_cycles = num;
    stage->current_cycle = -1;
  }
  if (fu_int->stage_list == NULL) {
    fprintf(stderr, "error: functional unit '%s' has no stages\n", fu_int->name);
    return -1;
  }

  return 0;
}


int
fu_mem_read(fu_mem_t **fu_mem_list, FILE *file) {
  fu_mem_t *fu_mem;
  fu_mem_stage_t *stage;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  sprintf(field_fmt, ", %%%d[^,\n]", MAX_FIELD_LENGTH);
  if (fscanf(file, field_fmt, field) != 1) {
    fprintf(stderr, "error: cannot parse options file\n");
    return -1;
  }
  i = strlen(field)-1;
  while (i > 0 && (field[i] == ' ' || field[i] == '\t'))
    field[i--] = 0;

  if (*fu_mem_list == NULL) {
    *fu_mem_list = (fu_mem_t *)malloc(sizeof(fu_mem_t));
    fu_mem = *fu_mem_list;
  } else {
    fu_mem = *fu_mem_list;
    while (fu_mem->next != NULL) fu_mem = fu_mem->next;
    fu_mem->next = (fu_mem_t *)malloc(sizeof(fu_mem_t));
    fu_mem = fu_mem->next;
  }
  if (fu_mem == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }

  fu_mem->name = (char *)malloc(strlen(field)+1);
  if (fu_mem->name == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }
  fu_mem->stage_list = NULL;
  fu_mem->next = NULL;
  strcpy(fu_mem->name, field);

  stage = NULL;
  while (fscanf(file, ", %d", &num) == 1) {

    if(num <= 0) {
      fprintf(stderr, "error: functional unit '%s' has a stage with an invalid number of cycles\n", fu_mem->name);
      return -1;
    }

    stage = (fu_mem_stage_t *)malloc(sizeof(fu_mem_stage_t));
    if (stage == NULL) {
      fprintf(stderr, "error: unable to allocate resources\n");
      return -1;
    }
    stage->prev = fu_mem->stage_list;
    fu_mem->stage_list = stage;
    stage->num_cycles = num;
    stage->current_cycle = -1;
  }
  if (fu_mem->stage_list == NULL) {
    fprintf(stderr, "error: functional unit '%s' has no stages\n", fu_mem->name);
    return -1;
  }

  return 0;
}


int
fu_fp_read(fu_fp_t **fu_fp_list,FILE *file) {
  fu_fp_t *fu_fp;
  fu_fp_stage_t *stage;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  sprintf(field_fmt,", %%%d[^,\n]",MAX_FIELD_LENGTH);
  if(fscanf(file,field_fmt,field) != 1) {
    fprintf(stderr,"error: cannot parse options file\n");
    return -1;
  }
  i = strlen(field)-1;
  while(i > 0 && (field[i] == ' ' || field[i] == '\t'))
    field[i--] = 0;

  if(*fu_fp_list == NULL) {
    *fu_fp_list = (fu_fp_t *)malloc(sizeof(fu_fp_t));
    fu_fp = *fu_fp_list;
  } else {
    fu_fp = *fu_fp_list;
    while(fu_fp->next != NULL) fu_fp = fu_fp->next;
    fu_fp->next = (fu_fp_t *)malloc(sizeof(fu_fp_t));
    fu_fp = fu_fp->next;
  }
  if(fu_fp == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return -1;
  }

  fu_fp->name = (char *)malloc(strlen(field)+1);
  if(fu_fp->name == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return -1;
  }
  fu_fp->stage_list = NULL;
  fu_fp->next = NULL;
  strcpy(fu_fp->name,field);

  stage = NULL;
  while(fscanf(file,", %d",&num) == 1) {

    if(num <= 0) {
      fprintf(stderr,"error: functional unit '%s' has a stage with an invalid number of cycles\n",fu_fp->name);
      return -1;
    }

    stage = (fu_fp_stage_t *)malloc(sizeof(fu_fp_stage_t));
    if(stage == NULL) {
      fprintf(stderr,"error: unable to allocate resources\n");
      return -1;
    }
    stage->prev = fu_fp->stage_list;
    fu_fp->stage_list = stage;
    stage->num_cycles = num;
    stage->current_cycle = -1;
  }
  if(fu_fp->stage_list == NULL) {
    fprintf(stderr,"error: functional unit '%s' has no stages\n",fu_fp->name);
    return -1;
  }

  return 0;
}


/* Functions to allocate functional units */
int
issue_fu_int(fu_int_t *fu_list, int tag, int branch, int link) {
  fu_int_t *fu;
  fu_int_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage->prev != NULL)
      stage = stage->prev;
    if (stage->current_cycle == -1) {
      stage->current_cycle = stage->num_cycles-1;
      stage->tag = tag;
      stage->branch = branch;
      stage->link = link;
      return 0;
    }
    fu = fu->next;
  }
  return -1;   // structural hazard... stall
}


int
issue_fu_mem(fu_mem_t *fu_list, int tag, int float_mem, int store) {
  fu_mem_t *fu;
  fu_mem_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage->prev != NULL)
      stage = stage->prev;
    if (stage->current_cycle == -1) {
      if (store)
	/* stores are issued in commit, so they'll go through 1
	   execute in the same cycle */
	stage->current_cycle = stage->num_cycles;
      else
	stage->current_cycle = stage->num_cycles-1;
      stage->tag = tag;
      stage->float_mem = float_mem;
      stage->store = store;
      return 0;
    }
    fu = fu->next;
  }
  return -1;   // structural hazard... stall
}


int
issue_fu_fp(fu_fp_t *fu_list, int tag) {
  fu_fp_t *fu;
  fu_fp_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage->prev != NULL)
      stage = stage->prev;
    if (stage->current_cycle == -1) {
      stage->current_cycle = stage->num_cycles-1;
      stage->tag = tag;
      return 0;
    }
    fu = fu->next;
  }
  return -1;   // structural hazard... stall
}


/* functions to cycle functional units */
void
advance_fu_int(fu_int_t *fu_list, wb_port_int_t wb_port[], int wb_port_num,
	       int *branch_tag) {
  fu_int_t *fu;
  fu_int_stage_t *stage, *next_stage;
  int i;

  fu = fu_list;
  while(fu != NULL) {
    stage = fu->stage_list;
    next_stage = NULL;
    while (stage != NULL) {
      switch (stage->current_cycle) {
	/* is fu stage free? */
      case -1:
	break;                                      /* do nothing */

	/* is fu stage done processing? */
      case 0:
	if (next_stage == NULL) {                    /* is this the last stage in the fu? */
	  if (stage->branch) {
	    if (*branch_tag == -1) {
	      if (stage->link) {
		if (wb_int(wb_port, wb_port_num, stage->tag) == 0) {
		  stage->current_cycle = -1;
		  *branch_tag = stage->tag;
		}
	      } else {
		stage->current_cycle = -1;
		*branch_tag = stage->tag;
	      }
	    }
	  } else {
	    if (wb_int(wb_port, wb_port_num, stage->tag) == 0)
	      stage->current_cycle = -1;
	  }
	} else {
	  if (next_stage->current_cycle == -1) {     /* move to next fu stage */
	    next_stage->tag = stage->tag;
	    next_stage->current_cycle = next_stage->num_cycles-1;
	    next_stage->branch = stage->branch;
	    next_stage->link = stage->link;
	    stage->current_cycle = -1;
	  }
	}
	break;

	/*  fu stage is still processing */
      default:
	stage->current_cycle--;
      }
      next_stage = stage;
      stage = stage->prev;
    }
    fu = fu->next;
  }
}

void
advance_fu_mem(fu_mem_t *fu_list, wb_port_int_t wb_int_port[], int wb_int_port_num,
	       wb_port_fp_t wb_fp_port[], int wb_fp_port_num) {
  fu_mem_t *fu;
  fu_mem_stage_t *stage, *next_stage;
  int i;

  fu = fu_list;
  while(fu != NULL) {
    stage = fu->stage_list;
    next_stage = NULL;
    while (stage != NULL) {
      switch (stage->current_cycle) {
	/* is fu stage free? */
      case -1:
	break;                                      /* do nothing */

	/* is fu stage done processing? */
      case 0:
	if (next_stage == NULL) {                    /* is this the last stage in the fu? */
	  if (stage->store) {
	    stage->current_cycle = -1;
	  } else {
	    if (stage->float_mem) {
	      if (wb_fp(wb_fp_port, wb_fp_port_num, stage->tag) == 0)
		stage->current_cycle = -1;
	    } else {
	      if (wb_int(wb_int_port, wb_int_port_num, stage->tag) == 0)
		stage->current_cycle = -1;
	    }
	  }
	} else {
	  if (next_stage->current_cycle == -1) {     /* move to next fu stage */
	    next_stage->tag = stage->tag;
	    next_stage->current_cycle = next_stage->num_cycles-1;
	    next_stage->float_mem = stage->float_mem;
	    next_stage->store = stage->store;
	    stage->current_cycle = -1;
	  }
	}
	break;

	/* fu stage is still processing */
      default:
	stage->current_cycle--;
      }
      next_stage = stage;
      stage = stage->prev;
    }
    fu = fu->next;
  }
}

void
advance_fu_fp(fu_fp_t *fu_list,wb_port_fp_t wb_port[],int wb_port_num) {
  fu_fp_t *fu;
  fu_fp_stage_t *stage, *next_stage;
  int i;

  fu = fu_list;
  while(fu != NULL) {
    stage = fu->stage_list;
    next_stage = NULL;
    while(stage != NULL) {
      switch(stage->current_cycle) {
	/* is fu stage free? */
      case -1:                                      /* do nothing */
	break;

	/* is fu stage done processing? */
      case 0:
	if(next_stage == NULL) {                  /* is this the last stage in the fu? */
	  if(wb_fp(wb_port, wb_port_num, stage->tag) == 0) {
	    stage->current_cycle = -1;
	  }
	} else {
	  if(next_stage->current_cycle == -1) {                                  /* move to next fu stage */
	    next_stage->tag = stage->tag;
	    next_stage->current_cycle = next_stage->num_cycles-1;
	    stage->current_cycle = -1;
	  }
	}
	break;

	/* fu stage is still processing */
      default:   
	stage->current_cycle--;
      }
      next_stage = stage;
      stage = stage->prev;
    }
    fu = fu->next;
  }
}


/* writeback arbitration functions */
int
wb_int(wb_port_int_t wb_port[], int wb_port_num, int tag) {
  int i;

  for(i = 0; i < wb_port_num; i++) {
    if(wb_port[i].tag == -1) {
      wb_port[i].tag = tag;
      return 0;
    }
  }
  return -1;   // structural hazard... stall
}

int
wb_fp(wb_port_fp_t wb_port[], int wb_port_num, int tag) {
  int i;

  for(i = 0; i < wb_port_num; i++) {
    if(wb_port[i].tag == -1) {
      wb_port[i].tag = tag;
      return 0;
    }
  }
  return -1;   // structural hazard... stall
}


/* decode an instruction */
const op_info_t *
decode_instr(int instr, int *use_imm) {
  const op_info_t *op_info;

  if (op_table[FIELD_OPCODE(instr)].level2_table == NULL) {
    /* Level 1 table */
    op_info = &op_table[FIELD_OPCODE(instr)].info;
  } else if (op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].level3_table == NULL) {
    /* Level 2 table */
    op_info = &op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].info;
  } else {
    /* Level 3 table */
    op_info = &op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].level3_table[FIELD_FUNC7(instr)].info;
  }

  if ((FIELD_OPCODE(instr) == 0) || (FIELD_OPCODE(instr) == 1))
    *use_imm = 0;
  else
    *use_imm = 1;

  return op_info;
}

/* perform an instruction */
operand_t perform_operation(int instr, unsigned long pc, operand_t operand1,
		  operand_t operand2) {
  const op_info_t *op_info, *op_info_fp;
  int use_imm, use_imm_fp;
  operand_t result;
  float f1;
  int i1;
  op_info = decode_instr(instr, &use_imm);
  op_info_fp = decode_instr(instr, &use_imm_fp);
  switch(op_info->fu_group_num) {
  case FU_GROUP_INT:
    switch(op_info->operation) {
    case OPERATION_ADD:
      result.integer.wu = (operand1.integer.wu + operand2.integer.wu);
      break;
      //case OPERATION_ADDU:
	//		result.integer.wu = operand1.integer.wu + operand2.integer.wu; //unsigned
	//		break;
		case OPERATION_SUB:
			result.integer.wu = (operand1.integer.wu - operand2.integer.wu);
			break;
	//	case OPERATION_SUBU:
	//		result.integer.wu = operand1.integer.wu - operand2.integer.wu; //unsigned
	//		break;
		case OPERATION_MULT:
			result.integer.w = operand1.integer.w * operand2.integer.w;
			break;
		case OPERATION_DIV:
			result.integer.w = operand1.integer.w / operand2.integer.w;
			break;
		case OPERATION_SLL:
			result.integer.w = operand1.integer.w << operand2.integer.w;
			break;
		case OPERATION_SRL:
			result.integer.w = operand1.integer.w >> operand2.integer.w;
			break;
		case OPERATION_AND:
			result.integer.w = operand1.integer.w & operand2.integer.w;
			break;
		case OPERATION_OR:
			result.integer.w = operand1.integer.w | operand2.integer.w;
			break;
		case OPERATION_XOR:
			result.integer.w = operand1.integer.w ^ operand2.integer.w;
			break;
		case OPERATION_SLT:
			result.integer.w = operand1.integer.w < operand2.integer.w;
			break;
	//	case OPERATION_SGT:
	//		result.integer.w = operand1.integer.w > operand2.integer.w;
	//		break;
	//	case OPERATION_SLTU:
	//		result.integer.wu = operand1.integer.wu < operand2.integer.wu; //unsigned
	//		break;
	//	case OPERATION_SGTU:
	//		result.integer.wu = operand1.integer.wu > operand2.integer.wu; //unsigned
	//		break;
		default:
			break;
    }
    break;

  case FU_GROUP_ADD:
    switch(op_info_fp->operation) {
      case OPERATION_ADD:
        result.flt = operand1.flt + operand2.flt;
      break;

      case OPERATION_SUB:
        result.flt = operand1.flt - operand2.flt;
      break;
    }
    break;
  case FU_GROUP_MULT:
    result.flt = operand1.flt * operand2.flt;
    break;
  case FU_GROUP_DIV:
     result.flt = operand1.flt / operand2.flt;
    break;

  case FU_GROUP_MEM:
    switch (op_info->operation)
    {
    case OPERATION_STORE:
      /* code */
      break;
    
    default:
      break;
    }
    break;

//  case FU_GROUP_BRANCH:
   	//switch(op_info->operation){
		//case OPERATION_J:
		//case OPERATION_JAL:
		//	result.integer.w = pc + operand1.integer.w + 4;//operand1 is the offset value
		//	break;
		//case OPERATION_JR:
		//case OPERATION_JALR:
		//	result.integer.w = operand1.integer.w;
		//	break;
		//case OPERATION_BEQZ:
		//case OPERATION_BNEZ:
		//	result.integer.w = pc + operand1.integer.w + 4;//operand1 is the immediate value, need to check whether (r1)==0? or (r1)!=0
		//	break;
//	}
  break;

  case FU_GROUP_HALT:
    break;

  case FU_GROUP_NONE:
    break;

  case FU_GROUP_INVALID:
    fprintf(stderr, "error: invalid opcode (instr = %.8X)\n", instr);
  }
  return result;
}
