#include <stdio.h> // FILE
#include <fstream> // std::ifstream
#include <cassert>

#include "Parser.h"
#include "PhastInput.h"

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#include "srcinput/message.h"
#undef EXTERNAL

extern void initialize(void);
extern int process_chem_names(void);
extern int process_file_names(int argc, char *argv[]);
extern int reset_transient_data(void);
extern int accumulate(void);
extern int backspace (FILE *file, int spaces);
extern int bc_free(struct bc *bc_ptr);
extern int cell_free(struct cell *cell_ptr);
extern int check_properties(void);
extern int chem_ic_free(struct chem_ic *chem_ic_ptr);
extern int copy_token (char *token_ptr, char **ptr, int *length);
extern int dup_print(const char *ptr, int emphasis);
extern int getopt (int argc, char * const *argv, const char *optstring);
extern int get_option (const char **opt_list, int count_opt_list, char **next_char);
// extern int error_msg (const char *err_str, const int stop);
extern int free_check_null(void *ptr);
extern int grid_elt_free(struct grid_elt *grid_elt_ptr);
extern int head_ic_free(struct head_ic *head_ic_ptr);
// COMMENT: {2/23/2005 1:25:53 PM}extern int malloc_error(void);
extern int property_free(struct property *property_ptr);
extern int read_input(void);
extern int replace(const char *str1, const char *str2, char *str);
extern int river_free (River *river_ptr);
extern int status(int count);
extern char * string_duplicate (const char *token);
extern int well_free (Well *well_ptr);
extern int write_hst(void);
extern int write_thru(int thru);

extern int copy_token (char *token_ptr, char **ptr, int *length);
///extern int error_msg (const char *err_str, const int stop);
extern int read_line_doubles(char *next_char, double **d, int *count_d, int *count_alloc);
int read_lines_doubles(char *next_char, double **d, int *count_d, int *count_alloc, const char **opt_list, int count_opt_list, int *opt);

extern char  error_string[10*MAX_LENGTH];
extern char  *prefix, *transport_name, *chemistry_name, *database_name;
extern int   head_ic_file_warning;
extern char  *line;
 
int get_logical_line(FILE *fp, int *l);
int get_line(FILE *fp);
int read_file_doubles(char *next_char, double **d, int *count_d, int *count_alloc);
int phast_input(bool bWritePhastTmp);
int load(bool bWritePhastTmp);
int load_first_sim(bool bWritePhastTmp);
int load_next_sim(bool bWritePhastTmp);
int error_msg (const char *err_str, const int stop);

#if !defined(__WPHAST__)
#error __WPHAST__ must be set
#endif

int load(bool bWritePhastTmp)
{
#ifdef SKIP_WPHAST
	/*
	 *   Add callbacks for error_msg and warning_msg
	 */
	add_message_callback(default_handler, NULL); 
	/*
	 * Initialize
	 */
	output_msg(OUTPUT_STDERR, "Initialize...\n");
	initialize();
/*
 *   Open files
 */
	output_msg(OUTPUT_STDERR, "Process file names...\n");
	process_file_names(argc, argv);
	input_file = transport_file;
/*	fprintf(std_error, "Done process file names...\n"); */
	output_msg(OUTPUT_ECHO, "Running PHASTINPUT.\n\nProcessing flow and transport data file.\n\n");
/*
 *   Use to cause output to be completely unbuffered
 */
	setbuf(echo_file, NULL);
/*	fprintf(std_error, "Done setbuf echo file...\n"); */
	setbuf(hst_file, NULL); 
/*	fprintf(std_error, "Done setbuf hst_file...\n"); */
/*
 *   Read input data for simulation
 */
	input = input_file;
#endif /* SKIP_WPHAST */
	if (read_input() == EOF) {
		error_msg("No data defined.", STOP);
	}
	if (flow_only == FALSE && input_error == 0) {
#ifdef SKIP_WPHAST
		process_chem_names();
#endif /* SKIP_WPHAST */
	}
	check_hst_units();
	check_time_series_data();
	if (input_error == 0) {
		collate_simulation_periods();
		for (simulation=0; simulation < count_simulation_periods; simulation++) {
			if (simulation > 0 && bWritePhastTmp) write_thru(FALSE);
			current_start_time = simulation_periods[simulation];
			if (simulation < count_simulation_periods - 1) {
				current_end_time = simulation_periods[simulation + 1];
			} else {
				current_end_time = time_end[count_time_end - 1].value*time_end[count_time_end - 1].input_to_user;
			}
			reset_transient_data();
			if (input_error > 0) break;
			output_msg(OUTPUT_STDERR, "Accumulate...\n");
			accumulate();
			if (input_error > 0) break;
			if (simulation == 0) {
				output_msg(OUTPUT_STDERR, "Check properties...\n");
				check_properties();
			}
			output_msg(OUTPUT_STDERR, "Write hst...\n");
			if (bWritePhastTmp) write_hst();
			if (input_error > 0) break;
		}
		if (bWritePhastTmp) write_thru(TRUE);
	}
	/*
	 *  Finish
	 */
	output_msg(OUTPUT_STDERR, "Clean up...\n");
	output_msg(OUTPUT_ECHO, "\nPHASTINPUT done.\n\n");
	return(input_error);
}

// COMMENT: {3/31/2005 6:18:10 PM}int load_first_sim(bool bWritePhastTmp)
// COMMENT: {3/31/2005 6:18:10 PM}{
// COMMENT: {3/31/2005 6:18:10 PM}	if (read_input() == EOF) {
// COMMENT: {3/31/2005 6:18:10 PM}		error_msg("No data defined.", STOP);
// COMMENT: {3/31/2005 6:18:10 PM}	}
// COMMENT: {3/31/2005 6:18:10 PM}	if (flow_only == FALSE && input_error == 0) {
// COMMENT: {3/31/2005 6:18:10 PM}#ifdef SKIP_WPHAST
// COMMENT: {3/31/2005 6:18:10 PM}		process_chem_names();
// COMMENT: {3/31/2005 6:18:10 PM}#endif /* SKIP_WPHAST */
// COMMENT: {3/31/2005 6:18:10 PM}	}
// COMMENT: {3/31/2005 6:18:10 PM}	//{{
// COMMENT: {3/31/2005 6:18:10 PM}	check_hst_units();
// COMMENT: {3/31/2005 6:18:10 PM}	check_time_series_data();
// COMMENT: {3/31/2005 6:18:10 PM}	if (input_error == 0) {
// COMMENT: {3/31/2005 6:18:10 PM}		collate_simulation_periods();
// COMMENT: {3/31/2005 6:18:10 PM}		for (simulation=0; simulation < count_simulation_periods; simulation++) {
// COMMENT: {3/31/2005 6:18:10 PM}			if (simulation > 0) write_thru(FALSE);
// COMMENT: {3/31/2005 6:18:10 PM}			current_start_time = simulation_periods[simulation];
// COMMENT: {3/31/2005 6:18:10 PM}			if (simulation < count_simulation_periods - 1) {
// COMMENT: {3/31/2005 6:18:10 PM}				current_end_time = simulation_periods[simulation + 1];
// COMMENT: {3/31/2005 6:18:10 PM}			} else {
// COMMENT: {3/31/2005 6:18:10 PM}				current_end_time = time_end[count_time_end - 1].value*time_end[count_time_end - 1].input_to_user;
// COMMENT: {3/31/2005 6:18:10 PM}			}
// COMMENT: {3/31/2005 6:18:10 PM}			reset_transient_data();
// COMMENT: {3/31/2005 6:18:10 PM}			if (input_error > 0) break;
// COMMENT: {3/31/2005 6:18:10 PM}			output_msg(OUTPUT_STDERR, "Accumulate...\n");
// COMMENT: {3/31/2005 6:18:10 PM}			accumulate();
// COMMENT: {3/31/2005 6:18:10 PM}			if (input_error > 0) break;
// COMMENT: {3/31/2005 6:18:10 PM}			if (simulation == 0) {
// COMMENT: {3/31/2005 6:18:10 PM}				output_msg(OUTPUT_STDERR, "Check properties...\n");
// COMMENT: {3/31/2005 6:18:10 PM}				check_properties();
// COMMENT: {3/31/2005 6:18:10 PM}			}
// COMMENT: {3/31/2005 6:18:10 PM}			output_msg(OUTPUT_STDERR, "Write hst...\n");
// COMMENT: {3/31/2005 6:18:10 PM}			if (bWritePhastTmp) write_hst();
// COMMENT: {3/31/2005 6:18:10 PM}			if (input_error > 0) break;
// COMMENT: {3/31/2005 6:18:10 PM}			break;
// COMMENT: {3/31/2005 6:18:10 PM}		}
// COMMENT: {3/31/2005 6:18:10 PM}// COMMENT: {2/25/2005 5:03:48 PM}		write_thru(TRUE);
// COMMENT: {3/31/2005 6:18:10 PM}	}
// COMMENT: {3/31/2005 6:18:10 PM}	simulation = 1;
// COMMENT: {3/31/2005 6:18:10 PM}	//}}
// COMMENT: {3/31/2005 6:18:10 PM}
// COMMENT: {3/31/2005 6:18:10 PM}// COMMENT: {2/25/2005 4:49:41 PM}	fprintf(std_error, "Accumulate...\n");
// COMMENT: {3/31/2005 6:18:10 PM}// COMMENT: {2/25/2005 4:49:41 PM}	accumulate();
// COMMENT: {3/31/2005 6:18:10 PM}// COMMENT: {2/25/2005 4:49:41 PM}	fprintf(std_error, "Check properties...\n");
// COMMENT: {3/31/2005 6:18:10 PM}// COMMENT: {2/25/2005 4:49:41 PM}	check_properties();
// COMMENT: {3/31/2005 6:18:10 PM}// COMMENT: {2/25/2005 4:49:41 PM}	fprintf(std_error, "Write hst...\n");
// COMMENT: {3/31/2005 6:18:10 PM}// COMMENT: {2/25/2005 4:49:41 PM}	if (bWritePhastTmp) write_hst();
// COMMENT: {3/31/2005 6:18:10 PM}
// COMMENT: {3/31/2005 6:18:10 PM}	return 0;
// COMMENT: {3/31/2005 6:18:10 PM}}

// COMMENT: {3/31/2005 6:18:22 PM}int load_next_sim(bool bWritePhastTmp)
// COMMENT: {3/31/2005 6:18:22 PM}{
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	simulation = 1;
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	reset_transient_data();
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	fprintf(std_error, "Read input...\n");
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	if (read_input() == EOF) {
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}		if (bWritePhastTmp) write_thru(TRUE);
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}		return EOF;
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	}
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	else {
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}		if (bWritePhastTmp) write_thru(FALSE);
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	}
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	if (simulation == 0 && flow_only == FALSE && input_error == 0) {
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}		//assert(false);
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}		//process_chem_names();
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	}
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	fprintf(std_error, "Accumulate...\n");
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	accumulate();
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	fprintf(std_error, "Check properties...\n");
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	check_properties();
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	fprintf(std_error, "Write hst...\n");
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 4:50:01 PM}	if (bWritePhastTmp) write_hst();
// COMMENT: {3/31/2005 6:18:22 PM}	for (; simulation < count_simulation_periods; simulation++) {
// COMMENT: {3/31/2005 6:18:22 PM}		if (simulation > 0) write_thru(FALSE);
// COMMENT: {3/31/2005 6:18:22 PM}		current_start_time = simulation_periods[simulation];
// COMMENT: {3/31/2005 6:18:22 PM}		if (simulation < count_simulation_periods - 1) {
// COMMENT: {3/31/2005 6:18:22 PM}			current_end_time = simulation_periods[simulation + 1];
// COMMENT: {3/31/2005 6:18:22 PM}		} else {
// COMMENT: {3/31/2005 6:18:22 PM}			current_end_time = time_end[count_time_end - 1].value*time_end[count_time_end - 1].input_to_user;
// COMMENT: {3/31/2005 6:18:22 PM}		}
// COMMENT: {3/31/2005 6:18:22 PM}		reset_transient_data();
// COMMENT: {3/31/2005 6:18:22 PM}		if (input_error > 0) break;
// COMMENT: {3/31/2005 6:18:22 PM}		output_msg(OUTPUT_STDERR, "Accumulate...\n");
// COMMENT: {3/31/2005 6:18:22 PM}		accumulate();
// COMMENT: {3/31/2005 6:18:22 PM}		if (input_error > 0) break;
// COMMENT: {3/31/2005 6:18:22 PM}		if (simulation == 0) {
// COMMENT: {3/31/2005 6:18:22 PM}			output_msg(OUTPUT_STDERR, "Check properties...\n");
// COMMENT: {3/31/2005 6:18:22 PM}			check_properties();
// COMMENT: {3/31/2005 6:18:22 PM}		}
// COMMENT: {3/31/2005 6:18:22 PM}		output_msg(OUTPUT_STDERR, "Write hst...\n");
// COMMENT: {3/31/2005 6:18:22 PM}		if (bWritePhastTmp) write_hst();
// COMMENT: {3/31/2005 6:18:22 PM}// COMMENT: {2/25/2005 5:00:49 PM}		if (input_error > 0) break;
// COMMENT: {3/31/2005 6:18:22 PM}		//{{
// COMMENT: {3/31/2005 6:18:22 PM}		return 0;
// COMMENT: {3/31/2005 6:18:22 PM}		//}}
// COMMENT: {3/31/2005 6:18:22 PM}	}
// COMMENT: {3/31/2005 6:18:22 PM}	write_thru(TRUE);
// COMMENT: {3/31/2005 6:18:22 PM}
// COMMENT: {3/31/2005 6:18:22 PM}	return EOF;
// COMMENT: {3/31/2005 6:18:22 PM}}

int phast_input(bool bWritePhastTmp)
{
	::load(bWritePhastTmp);
	return 0;
// COMMENT: {3/31/2005 6:19:44 PM}	for (simulation=0; ;simulation++) {
// COMMENT: {3/31/2005 6:19:44 PM}		reset_transient_data();
// COMMENT: {3/31/2005 6:19:44 PM}		fprintf(std_error, "Read input...\n");
// COMMENT: {3/31/2005 6:19:44 PM}		if (read_input() == EOF) {
// COMMENT: {3/31/2005 6:19:44 PM}			if (bWritePhastTmp) write_thru(TRUE);
// COMMENT: {3/31/2005 6:19:44 PM}			break;
// COMMENT: {3/31/2005 6:19:44 PM}		} else if (simulation > 0) {
// COMMENT: {3/31/2005 6:19:44 PM}			if (bWritePhastTmp) write_thru(FALSE);
// COMMENT: {3/31/2005 6:19:44 PM}		}
// COMMENT: {3/31/2005 6:19:44 PM}		if (simulation == 0 && flow_only == FALSE && input_error == 0) {
// COMMENT: {3/31/2005 6:19:44 PM}			//assert(false);
// COMMENT: {3/31/2005 6:19:44 PM}			//process_chem_names();
// COMMENT: {3/31/2005 6:19:44 PM}		}
// COMMENT: {3/31/2005 6:19:44 PM}		fprintf(std_error, "Accumulate...\n");
// COMMENT: {3/31/2005 6:19:44 PM}		accumulate();
// COMMENT: {3/31/2005 6:19:44 PM}		fprintf(std_error, "Check properties...\n");
// COMMENT: {3/31/2005 6:19:44 PM}		check_properties();
// COMMENT: {3/31/2005 6:19:44 PM}		fprintf(std_error, "Write hst...\n");
// COMMENT: {3/31/2005 6:19:44 PM}		if (bWritePhastTmp) write_hst();
// COMMENT: {3/31/2005 6:19:44 PM}		///break;
// COMMENT: {3/31/2005 6:19:44 PM}	}
// COMMENT: {3/31/2005 6:19:44 PM}	return 0;
}

/* ---------------------------------------------------------------------- */
int get_line(FILE *fp)
/* ---------------------------------------------------------------------- */
{
	return CPhastInput::GetParser()->get_line();
}

// COMMENT: {2/23/2005 2:15:54 PM}/* ---------------------------------------------------------------------- */
// COMMENT: {2/23/2005 2:15:54 PM}int error_msg (const char *err_str, const int stop)
// COMMENT: {2/23/2005 2:15:54 PM}/* ---------------------------------------------------------------------- */
// COMMENT: {2/23/2005 2:15:54 PM}{
// COMMENT: {2/23/2005 2:15:54 PM}	if (CPhastInput::GetParser()) {
// COMMENT: {2/23/2005 2:15:54 PM}		return CPhastInput::GetParser()->error_msg(err_str, stop);
// COMMENT: {2/23/2005 2:15:54 PM}	}
// COMMENT: {2/23/2005 2:15:54 PM}	return 0;
// COMMENT: {2/23/2005 2:15:54 PM}}

/* ---------------------------------------------------------------------- */
int read_file_doubles(char *next_char, double **d, int *count_d, int *count_alloc)
/* ---------------------------------------------------------------------- */
{
// COMMENT: {3/31/2005 6:20:34 PM}
// COMMENT: {3/31/2005 6:20:34 PM}/*
// COMMENT: {3/31/2005 6:20:34 PM} *      Reads doubles from a file
// COMMENT: {3/31/2005 6:20:34 PM} *      next_char contains file name
// COMMENT: {3/31/2005 6:20:34 PM} *      Appends to d.
// COMMENT: {3/31/2005 6:20:34 PM} *      Stops at EOF or ERROR
// COMMENT: {3/31/2005 6:20:34 PM} *
// COMMENT: {3/31/2005 6:20:34 PM} *      Input Arguments:
// COMMENT: {3/31/2005 6:20:34 PM} *         next_char    points to file name
// COMMENT: {3/31/2005 6:20:34 PM} *         d            points to array of doubles, must be malloced
// COMMENT: {3/31/2005 6:20:34 PM} *         count_d      number of elements in array
// COMMENT: {3/31/2005 6:20:34 PM} *         count_alloc  number of elements malloced
// COMMENT: {3/31/2005 6:20:34 PM} *
// COMMENT: {3/31/2005 6:20:34 PM} *      Output Arguments:
// COMMENT: {3/31/2005 6:20:34 PM} *         d            points to array of doubles, may have been
// COMMENT: {3/31/2005 6:20:34 PM} *                          realloced
// COMMENT: {3/31/2005 6:20:34 PM} *         count_d      updated number of elements in array
// COMMENT: {3/31/2005 6:20:34 PM} *         count_alloc  updated of elements malloced
// COMMENT: {3/31/2005 6:20:34 PM} *
// COMMENT: {3/31/2005 6:20:34 PM} *      Returns:
// COMMENT: {3/31/2005 6:20:34 PM} *         OK
// COMMENT: {3/31/2005 6:20:34 PM} *         ERROR if any errors reading doubles
// COMMENT: {3/31/2005 6:20:34 PM} */	
// COMMENT: {3/31/2005 6:20:34 PM}	int j, l, return_value;
// COMMENT: {3/31/2005 6:20:34 PM}	char token[MAX_LENGTH], name[MAX_LENGTH], property_file_name[MAX_LENGTH];
// COMMENT: {3/31/2005 6:20:34 PM}	////{{
	++input_error;
	error_msg("Properties cannot be defined using the \"file\" option.", CONTINUE);
	return ERROR;
// COMMENT: {3/31/2005 6:20:40 PM}	////}}
// COMMENT: {3/31/2005 6:20:40 PM}/*
// COMMENT: {3/31/2005 6:20:40 PM} *    open file
// COMMENT: {3/31/2005 6:20:40 PM} */
// COMMENT: {3/31/2005 6:20:40 PM}	return_value = OK;
// COMMENT: {3/31/2005 6:20:40 PM}	j = copy_token(token, &next_char, &l);
// COMMENT: {3/31/2005 6:20:40 PM}	std::ifstream ifs;
// COMMENT: {3/31/2005 6:20:40 PM}	ifs.open(token);
// COMMENT: {3/31/2005 6:20:40 PM}	if (!ifs.is_open()) {
// COMMENT: {3/31/2005 6:20:40 PM}		sprintf(error_string, "Can't open file, %s.", token);
// COMMENT: {3/31/2005 6:20:40 PM}		error_msg(error_string, STOP);
// COMMENT: {3/31/2005 6:20:40 PM}		return(ERROR);
// COMMENT: {3/31/2005 6:20:40 PM}	}
// COMMENT: {3/31/2005 6:20:40 PM}	CParser readDoubles(ifs);
// COMMENT: {3/31/2005 6:20:40 PM}	strcpy(property_file_name, token);
// COMMENT: {3/31/2005 6:20:40 PM}	strcpy(name, prefix);
// COMMENT: {3/31/2005 6:20:40 PM}	strcat(name, ".head.dat");
// COMMENT: {3/31/2005 6:20:40 PM}	if (strcmp(property_file_name, name) == 0) {
// COMMENT: {3/31/2005 6:20:40 PM}		head_ic_file_warning = TRUE;
// COMMENT: {3/31/2005 6:20:40 PM}	}
// COMMENT: {3/31/2005 6:20:40 PM}/*
// COMMENT: {3/31/2005 6:20:40 PM} *   read doubles
// COMMENT: {3/31/2005 6:20:40 PM} */
// COMMENT: {3/31/2005 6:20:40 PM}	for(;;) {
// COMMENT: {3/31/2005 6:20:40 PM}		j = readDoubles.get_line();
// COMMENT: {3/31/2005 6:20:40 PM}		if (j == EMPTY) {
// COMMENT: {3/31/2005 6:20:40 PM}			continue;
// COMMENT: {3/31/2005 6:20:40 PM}		} else if (j == EOF) {
// COMMENT: {3/31/2005 6:20:40 PM}			break;
// COMMENT: {3/31/2005 6:20:40 PM}		}
// COMMENT: {3/31/2005 6:20:40 PM}		next_char = line;
// COMMENT: {3/31/2005 6:20:40 PM}		if (read_line_doubles(next_char, d, count_d, count_alloc) == ERROR) {
// COMMENT: {3/31/2005 6:20:40 PM}			sprintf(error_string,"Reading from file %s\n%s", token, line) ;
// COMMENT: {3/31/2005 6:20:40 PM}			error_msg(error_string, CONTINUE);
// COMMENT: {3/31/2005 6:20:40 PM}			return_value = ERROR;
// COMMENT: {3/31/2005 6:20:40 PM}		}
// COMMENT: {3/31/2005 6:20:40 PM}	}
// COMMENT: {3/31/2005 6:20:40 PM}	if (*count_d > 0) {
// COMMENT: {3/31/2005 6:20:40 PM}		*d = (double*)realloc(*d, (size_t) *count_d * sizeof(double));
// COMMENT: {3/31/2005 6:20:40 PM}		*count_alloc = *count_d;
// COMMENT: {3/31/2005 6:20:40 PM}	}
// COMMENT: {3/31/2005 6:20:40 PM}	ifs.close();
// COMMENT: {3/31/2005 6:20:40 PM}	return(return_value);
}

/* ---------------------------------------------------------------------- */
int read_lines_doubles(char *next_char, double **d, int *count_d, int *count_alloc, const char **opt_list, int count_opt_list, int *opt)
/* ---------------------------------------------------------------------- */

	{
/*
 *      Reads doubles on line starting at next_char
 *      and on succeeding lines. Appends to d.
 *      Stops at KEYWORD, OPTION, and EOF
 *
 *      Input Arguments:
 *         next_char    points to line to read from
 *         d            points to array of doubles, must be malloced
 *         count_d      number of elements in array
 *         count_alloc  number of elements malloced
 *
 *      Output Arguments:
 *         d            points to array of doubles, may have been
 *                          realloced
 *         count_d      updated number of elements in array
 *         count_alloc  updated of elements malloced
 *
 *      Returns:
 *         KEYWORD
 *         OPTION
 *         EOF
 *         ERROR if any errors reading doubles
 */	

	if (read_line_doubles(next_char, d, count_d, count_alloc) == ERROR) {
		return(ERROR);
	}
	for(;;) {
		*opt = get_option(opt_list, count_opt_list, &next_char);
		if (*opt == OPTION_KEYWORD || *opt == OPTION_EOF || *opt ==OPTION_ERROR) {
			break;
		} else if (*opt >= 0) {
			break;
		} 
		next_char = line;
		if (read_line_doubles(next_char, d, count_d, count_alloc) == ERROR) {
			return(ERROR);
		}
	}
	////{{
	++input_error;
	error_msg("Properties cannot be defined using the \"by_element\" or \"by_cell\" option.", CONTINUE);
	return ERROR;
	////}}
	return(OK);
}

void GetDefaultMedia(struct grid_elt* p_grid_elt)
{
	extern struct cell *cells;
	int i;

	assert(p_grid_elt);
	assert(p_grid_elt->porosity);
	assert(p_grid_elt->kx);
	assert(p_grid_elt->ky);
	assert(p_grid_elt->kz);
	assert(p_grid_elt->storage);
	assert(p_grid_elt->alpha_long);
	assert(p_grid_elt->alpha_horizontal);
	assert(p_grid_elt->alpha_vertical);

	for (i = 0; i < nxyz; i++) {
		if (cells[i].is_element == FALSE) continue;
		if (cells[i].elt_active == FALSE) continue;
/* 
 *   Porosity
 */
		p_grid_elt->porosity->type = FIXED;
		p_grid_elt->porosity->count_v = 1;
		p_grid_elt->porosity->v[0] = cells[i].porosity;
/* 
 *   X hydraulic conductivity
 */
		p_grid_elt->kx->type = FIXED;
		p_grid_elt->kx->count_v = 1;
		p_grid_elt->kx->v[0] = cells[i].kx;

/* 
 *   Y hydraulic conductivity
 */
		p_grid_elt->ky->type = FIXED;
		p_grid_elt->ky->count_v = 1;
		p_grid_elt->ky->v[0] = cells[i].ky;
/* 
 *   Z hydraulic conductivity
 */
		p_grid_elt->kz->type = FIXED;
		p_grid_elt->kz->count_v = 1;
		p_grid_elt->kz->v[0] = cells[i].kz;
/* 
 *   Specific storage
 */
		p_grid_elt->storage->type = FIXED;
		p_grid_elt->storage->count_v = 1;
		p_grid_elt->storage->v[0] = cells[i].storage;

		if (flow_only != TRUE) {
/* 
 *   Longitudinal dispersivity
 */
			p_grid_elt->alpha_long->type = FIXED;
			p_grid_elt->alpha_long->count_v = 1;
			p_grid_elt->alpha_long->v[0] = cells[i].alpha_long;
/* 
 *   Horizontal dispersivity
 */
			p_grid_elt->alpha_horizontal->type = FIXED;
			p_grid_elt->alpha_horizontal->count_v = 1;
			p_grid_elt->alpha_horizontal->v[0] = cells[i].alpha_horizontal;
/* 
 *   Vertical dispersivity
 */
			p_grid_elt->alpha_vertical->type = FIXED;
			p_grid_elt->alpha_vertical->count_v = 1;
			p_grid_elt->alpha_vertical->v[0] = cells[i].alpha_vertical;
		}
		break;
	}		
}

/* ---------------------------------------------------------------------- */
void GetDefaultHeadIC(struct head_ic* p_head_ic)
/* ---------------------------------------------------------------------- */
{
	extern struct cell *cells;
	int i;

	assert(p_head_ic && p_head_ic->head);

/*
 *   check values
 */
	for (i = 0; i < nxyz; i++) {
		if (cells[i].cell_active == FALSE) continue;

/* 
 *   Head initial condition
 */
		p_head_ic->head->type = FIXED;
		p_head_ic->head->count_v = 1;
		p_head_ic->head->v[0] = cells[i].ic_head;
		break;
	}
}
/* ---------------------------------------------------------------------- */
void GetDefaultChemIC(struct chem_ic* p_chem_ic)
/* ---------------------------------------------------------------------- */
{
	extern struct cell *cells;
	int i;

	assert(p_chem_ic && p_chem_ic->solution);

/*
 *   check values
 */

	p_chem_ic->solution->type = FIXED;
	p_chem_ic->solution->count_v = 1;
	p_chem_ic->solution->v[0] = 1;

	if (count_chem_ic > 0) {
		for (i = 0; i < nxyz; i++) {
			if (cells[i].cell_active == FALSE) continue;
/* 
*   Solution initial condition
*/
			if (cells[i].ic_solution_defined == TRUE) {
				p_chem_ic->solution->v[0] = cells[i].ic_solution.i1;
				break;
			}
		}
	}
}
