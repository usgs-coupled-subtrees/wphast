#include <stdio.h> // FILE
#include <fstream> // std::ifstream
#include <cassert>

#include "Parser.h"
#include "PhastInput.h"

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#include "srcinput/message.h"
#undef EXTERNAL
#include "enum_fix.h"

extern void initialize(void);
extern int process_chem_names(void);
extern int process_file_names(int argc, char *argv[]);
extern int reset_transient_data(void);
extern int accumulate(void);
extern int backspace (FILE *file, int spaces);
extern int bc_free(struct bc *bc_ptr);
extern int cell_free(struct cell *cell_ptr);
extern int check_properties(bool defaults);
extern int chem_ic_free(struct chem_ic *chem_ic_ptr);
extern int copy_token (char *token_ptr, char **ptr, int *length);
extern int dup_print(const char *ptr, int emphasis);
extern int getopt (int argc, char * const *argv, const char *optstring);
extern int get_option (const char **opt_list, int count_opt_list, char **next_char);
// extern int error_msg (const char *err_str, const int stop);
extern void free_check_null(void *ptr);
extern int grid_elt_free(struct grid_elt *grid_elt_ptr);
extern int head_ic_free(struct head_ic *head_ic_ptr);
// COMMENT: {2/23/2005 1:25:53 PM}extern int malloc_error(void);
extern int property_free(struct property *property_ptr);
extern int read_input(void);
extern bool replace(const char *to_remove, const char *replacement, char *string_to_search);
extern int river_free (River *river_ptr);
extern int status(int count);
extern char * string_duplicate (const char *token);
extern int well_free (Well *well_ptr);
extern int write_hst(void);
extern int write_thru(int thru);
extern int next_keyword_or_option(const char **opt_list, int count_opt_list);
extern int read_line_doubles_delimited(char *next_char, double **d,
									   int *count_d, int *count_alloc);

extern int copy_token (char *token_ptr, char **ptr, int *length);
///extern int error_msg (const char *err_str, const int stop);
extern int read_line_doubles(char *next_char, double **d, int *count_d, int *count_alloc);
int read_lines_doubles(char *next_char, double **d, int *count_d, int *count_alloc, const char **opt_list, int count_opt_list, int *opt);

extern char  error_string[10*MAX_LENGTH];
extern char  *prefix, *transport_name, *chemistry_name, *database_name;
extern int   head_ic_file_warning;
extern char  *line;

int setup_grid(void);
void Tidy_prisms(void);
 
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
			if (bWritePhastTmp)
			{
				accumulate();
			}
			else
			{
// COMMENT: {7/2/2008 10:33:48 PM}				if (simulation == 0)
// COMMENT: {7/2/2008 10:33:48 PM}				{
// COMMENT: {7/2/2008 10:33:48 PM}					// need to setup prisms
// COMMENT: {7/2/2008 10:33:48 PM}					setup_grid();
// COMMENT: {7/2/2008 10:33:48 PM}					Tidy_prisms();
// COMMENT: {7/2/2008 10:33:48 PM}				}
				// Note: accumulate needs to be called in order
				// for GetDefaultHeadIC, GetDefaultChemIC, GetDefaultMedia
				// to work
				accumulate();
			}
			if (input_error > 0) break;
			if (simulation == 0) {
				output_msg(OUTPUT_STDERR, "Check properties...\n");
				if (bWritePhastTmp)
				{
					check_properties(true);
				}
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

int read(void)
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
	return(input_error);
}
int accumulate(bool bWritePhastTmp)
{
	check_hst_units();
	check_time_series_data();
	if (input_error == 0)
	{
		collate_simulation_periods();
		for (simulation=0; simulation < count_simulation_periods; simulation++)
		{
			if (simulation > 0 && bWritePhastTmp)
			{
				write_thru(FALSE);
			}
			current_start_time = simulation_periods[simulation];
			if (simulation < count_simulation_periods - 1)
			{
				current_end_time = simulation_periods[simulation + 1];
			}
			else
			{
				current_end_time = time_end[count_time_end - 1].value*time_end[count_time_end - 1].input_to_user;
			}
			reset_transient_data();
			if (input_error > 0)
			{
				break;
			}
			output_msg(OUTPUT_STDERR, "Accumulate...\n");
			accumulate_defaults();
			if (input_error > 0)
			{
				break;
			}
			if (simulation == 0)
			{
				output_msg(OUTPUT_STDERR, "Check properties...\n");
// COMMENT: {2/15/2011 4:21:36 PM}				if (bWritePhastTmp)
				{
					check_properties(true);
				}
			}
			output_msg(OUTPUT_STDERR, "Write hst...\n");
			if (bWritePhastTmp)
			{
				write_hst();
			}
			if (input_error > 0)
			{
				break;
			}
		}
		if (bWritePhastTmp)
		{
			write_thru(TRUE);
		}
	}
	/*
	 *  Finish
	 */
	output_msg(OUTPUT_STDERR, "Clean up...\n");
	output_msg(OUTPUT_ECHO, "\nPHASTINPUT done.\n\n");
	return(input_error);
}

int phast_input(bool bWritePhastTmp)
{
	::load(bWritePhastTmp);
	return 0;
}

/* ---------------------------------------------------------------------- */
int get_line(FILE *fp)
/* ---------------------------------------------------------------------- */
{
	return CPhastInput::GetParser()->get_line();
}

/* ---------------------------------------------------------------------- */
int read_file_doubles(char *next_char, double **d, int *count_d, int *count_alloc)
/* ---------------------------------------------------------------------- */
{
	++input_error;
	error_msg("Properties cannot be defined using the \"file\" option.", CONTINUE);
	return ERROR;
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

	++input_error;
	error_msg("Properties cannot be defined using the \"by_element\" or \"by_cell\" option.", CONTINUE);
	return (ERROR);
}

/* ---------------------------------------------------------------------- */
int
read_lines_doubles_delimited(char *next_char, double **d, int *count_d,
							 int *count_alloc, const char **opt_list,
							 int count_opt_list, int *opt)
/* ---------------------------------------------------------------------- */
{
/*
 *      Reads doubles on line starting at next_char
 *      and on succeeding lines. Appends to d.
 *      Doubles are delimited by <>.
 #
 *      Expets < to start. Stops at ">". Reads next line.
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
	int j, l;
	char token[MAX_LENGTH];

	replace("<", "< ", next_char);
	j = copy_token(token, &next_char, &l);
	while (j == EMPTY)
	{
		*opt = get_option(opt_list, count_opt_list, &next_char);
		if (*opt == OPTION_KEYWORD || *opt == OPTION_EOF
			|| *opt == OPTION_ERROR || *opt >= 0)
		{
			input_error++;
			error_msg("List of values missing for property\n", CONTINUE);
			return (ERROR);
		}
		replace("<", "< ", next_char);
		j = copy_token(token, &next_char, &l);
	}
	/*
	 *  Should start with "<"
	 */
	if (token[0] != '<')
	{
		input_error++;
		error_msg("Starting \"<\" missing in list of values for property\n",
				  CONTINUE);
		*opt = next_keyword_or_option(opt_list, count_opt_list);
		return (ERROR);
	}
	if ((j =
		 read_line_doubles_delimited(next_char, d, count_d,
									 count_alloc)) == ERROR)
	{
		return (ERROR);
	}
	else if (j == DONE)
	{
		*opt = get_option(opt_list, count_opt_list, &next_char);
		++input_error;
		error_msg("Properties cannot be defined using the \"by_element\" or \"by_cell\" option.", CONTINUE);
		return (ERROR);
	}
	for (;;)
	{
		*opt = get_option(opt_list, count_opt_list, &next_char);
		if (*opt == OPTION_KEYWORD || *opt == OPTION_EOF
			|| *opt == OPTION_ERROR || *opt >= 0)
		{
			break;
		}
		next_char = line;
		if ((j =
			 read_line_doubles_delimited(next_char, d, count_d,
										 count_alloc)) == ERROR)
		{
			return (ERROR);
		}
		else if (j == DONE)
		{
			*opt = get_option(opt_list, count_opt_list, &next_char);
			++input_error;
			error_msg("Properties cannot be defined using the \"by_element\" or \"by_cell\" option.", CONTINUE);
			return (ERROR);
		}
	}
	++input_error;
	error_msg("Properties cannot be defined using the \"by_element\" or \"by_cell\" option.", CONTINUE);
	return (ERROR);
}

void GetDefaultMedia(struct grid_elt* p_grid_elt)
{
	extern struct cell *cells;
	int i;

	assert(p_grid_elt);
	assert(p_grid_elt->active);
	assert(p_grid_elt->porosity);
	assert(p_grid_elt->kx);
	assert(p_grid_elt->ky);
	assert(p_grid_elt->kz);
	assert(p_grid_elt->storage);
	assert(p_grid_elt->alpha_long);
	assert(p_grid_elt->alpha_horizontal);
	assert(p_grid_elt->alpha_vertical);
	assert(p_grid_elt->tortuosity);

	for (i = 0; i < nxyz; i++)
	{

	/*
	 * Active
	 */
		if (cells[i].is_element)
		{
			p_grid_elt->active->type = PROP_FIXED;
			p_grid_elt->active->count_v = 1;
			//p_grid_elt->active->v[0] = cells[i].elt_active;
			p_grid_elt->active->v[0] = 1;
	/* 
	 *   Porosity
	 */
			p_grid_elt->porosity->type = PROP_FIXED;
			p_grid_elt->porosity->count_v = 1;
			p_grid_elt->porosity->v[0] = cells[i].porosity;
	/* 
	 *   X hydraulic conductivity
	 */
			p_grid_elt->kx->type = PROP_FIXED;
			p_grid_elt->kx->count_v = 1;
			p_grid_elt->kx->v[0] = cells[i].kx;

	/* 
	 *   Y hydraulic conductivity
	 */
			p_grid_elt->ky->type = PROP_FIXED;
			p_grid_elt->ky->count_v = 1;
			p_grid_elt->ky->v[0] = cells[i].ky;
	/* 
	 *   Z hydraulic conductivity
	 */
			p_grid_elt->kz->type = PROP_FIXED;
			p_grid_elt->kz->count_v = 1;
			p_grid_elt->kz->v[0] = cells[i].kz;
	/* 
	 *   Specific storage
	 */
			p_grid_elt->storage->type = PROP_FIXED;
			p_grid_elt->storage->count_v = 1;
			p_grid_elt->storage->v[0] = cells[i].storage;

			if (flow_only != TRUE)
			{
	/* 
	 *   Longitudinal dispersivity
	 */
				p_grid_elt->alpha_long->type = PROP_FIXED;
				p_grid_elt->alpha_long->count_v = 1;
				p_grid_elt->alpha_long->v[0] = cells[i].alpha_long;
	/* 
	 *   Horizontal dispersivity
	 */
				p_grid_elt->alpha_horizontal->type = PROP_FIXED;
				p_grid_elt->alpha_horizontal->count_v = 1;
				p_grid_elt->alpha_horizontal->v[0] = cells[i].alpha_horizontal;
	/* 
	 *   Vertical dispersivity
	 */
				p_grid_elt->alpha_vertical->type = PROP_FIXED;
				p_grid_elt->alpha_vertical->count_v = 1;
				p_grid_elt->alpha_vertical->v[0] = cells[i].alpha_vertical;
	/* 
	 *   Tortuosity
	 */
				if (cells[i].tortuosity_defined == FALSE)
				{
					cells[i].tortuosity = 1.0;
					cells[i].tortuosity_defined = TRUE;
				}
				p_grid_elt->tortuosity->type = PROP_FIXED;
				p_grid_elt->tortuosity->count_v = 1;
				p_grid_elt->tortuosity->v[0] = cells[i].tortuosity;

			}
		}

		break;
	}		
}

/* ---------------------------------------------------------------------- */
void GetDefaultHeadIC(struct Head_ic* p_head_ic)
/* ---------------------------------------------------------------------- */
{
	extern struct cell *cells;
	int i;

	assert(p_head_ic && p_head_ic->head);

/*
 *   check values
 */
	for (i = 0; i < nxyz; i++)
	{
		p_head_ic->ic_type = HIC_ZONE;
/* 
 *   Head initial condition
 */
		p_head_ic->head->type = PROP_FIXED;
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

	p_chem_ic->solution->type = PROP_FIXED;
	p_chem_ic->solution->count_v = 1;
	p_chem_ic->solution->v[0] = 1;

	if (count_chem_ic > 0)
	{
		for (i = 0; i < nxyz; i++)
		{
/* 
*   Solution initial condition
*/
			if (cells[i].ic_solution_defined == TRUE)
			{
				p_chem_ic->solution->v[0] = cells[i].ic_solution.i1;
#ifdef SAVE
				if (cells[i].ic_equilibrium_phases_defined == TRUE)
				{
					p_chem_ic->equilibrium_phases->type = PROP_FIXED;
					p_chem_ic->equilibrium_phases->count_v = 1;
					p_chem_ic->equilibrium_phases->v[0] = cells[i].ic_equilibrium_phases.i1;
				}
				if (cells[i].ic_exchange_defined == TRUE)
				{
					p_chem_ic->exchange->type = PROP_FIXED;
					p_chem_ic->exchange->count_v = 1;
					p_chem_ic->exchange->v[0] = cells[i].ic_exchange.i1;
				}
				if (cells[i].ic_equilibrium_phases_defined == TRUE)
				{
					p_chem_ic->equilibrium_phases->type = PROP_FIXED;
					p_chem_ic->equilibrium_phases->count_v = 1;
					p_chem_ic->equilibrium_phases->v[0] = cells[i].ic_equilibrium_phases.i1;
				}
				if (cells[i].ic_exchange_defined == TRUE)
				{
					p_chem_ic->exchange->type = PROP_FIXED;
					p_chem_ic->exchange->count_v = 1;
					p_chem_ic->exchange->v[0] = cells[i].ic_exchange.i1;
				}
				if (cells[i].ic_surface_defined == TRUE)
				{
					p_chem_ic->surface->type = PROP_FIXED;
					p_chem_ic->surface->count_v = 1;
					p_chem_ic->surface->v[0] = cells[i].ic_surface.i1;
				}
				if (cells[i].ic_gas_phase_defined == TRUE)
				{
					p_chem_ic->gas_phase->type = PROP_FIXED;
					p_chem_ic->gas_phase->count_v = 1;
					p_chem_ic->gas_phase->v[0] = cells[i].ic_gas_phase.i1;
				}
				if (cells[i].ic_solid_solutions_defined == TRUE)
				{
					p_chem_ic->solid_solutions->type = PROP_FIXED;
					p_chem_ic->solid_solutions->count_v = 1;
					p_chem_ic->solid_solutions->v[0] = cells[i].ic_solid_solutions.i1;
				}
				if (cells[i].ic_kinetics_defined == TRUE)
				{
					p_chem_ic->kinetics->type = PROP_FIXED;
					p_chem_ic->kinetics->count_v = 1;
					p_chem_ic->kinetics->v[0] = cells[i].ic_kinetics.i1;
				}
#endif
				break;
			}
		}
	}
}
