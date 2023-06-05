########################################################################
# CP1521 22T3 -- Assignment 1 -- Connect Four!
#
#
# !!! IMPORTANT !!!
# Before starting work on the assignment, make sure you set your tab-width to 8!
# It is also suggested to indent with tabs only.
# Instructions to configure your text editor can be found here:
#   https://cgi.cse.unsw.edu.au/~dp1092/22T3/resources/mips-editors.html
# !!! IMPORTANT !!!
#
#
# This program was written by Pyae Sone Oo (z5271704)
# on 3-10-2022
#
# Version 1.0 (18-09-2022):
#
########################################################################

#![tabsize(8)]

# Constant definitions.
# DO NOT CHANGE THESE DEFINITIONS

# MIPS doesn't have true/false by default
true  = 1
false = 0

# How many pieces we're trying to connect
CONNECT = 4

# The minimum and maximum board dimensions
MIN_BOARD_DIMENSION = 4
MAX_BOARD_WIDTH     = 9
MAX_BOARD_HEIGHT    = 16

# The three cell types
CELL_EMPTY  = '.'
CELL_RED    = 'R'
CELL_YELLOW = 'Y'

# The winner conditions
WINNER_NONE   = 0
WINNER_RED    = 1
WINNER_YELLOW = 2

# Whose turn is it?
TURN_RED    = 0
TURN_YELLOW = 1

########################################################################
# .DATA
# YOU DO NOT NEED TO CHANGE THE DATA SECTION
	.data

# char board[MAX_BOARD_HEIGHT][MAX_BOARD_WIDTH];
board:		.space  MAX_BOARD_HEIGHT * MAX_BOARD_WIDTH

# int board_width;
board_width:	.word 0

# int board_height;
board_height:	.word 0


enter_board_width_str:	.asciiz "Enter board width: "
enter_board_height_str: .asciiz "Enter board height: "
game_over_draw_str:	.asciiz "The game is a draw!\n"
game_over_red_str:	.asciiz "Game over, Red wins!\n"
game_over_yellow_str:	.asciiz "Game over, Yellow wins!\n"
board_too_small_str_1:	.asciiz "Board dimension too small (min "
board_too_small_str_2:	.asciiz ")\n"
board_too_large_str_1:	.asciiz "Board dimension too large (max "
board_too_large_str_2:	.asciiz ")\n"
red_str:		.asciiz "[RED] "
yellow_str:		.asciiz "[YELLOW] "
choose_column_str:	.asciiz "Choose a column: "
invalid_column_str:	.asciiz "Invalid column\n"
no_space_column_str:	.asciiz "No space in that column!\n"


############################################################
####                                                    ####
####   Your journey begins here, intrepid adventurer!   ####
####                                                    ####
############################################################


########################################################################
#
# Implement the following 7 functions,
# and check these boxes as you finish implementing each function
#
#  - [X] main
#  - [X] assert_board_dimension
#  - [X] initialise_board
#  - [X] play_game
#  - [X] play_turn
#  - [X] check_winner
#  - [X] check_line
#  - [X] is_board_full	(provided for you)
#  - [X] print_board	(provided for you)
#
########################################################################


########################################################################
# .TEXT <main>
	.text
main:
	# Args:     void
	# Returns:
	#   - $v0: int
	#
	# Frame:    [$ra, ...]
	# Uses:     [...]
	# Clobbers: [...]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   main
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

main__prologue:
	begin			# begin a new stack frame
	push	$ra		# | $ra

main__body:


	#########Entering Board Width#########
	la $a0, enter_board_width_str   	#printf("Enter board width: ");
	li $v0, 4						
	syscall

	li $v0, 5				#scanf("%d", &board_width);
	syscall
	sw $v0, board_width
	
	move $a0, $v0				#assert_board_dimension(board_width, MIN_BOARD_DIMENSION, MAX_BOARD_WIDTH);
	li $a1, MIN_BOARD_DIMENSION
	li $a2, MAX_BOARD_WIDTH
	jal assert_board_dimension

	#########Entering Board Height#########
	la $a0, enter_board_height_str		#printf("Enter board height: ");
	li $v0, 4
	syscall

	li $v0, 5				#scanf("%d", &board_height);
	syscall
	sw $v0, board_height
	
	move $a0, $v0				#assert_board_dimension(board_height, MIN_BOARD_DIMENSION, MAX_BOARD_HEIGHT);
	li $a1, MIN_BOARD_DIMENSION
	li $a2, MAX_BOARD_HEIGHT
	jal assert_board_dimension

	#########Other functions call#########
	jal initialise_board
	jal print_board
	jal play_game
	

main__epilogue:
	pop	$ra		# | $ra
	end			# ends the current stack frame

	li	$v0, 0
	jr	$ra		# return 0;


########################################################################
# .TEXT <assert_board_dimension>
	.text
assert_board_dimension:
	# Args:
	#   - $a0: int dimension
	#   - $a1: int min
	#   - $a2: int max
	# Returns:  void
	#
	# Frame:    [...]
	# Uses:     [...]
	# Clobbers: [...]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   assert_board_dimension
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

assert_board_dimension__prologue:
assert_board_dimension__body:


	######### Accessing parameter values #########
	move $t0, $a0
	move $t1, $a1
	move $t2, $a2
	
	######### Board Width Check #########
	if1:
		bge $t0, $t1, if1_end		#if (dimension < min) 
		la $a0, board_too_small_str_1	#printf("Board dimension too small (min %d)\n", min)
		li $v0, 4
		syscall
		
		move $a0, $t1
		li $v0, 1
		syscall
		
		la $a0, board_too_small_str_2
		li $v0, 4
		syscall
		
		li $v0, 10			#exit(1)
		syscall
		
	if1_end:
	
	######### Board Height Check #########
	if2:
		ble $t0, $t2, if2_end		#if (dimension > max)
		la $a0, board_too_large_str_1	#printf("Board dimension too large (max %d)\n", max)
		li $v0, 4
		syscall
		
		move $a0, $t2
		li $v0, 1
		syscall
		
		la $a0, board_too_large_str_2
		li $v0, 4
		syscall
		
		li $v0, 10			#exit(1)
		syscall
	if2_end:

assert_board_dimension__epilogue:
	jr	$ra		# return;


########################################################################
# .TEXT <initialise_board>
	.text
initialise_board:
	# Args:     void
	# Returns:  void
	#
	# Frame:    [...]
	# Uses:     [...]
	# Clobbers: [...]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   initialise_board
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

initialise_board__prologue:
initialise_board__body:

	lw $t0, board_height
	lw $t1, board_width
	
	li $s0, 0						#for (int row = 0; row < board_height; row++)
	for1:	bge $s0, $t0, for1_end
		li $s1, 0					#for (int col = 0; col < board_width; col++)
		for2:	bge $s1, $t1, for2_end	
			la $t3, board				#board[row][col] = CELL_EMPTY;
			mul $t4, $s0, MAX_BOARD_WIDTH	#$t1 
			mul $t5, $s1, 1
			add $t6, $t3, $t4
			add $t7, $t6, $t5
			
			li $s2, CELL_EMPTY
			sb $s2, ($t7)
			
			addi $s1, $s1, 1
			j for2
		for2_end:	
		addi $s0, $s0, 1
		j for1	
	for1_end:
initialise_board__epilogue:
	jr	$ra		# return;


########################################################################
# .TEXT <play_game>
	.text
play_game:
	# Args:     void
	# Returns:  void
	#
	# Frame:    [...]
	# Uses:     [...]
	# Clobbers: [...]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   play_game
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

play_game__prologue:
	begin
	push $ra
play_game__body:
	
	jal is_board_full
	move $t3, $v0
	
	li $t0, TURN_RED			#int whose_turn = TURN_RED;
	li $t1, WINNER_NONE			#int winner;
	li $t2, WINNER_NONE
	push $t2
	push $t0
	push $t1
	push $t3
	pgloop:	pop $t3
		pop $t1
		pop $t0
		pop $t2
		
		bne $t1, $t2, pgloop_end	#while (winner == WINNER_NONE && !is_board_full())
		beq $t3, 1, pgloop_end
		push $t2
		
		move $a0, $t0			#whose_turn = play_turn(whose_turn);
		jal play_turn
		move $t0, $v0
		push $t0
		
		jal print_board			#print_board();
		
		jal check_winner		#winner = check_winner();
		move $t1, $v0
		push $t1
		
		jal is_board_full
		move $t3, $v0
		push $t3
		
		j pgloop			
	pgloop_end:
	
	pgif:	li $t2, WINNER_NONE
		bne $t1, $t2, pg_else1			#if (winner == WINNER_NONE)
		la $t5, game_over_draw_str		#printf("The game is a draw!\n");
		move $a0, $t5
		li $v0, 4
		syscall
		
	pg_else1:	li $t6, WINNER_RED
			bne $t1, $t6, pg_else2		#else if (winner == WINNER_RED)
			la $t5, game_over_red_str	#printf("Game over, Red wins!\n");
			move $a0, $t5
			li $v0, 4
			syscall
			
	pg_else2:	li $t7, WINNER_YELLOW
			bne $t1, $t7, pgif_end		#else
			la $t5, game_over_yellow_str	#printf("Game over, Yellow wins!\n");
			move $a0, $t5
			li $v0, 4
			syscall	
		
	pgif_end:
	
play_game__epilogue:
	pop $ra
	end
	
	jr $ra		# return;


########################################################################
# .TEXT <play_turn>
	.text
play_turn:
	# Args:
	#   - $a0: int whose_turn
	# Returns:  void
	#
	# Frame:    [...]
	# Uses:     [...]
	# Clobbers: [...]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   play_turn
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

play_turn__prologue:
play_turn__body:

	move $t0, $a0				#whose_turn
	li $t1, TURN_RED
	li $t2, TURN_YELLOW
	
	pt_if1:	bne $t0, $t1, ptif1_else	#if (whose_turn == TURN_RED)
		la $a0, red_str			#printf("[RED] ")
		li $v0, 4
		syscall
	ptif1_else:bne $t0, $t2, ptif1_end	#else
		la $a0, yellow_str		# printf("[YELLOW] ")
		li $v0, 4
		syscall
	ptif1_end:
	la $a0, choose_column_str		#printf("Choose a column: ");
	li $v0, 4
	syscall
	
	li $t5, 0				#int target_col = 0;
	li $v0, 5				#scanf("%d", &target_col);
	syscall
	move $t5, $v0
	
	addi $t5, $t5, -1			# target_col--;
	ptif2: 		bge $t5, 0, ptif2_else		#if (target_col < 0
			la $a0, invalid_column_str	#printf("Invalid column\n");
			li $v0, 4
			syscall
			
			move $v0, $t0			#return whose_turn;
			j play_turn__epilogue
						
	ptif2_else:	lw $t6, board_width		#|| target_col >= board_width)
			blt $t5, $t6, ptif2_end		#printf("Invalid column\n");
			la $a0, invalid_column_str
			li $v0, 4
			syscall
			
			move $v0, $t0			#return whose_turn;
			j play_turn__epilogue
	ptif2_end:	 
	#t3,t4,t6, t7
	lw $t3, board_height
	lw $t4, board_width
	addi $t6, $t3, -1			#target_row
	ptloop:					#while(
	ptif3:	blt $t6, 0, ptloop_end		#target_row >= 0
	ptif4:	la $s0, board			#&& board[target_row][target_col] != CELL_EMPTY)
		mul $s1, $t6, MAX_BOARD_WIDTH	#$t4	
		mul $s2, $t5, 1
		add $s3, $s0, $s1
		add $s4, $s3, $s2
		lb $s5, ($s4)
		
		li $s6, CELL_EMPTY
		beq $s5, $s6, ptloop_end
		addi $t6, $t6, -1		#target_row--;
		
	ptif5:	bge $t6, 0, ptif5_end		#if (target_row < 0) {
		la $a0, no_space_column_str	#printf("No space in that column!\n");
		li $v0, 4
		syscall
		
		move $v0, $t0			#return whose_turn;
		j play_turn__epilogue
	ptif5_end:
	j ptloop
	ptloop_end: 
			
	ptif6:	bne $t0, $t1, pt_else6		#if (whose_turn == TURN_RED)
		la $s0, board			#board[target_row][target_col] = CELL_RED;
		mul $s1, $t6, MAX_BOARD_WIDTH	#$t4	
		mul $s2, $t5, 1
		add $s3, $s0, $s1
		add $s4, $s3, $s2
		li $s6, CELL_RED
		sb $s6, ($s4)
		
		move $v0, $t2			#return TURN_YELLOW;
		j play_turn__epilogue
		 
	pt_else6:bne $t0, $t2, ptif6_end
		la $s0, board			#board[target_row][target_col] = CELL_YELLOW;
		mul $s1, $t6, MAX_BOARD_WIDTH	#$t4	
		mul $s2, $t5, 1
		add $s3, $s0, $s1
		add $s4, $s3, $s2
		li $s6, CELL_YELLOW
		sb $s6, ($s4)
		
		move $v0, $t1			#return TURN_RED;
	ptif6_end:		
play_turn__epilogue:
	
	jr	$ra		# return;


########################################################################
# .TEXT <check_winner>
	.text
check_winner:
	# Args:	    void
	# Returns:
	#   - $v0: int
	#
	# Frame:    [...]
	# Uses:     [...]
	# Clobbers: [...]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   check_winner
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

check_winner__prologue:
	begin
	push $ra
check_winner__body:

	lw $s1, board_height
	lw $s3, board_width
	li $s4, WINNER_NONE					#WINNER_NONE
	
	li $s0, 0						#for (int row = 0; row < board_height; row++)
	cwfor1:	bge $s0, $s1, cwfor1_end
		li $s2, 0					#for (int col = 0; col < board_width; col++)
		cwfor2:	bge $s2, $s3, cwfor2_end
		
		con1:	move $a0, $s0				#check = check_line(row, col, 1, 0);
			move $a1, $s2
			li $a2, 1
			li $a3, 0
			jal check_line
			beq $v0, $s4, con2
			j check_winner__epilogue
			
		con2:	move $a0, $s0				#check = check_line(row, col, 0, 1);
			move $a1, $s2
			li $a2, 0
			li $a3, 1
			jal check_line
			beq $v0, $s4, con3
			j check_winner__epilogue
			
		con3:	move $a0, $s0				#check = check_line(row, col, 1, 1);
			move $a1, $s2
			li $a2, 1
			li $a3, 1
			jal check_line
			beq $v0, $s4, con4
			j check_winner__epilogue
			
		con4:	move $a0, $s0				#check = check_line(row, col, 1, -1);
			move $a1, $s2
			li $a2, 1
			li $a3, -1
			jal check_line
			beq $v0, $s4, fwd
			j check_winner__epilogue
			
		fwd:	addi $s2, $s2, 1
			j cwfor2	
		cwfor2_end:
		
		addi $s0, $s0, 1
		j cwfor1	
	cwfor1_end:
	
	move $v0, $s4
	
check_winner__epilogue:
	pop $ra
	end
	
	jr	$ra		# return;
########################################################################
# .TEXT <check_line>
	.text
check_line:
	# Args:
	#   - $a0: int start_row
	#   - $a1: int start_col
	#   - $a2: int offset_row
	#   - $a3: int offset_col
	# Returns:
	#   - $v0: int
	#
	# Frame:    [...]
	# Uses:     [...]
	# Clobbers: [...]
	#
	# Locals:
	#   - [...]
	#
	# Structure:
	#   check_line
	#   -> [prologue]
	#   -> body
	#   -> [epilogue]

check_line__prologue:
check_line__body:
		
	la $s5, board			#board address
	lw $s6, board_width		#board_width
	lw $s7, board_height		#board_height
	
	mul $t0, $a0, MAX_BOARD_WIDTH	#$s6			#char first_cell = board[start_row][start_col];
	mul $t1, $a1, 1
	add $t2, $t0, $s5
	add $t3, $t1, $t2
	lb $t4, ($t3)				#first_cell
	
	li $t2, CELL_EMPTY			#CELL_EMPTY
	clif1: bne $t4, $t2, clif1_end		#if (first_cell == CELL_EMPTY)
		li $v0, WINNER_NONE		#return WINNER_NONE;
		j check_line__epilogue
	clif1_end:
	
	add $t2, $a0, $a2			#int row = start_row + offset_row;
	add $t3, $a1, $a3			#int col = start_col + offset_col;
	
	li $t0, 0				#for (int i = 0; i < CONNECT - 1; i++)
	li $t1, CONNECT
	addi $t1, $t1, -1
	clfor:	bge $t0, $t1, clfor_end
	
		clif2_1:
			bge $t2, 0, clif2_2		#if (row < 0 || col < 0)
			li $v0, WINNER_NONE		#return WINNER_NONE;
			j check_line__epilogue
		
		clif2_2:
			bge $t3, 0, clif2_end
			li $v0, WINNER_NONE		#return WINNER_NONE;
			j check_line__epilogue
		
		clif2_end:
		
		clif3_1: blt $t2, $s7, clif3_2		#if (row >= board_height || col >= board_width)
			li $v0, WINNER_NONE		#return WINNER_NONE;
			j check_line__epilogue
		clif3_2: blt $t3, $s6, clif3_end
			li $v0, WINNER_NONE		#return WINNER_NONE;
			j check_line__epilogue
		clif3_end:
		
		mul $t5, $t2, MAX_BOARD_WIDTH	        #char cell = board[row][col];
		mul $t6, $t3, 1
		add $t5, $t5, $s5
		add $t5, $t5, $t6
		lb $t6, ($t5)				#cell
		
		clif4: beq $t6, $t4, clif4_end		#if (cell != first_cell)
			li $v0, WINNER_NONE		#return WINNER_NONE;
			j check_line__epilogue
		clif4_end:	
		
		add $t2, $t2, $a2			#row += offset_row;
		add $t3, $t3, $a3			#col += offset_col;

		addi $t0, $t0,1
		j clfor		
	clfor_end:
	
	li $t0, CELL_RED
	clif5: bne $t4, $t0, clif5_else
		li $v0, WINNER_RED		#return WINNER_RED;
		j check_line__epilogue
	clif5_else:
		li $v0, WINNER_YELLOW		#return WINNER_YELLOW;
		j check_line__epilogue
	clif5_end:
	
check_line__epilogue:
	
	jr	$ra		# return;


########################################################################
# .TEXT <is_board_full>
# YOU DO NOT NEED TO CHANGE THE IS_BOARD_FULL FUNCTION
	.text
is_board_full:
	# Args:     void
	# Returns:
	#   - $v0: bool
	#
	# Frame:    []
	# Uses:     [$v0, $t0, $t1, $t2, $t3]
	# Clobbers: [$v0, $t0, $t1, $t2, $t3]
	#
	# Locals:
	#   - $t0: int row
	#   - $t1: int col
	#
	# Structure:
	#   is_board_full
	#   -> [prologue]
	#   -> body
	#   -> loop_row_init
	#   -> loop_row_cond
	#   -> loop_row_body
	#     -> loop_col_init
	#     -> loop_col_cond
	#     -> loop_col_body
	#     -> loop_col_step
	#     -> loop_col_end
	#   -> loop_row_step
	#   -> loop_row_end
	#   -> [epilogue]

is_board_full__prologue:
is_board_full__body:
	li	$v0, true

is_board_full__loop_row_init:
	li	$t0, 0						# int row = 0;

is_board_full__loop_row_cond:
	lw	$t2, board_height
	bge	$t0, $t2, is_board_full__epilogue		# if (row >= board_height) goto is_board_full__loop_row_end;

is_board_full__loop_row_body:
is_board_full__loop_col_init:
	li	$t1, 0						# int col = 0;

is_board_full__loop_col_cond:
	lw	$t2, board_width
	bge	$t1, $t2, is_board_full__loop_col_end		# if (col >= board_width) goto is_board_full__loop_col_end;

is_board_full__loop_col_body:
	mul	$t2, $t0, MAX_BOARD_WIDTH			# row * MAX_BOARD_WIDTH
	add	$t2, $t2, $t1					# row * MAX_BOARD_WIDTH + col
	lb	$t3, board($t2)					# board[row][col];
	bne	$t3, CELL_EMPTY, is_board_full__loop_col_step	# if (cell != CELL_EMPTY) goto is_board_full__loop_col_step;

	li	$v0, false
	b	is_board_full__epilogue				# return false;

is_board_full__loop_col_step:
	addi	$t1, $t1, 1					# col++;
	b	is_board_full__loop_col_cond			# goto is_board_full__loop_col_cond;

is_board_full__loop_col_end:
is_board_full__loop_row_step:
	addi	$t0, $t0, 1					# row++;
	b	is_board_full__loop_row_cond			# goto is_board_full__loop_row_cond;

is_board_full__loop_row_end:
is_board_full__epilogue:
	jr	$ra						# return;


########################################################################
# .TEXT <print_board>
# YOU DO NOT NEED TO CHANGE THE PRINT_BOARD FUNCTION
	.text
print_board:
	# Args:     void
	# Returns:  void
	#
	# Frame:    []
	# Uses:     [$v0, $a0, $t0, $t1, $t2]
	# Clobbers: [$v0, $a0, $t0, $t1, $t2]
	#
	# Locals:
	#   - `int col` in $t0
	#   - `int row` in $t0
	#   - `int col` in $t1
	#
	# Structure:
	#   print_board
	#   -> [prologue]
	#   -> body
	#   -> for_header_init
	#   -> for_header_cond
	#   -> for_header_body
	#   -> for_header_step
	#   -> for_header_post
	#   -> for_row_init
	#   -> for_row_cond
	#   -> for_row_body
	#     -> for_col_init
	#     -> for_col_cond
	#     -> for_col_body
	#     -> for_col_step
	#     -> for_col_post
	#   -> for_row_step
	#   -> for_row_post
	#   -> [epilogue]

print_board__prologue:
print_board__body:
	li	$v0, 11			# syscall 11: print_int
	la	$a0, '\n'
	syscall				# printf("\n");

print_board__for_header_init:
	li	$t0, 0			# int col = 0;

print_board__for_header_cond:
	lw	$t1, board_width
	blt	$t0, $t1, print_board__for_header_body	# col < board_width;
	b	print_board__for_header_post

print_board__for_header_body:
	li	$v0, 1			# syscall 1: print_int
	addiu	$a0, $t0, 1		#              col + 1
	syscall				# printf("%d", col + 1);

	li	$v0, 11			# syscall 11: print_character
	li	$a0, ' '
	syscall				# printf(" ");

print_board__for_header_step:
	addiu	$t0, 1			# col++
	b	print_board__for_header_cond

print_board__for_header_post:
	li	$v0, 11
	la	$a0, '\n'
	syscall				# printf("\n");

print_board__for_row_init:
	li	$t0, 0			# int row = 0;

print_board__for_row_cond:
	lw	$t1, board_height
	blt	$t0, $t1, print_board__for_row_body	# row < board_height
	b	print_board__for_row_post

print_board__for_row_body:
print_board__for_col_init:
	li	$t1, 0			# int col = 0;

print_board__for_col_cond:
	lw	$t2, board_width
	blt	$t1, $t2, print_board__for_col_body	# col < board_width
	b	print_board__for_col_post

print_board__for_col_body:
	mul	$t2, $t0, MAX_BOARD_WIDTH           # MAX_BOARD_WIDTH
	add	$t2, $t1
	lb	$a0, board($t2)		# board[row][col]

	li	$v0, 11			# syscall 11: print_character
	syscall				# printf("%c", board[row][col]);
	
	li	$v0, 11			# syscall 11: print_character
	li	$a0, ' '
	syscall				# printf(" ");

print_board__for_col_step:
	addiu	$t1, 1			# col++;
	b	print_board__for_col_cond

print_board__for_col_post:
	li	$v0, 11			# syscall 11: print_character
	li	$a0, '\n'
	syscall				# printf("\n");

print_board__for_row_step:
	addiu	$t0, 1
	b	print_board__for_row_cond

print_board__for_row_post:
print_board__epilogue:
	jr	$ra			# return;


