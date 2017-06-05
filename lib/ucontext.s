.global getcontext
getcontext:
	subui $sp, $sp, 2
	sw $1, 0($sp)
	sw $2, 1($sp)
	#load ucp's pointer
	lw $1, 2($sp)
	
	#load register 1
	lw $2, 0($sp)
	#save reguster 1
	sw $2, ucontext_reg1($1)
	#load register 2
	lw $2,1($sp)
	sw $2, ucontext_reg2($1)
	sw $3, ucontext_reg3($1)
	sw $4, ucontext_reg4($1)
	sw $5, ucontext_reg5($1)
	sw $6, ucontext_reg6($1)
	sw $7, ucontext_reg7($1)
	sw $8, ucontext_reg8($1)
	sw $9, ucontext_reg9($1)
	sw $10, ucontext_reg10($1)
	sw $11, ucontext_reg11($1)
	sw $12, ucontext_reg12($1)
	sw $13, ucontext_reg13($1)
	sw $ra, ucontext_pc($1) #$ra is the only register not saved

	addui $sp, $sp, 2
	sw $sp, ucontext_sp($1)

	lw $1, -2($sp)
	lw $2, -1($sp)

	jr $ra

.global setcontext
setcontext:
	#13: pointer to ucp
	lw $13, 0($sp)

	lw $2, ucontext_reg2($13)
	lw $3, ucontext_reg3($13)
	lw $4, ucontext_reg4($13)
	lw $5, ucontext_reg5($13)
	lw $6, ucontext_reg6($13)
	lw $7, ucontext_reg7($13)
	lw $8, ucontext_reg8($13)
	lw $9, ucontext_reg9($13)
	lw $10, ucontext_reg10($13)
	lw $11, ucontext_reg11($13)
	lw $12, ucontext_reg12($13)
	
	#load return address as the pc
	lw $ra, ucontext_pc($13)

	lw $1, ucontext_ss_flags($13)
	beqz $1, setcontext_alternative_stack

	lw $sp, ucontext_sp($13)
	j load_reg_1
setcontext_alternative_stack:
	lw $sp, ucontext_ss_sp($13)
load_reg_1:

	lw $1, ucontext_reg1($13)
	lw $13, ucontext_reg13($13)
	jr $ra


.global get_curr_addr
get_curr_addr:
	sw $ra, 0($sp)
	jr $ra


.global _ctx_start
_ctx_start:
	lw $1, 0($sp)
	addui $sp, $sp, 1
	jalr $1
	sw $13, 0($sp)
	j _ctx_end


.global swapcontext
swapcontext:
	subui $sp, $sp, 1
	sw $1, 0($sp)
	#load ucp's pointer
	lw $1, 1($sp)
	#save register 2
	sw $2, ucontext_reg2($1)
	#load register 1
	lw $2, 0($sp)
	#save reguster 1
	sw $2, ucontext_reg2($1)
	sw $3, ucontext_reg3($1)
	sw $4, ucontext_reg4($1)
	sw $5, ucontext_reg5($1)
	sw $6, ucontext_reg6($1)
	sw $7, ucontext_reg7($1)
	sw $8, ucontext_reg8($1)
	sw $9, ucontext_reg9($1)
	sw $10, ucontext_reg10($1)
	sw $11, ucontext_reg11($1)
	sw $12, ucontext_reg12($1)
	sw $13, ucontext_reg13($1)
	sw $ra, ucontext_pc($1) #$ra is the only register not saved

	addui $sp, $sp, 1
	sw $sp, ucontext_sp($1)

	addui $sp, $sp, 1
	j setcontext

# .global makecontext
# makecontext:
# 	# save registers 1, 2, 3, 4, 5 on the stack
# 	subui $sp, $sp, 5
# 	sw $1, 0($sp)
# 	sw $2, 1($sp)
# 	sw $3, 2($sp)
# 	sw $4, 3($sp)
# 	sw $5, 4($sp)

# 	# first arg: pointer to ucp
# 	# second arg: function pointer
# 	lw $1, 5($sp)
# 	lw $2, 6($sp)
# 	#set pc to the start of the function
# 	sw $2, ucontext_pc($1)

# 	# third arg: argc
# 	lw $2, 7($sp)

# 	#if ss_flags are set to 0, we would use ucontext_ss_sp, otherwise the normal stack
# 	lw $3, ucontext_ss_flags($1)
# 	beqz $3, makecontext_alternative_stack

# 	lw $3, ucontext_sp($1)
# 	j makecontext_load_parameters
# makecontext_load_alternative_stack:
# 	lw $3, ucontext_ss_sp($1)
# makecontext_load_parameters:
# 	#stack allocated for argc
# 	subui $3, $3, 1
# 	#stack allocated for arguments after argc
# 	subu $3, $3, $2

# 	lw $5, ucontext_ss_flags($1)
# 	beqz $5, makecontext_save_alternative_stack

# 	#store ucp stack to the ucp's stack
# 	sw $3, ucontext_sp($1)
# 	j makecontext_load_argc
# makecontext_save_alternative_stack:
# 	sw $3, ucontext_ss_sp($1)
# makecontext_load_argc:

# 	#copy argc to ucp's stack
# 	sw $2, 0($3)
# 	addui $3, $3, 1

# 	#move current sp to the fourth argument, if there is any
# 	addui $4, $sp, 8

# 	# $1: pointer to the ucp
# 	# $2: argc
# 	# $3: stack pointer for ucp
# 	# $4: stack pointer of the current context
# 	# $5: tmp value
# loop:
# 	beqz $2, end
# 	lw $5, 0($4)
# 	sw $5, 0($3)
# 	addui $4, $4, 1
# 	addui $3, $3, 1
# 	subui $2, $2, 1
# end:
# 	lw $1, 0($sp)
# 	lw $2, 1($sp)
# 	lw $3, 2($sp)
# 	lw $4, 3($sp)
# 	lw $5, 4($sp)
# 	addui $sp, $sp, 5
# 	jr $ra


.equ ucontext_reg1, 0
.equ ucontext_reg2, 1
.equ ucontext_reg3, 2
.equ ucontext_reg4, 3
.equ ucontext_reg5, 4
.equ ucontext_reg6, 5
.equ ucontext_reg7, 6
.equ ucontext_reg8, 7
.equ ucontext_reg9, 8
.equ ucontext_reg10, 9
.equ ucontext_reg11, 10
.equ ucontext_reg12, 11
.equ ucontext_reg13, 12
.equ ucontext_sp, 13
.equ ucontext_ra, 14
.equ ucontext_pc, 15
.equ ucontext_ss_sp, 16
.equ ucontext_ss_flags, 17
.equ ucontext_ss_size, 18