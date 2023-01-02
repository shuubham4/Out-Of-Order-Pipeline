#include <stdlib.h>
#include "string.h"
#include "fu.h"
#include "pipeline.h"
int
commit(state_t *state, int*num1 ) {
       int imm_d;
       int instr = state->ROB[state->ROB_head].instr;
       const op_info_t *op_info = decode_instr(instr, &imm_d);
       if (state->ROB[state->ROB_head].completed == TRUE && instr != 0){
          switch(op_info->fu_group_num){
                case FU_GROUP_INT:
                                 state->rf_int.reg_int.integer[FIELD_RD(instr)].wu = state->ROB[state->ROB_head].result.integer.wu; 
                                 if (state->rf_int.tag[FIELD_RD(instr)] == state->ROB_head){
                                    state->rf_int.tag[FIELD_RD(instr)] = -1;
                                 }                         
                                 *num1 = *num1 + 1;
                                 state->ROB_head = (state->ROB_head + 1)%64;
                                 break;
                case FU_GROUP_MEM:
                                 switch(op_info->operation){
                                       case OPERATION_STORE:
                                           //*num1 = *num1 + 1;
                                           switch (op_info->data_type){
                                                   case DATA_TYPE_W:  
                                                       if (issue_fu_mem(state->fu_mem_list, state-> ROB_head,  0, 1) != -1){ 
                                                          memcpy(&(state->mem[state->ROB[state->ROB_head].target.integer.wu]), 
                                                                  &(state->rf_int.reg_int.integer[FIELD_RS2(instr)].wu), 4); 
                                                          state->ROB_head = (state->ROB_head + 1)%64;  
                                                          *num1 = *num1 + 1; 
                                                       }
                                                       break;
                                                   case DATA_TYPE_F:
                                                       if (issue_fu_mem(state->fu_mem_list, state-> ROB_head,  1, 1) != -1){
                                                          memcpy(&(state->mem[state->ROB[state->ROB_head].target.integer.wu]),
                                                                  &(state->rf_fp.reg_fp.flt[FIELD_RS2(instr)]), 4);
                                                          state->ROB_head = (state->ROB_head + 1)%64;
                                                          *num1 = *num1 + 1;
                                                       }
                                                       break;                                    
                                           }
                                           break;

                                       case OPERATION_LOAD:
                                            *num1 = *num1 + 1;
                                            switch(op_info->data_type){ 
                                                  case DATA_TYPE_W: 
                                                       memcpy(&(state->rf_int.reg_int.integer[FIELD_RD(instr)].wu),
                                                               &(state->mem[state->ROB[state->ROB_head].target.integer.wu]), 4);
                                		       if (state->rf_int.tag[FIELD_RD(instr)] == state->ROB_head){
                                                          state->rf_int.tag[FIELD_RD(instr)] = -1;
                                                       }                                                   
                                                       break;
                                                  case DATA_TYPE_F:
                                                       memcpy(&(state->rf_fp.reg_fp.flt[FIELD_RD(instr)]),
                                                              &(state->mem[state->ROB[state->ROB_head].target.integer.wu]), 4);
                                                       if (state->rf_fp.tag[FIELD_RD(instr)] == state->ROB_head){
                                                          state->rf_fp.tag[FIELD_RD(instr)] = -1;
                                                       }
                                                       break;

                                                  default: break;
                                            }
                                            state->ROB_head = (state->ROB_head + 1)%64;
                                            break;  // break from operation case                                
                                 }                                     
                                 break;
                case FU_GROUP_HALT: state->halt = 1;
                                    *num1 = *num1 + 1;
                                    state->ROB_head = (state->ROB_head + 1)%64;
                                    break;

                case FU_GROUP_ADD:
                case FU_GROUP_MULT:
                case FU_GROUP_DIV:
                                 *num1 = * num1 + 1;
                                 state->rf_fp.reg_fp.flt[FIELD_RD(instr)] = state->ROB[state->ROB_head].result.flt;
                                 if (state->rf_fp.tag[FIELD_RD(instr)] == state->ROB_head){
                                    state->rf_fp.tag[FIELD_RD(instr)] = -1;
                                 }
                                 state->ROB_head = (state->ROB_head + 1)%64;
                                 break;

                case FU_GROUP_BRANCH: 
                                *num1 = *num1 + 1;
                                 state->ROB_head = (state->ROB_head + 1)%64;  
                                 break;

            } // closed brackets for op_info->fu_group_num
           //state->ROB_head = (state->ROB_head + 1)%64; 
        }  // closed brackets for if      
}  // closed brackets for commit

void
writeback(state_t *state) {
 
     int imm_d,instr;
     const op_info_t *op_info;
     int i;
     int temp_CQ_head, temp_IQ_head;

     if (state->branch_tag != -1){
        instr = state->ROB[state->branch_tag].instr;
        state->ROB[state->branch_tag].completed = TRUE;
        op_info = decode_instr(instr, &imm_d);
        switch(op_info->fu_group_num){
              case FU_GROUP_BRANCH:
                   if (state->ROB[state->branch_tag].result.integer.wu == 1) {
                      state->pc = state->ROB[state->branch_tag].target.integer.wu;
                      (state->if_id).instr = NOP;
                   }
                   state->fetch_lock = FALSE;
                   break;
              default: break;
        }
        state->branch_tag = -1;
     }

    for (i = 0 ; i <= 1; i++){ 
       if (state->wb_port_int[i].tag != -1){
       if (state->wb_port_int[i].tag > 63){
           instr = state->ROB[state->wb_port_int[i].tag - 64].instr; 
       } 
       else if (state->wb_port_int[i].tag <= 63){
           instr = state->ROB[state->wb_port_int[i].tag].instr; 
       } 
       temp_IQ_head = state->IQ_head - 1;
       temp_CQ_head = state->CQ_head - 1;   
       op_info = decode_instr(instr, &imm_d);
       switch(op_info->fu_group_num){
             case FU_GROUP_INT: 
                       state->ROB[state->wb_port_int[i].tag].completed = TRUE;  
                       do { 
                              temp_IQ_head = (temp_IQ_head + 1)%32;
                              if (state->IQ[temp_IQ_head].tag1 == state->wb_port_int[i].tag) { 
                                  state->IQ[temp_IQ_head].tag1 = -1;                                                                   
                                  state->IQ[temp_IQ_head].operand1.integer.wu = state->ROB[state->wb_port_int[i].tag].result.integer.wu; 
                              }
                              if (state->IQ[temp_IQ_head].tag2 == state->wb_port_int[i].tag){  
                                 state->IQ[temp_IQ_head].tag2 = -1;
                                 state->IQ[temp_IQ_head].operand2.integer.wu = state->ROB[state->wb_port_int[i].tag].result.integer.wu; 
                              }
                       } while(temp_IQ_head != state->IQ_tail );

                       do { 
                             temp_CQ_head = (temp_CQ_head + 1)%32;
                             switch(state->CQ[temp_CQ_head].store){
                                    case 1:
                                          if (state->CQ[temp_CQ_head].tag2 == state->wb_port_int[i].tag){ 
                                             state->CQ[temp_CQ_head].tag2 = -1;
                                             state->CQ[temp_CQ_head].result = state->ROB[state->wb_port_int[i].tag].result;
                                          }
                                          break;
                             } 
                       } while(temp_CQ_head != state->CQ_tail );                       
                       break;

             case FU_GROUP_MEM: 
              switch (op_info->operation) { 
               case OPERATION_LOAD:

                        if (state->wb_port_int[i].tag <= 63) { 
                                 state->ROB[state->wb_port_int[i].tag].completed = TRUE; 
                                 do { 
                                         temp_IQ_head = (temp_IQ_head + 1)%32; 
                                         if (state->IQ[temp_IQ_head].tag1 == state->wb_port_int[i].tag){ 
                                                 state->IQ[temp_IQ_head].tag1 = -1;
                                                 state->IQ[temp_IQ_head].operand1.integer.wu = state->ROB[state->wb_port_int[i].tag].result.integer.wu;
                                         }
                                         if (state->IQ[temp_IQ_head].tag2 == state->wb_port_int[i].tag){ 
                                                 state->IQ[temp_IQ_head].tag2 = -1;
                                                 state->IQ[temp_IQ_head].operand2.integer.wu = state->ROB[state->wb_port_int[i].tag].result.integer.wu; 
                                         }
                                 } while(temp_IQ_head != state->IQ_tail);

                                 do { 
                                          temp_CQ_head = (temp_CQ_head + 1)%32;
                                          switch(state->CQ[temp_CQ_head].store){
                                                case 1:
                                                      if (state->CQ[temp_CQ_head].tag2 == state->wb_port_int[i].tag){
                                                         state->CQ[temp_CQ_head].tag2 = -1;
                                                         state->CQ[temp_CQ_head].result.integer.wu = state->ROB[state->wb_port_int[i].tag].result.integer.wu;      
                                                      }
                                                      break;
                                          }
                                 } while(temp_CQ_head != state->CQ_tail);
                        }

                        if (state->wb_port_int[i].tag >= 64) {  
                                 do {  
                                    temp_CQ_head = (temp_CQ_head + 1)%32;
                                    if (state->wb_port_int[i].tag ==  state->CQ[temp_CQ_head].tag1){
                                          state->CQ[temp_CQ_head].address = state->ROB[state->wb_port_int[i].tag].target;  
                                          state->CQ[temp_CQ_head].tag1 = -1;
                                          break;
                                    }
                                 } while (temp_CQ_head != state->CQ_tail);
                        }
                        break;
          
               case OPERATION_STORE: 
                         switch (op_info->data_type){
                              case DATA_TYPE_W:
                              case DATA_TYPE_F:
                                    if (state->wb_port_int[i].tag >= 64){
                                            do { 
                                                  temp_CQ_head = (temp_CQ_head + 1)%32;
                                                  if (state->wb_port_int[i].tag ==  state->CQ[temp_CQ_head].tag1){
                                                        state->CQ[temp_CQ_head].address = state->ROB[state->wb_port_int[i].tag].target;
                                                        state->CQ[temp_CQ_head].tag1 = -1;
                                                        break;
                                                  }
                                            } while (temp_CQ_head != state->CQ_tail);
                                     }
                                     break;
                         }
                     
          
             default: break;                          
            } // case FU_GROUP_MEM 
            break;                                 
           
          default: break;
       }//switch fu_group_mem 
       state->wb_port_int[i].tag = -1;
       }// if

       if (state->wb_port_fp[i].tag != -1){
          instr = state->ROB[state->wb_port_fp[i].tag].instr;
          temp_IQ_head = state->IQ_head - 1;
          temp_CQ_head = state->CQ_head - 1;
          op_info = decode_instr(instr, &imm_d);
          state->ROB[state->wb_port_fp[i].tag].completed = TRUE;
          switch(op_info->fu_group_num){
             case FU_GROUP_ADD:
             case FU_GROUP_MULT:
             case FU_GROUP_DIV:
                       do { 
                              temp_IQ_head = (temp_IQ_head + 1)%32;
                              if (state->IQ[temp_IQ_head].tag1 == state->wb_port_fp[i].tag){
                                 state->IQ[temp_IQ_head].tag1 = -1;
                                 state->IQ[temp_IQ_head].operand1.flt = state->ROB[state->wb_port_fp[i].tag].result.flt; 
                              }
                              if (state->IQ[temp_IQ_head].tag2 == state->wb_port_fp[i].tag){
                                 state->IQ[temp_IQ_head].tag2 = -1;
                                 state->IQ[temp_IQ_head].operand2.flt = state->ROB[state->wb_port_fp[i].tag].result.flt; 
                              }
                       } while(temp_IQ_head != state->IQ_tail);

                       do { 
                             temp_CQ_head = (temp_CQ_head + 1)%32;
                             switch(state->CQ[temp_CQ_head].store){
                                    case 1:
                                          if (state->CQ[temp_CQ_head].tag2 == state->wb_port_fp[i].tag){
                                             state->CQ[temp_CQ_head].tag2 = -1;
                                             state->CQ[temp_CQ_head].result = state->ROB[state->wb_port_fp[i].tag].result;
                                          }
                                          break;
                             }
                       } while(temp_CQ_head != state->CQ_tail);

                       break;
             case FU_GROUP_MEM:
              switch (op_info->operation) {
               case OPERATION_LOAD:
                        if (state->wb_port_int[i].tag <= 63) {
                                 do { 
                                         if (state->IQ[temp_IQ_head].tag1 == state->wb_port_fp[i].tag){
                                                 state->IQ[temp_IQ_head].tag1 = -1;
                                                 state->IQ[temp_IQ_head].operand1.flt = state->ROB[state->wb_port_fp[i].tag].result.flt;      
                                         }
                                         if (state->IQ[temp_IQ_head].tag2 == state->wb_port_fp[i].tag){
                                                 state->IQ[temp_IQ_head].tag2 = -1;
                                                 state->IQ[temp_IQ_head].operand2.flt = state->ROB[state->wb_port_fp[i].tag].result.flt;
                                         }
                                         temp_IQ_head = (temp_IQ_head + 1)%32;
                                 } while(temp_IQ_head != state->IQ_tail);


                                 do { 
                                          switch(state->CQ[temp_CQ_head].store){
                                                case 1:
                                                      if (state->CQ[temp_CQ_head].tag2 == state->wb_port_fp[i].tag){
                                                         state->CQ[temp_CQ_head].tag2 = -1;
                                                         state->CQ[temp_CQ_head].result.flt = state->ROB[state->wb_port_fp[i].tag].result.flt;
                                                      }
                                                      break;
                                          }
                                          temp_CQ_head = (temp_CQ_head + 1)%32;
                                 } while(temp_CQ_head != state->CQ_tail);
                        } // closed brackets for tag <= 63

                        break; // break from operation load 
            
             } // closed bracket for op_info->operation
             default: break;

          } // closed bracket from fu_group_num
 
       state->wb_port_fp[i].tag = -1;
       } //if      
    }// for 
}// function

void
execute(state_t *state) {        
    advance_fu_int(state->fu_int_list, state->wb_port_int, 2, &(state->branch_tag));
    advance_fu_mem(state->fu_mem_list, state->wb_port_int, 2, state->wb_port_fp, 2);
    advance_fu_fp(state->fu_add_list , state->wb_port_fp, 2);
    advance_fu_fp(state->fu_mult_list, state->wb_port_fp , 2);
    advance_fu_fp(state->fu_div_list , state->wb_port_fp, 2);
}

int
memory_disambiguation(state_t *state) {
   int temp_CQ_head = state->CQ_head - 1;
   int temp_CQ_tail = state->CQ_tail + 1;
   int invalid = 0, store_invalid = 0, load_invalid = 0;
   do { 
     temp_CQ_head = (temp_CQ_head + 1)%32;
     int instr = state->CQ[temp_CQ_head].instr;
     int imm_d;
     const op_info_t *op_info = decode_instr(instr, &imm_d);
     switch (state->CQ[temp_CQ_head].store){
          case 1:
                         if(state->CQ[temp_CQ_head].tag1 == -1 && state->CQ[temp_CQ_head].tag2 == -1) { 
                           state->CQ[temp_CQ_head].issued = TRUE;
                           state->ROB[state->CQ[temp_CQ_head].ROB_index].completed = TRUE; 
                           if (op_info->data_type == DATA_TYPE_W){
                           	state->ROB[state->CQ[temp_CQ_head].ROB_index].result.integer.wu = state->CQ[temp_CQ_head].result.integer.wu;
                           } 
                           if (op_info->data_type == DATA_TYPE_F){
                                state->ROB[state->CQ[temp_CQ_head].ROB_index].result.flt = state->CQ[temp_CQ_head].result.flt;
                           }                            
                           invalid = 1;
                         }
                         else if (state->CQ[temp_CQ_head].tag1 != -1 && state->CQ[temp_CQ_head].tag2 == -1)
                              store_invalid = 1;
                         break;
 
          case 0:
                 switch (op_info->data_type){
                     
                    case DATA_TYPE_W:   
                           if(state->CQ[temp_CQ_head].tag1 == -1 && state->CQ[temp_CQ_head].issued == FALSE) { 
                              issue_fu_mem(state->fu_mem_list, state->CQ[temp_CQ_head].ROB_index,  0, 0);
                              state->CQ[temp_CQ_head].issued = TRUE; 
                              if(load_invalid == 0) { 
                                     memcpy(&(state->ROB[state->CQ[temp_CQ_head].ROB_index].result.integer.wu), 
                                             &(state->mem[state->ROB[state->CQ[temp_CQ_head].ROB_index].target.integer.wu]), 4);       
                                     invalid = 1;
                              } 
                           }
                           break;
               
                    case DATA_TYPE_F:
                           if(state->CQ[temp_CQ_head].tag1 == -1 && state->CQ[temp_CQ_head].issued == FALSE) {
                              issue_fu_mem(state->fu_mem_list, state->CQ[temp_CQ_head].ROB_index,  1, 0); 
                                 state->CQ[temp_CQ_head].issued = TRUE;
                                 if(load_invalid == 0) {
                                     memcpy(&(state->ROB[state->CQ[temp_CQ_head].ROB_index].result.flt),
                                             &(state->mem[state->ROB[state->CQ[temp_CQ_head].ROB_index].target.integer.wu]), 4);
                                     invalid = 1;
                                 }
                           }
                 }
                 break;
          
          
          default: break;  
     } //switch  
       
   } while (temp_CQ_head != state->CQ_tail  && invalid == 0 && store_invalid == 0 && load_invalid == 0 && state->CQ[temp_CQ_head].issued == FALSE);//while 
   
  while (state->CQ[state->CQ_head].issued == TRUE && (state->CQ_head != state->CQ_tail))
  { 
        state->CQ_head = (state->CQ_head + 1)%32; 
  }
  return 0;       
}//func

int
issue(state_t *state) {
   int temp_IQ_head = state->IQ_head - 1;
   int invalid = 0;
   do {
     temp_IQ_head = (temp_IQ_head + 1)%32;
     int instr = state->IQ[temp_IQ_head].instr;
     if (instr != 0 && state->IQ[temp_IQ_head].issued == FALSE){
     int imm_d, DTG;
     DTG = state->IQ[temp_IQ_head].ROB_index;
     const op_info_t *op_info = decode_instr(instr, &imm_d);
     operand_t operand1, operand2, result, target;

     switch(op_info->fu_group_num){ 
            case FU_GROUP_MEM:
                 switch (op_info->operation){
                      case OPERATION_LOAD:
                      case OPERATION_STORE: 

                                         if (state->IQ[temp_IQ_head].tag1 == -1 && state->IQ[temp_IQ_head].tag2 == -1){
                                         if (issue_fu_int(state->fu_int_list, DTG + 64, 0, 0) == -1){ 
                                            break;
                                         }
                                         else{
                                            operand1.integer.wu  = state->IQ[temp_IQ_head].operand1.integer.wu;
                                            operand2.integer.wu  = state->IQ[temp_IQ_head].operand2.integer.wu;
                                            state->IQ[temp_IQ_head].issued = TRUE;
                                            state->ROB[state->IQ[temp_IQ_head].ROB_index].target.integer.wu = operand1.integer.wu + operand2.integer.wu;                                                            invalid = 1;
                                            break;
                                            }
                                         }
                           break;
                 }                       
                 break;
            case FU_GROUP_INT:
                 switch (imm_d){ 
                        case 0:
                        case 1:
                          if (state->IQ[temp_IQ_head].tag1 == -1 && state->IQ[temp_IQ_head].tag2 == -1){         
                              if (issue_fu_int(state->fu_int_list, DTG, 0, 0) == -1){
                                 break;
                              }
                              else{
                                 operand1.integer.wu  = state->IQ[temp_IQ_head].operand1.integer.wu;
                                 operand2.integer.wu  = state->IQ[temp_IQ_head].operand2.integer.wu; 
                                 state->IQ[temp_IQ_head].issued = TRUE;
                                 result.integer.wu = perform_operation(instr, state->pc, operand1.integer.wu, operand2.integer.wu); 
                                 state->ROB[state->IQ[temp_IQ_head].ROB_index].result.integer.wu = result.integer.wu;
                                 invalid = 1;
                                 break;
                              }
                          }
                          break; 
                 }
                 break;

            case FU_GROUP_ADD: 
               switch (op_info->operation){
                 case OPERATION_ADD:

                 if (state->IQ[temp_IQ_head].tag1 == -1 && state->IQ[temp_IQ_head].tag2 == -1) {
                     if (issue_fu_fp(state->fu_add_list, DTG) == -1){
                         break;
                     }
                     else{
                         operand1.flt  = state->IQ[temp_IQ_head].operand1.flt;
                         operand2.flt  = state->IQ[temp_IQ_head].operand2.flt;
                         state->IQ[temp_IQ_head].issued = TRUE;
                         result.flt = operand1.flt + operand2.flt;
                         state->ROB[state->IQ[temp_IQ_head].ROB_index].result.flt = result.flt;
                         invalid = 1;
                         break;
                     }
                 }
                 break;

                 case OPERATION_SUB:     

                 if (state->IQ[temp_IQ_head].tag1 == -1 && state->IQ[temp_IQ_head].tag2 == -1) {
                     if (issue_fu_fp(state->fu_add_list, DTG) == -1){
                         break;
                     }
                     else{
                         operand1.flt  = state->IQ[temp_IQ_head].operand1.flt;
                         operand2.flt  = state->IQ[temp_IQ_head].operand2.flt;
                         state->IQ[temp_IQ_head].issued = TRUE;
                         result.flt = operand1.flt - operand2.flt;
                         state->ROB[state->IQ[temp_IQ_head].ROB_index].result.flt = result.flt;
                         invalid = 1;
                         break;
                     }
                 }
                 break;
                    

               }
            break; 
           
            case FU_GROUP_MULT:

                 if (state->IQ[temp_IQ_head].tag1 == -1 && state->IQ[temp_IQ_head].tag2 == -1) {
                     if (issue_fu_fp(state->fu_mult_list, DTG) == -1){
                         break;
                     }
                     else{
                         operand1.flt  = state->IQ[temp_IQ_head].operand1.flt;
                         operand2.flt  = state->IQ[temp_IQ_head].operand2.flt;
                         state->IQ[temp_IQ_head].issued = TRUE;
                         result.flt = operand1.flt*operand2.flt;
                         state->ROB[state->IQ[temp_IQ_head].ROB_index].result.flt = result.flt;
                         invalid = 1;
                         break;
                     }
                 }
                 break;

            case FU_GROUP_DIV:

                 if (state->IQ[temp_IQ_head].tag1 == -1 && state->IQ[temp_IQ_head].tag2 == -1) {
                     if (issue_fu_fp(state->fu_div_list, DTG) == -1){
                         break;
                     }
                     else{
                         operand1.flt  = state->IQ[temp_IQ_head].operand1.flt;
                         operand2.flt  = state->IQ[temp_IQ_head].operand2.flt;
                         state->IQ[temp_IQ_head].issued = TRUE;
                         result.flt = operand1.flt/operand2.flt;
                         state->ROB[state->IQ[temp_IQ_head].ROB_index].result.flt = result.flt;
                         invalid = 1;
                         break;
                     }
                 }
                 break; 

            case FU_GROUP_BRANCH:

                 switch (op_info->operation){

                    case OPERATION_JAL:
                         if (issue_fu_int(state->fu_int_list, DTG, 1, 1) == -1) {
                            break;
                         }
                         else{
                           state->ROB[state->IQ[temp_IQ_head].ROB_index].target.integer.wu  = state->IQ[temp_IQ_head].pc  + FIELD_OFFSET(instr);
                           invalid  = 1;
                           state->IQ[temp_IQ_head].issued = TRUE;
                           state->ROB[state->IQ[temp_IQ_head].ROB_index].result.integer.wu = 1;
                           break;
                         }
                       break;

                    case OPERATION_BEQ:
                        if(state->IQ[temp_IQ_head].tag1 == -1 && state->IQ[temp_IQ_head].tag2 == -1 ){
                             if (issue_fu_int(state->fu_int_list, DTG, 1, 0) == -1) {
                                break;
                             }
                             else if (state->IQ[temp_IQ_head].operand1.integer.wu == 0){
                                   state->ROB[state->IQ[temp_IQ_head].ROB_index].target.integer.wu  = state->IQ[temp_IQ_head].pc + FIELD_IMM_S(instr); 
                                   state->ROB[state->IQ[temp_IQ_head].ROB_index].result.integer.wu = 1;
                                   state->IQ[temp_IQ_head].issued = TRUE;  
                             }
                             else{
                                  state->ROB[state->IQ[temp_IQ_head].ROB_index].target.integer.wu  = state->IQ[temp_IQ_head].pc + FIELD_IMM_S(instr);
                                  state->ROB[state->IQ[temp_IQ_head].ROB_index].result.integer.wu = 0;
                                  state->IQ[temp_IQ_head].issued = TRUE;
                             }
                        }
                        break;
                 }
                 break;
     }// switch
     }// if
   } while (temp_IQ_head != state->IQ_tail  && invalid == 0);// while

   if (state->IQ[state->IQ_head].issued == TRUE && (state->IQ_tail + 1)%32 == state->IQ_head){
       state->fetch_lock = FALSE;
   }

   while ((state->IQ_head != state->IQ_tail) && (state->IQ[state->IQ_head].issued == TRUE))
   { 
        state->IQ_head = (state->IQ_head + 1)%32;
   }
   return 0;
}// function

  
int
dispatch(state_t *state) {
        int instr = (state->if_id).instr;
        if (instr != 0){
        
        int imm_d;
        const op_info_t *op_info = decode_instr(instr, &imm_d);
        unsigned long pc_temp = state->if_id.pc;
           switch(op_info->fu_group_num){ 
                 case FU_GROUP_MEM:   
                      switch(op_info->operation){  
                            case OPERATION_LOAD:
                                             state->ROB[state->ROB_tail].instr = instr;
                                             state->ROB[state->ROB_tail].completed = FALSE;
                                             state->IQ[state->IQ_tail].instr = instr;
                                             state->IQ[state->IQ_tail].pc = pc_temp;
                                             state->IQ[state->IQ_tail].issued = FALSE;
                                             state->IQ[state->IQ_tail].ROB_index = state->ROB_tail;
                                             state->IQ[state->IQ_tail].tag1 = state->rf_int.tag[FIELD_RS1(instr)];
                                             if (state->IQ[state->IQ_tail].tag1 == -1){
                                                state->IQ[state->IQ_tail].operand1.integer.wu = state->rf_int.reg_int.integer[FIELD_RS1(instr)].wu;
                                             }                                             
                                             else { 
                                                  // check the rob at tag1 and if completed is true bring the result in op1
                                                  if (state->ROB[state->IQ[state->IQ_tail].tag1].completed == TRUE){ 
                                                     state->IQ[state->IQ_tail].operand1.integer.wu = state->ROB[state->IQ[state->IQ_tail].tag1].result.integer.wu;
						     state->IQ[state->IQ_tail].tag1 = -1;	   
                                                  }                       
                                             }
                                             state->IQ[state->IQ_tail].tag2 = -1;
                                             state->IQ[state->IQ_tail].operand2.integer.wu = FIELD_IMM_I(instr); 
                                             state->IQ_tail = (state->IQ_tail + 1)%32;

                                             state->CQ[state->CQ_tail].instr = instr;
                                             state->CQ[state->CQ_tail].store = 0;
                                             state->CQ[state->CQ_tail].issued = FALSE;
                                             state->CQ[state->CQ_tail].ROB_index = state->ROB_tail; // ROB index for CQ inst should be > 64
                                             state->CQ[state->CQ_tail].tag1 = state->ROB_tail + 64; 
                                             state->CQ[state->CQ_tail].tag2 = -1;
                                             state->CQ_tail = (state->CQ_tail + 1)%32;

                                             if (op_info->data_type == DATA_TYPE_W){
                                                 state->rf_int.tag[FIELD_RD(instr)] = state->ROB_tail;
                                             }
                                             if (op_info->data_type == DATA_TYPE_F){
                                                 state->rf_fp.tag[FIELD_RD(instr)] = state->ROB_tail;
                                             }
                                             state->ROB_tail = (state->ROB_tail + 1)%64;
                                             break;
                            case OPERATION_STORE:
                                             state->ROB[state->ROB_tail].instr = instr;
                                             state->ROB[state->ROB_tail].completed = FALSE;

                                             state->IQ[state->IQ_tail].instr = instr;
                                             state->IQ[state->IQ_tail].pc = pc_temp;
                                             state->IQ[state->IQ_tail].issued = FALSE;
                                             state->IQ[state->IQ_tail].ROB_index = state->ROB_tail;
                                             state->IQ[state->IQ_tail].tag1 = state->rf_int.tag[FIELD_RS1(instr)];
                                             if (state->IQ[state->IQ_tail].tag1 == -1){
                                                state->IQ[state->IQ_tail].operand1.integer.wu = state->rf_int.reg_int.integer[FIELD_RS1(instr)].wu;
                                             }
                                             else {
                                                // check the rob at tag1 and if completed is true bring the result in op1
                                                  if (state->ROB[state->IQ[state->IQ_tail].tag1].completed == TRUE){
                                                     state->IQ[state->IQ_tail].operand1.integer.wu = state->ROB[state->IQ[state->IQ_tail].tag1].result.integer.wu;
                                                     state->IQ[state->IQ_tail].tag1 = -1;  
                                                  }
                                             }
                                             state->IQ[state->IQ_tail].tag2 = -1;
                                             state->IQ[state->IQ_tail].operand2.integer.wu = FIELD_IMM_S(instr);
                                             state->IQ_tail = (state->IQ_tail + 1)%32;

                                             state->CQ[state->CQ_tail].instr = instr;
                                             state->CQ[state->CQ_tail].store = 1;
                                             state->CQ[state->CQ_tail].issued = FALSE;
                                             state->CQ[state->CQ_tail].ROB_index = state->ROB_tail;   // ROB index for CQ inst should be > 64
                                             state->CQ[state->CQ_tail].tag1 = state->ROB_tail + 64;
                                          
                                             if (op_info->data_type == DATA_TYPE_W){
                                                 state->CQ[state->CQ_tail].tag2 = state->rf_int.tag[FIELD_RS2(instr)];
                                                 if (state->rf_int.tag[FIELD_RS2(instr)] == -1){
                                                    state->CQ[state->CQ_tail].result.integer.wu = state->rf_int.reg_int.integer[FIELD_RS2(instr)].wu;
                                                 }
                                                 else {
                                                    if (state->ROB[state->rf_int.tag[FIELD_RS2(instr)]].completed == TRUE){
                                                       state->CQ[state->CQ_tail].result.integer.wu = state->ROB[state->CQ[state->CQ_tail].tag2].result.integer.wu;
                                                       state->CQ[state->CQ_tail].tag2 = -1;
                                                    }
                                                 }

                                             }

                                             if (op_info->data_type == DATA_TYPE_F){
                                                 state->CQ[state->CQ_tail].tag2 = state->rf_fp.tag[FIELD_RS2(instr)];
                                                 if (state->rf_fp.tag[FIELD_RS2(instr)] == -1){
                                                    state->CQ[state->CQ_tail].result.flt = state->rf_fp.reg_fp.flt[FIELD_RS2(instr)];
                                                 }
                                                 else {
                                                    if (state->ROB[state->rf_int.tag[FIELD_RS2(instr)]].completed == TRUE){
                                                       state->CQ[state->CQ_tail].result.flt = state->ROB[state->CQ[state->CQ_tail].tag2].result.flt;
                                                       state->CQ[state->CQ_tail].tag2 = -1;
                                                    }
                                                 }
                                             }

                                             state->CQ_tail = (state->CQ_tail + 1)%32;
 
                                             state->ROB_tail = (state->ROB_tail + 1)%64;
                                             break;                  
                      }
                      break;                    
                 case FU_GROUP_INT:
                      switch(imm_d){ 
                            case 0:   
                                 state->ROB[state->ROB_tail].instr = instr;
                                 state->ROB[state->ROB_tail].completed = FALSE;
                                 state->IQ[state->IQ_tail].instr = instr;
                                 state->IQ[state->IQ_tail].pc = pc_temp;
                                 state->IQ[state->IQ_tail].issued = FALSE;
                                 state->IQ[state->IQ_tail].ROB_index = state->ROB_tail;
                                 state->IQ[state->IQ_tail].tag1 = state->rf_int.tag[FIELD_RS1(instr)];
                                 if (state->IQ[state->IQ_tail].tag1 == -1){
                                    state->IQ[state->IQ_tail].operand1.integer.wu = state->rf_int.reg_int.integer[FIELD_RS1(instr)].wu;
                                 }
                                 else {
                                 // check the rob at tag1 and if completed is true bring the result in op1 
                                       if (state->ROB[state->rf_int.tag[FIELD_RS1(instr)]].completed == TRUE){
                                           state->IQ[state->IQ_tail].operand1.integer.wu = state->ROB[state->IQ[state->IQ_tail].tag1].result.integer.wu;
                                           state->IQ[state->IQ_tail].tag1 = -1;
                                       }                                 
                                 }
                                 state->IQ[state->IQ_tail].tag2 = state->rf_int.tag[FIELD_RS2(instr)];
                                 if (state->IQ[state->IQ_tail].tag2 == -1){
                                    state->IQ[state->IQ_tail].operand2.integer.wu = state->rf_int.reg_int.integer[FIELD_RS2(instr)].wu;
                                 }
                                 else {
                                 // check the rob at tag2 and if completed is true bring the result in op2
                                       if (state->ROB[state->rf_int.tag[FIELD_RS2(instr)]].completed == TRUE){
                                           state->IQ[state->IQ_tail].operand2.integer.wu = state->ROB[state->IQ[state->IQ_tail].tag2].result.integer.wu;
                                           state->IQ[state->IQ_tail].tag2 = -1; 
                                       }
                                 }
                                 state->rf_int.tag[FIELD_RD(instr)] = state->ROB_tail;
                                 state->IQ_tail = (state->IQ_tail + 1)%32;
                                 state->ROB_tail = (state->ROB_tail + 1)%64;                      
                                 break; 

                            case 1: 
                                 state->ROB[state->ROB_tail].instr = instr;
                                 state->ROB[state->ROB_tail].completed = FALSE;
                                 state->IQ[state->IQ_tail].instr = instr;
                                 state->IQ[state->IQ_tail].pc = pc_temp;
                                 state->IQ[state->IQ_tail].issued = FALSE;
                                 state->IQ[state->IQ_tail].ROB_index = state->ROB_tail;
                                 state->IQ[state->IQ_tail].tag1 = state->rf_int.tag[FIELD_RS1(instr)];
                                 if (state->IQ[state->IQ_tail].tag1 == -1){
                                    state->IQ[state->IQ_tail].operand1.integer.wu = state->rf_int.reg_int.integer[FIELD_RS1(instr)].wu;
                                 } 
                                 else {
                                    //check the rob at tag1 and if completed is true bring the result in op1
                                       if (state->ROB[state->rf_int.tag[FIELD_RS1(instr)]].completed == TRUE){
                                           state->IQ[state->IQ_tail].operand1.integer.wu = state->ROB[state->IQ[state->IQ_tail].tag1].result.integer.wu;
                                           state->IQ[state->IQ_tail].tag1 = -1;  
                                       }
                                 } 
                                 state->IQ[state->IQ_tail].tag2 = -1;
                                 state->IQ[state->IQ_tail].operand2.integer.wu = FIELD_IMM_I(instr);

                                 state->rf_int.tag[FIELD_RD(instr)] = state->ROB_tail;
                                 state->IQ_tail = (state->IQ_tail + 1)%32;
                                 state->ROB_tail = (state->ROB_tail + 1)%64;
                                 break; 
                            }                             
                      break;
                 case FU_GROUP_HALT:                     
                      state->ROB[state->ROB_tail].instr = instr;
                      state->ROB[state->ROB_tail].completed = TRUE;
                      state->ROB[state->ROB_tail].target.integer.wu = 0;
                      state->ROB[state->ROB_tail].result.integer.wu = 0;
                      state->ROB_tail = (state->ROB_tail + 1)%64;
                      state->fetch_lock = TRUE;
                      memcpy( &((state->if_id).instr), &(state->mem[state->pc]), 4);
                      state->pc = state->pc + 4;
                      break;

                 case FU_GROUP_BRANCH: 
                      state->ROB[state->ROB_tail].instr = instr;
                      state->ROB[state->ROB_tail].completed = FALSE;
                      state->IQ[state->IQ_tail].instr = instr;
                      state->IQ[state->IQ_tail].pc = pc_temp; // reading of pc at branch instruction
                      state->IQ[state->IQ_tail].issued = FALSE;
                      state->IQ[state->IQ_tail].ROB_index = state->ROB_tail;
                      memcpy( &((state->if_id).instr), &(state->mem[state->pc]), 4);  
                      state->pc = state->pc + 4;
                      switch (op_info->operation){
                         case OPERATION_JAL: 
                                     state->IQ[state->IQ_tail].tag1 = -1;
                                     state->IQ[state->IQ_tail].tag2 = -1;
                              break;

                         case OPERATION_BEQ: 
                                   state->IQ[state->IQ_tail].tag1 = state->rf_int.tag[FIELD_RS1(instr)];
                                   if (state->IQ[state->IQ_tail].tag1 == -1){
                                      state->IQ[state->IQ_tail].operand1.integer.wu = state->rf_int.reg_int.integer[FIELD_RS1(instr)].wu;
                                   }
                                   else {
                                        if (state->ROB[state->rf_int.tag[FIELD_RS1(instr)]].completed == TRUE){
                                           state->IQ[state->IQ_tail].operand1.integer.wu = state->ROB[state->IQ[state->IQ_tail].tag1].result.integer.wu;
                                           state->IQ[state->IQ_tail].tag1 = -1;
                                        }
                                   }
                                   state->IQ[state->IQ_tail].tag2 = state->rf_int.tag[FIELD_RS2(instr)];
                                   if (state->IQ[state->IQ_tail].tag2 == -1){
                                      state->IQ[state->IQ_tail].operand2.integer.wu = state->rf_int.reg_int.integer[FIELD_RS2(instr)].wu;
                                   }
                                   else {
                                        if (state->ROB[state->rf_int.tag[FIELD_RS2(instr)]].completed == TRUE){
                                           state->IQ[state->IQ_tail].operand2.integer.wu = state->ROB[state->IQ[state->IQ_tail].tag2].result.integer.wu;
                                           state->IQ[state->IQ_tail].tag2 = -1;
                                        }
                                  }               
                                  break;
                      }

                      state->IQ_tail = (state->IQ_tail + 1)%32;
                      state->ROB_tail = (state->ROB_tail + 1)%64;
                      state->fetch_lock = TRUE;
                      break;

                 case FU_GROUP_ADD:
                 case FU_GROUP_MULT:
                 case FU_GROUP_DIV:
                      state->ROB[state->ROB_tail].instr = instr;
                      state->ROB[state->ROB_tail].completed = FALSE;
                      state->IQ[state->IQ_tail].instr = instr;
                      state->IQ[state->IQ_tail].pc = pc_temp;
                      state->IQ[state->IQ_tail].issued = FALSE;
                      state->IQ[state->IQ_tail].ROB_index = state->ROB_tail;
                      state->IQ[state->IQ_tail].tag1 = state->rf_fp.tag[FIELD_RS1(instr)];
                      if (state->IQ[state->IQ_tail].tag1 == -1){
                          state->IQ[state->IQ_tail].operand1.flt = state->rf_fp.reg_fp.flt[FIELD_RS1(instr)];
                      }
                      else {
                            if (state->ROB[state->rf_fp.tag[FIELD_RS1(instr)]].completed == TRUE){
                                state->IQ[state->IQ_tail].operand1.flt = state->ROB[state->IQ[state->IQ_tail].tag1].result.flt;
                                state->IQ[state->IQ_tail].tag1 = -1;
                            }
                      }
                      state->IQ[state->IQ_tail].tag2 = state->rf_fp.tag[FIELD_RS2(instr)];
                      if (state->IQ[state->IQ_tail].tag2 == -1){
                          state->IQ[state->IQ_tail].operand2.flt = state->rf_fp.reg_fp.flt[FIELD_RS2(instr)];
                      }
                      else {
                            if (state->ROB[state->rf_fp.tag[FIELD_RS2(instr)]].completed == TRUE){
                                 state->IQ[state->IQ_tail].operand2.flt = state->ROB[state->IQ[state->IQ_tail].tag2].result.flt;
                                 state->IQ[state->IQ_tail].tag2 = -1;
                            }
                      }
                      state->rf_fp.tag[FIELD_RD(instr)] = state->ROB_tail;
                      state->IQ_tail = (state->IQ_tail + 1)%32;
                      state->ROB_tail = (state->ROB_tail + 1)%64;
                      break;  

                 default:
                      break;              
           }//switch
               
        if ((state->IQ_tail+1)%32 == state->IQ_head || (state->CQ_tail+1)%32 == state->CQ_head || (state->ROB_tail+1)%64 == state->ROB_head){
            state->fetch_lock = TRUE;
            memcpy( &((state->if_id).instr), &(state->mem[state->pc]), 4);
            state->pc = state->pc + 4;    
        }
        return 0;
     }//if

}//dispatch

void
fetch(state_t *state) {
     if (state->fetch_lock != TRUE){
        state->if_id.pc = state->pc;
        memcpy( &((state->if_id).instr), &(state->mem[state->pc]), 4);
        state->pc += 4;
    }   
}
